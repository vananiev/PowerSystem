//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "EditGrid.h"
#include "Scope.h"
#include "ObectsLibrary.h"
//#include "kernel.h"			// ����� ��� ������ � ��������
#include <fstream>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
void err(unsigned num, String text){
   static unsigned cnt=0;
   cnt++;
	ShowMessage(text);
}
//---------------------------------------------------------------------------
__fastcall TfrmEditGrid::TfrmEditGrid(TComponent* Owner)
	: TForm(Owner), LinkLines(this)
{
   // �������������� �������
   LinkLines.Clear();     
   Objects.clear();
	// ��������� ����������� ������
   LinkLines.Parent = this;
   //LinkLines.OnDelete = LinkDelete;
   // �������� ����������� ����������� (����� ���� ������������� ��-���)
   ComponentOwner = this;
   // �������� ��������
   //DoubleBuffered = true;
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditGrid::UnitDelete(TObject *Sender){
   vector<PBase*>::iterator iter = Objects.begin();
    while( iter != Objects.end() )
    {
      if( *iter == (PBase*)Sender){
        Objects.erase( iter );
        break;}
      else
        ++iter;
    }
   // ������� ��� ����� � ���� ��������
   PElement* obj = ((PBase*)Sender)->Obj();
   for(int i=0, n=obj->Out.Count(); i<n; i++)
   	for(int j=0, m=obj->Out[i].Count; j<m; j++)
      	LinkLines.Delete( &obj->Out[i][j] );			// ������ ��� �����, ��������� � �������
	// ���������� �����
   LinkLines.Paint();
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditGrid::PinDragDrop (System::TObject* Sender, System::TObject* Source, int X, int Y){
   // ���� ���������� �� ������ ����, �� �� ���������
   if( Sender == Source ) return;
   // ��������� ���������� ����������� � �����
   Pair_PinConrtol pcon;
   pcon.First = (PPinControl*)Source;
   pcon.Second= (PPinControl*)Sender;
   LinkLines.Add(pcon);
   // ��������� �����
   LinkLines.Paint();
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditGrid::FormDragOver(TObject *Sender, TObject *Source,
      int X, int Y, TDragState State, bool &Accept)
{
   Accept = false;
   // ���� � �������� ���������� �������
   if(PPinControl *pc = dynamic_cast<PPinControl*>(Source)){
   		int from_X = pc->Left + pc->Width/2;
      	int from_Y = pc->Top  + pc->Height/2;
      	TColor color = Canvas->Pen->Color;
      	Canvas->Pen->Color = clBlack;
      	Repaint();                       // ������� ��� ��������� �����
      	Canvas->MoveTo(from_X, from_Y);
      	Canvas->LineTo(X, Y);
      	Canvas->Pen->Color = color;
         Accept = true;
      }
   // ���������� ����� ������ �� ���������� �����������
   else if(Source->ClassNameIs("TTreeView")){
		Accept = true;
   	}
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditGrid::FormPaint(TObject *Sender)
{
	// ������ ����� PaintLines()
   LinkLines.Paint();
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditGrid::FormDragDrop(TObject *Sender, TObject *Source,
      int X, int Y)
{
   // ������� ��� ��������� ����� ��������� ��� ���������� �������
   if(Source->ClassNameIs("PPinControl"))
   	Repaint();
   // ���������� ����� ������ �� ���������� �����������
   else if(Source->ClassNameIs("TTreeView")){
   	TTreeView *obj = (TTreeView*)Source;
   	int n = obj->Selected->AbsoluteIndex;
   	if ( n != -1){
      	TTreeNode *Node = obj->Items->Item[n];
         if(Node != NULL){// && !Node->HasChildren){
            // ������� �������
      		// ��������� ��� � ������ � ��������� ��������� �� ���������
            TTreeNode *Prev = obj->Selected;
            String Adress = Prev->Text;
   			while ( Prev ){
     				if ( Prev->HasChildren ) Adress = Prev -> Text + "\\" + Adress;
     				Prev = Prev->Parent;
   				}
            AddObject( Adress, X, Y);
            }
        	}
      }
}
//---------------------------------------------------------------------------
void TfrmEditGrid::AddObject(String TypeName, int X, int Y){    // ������ TypeName - ���������� �� frmObjLib->tvList
   String cName = "";
   String ClassName = "PBase";
   if(TypeName == "���������\\��������� ����������")           cName = "PSource";
   else if(TypeName == "��������������\\�������\\����������")  cName = "PTransformer_3";
   else if(TypeName == "��������������\\�������\\����������")  cName = "PTransformer_1";
   else if(TypeName == "��������������\\����\\����������")     cName = "PCurrent_Transformer_1";
   else if(TypeName == "�����\\���������� ��� ��")             cName = "PLine";
   else if(TypeName == "�����������\\����������")              cName = "PConsumer";
   else if(TypeName == "�������������� ����������\\�����������")cName = "PBreaker";
   else if(TypeName == "�������")										{cName = "PSubGrid";    ClassName = "PGridComponent"; }
	// ��������� ��� � ������ � ��������� ��������� �� ���������
   if(cName!=""){
      // �������� ���������-������
   	CreateControl(ClassName, cName);
      PBase *control = Objects.back();
      control->Left = X - control->Width/2;
      control->Top  = Y - control->Height/2;
   	}
   else{
      err(3, "PowerSystem: �� ���������� �������� ��� �������� ���������� ���������� �� ���������� ��������");
   	}
}
//---------------------------------------------------------------------------
void TfrmEditGrid::CreateControl(String Class, String ModelClass){
      // �������� ���������-������
      PBase *control;
      TComponent *owner;
      if(ComponentOwner) owner = ComponentOwner;
      else               owner = this;
      if(Class == "PGridComponent")       control = new PGridComponent( owner );
      else                    	         control = new PBase( owner );
      AddControl( control );              // �������� ��������� � ������ ������������ � ���������� ���������
      control->ObjType( ModelClass );     // ����������� ��������� � ������� ����
}
//---------------------------------------------------------------------------
// �������� ��������� � ������ ������������ � ���������� ���������
void TfrmEditGrid::AddControl(PBase *control){
   control->Parent = this;
	//control->OnChange = UnitChange;
   //control->OnPinMouseDown = PinMouseDown;
   control->OnPinDragDrop = PinDragDrop;
   control->OnDelete = UnitDelete;
   Objects.push_back( control );    		// ��������� � ������ �����������
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditGrid::mnuObjLibClick(TObject *Sender)
{
      // ����� ���������� ��������
      TfrmObjLib *frmOL = new TfrmObjLib(this);
      frmOL->Show();
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditGrid::FormMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
   // �������� ������ ������
	if(Button == mbRight){
      // ����� �� ��������� �� ����� �����?
      LinkLines.ShowMenuFor( X, Y );
   	}
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditGrid::mnuSaveClick(TObject *Sender)
{
   // ������� ������ � ���������
   saveDlg->Filter = ".psf|*.psf|����� ����|*.*";
   if( saveDlg->Execute() ){
     	AnsiString Name = saveDlg->FileName;
      ofstream out(Name.c_str());
      if(out){
      	// ��������� �������
         int n = Objects.size();
         out << n << " ";
         for(int i=0; i<n; i++) {
            out << StringForSave( &(String)Objects[i]->ClassName() ) << " ";
            Objects[i]->Save( out );
            }
         //  �������� ���������� � ������
         LinkLines.Save( out );
      	}
    }
}
//---------------------------------------------------------------------------
PElement* TfrmEditGrid::FindObj(String Name){
   for(int i=0, n=Objects.size(); i<n; i++)
      if( Name == Objects[i]->Obj()->Name )
         return Objects[i]->Obj();
   return NULL;
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditGrid::mnuOpenClick(TObject *Sender)
{
   // ������� ������ � ���������
   openDlg->Filter = ".psf|*.psf|����� ����|*.*";
   if( openDlg->Execute() ){
      OpenFile(openDlg->FileName);
    }
}
//---------------------------------------------------------------------------
void TfrmEditGrid::OpenFile(String name){
      ifstream in(name.c_str());
      if(in){
         // ������� ������������ ����
         mnuNewClick(NULL);
      	// ��������� �������
         int n;
         in >> n;
         for(int i=0; i<n; i++)  {
            // ������ ���
            char cname[1024];
            in >> cname; String cName = cname;
            // ��������� ���������, ����������� �� ������� ������
            CreateControl( cName, "" );
            // ��������� ��������� ����������
            Objects.back()->Read( in );
            }
         // ��������� ���������� � ������
         LinkLines.Read( in );
      	// �������� ��������� ����
         if( Name.Length() < 32 )
            Caption = name;
         else
            Caption = name.SubString(Name.Length()-32, 32);
         }
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditGrid::mnuNewClick(TObject *Sender)
{
   // ������� ������������ ����
   for(int i=0, n=Objects.size(); i<n; i++)
      delete Objects[i];
   Objects.clear();
   LinkLines.Clear();
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditGrid::FormClose(TObject *Sender,
      TCloseAction &Action)
{
      // �������� ��������� � ������
      if(LinkLines.Count()==0 && Objects.size()==1){
         err(0, "EditGrid: ������� ������ ��������� ����� 1 ����������");
         Action = caNone;
         }
      else if( Objects.size() == 0 || LinkLines.Count()!=0){}
      else{
   	   err(0, "EditGrid: �� �������� ������� �������, �.�. �� ������� �� ����� ����� ����� ����������");
         Action = caNone;
         }
      // �������� ������������ ���� � ���� �������
      for(int i=0,n=Objects.size(); i<n; i++){
         String name = Objects[i]->Obj()->Name;
         for(int j=i+1; j<n; j++)
            if(Objects[j]->Obj()->Name == name){
               err(0, "EditGrid: ������ ���� ������� ������ ����� ���������� �����");
               Action = caNone;
         		}
      	}
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditGrid::FormShow(TObject *Sender)
{
  if(GridCtrl){
  	// ���������� ������� ��� ������ � �����
         for(int k=0, o=Objects.size(); k<o; k++){
         	// �������� ������ ����� ����������
         	PBase *pb = dynamic_cast<PBase*>( Objects[k] );
         	if( pb ){
         		for(int h = 0;h < pb->ComponentCount; h++)
            		if(pb->Components[h]->ClassNameIs("PPinControl")){
               		// ���� ���������-����� ��������� �� ������ ��� �����, �� ��������
                  	// ����������� �������������� ����� ����������
                  	((PPinControl*)pb->Components[h])->Pen->Color = clGray;
                  	}
               }
            }
   // ������ ������� ������ �� ����
   for(int i=0, n=GridCtrl->Obj()->Out.Count(); i<n; i++)
      for(int j=0, m=GridCtrl->Obj()->Out[i].Count; j<m; j++){
      	// ���� ����� ����� ������
         PPin *pin = &GridCtrl->Obj()->Out[i][j];
         for(int k=0, o=Objects.size(); k<o; k++){
         	// �������� ������ ����� ����������
         	PBase *pb = dynamic_cast<PBase*>( Objects[k] );
         	if( pb ){
         		for(int h = 0;h < pb->ComponentCount; h++)
            		if(pb->Components[h]->ClassNameIs("PPinControl")){
               		// ���� ���������-����� ��������� �� ������ ��� �����, �� ��������
                  	// ����������� �������������� ����� ����������
                     PPinControl *pctrl = (PPinControl*)pb->Components[h];
               		if	(pctrl->Pin() == pin )
                  		pctrl->Pen->Color = clRed;
               		}
            	}
            }
   		}
	}
}
//---------------------------------------------------------------------------
