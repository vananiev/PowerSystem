//---------------------------------------------------------------------------

#include <vcl.h>

#pragma hdrstop

#include <vector>
#include "PBase.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
// настройки компонента
#include "frmProp.h"
typedef  TfrmProperty  HelpForm;       // класс формы которая будет вызвана в качестве формы свойств
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
	//ControlAtPos 	Определить элемент управления, содержащийся в указанной позиции. 	TWinControl
   Parent = dynamic_cast<TWinControl*>(Owner);
   Width=70;
   Height=70;
   // параметры холста
   Canvas->Brush->Color = clWhite;   // Brush - заливка
   Canvas->Pen->Color = clGray;      // Pen - рисование линий
   Canvas->Font->Size = 8;
   Canvas->Font->Color = clGray;
   Canvas->Font->Name="Times New Roman";
   // задаем уникальное имя компоненту
   int PBaseNameId = 0;
   String name = (String)ClassName() + IntToStr( ++PBaseNameId );
   while(  Owner->FindComponent( name )  )
      name = (String)ClassName() + IntToStr( ++PBaseNameId );
   Name = name;
   // прорисовка компонента
   //Paint();
   // способ перетаскивания
   DragKind = dkDrag;
   DragMode = dmManual;
   // в момент перетаскивания устанавливается в true
   _Drag = false;
   // контексттное меню
   Menu = new TPopupMenu(this);
      // поворот
   TMenuItem *miRotate = new TMenuItem(Menu);
   miRotate->Name = "Rotate";
   miRotate->Caption = "Повернуть";
   miRotate->OnClick = MenuItemClick;
   Menu->Items->Add(miRotate);
      // удалить
   TMenuItem *miDel = new TMenuItem(Menu);
   miDel->Name = "Delete";
   miDel->Caption = "Удалить";
   miDel->OnClick = MenuItemClick;
   Menu->Items->Add(miDel);
      // свойства
   TMenuItem *miProp = new TMenuItem(Menu);
   miProp->Name = "Properties";
   miProp->Caption = "Свойства";
   miProp->OnClick = MenuItemClick;
   Menu->Items->Add(miProp);
   // силовой объект ассоциированный с компонентом
   _Obj = NULL;
   // определяем где какая группа выводов отображается
   _LEFT_ = 0;
   _RIGHT_= 1;
   _BOTTOM_=2;
   _TOP_  = 3;
   // событие об изменении элемеента
   OnChange = NULL;
   // событие возникает при клике по выводу
   OnPinMouseDown = NULL;
   // возникает в конце перетаскивания на объект
   OnPinDragDrop = NULL;
   OnPinDragOver = NULL;
   // Вызывается при удалении
   OnDelete = NULL;
    // прозрачная кисть - замкнутые участки делаются прозрачными
   //Canvas->Brush->Style = bsClear;
}
//---------------------------------------------------------------------------
__fastcall PBase::~PBase(){
   //delete Menu; удаляется по владельцу
   pList.clear();	// иначе при удалении происходит вызов SetParent
   // удалим саму модель
   if( _Obj->Owner == this ){
      delete _Obj;  _Obj = NULL;
      }
}
//---------------------------------------------------------------------------
void __fastcall PBase::Paint(void){
	//SetBkMode(Canvas->Handle, TRANSPARENT );     // Handle - это описательокна в windows
   															//Canvas.Handle == контекст отображения
	//BitBlt(((TForm*)Parent)->Canvas->Handle, 200, 200, Width, Height, ((TForm*)Parent)->Canvas->Handle, Left, Top, SRCCOPY);
   if(Parent == NULL) return;
   // рисуем тело
   Canvas->Rectangle(10,10,Width-10,Height-10);
   // имя
   String name;
   if(_Obj)
      name = _Obj->Name;
   else
      name = Name;
   Canvas->TextOutA(Width/2 - 3.0*name.Length(), (Height-20)/2+4,name);
   // прорисовываем выводы
   int outWidth;
   int outStep;
   int pins_cnt;
   if(_Obj){
      int n = _Obj->Out.Count();  // число групп выводов
      int fontSize = Canvas->Font->Size;
      Canvas->Font->Size = 6;
      if(n > _TOP_){
         // выводы сверху
         outWidth = Width - 24;
         pins_cnt = _Obj->Out[_TOP_].Count;
         outStep = (float)outWidth/(pins_cnt-1);
         for(int i=0; i<pins_cnt;i++){
            Canvas->MoveTo(12+i*outStep, 2);
            Canvas->LineTo(12+i*outStep, 10);
            // располагаем вывод
            PPinControl *pc = PinControl(&_Obj->Out[_TOP_][i]);
            if( pc){
               pc->Left = XtoParent(12+i*outStep - pc->Width/2);
               pc->Top  = YtoParent(0 - pc->Height/2);
               }
            }
         Canvas->TextOutA(Width/2,11,_Obj->Out[_TOP_].Name);
         }
      if(n > _BOTTOM_){
         // выводы снизу
         outWidth = Width - 24;
         pins_cnt = _Obj->Out[_BOTTOM_].Count;
         outStep = (float)outWidth/(pins_cnt-1);
         for(int i=0; i<pins_cnt;i++){
            Canvas->MoveTo(12+i*outStep, Height-10);
            Canvas->LineTo(12+i*outStep, Height-2);
            // располагаем вывод
            PPinControl *pc = PinControl(&_Obj->Out[_BOTTOM_][i]);
            if( pc){
               pc->Left = XtoParent(12+i*outStep - pc->Width/2);
               pc->Top  = YtoParent(Height - pc->Height/2);
               }
            }
         Canvas->TextOutA(Width/2,Height-22,_Obj->Out[_BOTTOM_].Name);
         }
      if(n > _RIGHT_){
         // выводы справа
         outWidth = Height - 24;
         pins_cnt = _Obj->Out[_RIGHT_].Count;
         outStep = (float)outWidth/(pins_cnt-1);
         for(int i=0; i<pins_cnt;i++){
            Canvas->MoveTo(Width-10, 12+i*outStep);
            Canvas->LineTo(Width-2,  12+i*outStep);
            // располагаем вывод
            PPinControl *pc = PinControl(&_Obj->Out[_RIGHT_][i]);
            if( pc){
               pc->Left = XtoParent(Width - pc->Width/2);
               pc->Top  = YtoParent(12+i*outStep - pc->Height/2);
               }
            }
         Canvas->TextOutA(Width-12-6*_Obj->Out[_RIGHT_].Name.Length(),20,_Obj->Out[_RIGHT_].Name);
         }
      if(n > _LEFT_){
         // выводы слева
         outWidth = Height - 24;
         pins_cnt = _Obj->Out[_LEFT_].Count;
         outStep = (float)outWidth/(pins_cnt-1);
         for(int i=0; i<pins_cnt;i++){
            Canvas->MoveTo(2, 12+i*outStep);
            Canvas->LineTo(10,12+i*outStep);
            // располагаем вывод
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
   // осуществляем перетаскивание
   if(_Drag){
      Left += X-MouseDownPos_X;
      Top += Y-MouseDownPos_Y;
      }
   if(OnMouseMove) OnMouseMove(this, Shift, X, Y);
}
//---------------------------------------------------------------------------
void __fastcall PBase::MouseDown(TMouseButton Button, Classes::TShiftState Shift, int X, int Y){
   if(Button==mbLeft){
      // перетаскиваем элемент
      _Drag = true;
      MouseDownPos_X = X;
      MouseDownPos_Y = Y;
      }
   else if (Button==mbRight){
      // определим координаты относительно окна
      TPoint p, sp;
      p.x = X; p.y = Y;
      sp = ClientToScreen(p);
      /*/ если это сеть
      if(dynamic_cast<PSubGrid*>(_Obj))
         Menu->Items->Find("Редактировать сеть")->Visible = true;
      else
         Menu->Items->Find("Редактировать сеть")->Visible = false;*/
      // выводим меню
      Menu->Popup(sp.x,sp.y);
      }
   // вызываем дополнительный внешний обработчик
   if(OnMouseDown)  OnMouseDown(this, Button, Shift, X, Y);
}
//---------------------------------------------------------------------------
void __fastcall PBase::MouseUp(TMouseButton Button, Classes::TShiftState Shift, int X, int Y){
   _Drag = false;
   if(OnMouseUp) OnMouseUp(this, Button, Shift, X, Y);
}
//---------------------------------------------------------------------------
void PBase::createPins(){
   // удаляем раннее созданные объекты
   for(int i=0, n=pList.size(); i<n; i++)  	delete pList[i]; // при этом происходит извещении списка связей об удалении для удаления связей
   pList.clear();
   // создаем новые объекты
   for(int i=0, n=_Obj->Out.Count(); i<n; i++)
      for(int j=0, m=_Obj->Out[i].Count; j<m; j++){
         pList.push_back(new PPinControl(this));   // владельцем должен быть этот компонент, иначе компонент-вывод удалится не вместе с этим компонентом 
         pList.back()->Name = Name + "_" +IntToStr(i) + "_" +IntToStr(j); // имя компонента вывода
         pList.back()->Parent = Parent;           // а родителем тот на ком рисуем
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
      // меню свойства
      HelpForm *frmP = new HelpForm(this);
      frmP->Obj = this;
      frmP->ShowModal();
      delete frmP;
      Repaint();  // если произошли изменения внешнего вида, то отобразим
      // событие-уведомление об изменении элемента
      if(OnChange) OnChange(this);
      }
   else if(itemName == "Rotate"){
      // поворот
      int tmp = _LEFT_;
      _LEFT_=_BOTTOM_;
      _BOTTOM_=_RIGHT_;
      _RIGHT_=_TOP_;
      _TOP_=tmp;
      Repaint();
      }
   else if(itemName == "Delete"){
      // удаление
      if(OnDelete) OnDelete(this);
      // удалим объект и модель
      delete this;
      }
}
//---------------------------------------------------------------------------
void PBase::Obj(PElement *obj){
   if(_Obj!=NULL){
      err(0, "PBase: компонент уже асоциирован с моделю " + _Obj->ClassName() + ". Новая ассоциация не выполнена.");
      return;
      }
   if(obj){
      _Obj = obj;
      if(obj->Owner==NULL) _Obj->Owner = this;  // если у объекта нет родителя
   	createPins();  // создаем графическое представление выводов
      }
}
//---------------------------------------------------------------------------
PElement * PBase::Obj(){
	return _Obj;
}
//---------------------------------------------------------------------------
 // создаем объект и ассоциируем его с компонентом
PElement* PBase::ObjType(String ClassName){
   Visible = false;
   vector<void*> Params;
   if(ClassName == "PLine"){
      int cells = 1;
      TryStrToInt(InputBox("Параметры ЛЭП", "Введите число звеньев", "1"), cells);
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
   // вызываем внешний обработчик события
   if(OnPinMouseDown)OnPinMouseDown(Sender, Button, Shift, X, Y);
}
//---------------------------------------------------------------------------
void __fastcall PBase::PinDragDrop (System::TObject* Sender, System::TObject* Source, int X, int Y){
   // вызываем внешний обработчик события
   if(OnPinDragDrop) OnPinDragDrop(Sender, Source, X, Y);
}
//---------------------------------------------------------------------------
void __fastcall PBase::PinDragOver(System::TObject* Sender, System::TObject* Source, int X, int Y, TDragState State, bool &Accept){
   // вызываем внешний обработчик события
   if(OnPinDragOver) OnPinDragOver(Sender, Source, X, Y, State, Accept);
}
//---------------------------------------------------------------------------
void PBase::Save(ostream &stream){		// сохранение параметров объекта в поток
   if(Name==""){
      err(0, "PBase: компонент имеет нулевое имя. Возможно файл будет неоткрываем.");
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
   // сохранить сам объект
   SaveObj( _Obj, stream );
}
//---------------------------------------------------------------------------
/*void PBase::SaveComponentOnly(ostream &stream){		// сохранение параметров объекта в поток
   if(Name==""){
      err(0, "PBase: компонент имеет нулевое имя. Возможно файл будет неоткрываем.");
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
void PBase::Read(istream &stream){    // загрузка параметров объекта из потока
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
   // была ли сохранена модель
   bool saveModel = true;
   stream >> saveModel;
   // создадим объект
   if(saveModel) Obj( CreateObj(stream) );
}
//---------------------------------------------------------------------------
/*void PBase::ReadComponentOnly(istream &stream){    // загрузка параметров объекта из потока
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
   // была ли сохранена модель
   bool saveModel = true;
   stream >> saveModel;   // должно быть false
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
   _Pin = NULL; // нет ассоциированного вывода для компонента
   // графические параметры вывода
   Shape = stCircle;
   Width = 4;
   Height = 4;
   Pen->Color = clGray;
   // на какой компонент рисуем
   //Parent = (TWinControl*)Owner;   не всегда владелец == родитель
   // всплывающая подсказка
   ShowHint = false;
   // обработка Drag & Drop
   DragMode = dmManual;
   // событие возникает при клике
   OnMouseDown = NULL;
   // возникает в конце перетаскивания на объект
   OnDragDrop = NULL;
   OnDragOver = NULL;
}
//---------------------------------------------------------------------------
__fastcall PPinControl::~PPinControl(){
   //5 == 5;
   if(OnDelete) OnDelete(this); // оповещаем об удалении
}
//---------------------------------------------------------------------------
void PPinControl::Pin(PPin* pin){
   _Pin = pin;
   PElement* obj = pin->Obj();
   // определяем фазу и сохраняем ее во всплывающей подсказке
   Hint = pin->Node();
   for(int i=0, n=obj->Out.Count(); i<n; i++)
      for(int j=0, m=obj->Out[i].Count; j<m; j++)
         if(obj->Out[i][j].Node() == pin->Node()){
            switch(j){
               case _N_:    Hint = "N"; break;       // break; - это специально, т.к. вывод
               case _A_:    Hint = "A"; break;
               case _B_:    Hint = "B"; break;
               case _C_:    Hint = "C"; break;
               }
            i=n;j=m; // завершаем циклы
            }
   ShowHint = true;
}
//---------------------------------------------------------------------------
PPin* PPinControl::Pin(){
   return _Pin;
}
//---------------------------------------------------------------------------
void __fastcall PPinControl::DragOver(System::TObject* Source, int X, int Y, TDragState State, bool &Accept){
   // если перетаскиваем вывод
   if(dynamic_cast<PPinControl*>(Source))
      Accept = true; // разрешаем принять объект
   // внешний обработчик
   if(OnDragOver) OnDragOver(this, Source, X, Y, State, Accept);
}
//---------------------------------------------------------------------------
void __fastcall PPinControl::DragDrop (System::TObject* Source, int X, int Y){
   // внешний обработчик
   if(OnDragDrop) OnDragDrop(this, Source, X, Y);
}
//---------------------------------------------------------------------------
void __fastcall PPinControl::MouseDown(TMouseButton Button, Classes::TShiftState Shift, int X, int Y){
   BeginDrag(true, -1); // начинаем перетаскивание объекта
   // внешний обработчик
   if(OnMouseDown) OnMouseDown(this, Button, Shift, X, Y);
}
//---------------------------------------------------------------------------
void __fastcall PPinControl::MouseEnter(TMessage& Msg){
   savedWidth = Width; savedHeight = Height; // запоминаем размеры
   savedColor = Pen->Color;          // цвет карандаша
   // делаем вывод больше по размеру и изменяем положение
   Pen->Color = clRed;
   Width = 2*savedWidth;
   Height= 2*savedHeight;
}
//---------------------------------------------------------------------------
void __fastcall PPinControl::MouseLeave(TMessage& Msg){
   // возвращаем размеры и положение
   Width = savedWidth;
   Height= savedHeight;
   Pen->Color = savedColor;
   Repaint();
}
//---------------------------------------------------------------------------
PLinkComponent::PLinkComponent(TComponent *owner){
   Owner = owner;
   Menu = new TPopupMenu(NULL);
	// удалить
   TMenuItem *miDel = new TMenuItem(Menu);
   miDel->Name = "Delete";
   miDel->Caption = "Удалить связь";
   miDel->OnClick = MenuItemClick;
   Menu->Items->Add(miDel);
   // установим при отображении меню
   menuFor = -1;
   // на ком рисуем
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
	// удалить
   TMenuItem *miDel = new TMenuItem(Menu);
   miDel->Name = "Delete";
   miDel->Caption = "Удалить связь";
   miDel->OnClick = MenuItemClick;    // адрес функции другой
   Menu->Items->Add(miDel);
   // установим при отображении меню
   menuFor = Obj.menuFor;
   // на ком рисуем
   Parent =  Obj.Parent;
   // копируем связи
   Links = Obj.Links;
   // перенаправим оповещалки
   for(int i=0, n=Count(); i<n; i++){
      Links[i].First->OnDelete = PinDelete;     // оповещать функцию PinDelete при удалении вывода
      Links[i].Second->OnDelete = PinDelete;
      }
   return *this;
}
//---------------------------------------------------------------------------
PLinkComponent::~PLinkComponent(){
   // перенаправим оповещалки
   for(int i=0, n=Count(); i<n; i++){
      Links[i].First->OnDelete = NULL;     // не оповещать функцию PinDelete при удалении вывода
      Links[i].Second->OnDelete = NULL;
      }
}
//---------------------------------------------------------------------------
void __fastcall PLinkComponent::MenuItemClick(TObject *Sender){
   // menuFor - содержит номер связи для которой происходил вывод меню
   String itemName =  ((TMenuItem*)Sender)->Name;
	if(itemName == "Delete"){
      // удаление
      if( menuFor>=0 || menuFor<(int)Links.size() ){
      	Links.erase(Links.begin() + menuFor);
         if(OnDelete) OnDelete(this);
         Paint();
         }
      }
}
//---------------------------------------------------------------------------
void PLinkComponent::ShowMenuFor(int LinkNumber){	// показать меню для заданной связи
   if( LinkNumber<0 || LinkNumber>=(int)Links.size() ){
      err(3, "PLinkComponent: отображение меню. Компонента с таким номером не существует.");
      return;
   	}
	// определим координаты относительно окна
   TPoint sp;
   GetCursorPos(&sp);
   // выводим меню
   menuFor = LinkNumber;
   Menu->Popup(sp.x,sp.y);
}
//---------------------------------------------------------------------------
void PLinkComponent::ShowMenuFor(int X, int Y){	// показать меню для связи c заданными координатами
	int n = FindAtPos(X,Y);
   if( n != -1 )
   	ShowMenuFor( n );
}
//---------------------------------------------------------------------------
void PLinkComponent::Add   (Pair_PinConrtol link){  // добавить новую пару
	Links.push_back(link);
   link.First->OnDelete = PinDelete;     // оповещать функцию PinDelete при удалении
   link.Second->OnDelete = PinDelete;
   // сообщаем об изменении
   if(OnAdd) OnAdd(this);
}
//---------------------------------------------------------------------------
void __fastcall PLinkComponent::PinDelete(PPinControl *pc){
   // оповестили об удалении компонента, удалим связи с ним
	Delete( pc );
}
//---------------------------------------------------------------------------
void PLinkComponent::Delete(PPinControl* pc){			// удалить связи, где присутствует указанный объект
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
void PLinkComponent::Delete(PPin* pin){			// удалить связи, где присутствует указанный объект
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
// линию проходящую через заданную точку
int PLinkComponent::FindAtPos(int X, int Y){
	for(int i=0, n = Links.size(); i<n;i++){
   	int X1 = Links[i].First->Left+ 1;
      int Y1 = Links[i].First->Top + 1;
      int X2 =  Links[i].Second->Left+ 1;
      int Y2 =  Links[i].Second->Top + 1;
      // находимся внутри отрезка ?
      if(  ((X>(X1-3) && X<(X2+3))||(X>(X2-3) && X<(X1+3)))  &&
           ((Y>(Y1-3) && Y<(Y2+3))||(Y>(Y2-3) && Y<(Y1+3)))     ){
         // tсли вертикальная или горизонтальная линия, то достаточно условия вхождения в прямоуг
         if( X1==X2 || Y1==Y2 )
            return i;
      	// удовлетворяем ли уравнению линии
         float k = 1.0*(Y1-Y2)/(X1-X2);
         float c = Y1 - k*X1;
         if(fabs(Y- k*X-c)<=4) // промахнулись на 4 пиксел
            return  i;	// возвращаем связь
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
   Parent->Repaint();                      // очищаем все временные линии
   // рисуем линии связи
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
   // закончили рисовать
   Painting = false;
}
//----------------------------------------------------------------------------
void PLinkComponent::Save(ostream &stream){			// сохранить контрол
   int n = Links.size();
   stream << n << " ";
   for(int i=0; i<n; i++){
      // сохраним имя компонента вывода
      stream << StringForSave( &(Links[i].First ->Name) ) << " ";
      stream << StringForSave( &(Links[i].Second->Name) ) << " ";
      }
}
//---------------------------------------------------------------------------
void PLinkComponent::Read(istream &stream){			// загрузить контрол
   int n;   // число связей
   stream >> n;
   for(int i=0; i<n; i++){
      // это пара объединяемых компонент
      Pair_PinConrtol pc;
      pc.First=NULL; pc.Second=NULL;
      // прочитаем имя компонен
      String X, Y;
      char tmp[1024];
      stream >> tmp;    X = tmp;
      stream >> tmp;    Y = tmp;
      // найдем эти компоненты (родителем компонентов д.б. PBase) и добавим их в список связанных
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
         err(0, "PLinkComponent: не найден вывод с заданным именем (" +X+ ") на указанной форме");
      else if(pc.Second==NULL )
         err(0, "PLinkComponent: не найден вывод с заданным именем (" +Y+ ") на указанной форме");
      else
         Add( pc );
      }
}
//---------------------------------------------------------------------------
void PLinkComponent::Clear(){
   Links.clear();
}
//---------------------------------------------------------------------------

