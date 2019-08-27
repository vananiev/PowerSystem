//---------------------------------------------------------------------------

#include <vcl.h>

#pragma hdrstop

#include <vector>
#include "PBase.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
// ��������� ����������
#include "frmProp.h"
typedef  TfrmProperty  HelpForm;       // ����� ����� ������� ����� ������� � �������� ����� �������
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//
//---------------------------------------------------------------------------
/*namespace Pbase
{
   void __fastcall PACKAGE Register()
   {
       TComponentClass classes[1] = {__classid(PBase)};
       RegisterComponents("Samples", classes, 0);
   }
} */
//---------------------------------------------------------------------------
static inline void ValidCtrCheck(PBase *)
{
   new PBase(NULL);
}
//---------------------------------------------------------------------------
__fastcall PBase::PBase(TComponent* Owner)
   : TGraphicControl(Owner)
{
	//ControlAtPos 	���������� ������� ����������, ������������ � ��������� �������. 	TWinControl
   Parent = dynamic_cast<TWinControl*>(Owner);
   Width=70;
   Height=70;
   // ��������� ������
   Canvas->Brush->Color = clWhite;   // Brush - �������
   Canvas->Pen->Color = clGray;      // Pen - ��������� �����
   Canvas->Font->Size = 8;
   Canvas->Font->Color = clGray;
   Canvas->Font->Name="Times New Roman";
   // ������ ���������� ��� ����������
   int PBaseNameId = 0;
   String name = (String)ClassName() + IntToStr( ++PBaseNameId );
   while(  Owner->FindComponent( name )  )
      name = (String)ClassName() + IntToStr( ++PBaseNameId );
   Name = name;
   // ���������� ����������
   //Paint();
   // ������ ��������������
   DragKind = dkDrag;
   DragMode = dmManual;
   // � ������ �������������� ��������������� � true
   _Drag = false;
   // ������������ ����
   Menu = new TPopupMenu(this);
      // �������
   TMenuItem *miRotate = new TMenuItem(Menu);
   miRotate->Name = "Rotate";
   miRotate->Caption = "���������";
   miRotate->OnClick = MenuItemClick;
   Menu->Items->Add(miRotate);
      // �������
   TMenuItem *miDel = new TMenuItem(Menu);
   miDel->Name = "Delete";
   miDel->Caption = "�������";
   miDel->OnClick = MenuItemClick;
   Menu->Items->Add(miDel);
      // ��������
   TMenuItem *miProp = new TMenuItem(Menu);
   miProp->Name = "Properties";
   miProp->Caption = "��������";
   miProp->OnClick = MenuItemClick;
   Menu->Items->Add(miProp);
   // ������� ������ ��������������� � �����������
   _Obj = NULL;
   // ���������� ��� ����� ������ ������� ������������
   _LEFT_ = 0;
   _RIGHT_= 1;
   _BOTTOM_=2;
   _TOP_  = 3;
   // ������� �� ��������� ���������
   OnChange = NULL;
   // ������� ��������� ��� ����� �� ������
   OnPinMouseDown = NULL;
   // ��������� � ����� �������������� �� ������
   OnPinDragDrop = NULL;
   OnPinDragOver = NULL;
   // ���������� ��� ��������
   OnDelete = NULL;
    // ���������� ����� - ��������� ������� �������� �����������
   //Canvas->Brush->Style = bsClear;
}
//---------------------------------------------------------------------------
__fastcall PBase::~PBase(){
   //delete Menu; ��������� �� ���������
   pList.clear();	// ����� ��� �������� ���������� ����� SetParent
   // ������ ���� ������
   if( _Obj->Owner == this ){
      delete _Obj;  _Obj = NULL;
      }
}
//---------------------------------------------------------------------------
void __fastcall PBase::Paint(void){
	//SetBkMode(Canvas->Handle, TRANSPARENT );     // Handle - ��� ������������� � windows
   															//Canvas.Handle == �������� �����������
	//BitBlt(((TForm*)Parent)->Canvas->Handle, 200, 200, Width, Height, ((TForm*)Parent)->Canvas->Handle, Left, Top, SRCCOPY);
   if(Parent == NULL) return;
   // ������ ����
   Canvas->Rectangle(10,10,Width-10,Height-10);
   // ���
   String name;
   if(_Obj)
      name = _Obj->Name;
   else
      name = Name;
   Canvas->TextOutA(Width/2 - 3.0*name.Length(), (Height-20)/2+4,name);
   // ������������� ������
   int outWidth;
   int outStep;
   int pins_cnt;
   if(_Obj){
      int n = _Obj->Out.Count();  // ����� ����� �������
      int fontSize = Canvas->Font->Size;
      Canvas->Font->Size = 6;
      if(n > _TOP_){
         // ������ ������
         outWidth = Width - 24;
         pins_cnt = _Obj->Out[_TOP_].Count;
         outStep = (float)outWidth/(pins_cnt-1);
         for(int i=0; i<pins_cnt;i++){
            Canvas->MoveTo(12+i*outStep, 2);
            Canvas->LineTo(12+i*outStep, 10);
            // ����������� �����
            PPinControl *pc = PinControl(&_Obj->Out[_TOP_][i]);
            if( pc){
               pc->Left = XtoParent(12+i*outStep - pc->Width/2);
               pc->Top  = YtoParent(0 - pc->Height/2);
               }
            }
         Canvas->TextOutA(Width/2,11,_Obj->Out[_TOP_].Name);
         }
      if(n > _BOTTOM_){
         // ������ �����
         outWidth = Width - 24;
         pins_cnt = _Obj->Out[_BOTTOM_].Count;
         outStep = (float)outWidth/(pins_cnt-1);
         for(int i=0; i<pins_cnt;i++){
            Canvas->MoveTo(12+i*outStep, Height-10);
            Canvas->LineTo(12+i*outStep, Height-2);
            // ����������� �����
            PPinControl *pc = PinControl(&_Obj->Out[_BOTTOM_][i]);
            if( pc){
               pc->Left = XtoParent(12+i*outStep - pc->Width/2);
               pc->Top  = YtoParent(Height - pc->Height/2);
               }
            }
         Canvas->TextOutA(Width/2,Height-22,_Obj->Out[_BOTTOM_].Name);
         }
      if(n > _RIGHT_){
         // ������ ������
         outWidth = Height - 24;
         pins_cnt = _Obj->Out[_RIGHT_].Count;
         outStep = (float)outWidth/(pins_cnt-1);
         for(int i=0; i<pins_cnt;i++){
            Canvas->MoveTo(Width-10, 12+i*outStep);
            Canvas->LineTo(Width-2,  12+i*outStep);
            // ����������� �����
            PPinControl *pc = PinControl(&_Obj->Out[_RIGHT_][i]);
            if( pc){
               pc->Left = XtoParent(Width - pc->Width/2);
               pc->Top  = YtoParent(12+i*outStep - pc->Height/2);
               }
            }
         Canvas->TextOutA(Width-12-6*_Obj->Out[_RIGHT_].Name.Length(),20,_Obj->Out[_RIGHT_].Name);
         }
      if(n > _LEFT_){
         // ������ �����
         outWidth = Height - 24;
         pins_cnt = _Obj->Out[_LEFT_].Count;
         outStep = (float)outWidth/(pins_cnt-1);
         for(int i=0; i<pins_cnt;i++){
            Canvas->MoveTo(2, 12+i*outStep);
            Canvas->LineTo(10,12+i*outStep);
            // ����������� �����
            PPinControl *pc = PinControl(&_Obj->Out[_LEFT_][i]);
            if( pc){
               pc->Left = XtoParent(0 - pc->Width/2);
               pc->Top = YtoParent(12+i*outStep - pc->Height/2);
               }
            }
         Canvas->TextOutA(12,20,_Obj->Out[_LEFT_].Name);
         }
      Canvas->Font->Size = fontSize;
      }
}
//---------------------------------------------------------------------------
int __fastcall PBase::XtoParent(int x){return x+Left;}
int __fastcall PBase::YtoParent(int y){return y+Top;}
//---------------------------------------------------------------------------
void __fastcall PBase::MouseMove(Classes::TShiftState Shift, int X, int Y){
   // ������������ ��������������
   if(_Drag){
      Left += X-MouseDownPos_X;
      Top += Y-MouseDownPos_Y;
      }
   if(OnMouseMove) OnMouseMove(this, Shift, X, Y);
}
//---------------------------------------------------------------------------
void __fastcall PBase::MouseDown(TMouseButton Button, Classes::TShiftState Shift, int X, int Y){
   if(Button==mbLeft){
      // ������������� �������
      _Drag = true;
      MouseDownPos_X = X;
      MouseDownPos_Y = Y;
      }
   else if (Button==mbRight){
      // ��������� ���������� ������������ ����
      TPoint p, sp;
      p.x = X; p.y = Y;
      sp = ClientToScreen(p);
      /*/ ���� ��� ����
      if(dynamic_cast<PSubGrid*>(_Obj))
         Menu->Items->Find("������������� ����")->Visible = true;
      else
         Menu->Items->Find("������������� ����")->Visible = false;*/
      // ������� ����
      Menu->Popup(sp.x,sp.y);
      }
   // �������� �������������� ������� ����������
   if(OnMouseDown)  OnMouseDown(this, Button, Shift, X, Y);
}
//---------------------------------------------------------------------------
void __fastcall PBase::MouseUp(TMouseButton Button, Classes::TShiftState Shift, int X, int Y){
   _Drag = false;
   if(OnMouseUp) OnMouseUp(this, Button, Shift, X, Y);
}
//---------------------------------------------------------------------------
void PBase::createPins(){
   // ������� ������ ��������� �������
   for(int i=0, n=pList.size(); i<n; i++)  	delete pList[i]; // ��� ���� ���������� ��������� ������ ������ �� �������� ��� �������� ������
   pList.clear();
   // ������� ����� �������
   for(int i=0, n=_Obj->Out.Count(); i<n; i++)
      for(int j=0, m=_Obj->Out[i].Count; j<m; j++){
         pList.push_back(new PPinControl(this));   // ���������� ������ ���� ���� ���������, ����� ���������-����� �������� �� ������ � ���� ����������� 
         pList.back()->Name = Name + "_" +IntToStr(i) + "_" +IntToStr(j); // ��� ���������� ������
         pList.back()->Parent = Parent;           // � ��������� ��� �� ��� ������
         pList.back()->Pin( &_Obj->Out[i][j]);
         pList.back()->OnMouseDown = PinMouseDown;
         pList.back()->OnDragDrop = PinDragDrop;
         pList.back()->OnDragOver = PinDragOver;
         pList.back()->OnMouseDown = PinMouseDown;
         }
}
//---------------------------------------------------------------------------
void __fastcall PBase::SetParent(TWinControl* AParent){
   inherited::SetParent(AParent);
   for(int i=0, n=pList.size(); i<n; i++)
      pList[i]->Parent = AParent;
}
//---------------------------------------------------------------------------
void __fastcall PBase::MenuItemClick(TObject *Sender){
   String itemName =  ((TMenuItem*)Sender)->Name;
   if(itemName == "Properties"){
      // ���� ��������
      HelpForm *frmP = new HelpForm(this);
      frmP->Obj = this;
      frmP->ShowModal();
      delete frmP;
      Repaint();  // ���� ��������� ��������� �������� ����, �� ���������
      // �������-����������� �� ��������� ��������
      if(OnChange) OnChange(this);
      }
   else if(itemName == "Rotate"){
      // �������
      int tmp = _LEFT_;
      _LEFT_=_BOTTOM_;
      _BOTTOM_=_RIGHT_;
      _RIGHT_=_TOP_;
      _TOP_=tmp;
      Repaint();
      }
   else if(itemName == "Delete"){
      // ��������
      if(OnDelete) OnDelete(this);
      // ������ ������ � ������
      delete this;
      }
}
//---------------------------------------------------------------------------
void PBase::Obj(PElement *obj){
   if(_Obj!=NULL){
      err(0, "PBase: ��������� ��� ����������� � ������ " + _Obj->ClassName() + ". ����� ���������� �� ���������.");
      return;
      }
   if(obj){
      _Obj = obj;
      if(obj->Owner==NULL) _Obj->Owner = this;  // ���� � ������� ��� ��������
   	createPins();  // ������� ����������� ������������� �������
      }
}
//---------------------------------------------------------------------------
PElement * PBase::Obj(){
	return _Obj;
}
//---------------------------------------------------------------------------
 // ������� ������ � ����������� ��� � �����������
PElement* PBase::ObjType(String ClassName){
   Visible = false;
   vector<void*> Params;
   if(ClassName == "PLine"){
      int cells = 1;
      TryStrToInt(InputBox("��������� ���", "������� ����� �������", "1"), cells);
      if( cells < 1 ) cells = 1;
      Params.push_back((void*)(&cells));
      }
   Obj( CreateObj(ClassName, Params) );
   Visible = true;
   return _Obj;
}
//---------------------------------------------------------------------------
PPinControl* PBase::PinControl (PPin *pin){
   for(int k=0, m=pList.size(); k<m; k++)
      if( pList[k]->Pin() == pin)
         return pList[k];
   return NULL;
}
void __fastcall PBase::PinMouseDown(TObject* Sender, TMouseButton Button, Classes::TShiftState Shift, int X, int Y){
   // �������� ������� ���������� �������
   if(OnPinMouseDown)OnPinMouseDown(Sender, Button, Shift, X, Y);
}
//---------------------------------------------------------------------------
void __fastcall PBase::PinDragDrop (System::TObject* Sender, System::TObject* Source, int X, int Y){
   // �������� ������� ���������� �������
   if(OnPinDragDrop) OnPinDragDrop(Sender, Source, X, Y);
}
//---------------------------------------------------------------------------
void __fastcall PBase::PinDragOver(System::TObject* Sender, System::TObject* Source, int X, int Y, TDragState State, bool &Accept){
   // �������� ������� ���������� �������
   if(OnPinDragOver) OnPinDragOver(Sender, Source, X, Y, State, Accept);
}
//---------------------------------------------------------------------------
void PBase::Save(ostream &stream){		// ���������� ���������� ������� � �����
   if(Name==""){
      err(0, "PBase: ��������� ����� ������� ���. �������� ���� ����� �����������.");
   	}
   stream << StringForSave( &Name ) << " ";
   stream << Left << " ";
   stream << Top << " ";
   stream << Width << " ";
   stream << Height << " ";
   stream << _LEFT_ << " ";
   stream << _RIGHT_ << " ";
   stream << _TOP_ << " ";
   stream << _BOTTOM_ << " ";
   stream << true << " ";
   // ��������� ��� ������
   SaveObj( _Obj, stream );
}
//---------------------------------------------------------------------------
/*void PBase::SaveComponentOnly(ostream &stream){		// ���������� ���������� ������� � �����
   if(Name==""){
      err(0, "PBase: ��������� ����� ������� ���. �������� ���� ����� �����������.");
   	}
   stream << StringForSave( &Name ) << " ";
   stream << Left << " ";
   stream << Top << " ";
   stream << Width << " ";
   stream << Height << " ";
   stream << _LEFT_ << " ";
   stream << _RIGHT_ << " ";
   stream << _TOP_ << " ";
   stream << _BOTTOM_ << " ";
   stream << false << " ";
}*/
//---------------------------------------------------------------------------
void PBase::Read(istream &stream){    // �������� ���������� ������� �� ������
   char name[1024];
   stream >> name;
   Name = name;
   int left, top, width, height;
   stream >> left; 	Left = left;
   stream >> top;    Top = top;
   stream >> width;  Width = width;
   stream >> height; Height = height;
   stream >> _LEFT_;
   stream >> _RIGHT_;
   stream >> _TOP_;
   stream >> _BOTTOM_;
   // ���� �� ��������� ������
   bool saveModel = true;
   stream >> saveModel;
   // �������� ������
   if(saveModel) Obj( CreateObj(stream) );
}
//---------------------------------------------------------------------------
/*void PBase::ReadComponentOnly(istream &stream){    // �������� ���������� ������� �� ������
   char name[1024];
   stream >> name;
   Name = name;
   int left, top, width, height;
   stream >> left; 	Left = left;
   stream >> top;    Top = top;
   stream >> width;  Width = width;
   stream >> height; Height = height;
   stream >> _LEFT_;
   stream >> _RIGHT_;
   stream >> _TOP_;
   stream >> _BOTTOM_;
   // ���� �� ��������� ������
   bool saveModel = true;
   stream >> saveModel;   // ������ ���� false
} */
//---------------------------------------------------------------------------
/*
void __fastcall PBase::CreateParams(TCreateParams &Params)
{
    inherited::CreateParams( Params);
    Params.Style   |= WS_CLIPCHILDREN;
    //Params.ExStyle |= WS_EX_TRANSPARENT;
}
/*void __fastcall PBase::WndProc( Messages::TMessage &message)
{
    if( message.Msg == WM_ERASEBKGND)
    {
        DefaultHandler( &message);
        return;
    }
    inherited::WndProc( message);
}*
void __fastcall PBase::SetBounds( int left, int top, int width, int height)
{
    if( left  == this->Left  && top    == this->Top &&
        width == this->Width && height == this->Height)
        return;
    if( this->HandleAllocated()){
        SetWindowPos( this->Handle, 0, left, top, width, height,
                      SWP_NOZORDER + SWP_NOACTIVATE);// + SWP_NOREDRAW);
    		}
    else
        this->UpdateBoundsRect( TRect( left, top, left + width, top + height));
} */

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
__fastcall PPinControl::PPinControl(TComponent* Owner)
           : TShape(Owner){
   _Pin = NULL; // ��� ���������������� ������ ��� ����������
   // ����������� ��������� ������
   Shape = stCircle;
   Width = 4;
   Height = 4;
   Pen->Color = clGray;
   // �� ����� ��������� ������
   //Parent = (TWinControl*)Owner;   �� ������ �������� == ��������
   // ����������� ���������
   ShowHint = false;
   // ��������� Drag & Drop
   DragMode = dmManual;
   // ������� ��������� ��� �����
   OnMouseDown = NULL;
   // ��������� � ����� �������������� �� ������
   OnDragDrop = NULL;
   OnDragOver = NULL;
}
//---------------------------------------------------------------------------
__fastcall PPinControl::~PPinControl(){
   //5 == 5;
   if(OnDelete) OnDelete(this); // ��������� �� ��������
}
//---------------------------------------------------------------------------
void PPinControl::Pin(PPin* pin){
   _Pin = pin;
   PElement* obj = pin->Obj();
   // ���������� ���� � ��������� �� �� ����������� ���������
   Hint = pin->Node();
   for(int i=0, n=obj->Out.Count(); i<n; i++)
      for(int j=0, m=obj->Out[i].Count; j<m; j++)
         if(obj->Out[i][j].Node() == pin->Node()){
            switch(j){
               case _N_:    Hint = "N"; break;       // break; - ��� ����������, �.�. �����
               case _A_:    Hint = "A"; break;
               case _B_:    Hint = "B"; break;
               case _C_:    Hint = "C"; break;
               }
            i=n;j=m; // ��������� �����
            }
   ShowHint = true;
}
//---------------------------------------------------------------------------
PPin* PPinControl::Pin(){
   return _Pin;
}
//---------------------------------------------------------------------------
void __fastcall PPinControl::DragOver(System::TObject* Source, int X, int Y, TDragState State, bool &Accept){
   // ���� ������������� �����
   if(dynamic_cast<PPinControl*>(Source))
      Accept = true; // ��������� ������� ������
   // ������� ����������
   if(OnDragOver) OnDragOver(this, Source, X, Y, State, Accept);
}
//---------------------------------------------------------------------------
void __fastcall PPinControl::DragDrop (System::TObject* Source, int X, int Y){
   // ������� ����������
   if(OnDragDrop) OnDragDrop(this, Source, X, Y);
}
//---------------------------------------------------------------------------
void __fastcall PPinControl::MouseDown(TMouseButton Button, Classes::TShiftState Shift, int X, int Y){
   BeginDrag(true, -1); // �������� �������������� �������
   // ������� ����������
   if(OnMouseDown) OnMouseDown(this, Button, Shift, X, Y);
}
//---------------------------------------------------------------------------
void __fastcall PPinControl::MouseEnter(TMessage& Msg){
   savedWidth = Width; savedHeight = Height; // ���������� �������
   savedColor = Pen->Color;          // ���� ���������
   // ������ ����� ������ �� ������� � �������� ���������
   Pen->Color = clRed;
   Width = 2*savedWidth;
   Height= 2*savedHeight;
}
//---------------------------------------------------------------------------
void __fastcall PPinControl::MouseLeave(TMessage& Msg){
   // ���������� ������� � ���������
   Width = savedWidth;
   Height= savedHeight;
   Pen->Color = savedColor;
   Repaint();
}
//---------------------------------------------------------------------------
PLinkComponent::PLinkComponent(TComponent *owner){
   Owner = owner;
   Menu = new TPopupMenu(NULL);
	// �������
   TMenuItem *miDel = new TMenuItem(Menu);
   miDel->Name = "Delete";
   miDel->Caption = "������� �����";
   miDel->OnClick = MenuItemClick;
   Menu->Items->Add(miDel);
   // ��������� ��� ����������� ����
   menuFor = -1;
   // �� ��� ������
   Parent =  (TForm*)owner;
}
/*/---------------------------------------------------------------------------
PLinkComponent::~PLinkComponent(){
	delete Menu; Menu = NULL;
} */
//---------------------------------------------------------------------------
PLinkComponent PLinkComponent::operator=(const PLinkComponent &Obj){
   Owner = Obj.Owner;
   Menu = new TPopupMenu(NULL);
	// �������
   TMenuItem *miDel = new TMenuItem(Menu);
   miDel->Name = "Delete";
   miDel->Caption = "������� �����";
   miDel->OnClick = MenuItemClick;    // ����� ������� ������
   Menu->Items->Add(miDel);
   // ��������� ��� ����������� ����
   menuFor = Obj.menuFor;
   // �� ��� ������
   Parent =  Obj.Parent;
   // �������� �����
   Links = Obj.Links;
   // ������������ ����������
   for(int i=0, n=Count(); i<n; i++){
      Links[i].First->OnDelete = PinDelete;     // ��������� ������� PinDelete ��� �������� ������
      Links[i].Second->OnDelete = PinDelete;
      }
   return *this;
}
//---------------------------------------------------------------------------
PLinkComponent::~PLinkComponent(){
   // ������������ ����������
   for(int i=0, n=Count(); i<n; i++){
      Links[i].First->OnDelete = NULL;     // �� ��������� ������� PinDelete ��� �������� ������
      Links[i].Second->OnDelete = NULL;
      }
}
//---------------------------------------------------------------------------
void __fastcall PLinkComponent::MenuItemClick(TObject *Sender){
   // menuFor - �������� ����� ����� ��� ������� ���������� ����� ����
   String itemName =  ((TMenuItem*)Sender)->Name;
	if(itemName == "Delete"){
      // ��������
      if( menuFor>=0 || menuFor<(int)Links.size() ){
      	Links.erase(Links.begin() + menuFor);
         if(OnDelete) OnDelete(this);
         Paint();
         }
      }
}
//---------------------------------------------------------------------------
void PLinkComponent::ShowMenuFor(int LinkNumber){	// �������� ���� ��� �������� �����
   if( LinkNumber<0 || LinkNumber>=(int)Links.size() ){
      err(3, "PLinkComponent: ����������� ����. ���������� � ����� ������� �� ����������.");
      return;
   	}
	// ��������� ���������� ������������ ����
   TPoint sp;
   GetCursorPos(&sp);
   // ������� ����
   menuFor = LinkNumber;
   Menu->Popup(sp.x,sp.y);
}
//---------------------------------------------------------------------------
void PLinkComponent::ShowMenuFor(int X, int Y){	// �������� ���� ��� ����� c ��������� ������������
	int n = FindAtPos(X,Y);
   if( n != -1 )
   	ShowMenuFor( n );
}
//---------------------------------------------------------------------------
void PLinkComponent::Add   (Pair_PinConrtol link){  // �������� ����� ����
	Links.push_back(link);
   link.First->OnDelete = PinDelete;     // ��������� ������� PinDelete ��� ��������
   link.Second->OnDelete = PinDelete;
   // �������� �� ���������
   if(OnAdd) OnAdd(this);
}
//---------------------------------------------------------------------------
void __fastcall PLinkComponent::PinDelete(PPinControl *pc){
   // ���������� �� �������� ����������, ������ ����� � ���
	Delete( pc );
}
//---------------------------------------------------------------------------
void PLinkComponent::Delete(PPinControl* pc){			// ������� �����, ��� ������������ ��������� ������
   if(Links.size()==0)return;
   vector<Pair_PinConrtol>::iterator iter = Links.begin();
    while( iter != Links.end() ) 
    {
      if(iter->First == pc  ||  iter->Second == pc)
        iter = Links.erase( iter );
      else
        ++iter;
    }
}
//---------------------------------------------------------------------------
void PLinkComponent::Delete(PPin* pin){			// ������� �����, ��� ������������ ��������� ������
   vector<Pair_PinConrtol>::iterator iter = Links.begin();
    while( iter != Links.end() )
    {
      if(iter->First->Pin() == pin  ||  iter->Second->Pin() == pin)
        iter = Links.erase( iter );
      else
        ++iter;
    }
}
//---------------------------------------------------------------------------
Pair_PinConrtol& PLinkComponent::operator[](int i){
	return Links.at(i);
}
//---------------------------------------------------------------------------
int PLinkComponent::Count(){
	return Links.size();
}
//---------------------------------------------------------------------------
// ����� ���������� ����� �������� �����
int PLinkComponent::FindAtPos(int X, int Y){
	for(int i=0, n = Links.size(); i<n;i++){
   	int X1 = Links[i].First->Left+ 1;
      int Y1 = Links[i].First->Top + 1;
      int X2 =  Links[i].Second->Left+ 1;
      int Y2 =  Links[i].Second->Top + 1;
      // ��������� ������ ������� ?
      if(  ((X>(X1-3) && X<(X2+3))||(X>(X2-3) && X<(X1+3)))  &&
           ((Y>(Y1-3) && Y<(Y2+3))||(Y>(Y2-3) && Y<(Y1+3)))     ){
         // t��� ������������ ��� �������������� �����, �� ���������� ������� ��������� � �������
         if( X1==X2 || Y1==Y2 )
            return i;
      	// ������������� �� ��������� �����
         float k = 1.0*(Y1-Y2)/(X1-X2);
         float c = Y1 - k*X1;
         if(fabs(Y- k*X-c)<=4) // ������������ �� 4 ������
            return  i;	// ���������� �����
         }
   	}
   return -1;
}
//---------------------------------------------------------------------------
void __fastcall PLinkComponent::Paint(){
   if(Parent==NULL) return;
	static bool Painting = false;
   if(Painting) return;
   Painting = true;
   Parent->Repaint();                      // ������� ��� ��������� �����
   // ������ ����� �����
   TColor color = Parent->Canvas->Pen->Color;
   TBrushStyle bStyle= Parent->Canvas->Brush->Style;
   Parent->Canvas->Pen->Color = clGray;
   Parent->Canvas->Brush->Style = bsClear;
   for(int i=0, n=Count(); i<n; i++){
      int from_X = Links[i].First->Left+ 1;
      int from_Y = Links[i].First->Top + 1;
      int to_X =  Links[i].Second->Left+ 1;
      int to_Y =  Links[i].Second->Top + 1;
         Parent->Canvas->MoveTo(from_X, from_Y);
         Parent->Canvas->LineTo(to_X, to_Y);
      }
   Parent->Canvas->Pen->Color = color;
   Parent->Canvas->Brush->Style = bStyle;
   // ��������� ��������
   Painting = false;
}
//----------------------------------------------------------------------------
void PLinkComponent::Save(ostream &stream){			// ��������� �������
   int n = Links.size();
   stream << n << " ";
   for(int i=0; i<n; i++){
      // �������� ��� ���������� ������
      stream << StringForSave( &(Links[i].First ->Name) ) << " ";
      stream << StringForSave( &(Links[i].Second->Name) ) << " ";
      }
}
//---------------------------------------------------------------------------
void PLinkComponent::Read(istream &stream){			// ��������� �������
   int n;   // ����� ������
   stream >> n;
   for(int i=0; i<n; i++){
      // ��� ���� ������������ ���������
      Pair_PinConrtol pc;
      pc.First=NULL; pc.Second=NULL;
      // ��������� ��� ��������
      String X, Y;
      char tmp[1024];
      stream >> tmp;    X = tmp;
      stream >> tmp;    Y = tmp;
      // ������ ��� ���������� (��������� ����������� �.�. PBase) � ������� �� � ������ ���������
      for(int i = 0;i < Owner->ComponentCount; i++){
         PBase *pb = dynamic_cast<PBase*>( Owner->Components[i] );
         if( pb ){
         for(int j = 0;j < pb->ComponentCount; j++)
            if(pb->Components[j]->ClassNameIs("PPinControl")){
               if     ( pb->Components[j]->Name == X )
                  pc.First = (PPinControl*)pb->Components[j];
               else if( pb->Components[j]->Name == Y )
                  pc.Second= (PPinControl*)pb->Components[j];
               }
            }
         }
      if( pc.First==NULL)
         err(0, "PLinkComponent: �� ������ ����� � �������� ������ (" +X+ ") �� ��������� �����");
      else if(pc.Second==NULL )
         err(0, "PLinkComponent: �� ������ ����� � �������� ������ (" +Y+ ") �� ��������� �����");
      else
         Add( pc );
      }
}
//---------------------------------------------------------------------------
void PLinkComponent::Clear(){
   Links.clear();
}
//---------------------------------------------------------------------------

