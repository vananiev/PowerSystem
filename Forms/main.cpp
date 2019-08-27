//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "main.h"
#include "Scope.h"
#include "ObectsLibrary.h"
#include "kernel.h"			// ����� ��� ������ � ��������
#include <fstream>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
//TfrmMain *frmMain;
//FFT obj((double)T, (unsigned)N);
//Current_Transformer CT1;
/*PSource C;
PLine w1(2);
PConsumer H;
PTransformer_1 T1;
PTransformer_3 T3;
PSingleTransformer T0;
PCurrent_Transformer_1 TA;
PBreaker Q;
//---------------------------------------------------------------------------
PType fa(PElement* obj, double &time){ return 100*sqrt(2)*(exp(-time/0.1)+sin(w*time));}            //5773*sqrt(2)*sin(w*time);    //93897    //8573
PType fb(PElement* obj, double &time){ return 5773*sqrt(2)*sin(w*time-2*M_PI/3);}
PType fc(PElement* obj, double &time){ return 5773*sqrt(2)*sin(w*time+2*M_PI/3);}       //9859     //900
PType I_F(PType &F){  	return 1e7*pow(F,7)+2*F;} // 3e18*pow(F,9)+10*F
PType I_F_TA(PType &F){ return 2e21*pow(F,7)+1e4*F;} // 3e18*pow(F,9)+10*F
PType U_I(PType &I){  	return pow(I,1.7);} */
//---------------------------------------------------------------------------
void err(unsigned num, String text){
   static unsigned cnt=0;
   cnt++;
   //if(cnt>3)
   //   frmMain->Timer->Enabled = false;
	ShowMessage(text);
}
//---------------------------------------------------------------------------
__fastcall TfrmMain::TfrmMain(TComponent* Owner)
	: TForm(Owner), LinkLines(this)
{
   // �������������� �������
   grid = NULL;
   LinkLines.Clear();   
   Objects.clear();
	// ��������� ����������� ������
   LinkLines.Parent = this;
   LinkLines.OnDelete = LinkDelete;
   LinkLines.OnAdd = LinkAdd;
   // ���� ������� ����� � ���� �������� �������� - ��������� ����
   if(Name == "frmMain") if( ParamCount()>0 )  OpenFile( ParamStr(1) );

   // �������� ����������� ����������� (����� ���� ������������� ��-���)
   ComponentOwner = this;
   // �������� ��������
   //DoubleBuffered = true;

   /*/ ��������� ����������
   C.Name = "G";
   C.Ea = fa;
   C.Eb = fb;
   C.Ec = fc;
   C.R_[_A_] = 1.0;
   C.R_[_N_] = 1e-6;
   C.accept();

   // ��������� �����
   w1.length = 10;
   w1.R0[_A_] = 0.5;
   w1.R0[_B_] = 0.5;
   w1.R0[_C_] = 0.5;
   w1.L0[_A_] = 0.667/314 ;
   w1.L0[_B_] = 0.667/314 ;
   w1.L0[_C_] = 0.667/314 ;
   w1.M0[_AB_] = 0.267/314 ;
   w1.M0[_BC_] = 0.267/314 ;
   w1.M0[_CA_] = 0.267/314 ;
   //w1.Gz_( 1e-5 );
   //w1.Cz_ ( 2e-7 );
   w1.accept();

   // ��������� ��������
   H.R_[_A_] = 1322.5/sqrt(2);         //11.0   //1322.5
   H.R_[_B_] = 1322.5/sqrt(2);
   H.R_[_C_] = 1322.5/sqrt(2);
   H.L_[_A_] = 1322.5/314.16/sqrt(2);
   H.L_[_B_] = 1322.5/314.16/sqrt(2);
   H.L_[_C_] = 1322.5/314.16/sqrt(2);
   H.L_[_N_] = 1e-3;   		            // �������� ���������
   H.accept();

   // ��������� ��������������
   T3.Name = "T";
   T3.U1   = 115000;
   T3.U2   = 10500 ;
   T3.S    = 10e6  ;
   T3.dPk  = 58e3  ;
   T3.Uk   = 10.5  ;
   T3.dPx  = 14e3  ;
   T3.Ix   = 0.9   ;
   T3.F_nom= 0.07  ;
   T3.mType = Three_Rod;   //Armored
   T3.Winding_X1 = wGroundedStar;
   T3.Winding_X2 = wTriangle;
   T3.mCurve.func( I_F );
   T3.accept();
   // ������������� ����
   TA.R1 = 0;
   TA.L1 = 0;
   TA.R2 =0.96;    // ����� ��������
   TA.L2 =2.29e-3;
   TA.n1 = 1;
   TA.n2 = 20;
   TA.I1 = 100;
   TA.S = 30;
   TA.K = 10;  // ������� ����������� (����� ��������)
   TA.d = 1;   // ������� �����������
   //TA.mCurve.func( I_F_TA );
   TA.accept();
   // �����������
   Q.Rmax = 1e6;
   Q.accept(); */

	/*PBreaker *G = new PBreaker();
   grid = new PGrid(G);
   grid->Make_Equation();
   int i=0;
   while (i<1e3){
      grid->calc();
      TIME += smp;
      i++;
   	}
   btnStart->Enabled = true;*/

   /*
   �������������� �����
   double i[3];
   CT1.get(i);
   vector y = obj.calc(i[0]);
   Out->Text = y.mod;
   static int cnt;
   cnt++;
   lbl->Caption = cnt;*/
   /*/������ � ���������
   const unsigned n=3, m=3;
   Matrix <double> A(n,m), B(n,m), S(m,n);
   double C[n][m], D[n][m], E[m][n];
   for(unsigned i=0; i<n; i++)
   	for(unsigned  j=0; j<m; j++){
   		C[i][j] = 1.0*random(10);
         A[i][j] = C[i][j];
         }
   for(unsigned i=0; i<n; i++)
   	for(unsigned  j=0; j<m; j++){
   		D[i][j] = 1.0*random(10);
         B[i][j] = D[i][j];
         }
   //double det = B.det();
   S = B.inverse();
   S = S*B;
   for(unsigned i=0; i<m; i++)
   	for(unsigned  j=0; j<n; j++){
         E[i][j] = S[i][j];
         }*/
   // ������ � �������
   //TPL w1[1];
   //PGrid grid(w1);
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::mnuScopeClick(TObject *Sender)
{
	// ���� ������� ��������
   if(grid && btnStart->Enabled){
      frmScope.push_back(new TfrmScope(this));
      frmScope[frmScope.size()-1]->obj = grid;
      frmScope[frmScope.size()-1]->Show();
   }else
      err(1,"PowerSystem: ����� ������� ������������ ��������� ������ ����");
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::btnStartClick(TObject *Sender)
{
   Timer->Enabled = !Timer->Enabled;
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::btnPauseClick(TObject *Sender)
{
   Timer->Enabled = false;
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::TimerTimer(TObject *Sender)
{
   for(unsigned i=0;i<50;i++){
      TIME += smp;
      grid->calc();
      }
   lblTime->Caption=FloatToStr(TIME);
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::FormDestroy(TObject *Sender)
{
   // Owner � grid �� ������������, � ������ �� ��������� ������������� � model.cpp
   // � ��������� PObjectList
   //delete grid;  grid = NULL;
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::btnStopClick(TObject *Sender)
{
   Timer->Enabled=false;
   TIME = 0;
   lblTime->Caption=FloatToStr(TIME);
}
//---------------------------------------------------------------------------
// �������� ������������
void TfrmMain::CloseScope(){
   /*if(frmScope.size()>0){
      err(_NOTICE_, "PowerSystem: ����� �������� ������� �������� ��� ������������");
      return;
   	}*/
   for(int i=0, n=frmScope.size(); i<n; i++){
      frmScope[i]->Timer->Enabled = false;
      delete frmScope[i]; 	frmScope[i] = NULL;
   	}
   frmScope.clear();
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::BuildGridClick(TObject *Sender)
{
	Timer->Enabled = false;  // ������������� �������
	if(Objects.size()==0){
   	err(_NOTICE_,"PowerSystem: � ���� ��� �� ������ ������� ��� ������");
   	return;
      }
   // ��������� ��� �������� ������������
   CloseScope();

   static bool cntBuild = 0;   // ����� ������ ����
   //if(cntBuild!=0)
   //   err(_NOTICE_,"PowerSystem: ���������� ����. �������� ����������� � ����� �������� �� ��������������� ��������.");
   // ������� ������ ����
   delete grid;  grid = NULL;
	// ������� ����� accept() �� ���� ��������
   // ��� ���������� ������ �� ���������� ���������,
   // �.�. ��� ����� �������� ���������� � ���������� ������
   Pins_Link Link;
   for(int i=0, n=Objects.size(); i<n; i++){
   	Objects[i]->Obj()->accept();
      // ������ ��� ������� � ������� � ��� ��� ����������
      // � �������� accept() � ��������� �������
      if(   (String)Objects[i]->ClassName() == "PGridComponent" )
         SubGrid_Accept( (PGridComponent*)Objects[i], Link);       // ����� ����� �������� � ��������� ������� �� ����� (� ������ ���� ��������)
                                                                   // Link.add( Objects[i]->Obg()->Links ) - ��� ������ ��������, �� ���� �� �������������
                                                                   // ������� �� ���������, ������������ � �������.
      }
   // ��������� ������
	for(int i=0, n=LinkLines.Count(); i<n; i++)
   	Link.add(*LinkLines[i].First->Pin(), *LinkLines[i].Second->Pin());
	// ������� ����
   if(Link.Number()==0 && Objects.size()==1 && Objects[0]->Obj()->ClassName() != "PSubGrid")
      grid = new PGrid((PUnit *)Objects[0]->Obj());
   else if( Objects.size() != 0 && Link.Number()!=0 ){
   	grid = new PGrid(Link);
      }
   else{
   	err(_ERROR_, "PowerSystem: �� �������� ������� ����, �.�. �� ������� �� ����� ����� ����� ����������");
      return;
      }
   if(  grid->Make_Equation() ){
      // ��������� ������������ ��� ������
      btnStart->Enabled = true;
      btnStart->SetFocus();
      mnuScope->Enabled = true;
      // ����������� ����� ������ ���� (������ ����������)
      cntBuild++;
      }
   else{
      // ���� ������ ��� ������ ����
      delete grid; grid = NULL;
      btnStart->Enabled = false;
      mnuScope->Enabled = false;
      }
}
//---------------------------------------------------------------------------
// ���������� ������� �������
void __fastcall TfrmMain::SubGrid_Accept(PGridComponent *ctrl, Pins_Link &Link){
   Link.add( ctrl->Obj()->Internal_Links );
   // ������ ���� ������ ���� ����
   for(int i=0, n=ctrl->Objects.size();i < n; i++){
      // �������� accept()
      PElement *obj = ctrl->Objects[i]->Obj();
      if(obj)
         obj->accept();
      PGridComponent *pb = dynamic_cast<PGridComponent*>( ctrl->Objects[i] );
      if( pb )
         SubGrid_Accept( pb , Link);
      }
}
//---------------------------------------------------------------------------
/* ������� ���������� ��� ��������� �������� */
void __fastcall TfrmMain::UnitChange(TObject *Sender){
   // ��� ��������� ������� �������, ����� ��������� ������ ���� ����������� ����
   // ����� ���������� ������ ������ ������������
   Timer->Enabled = false;
   btnStart->Enabled = false;
   mnuScope->Enabled = false;
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::UnitDelete(TObject *Sender){
   // ��������� ��� �������� ������������
   CloseScope();
   // ��������� ������� � �������� �����������
   Timer->Enabled = false;
   btnStart->Enabled = false;
   mnuScope->Enabled = false;
   // ������� ��������� �� ������
   /*for(int i=0, n=Objects.size(); i<n; i++)
   	if(Objects[i] == (PBase*)Sender)
      	Objects.erase(Objects.begin() + i);*/
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
void __fastcall TfrmMain::LinkDelete(PLinkComponent *Sender){
   // ��������� ������� � �������� �����������
   Timer->Enabled = false;
   btnStart->Enabled = false;
   mnuScope->Enabled = false;
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::LinkAdd(PLinkComponent *Sender){	// ���������� ��� ��������� ������
   // ��������� ������� � �������� �����������
   Timer->Enabled = false;
   btnStart->Enabled = false;
   mnuScope->Enabled = false;
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::PinMouseDown(TObject* Sender, TMouseButton Button, Classes::TShiftState Shift, int X, int Y){
   if(Shift.Contains(ssLeft)&&Shift.Contains(ssShift))   // ��� ����� ����
      // ��������� �����������
      if( grid && mnuScope->Enabled ){
         frmScope.push_back(new TfrmScope(this));
         frmScope[frmScope.size()-1]->obj = grid;
         String name = ((PPinControl*)Sender)->Pin()->Obj()->Name + ":" + IntToStr(((PPinControl*)Sender)->Pin()->Node());
         String name2 = ((PPinControl*)Sender)->Pin()->Obj()->Name + ":" + IntToStr( (TObject*)((PPinControl*)Sender) -> Pin() ->Obj() -> Grounded_Node );
         // ��������� ��������� ���� ������������ ����������� ����
         frmScope[frmScope.size()-1]->cmbPot1->AddItem(name, NULL);
         frmScope[frmScope.size()-1]->cmbPot1->ItemIndex = 0;
         frmScope[frmScope.size()-1]->cmbPot2->AddItem(name2, NULL);
         frmScope[frmScope.size()-1]->cmbPot2->ItemIndex = 0;
         frmScope[frmScope.size()-1]->rbVoltage->Checked = true;
         frmScope[frmScope.size()-1]->Show();
      }else
         err(1,"PowerSystem: ����� ������� ������������ ��������� ������ ����");
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::PinDragDrop (System::TObject* Sender, System::TObject* Source, int X, int Y){
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
void __fastcall TfrmMain::FormDragOver(TObject *Sender, TObject *Source,
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
void __fastcall TfrmMain::FormPaint(TObject *Sender)
{
	// ������ ����� PaintLines()
   LinkLines.Paint();
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::FormDragDrop(TObject *Sender, TObject *Source,
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
void TfrmMain::AddObject(String TypeName, int X, int Y){    // ������ TypeName - ���������� �� frmObjLib->tvList
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
      err(_WARNING_, "PowerSystem: �� ���������� �������� ��� �������� ���������� ���������� �� ���������� ��������");
   	}
}
//---------------------------------------------------------------------------
void TfrmMain::CreateControl(String Class, String ModelClass){
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
void TfrmMain::AddControl(PBase *control){
   control->Parent = this;
	control->OnChange = UnitChange;
   control->OnPinMouseDown = PinMouseDown;
   control->OnPinDragDrop = PinDragDrop;
   control->OnDelete = UnitDelete;
   Objects.push_back( control );    		// ��������� � ������ �����������
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::mnuObjLibClick(TObject *Sender)
{
      // ����� ���������� ��������
      TfrmObjLib *frmOL = new TfrmObjLib(this);
      frmOL->Show();
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::FormMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
   // �������� ������ ������
	if(Button == mbRight){
      // ����� �� ��������� �� ����� �����?
      LinkLines.ShowMenuFor( X, Y );
   	}
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::mnuSaveClick(TObject *Sender)
{
   // ������������� �������
   Timer->Enabled = false;
   /*/ �������� ���� ���� ��������
   for(int i=0, n=Objects.size(); i<n; i++){
   	String testName = Objects[i]->Obj()->Name;
   	for(int j=i+1; j<n; j++)
   		if( testName == Objects[j]->Obj()->Name ){
         	err(_ERROR_, "PoserSystem: � �������� ���� �� ������ ���� ���� �������� � ����������� ������� ("+ testName+"). ���������� ��������.");
         	return;
         }
      } */
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
PElement* TfrmMain::FindObj(String Name){
   for(int i=0, n=Objects.size(); i<n; i++)
      if( Name == Objects[i]->Obj()->Name )
         return Objects[i]->Obj();
   return NULL;
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::mnuOpenClick(TObject *Sender)
{
   // ��������� ��� �������� ������������
   CloseScope();
   // ������������� �������
   Timer->Enabled = false;
   btnStart->Enabled = false;
   mnuScope->Enabled = false;
   // ������� ������ � ���������
   openDlg->Filter = ".psf|*.psf|����� ����|*.*";
   if( openDlg->Execute() ){
      OpenFile(openDlg->FileName);
    }
}
//---------------------------------------------------------------------------
void TfrmMain::OpenFile(String name){
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
void __fastcall TfrmMain::mnuNewClick(TObject *Sender)
{
   // ��������� ��� �������� ������������
   CloseScope();
	//�������� �����
   Timer->Enabled = false;
   btnStart->Enabled = false;
   mnuScope->Enabled = false;
   TIME = 0;
   lblTime->Caption = 0;
   // ������� ������������ ����
   for(int i=0, n=Objects.size(); i<n; i++)
      delete Objects[i];
   Objects.clear();
   LinkLines.Clear();
   // ������� ����
   delete grid;  grid = NULL;
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::mnuAboutClick(TObject *Sender)
{
   MessageBoxA(NULL,
					"Power System	\n������:  0.1 ( Violet )	\n������:  ������� �., ������ �. (�)2012",
               "About",
               MB_OK | MB_ICONINFORMATION);

}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::FormResize(TObject *Sender)
{
   //Bevel->Left     = Width - 161;
	btnStart->Left = Width - 89;
   btnPause->Left  = Width - 65;
   btnStop->Left   = Width - 41;
   lblTime->Left   = Width - 153;
   lblSec->Left    = Width - 113;
}

