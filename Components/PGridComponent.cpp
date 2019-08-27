//---------------------------------------------------------------------------
#pragma hdrstop

#include "PGridComponent.h"
//---------------------------------------------------------------------------
#include "EditGrid.h"
typedef  TfrmEditGrid      GridEditForm;   // ����� ����� ������� ����� ������� � �������� ����� �������
//---------------------------------------------------------------------------
__fastcall PGridComponent::PGridComponent(TComponent* Owner) :
   PBase(Owner),
   LinkLines(this){
   LinkLines.Parent = NULL;
      // ������������� ����
   TMenuItem *miGrid = new TMenuItem(Menu);
   miGrid->Name = "EditGrid";
   miGrid->Caption = "������������� ����";
   miGrid->OnClick = MenuItemClick;
   Menu->Items->Add(miGrid);
}
//---------------------------------------------------------------------------
void PGridComponent::Save(ostream &stream){		// ��������� ������� � ������
   // �������� ������� ���������
   sire::Save(stream);
   // ��������� ����������
   int n=Objects.size();
   stream << n << " ";
   for(int i=0; i<n; i++){
      stream << StringForSave( &(String)Objects[i]->ClassName() ) << " ";
      Objects[i]->Save(stream);
      }
   //  �������� ���������� � ������
   LinkLines.Save( stream );
   // �������� ������ �������
   String Outs = ((PSubGrid*)_Obj)->getOuts();
   stream << StringForSave( &Outs ) << " ";
}
//---------------------------------------------------------------------------
void PGridComponent::Read(istream &stream){		// ��������� ������� � ������
   // ������� ������������ ����
   Objects.clear();
   LinkLines.Clear();
   // ������ ������� ���������
   sire::Read(stream);
   PSubGrid *grid = dynamic_cast<PSubGrid*>(_Obj);
   if(!grid){err(0, "PGridComponent: ��� �������� �� ������ ���������");}
   // ������ ����������
   int n=0;
   stream >> n;
   for(int i=0; i<n; i++){
      // ������ ��� ����������
      char cname[1024];
      stream >> cname; String cName = cname;
      PBase *ctrl = AddControl(cName, "");// ��������� ���������, ����������� �� ������� ������
      ctrl->Read(stream);                 // ��������� ��������� ����� ����������
      }
   //  ������ ���������� � ������
   LinkLines.Read( stream );

   // �������� ��������� � ������ � ������������ �� �������
   // ���������� � ������ ��� �������� � ������
   Pins_Link Link;
	for(int i=0, n=LinkLines.Count(); i<n; i++)
      Link.add(*LinkLines[i].First->Pin(), *LinkLines[i].Second->Pin());
   // ������ ��� ������� ������ ���� ����
   vector<PSubGrid*> sgList;
   PSubGrid *sg;
   for(int i=0, n=Objects.size(); i<n; i++)
   	if(Objects[i]->Obj()->ClassName() == "PSubGrid")
      	sgList.push_back( (PSubGrid*)Objects[i]->Obj() );
   grid->Change( Link, sgList );
   // �������� ������ �������
   char outs[1024];
   stream >> outs; String Outs = outs;
   grid->setOuts( Outs );
   // �������� ����������-������
   createPins();
}
//---------------------------------------------------------------------------
PBase* PGridComponent::AddControl(String Class, String ModelClass){
      // �������� ���������-������
      PBase *control = NULL;
      if(Class == "PGridComponent")       control = new PGridComponent( this );
      else                    	         control = new PBase( this );
      control->ObjType( ModelClass );        // ����������� ��������� � ������� ����
      Objects.push_back( control );    		// ��������� � ������ �����������
      return control;
}
//---------------------------------------------------------------------------
void __fastcall PGridComponent::MenuItemClick(TObject *Sender){         // ���������� ��� ����� �� ������� ����
   // ������������ ������� �������� ������������ � ������� ������
   sire::MenuItemClick( Sender );
   // �������� ����������� ��� ����������-����
   String itemName =  ((TMenuItem*)Sender)->Name;
   if(itemName == "EditGrid"){
      PSubGrid *grid = dynamic_cast<PSubGrid*>(_Obj);
      if(!grid) return;
      // �������� ��������� ���� ��������������
      GridEditForm *frmGE = new GridEditForm(this);
      // ���������� ���� �����������, ��������� �� �����, ����� ���� ���������
      frmGE->ComponentOwner = this;
      // ���������� ����������� ��������� ����
      frmGE->GridCtrl = this;
      // ����������� �� ��� ����������
      frmGE->LinkLines.Clear();
      for(int i=0, n=Objects.size(); i<n; i++)
      	frmGE->AddControl( Objects[i] );      // ���������� ������ ���� ���������� �� �����
      // ����� ����� ��������
      frmGE->LinkLines = LinkLines;
      frmGE->LinkLines.Parent = frmGE;    // ��������� ������ ���� ��������� �� �����
      // ���������� �����
      frmGE->Caption = _Obj->Name;
      frmGE->ShowModal();
      // �������� ��������� � ����������� � ������
      Objects = frmGE->Objects;
      LinkLines = frmGE->LinkLines;
      // �������� ����� ������ ���� �� ��������� �������
      String oldOuts = grid->getOuts();
      // �������� ��������� � ������
      if(LinkLines.Count()==0 && Objects.size()==1)
         err(0, "PGridComponent: ������� ������ ��������� ����� 1 ����������");
      else if( Objects.size() != 0 && LinkLines.Count()!=0){
   	   // ��������� ������
         Pins_Link Link;
         Link.clear();
		   for(int i=0, n=LinkLines.Count(); i<n; i++)
   		   Link.add(*LinkLines[i].First->Pin(), *LinkLines[i].Second->Pin());
         // ������ ��� ������� ������ ���� ����
         vector<PSubGrid*> sgList;
         PSubGrid *sg;
         for(int i=0, n=Objects.size(); i<n; i++)
         	if(Objects[i]->Obj()->ClassName() == "PSubGrid")
            	sgList.push_back( (PSubGrid*)Objects[i]->Obj() );
         // ������ ����
         grid->Change( Link, sgList );
         }
      else if( Objects.size() == 0 ){
   	   // ������� ������ ����
         Pins_Link Link;
         vector<PSubGrid*> sgList;
         grid->Change( Link, sgList );
      	}
      else
   	   err(0, "PGridComponent: �� �������� ������� �������, �.�. �� ������� �� ����� ����� ����� ����������");
      // ����� ������ ���������� �� ����
      String Outs = InputBox("������ �� ����", "� ������� \"������1.X; ������2.X1; [...]\"", grid->getOuts() );
      grid->setOuts( Outs );
      // ��� ��������� ���� ������ ����� ������������ - ����� ���� ����������� ����������-������
      if( grid->getOuts() != oldOuts ){
      	// ����������� ������
      	createPins();
         }
      // ����� ���������� �� ���������
      for(int i=0, n=Objects.size(); i<n; i++) Objects[i]->Parent = NULL;
      LinkLines.Parent = NULL;
      // Owner ����������� �������� �� ����� �������� ���� ���������, � Parent==NULL, ������� �������� ����� �� �������� � �������� ��������� �����������
      delete frmGE;
      Repaint();  // ���� ��������� ��������� �������� ����, �� ���������
      // �������-����������� �� ��������� ��������
      if(OnChange) OnChange(this);
   	}
}
#pragma package(smart_init)
