//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "EditGrid.h"
#include "Scope.h"
#include "ObectsLibrary.h"
//#include "kernel.h"			// хидер для работы с моделями
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
   // инициализируем объекта
   LinkLines.Clear();     
   Objects.clear();
	// компонент отображения связей
   LinkLines.Parent = this;
   //LinkLines.OnDelete = LinkDelete;
   // владелец создаваемых компонентов (может быть переопределен из-вне)
   ComponentOwner = this;
   // исключим мерцание
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
   // удаляем все связи с этим объектом
   PElement* obj = ((PBase*)Sender)->Obj();
   for(int i=0, n=obj->Out.Count(); i<n; i++)
   	for(int j=0, m=obj->Out[i].Count; j<m; j++)
      	LinkLines.Delete( &obj->Out[i][j] );			// удалим все связи, связанные с выводом
	// перерисуем связи
   LinkLines.Paint();
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditGrid::PinDragDrop (System::TObject* Sender, System::TObject* Source, int X, int Y){
   // если перетащили на самого себя, то не добавляем
   if( Sender == Source ) return;
   // сохраняем информацию графической о связи
   Pair_PinConrtol pcon;
   pcon.First = (PPinControl*)Source;
   pcon.Second= (PPinControl*)Sender;
   LinkLines.Add(pcon);
   // отобразим связь
   LinkLines.Paint();
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditGrid::FormDragOver(TObject *Sender, TObject *Source,
      int X, int Y, TDragState State, bool &Accept)
{
   Accept = false;
   // если в процессе соединения выводов
   if(PPinControl *pc = dynamic_cast<PPinControl*>(Source)){
   		int from_X = pc->Left + pc->Width/2;
      	int from_Y = pc->Top  + pc->Height/2;
      	TColor color = Canvas->Pen->Color;
      	Canvas->Pen->Color = clBlack;
      	Repaint();                       // очищаем все временные линии
      	Canvas->MoveTo(from_X, from_Y);
      	Canvas->LineTo(X, Y);
      	Canvas->Pen->Color = color;
         Accept = true;
      }
   // перетащили новый объект из библиоьеки компонентов
   else if(Source->ClassNameIs("TTreeView")){
		Accept = true;
   	}
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditGrid::FormPaint(TObject *Sender)
{
	// рисуем линии PaintLines()
   LinkLines.Paint();
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditGrid::FormDragDrop(TObject *Sender, TObject *Source,
      int X, int Y)
{
   // очищаем все временные линии созданные при соединении выводов
   if(Source->ClassNameIs("PPinControl"))
   	Repaint();
   // перетащили новый объект из библиоьеки компонентов
   else if(Source->ClassNameIs("TTreeView")){
   	TTreeView *obj = (TTreeView*)Source;
   	int n = obj->Selected->AbsoluteIndex;
   	if ( n != -1){
      	TTreeNode *Node = obj->Items->Item[n];
         if(Node != NULL){// && !Node->HasChildren){
            // создаем элемент
      		// сохраняем его в списке и применяем параметры по умолчанию
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
void TfrmEditGrid::AddObject(String TypeName, int X, int Y){    // список TypeName - передается из frmObjLib->tvList
   String cName = "";
   String ClassName = "PBase";
   if(TypeName == "Источники\\Генератор трехфазный")           cName = "PSource";
   else if(TypeName == "Трансформаторы\\Силовые\\Трехфазный")  cName = "PTransformer_3";
   else if(TypeName == "Трансформаторы\\Силовые\\Однофазный")  cName = "PTransformer_1";
   else if(TypeName == "Трансформаторы\\Тока\\Однофазный")     cName = "PCurrent_Transformer_1";
   else if(TypeName == "Линии\\Одноцепная без ГТ")             cName = "PLine";
   else if(TypeName == "Потребители\\Трехфазный")              cName = "PConsumer";
   else if(TypeName == "Коммутационная аппаратура\\Выключатель")cName = "PBreaker";
   else if(TypeName == "Подсеть")										{cName = "PSubGrid";    ClassName = "PGridComponent"; }
	// сохраняем его в списке и применяем параметры по умолчанию
   if(cName!=""){
      // добавить компонент-модель
   	CreateControl(ClassName, cName);
      PBase *control = Objects.back();
      control->Left = X - control->Width/2;
      control->Top  = Y - control->Height/2;
   	}
   else{
      err(3, "PowerSystem: не определено действие для создания указанного компонента из Библиотеки объектов");
   	}
}
//---------------------------------------------------------------------------
void TfrmEditGrid::CreateControl(String Class, String ModelClass){
      // добавить компонент-модель
      PBase *control;
      TComponent *owner;
      if(ComponentOwner) owner = ComponentOwner;
      else               owner = this;
      if(Class == "PGridComponent")       control = new PGridComponent( owner );
      else                    	         control = new PBase( owner );
      AddControl( control );              // добавить компонент в список отображаемых и установить параметры
      control->ObjType( ModelClass );     // ассоциируем компонент с моделью типа
}
//---------------------------------------------------------------------------
// добавить компонент в список отображаемых и установить параметры
void TfrmEditGrid::AddControl(PBase *control){
   control->Parent = this;
	//control->OnChange = UnitChange;
   //control->OnPinMouseDown = PinMouseDown;
   control->OnPinDragDrop = PinDragDrop;
   control->OnDelete = UnitDelete;
   Objects.push_back( control );    		// сохраняем в список компонентов
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditGrid::mnuObjLibClick(TObject *Sender)
{
      // вызов библиотеки объектов
      TfrmObjLib *frmOL = new TfrmObjLib(this);
      frmOL->Show();
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditGrid::FormMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
   // щелкнули правую кнопку
	if(Button == mbRight){
      // может мы указываем на линию связи?
      LinkLines.ShowMenuFor( X, Y );
   	}
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditGrid::mnuSaveClick(TObject *Sender)
{
   // выводим диалог и сохраняем
   saveDlg->Filter = ".psf|*.psf|любой файл|*.*";
   if( saveDlg->Execute() ){
     	AnsiString Name = saveDlg->FileName;
      ofstream out(Name.c_str());
      if(out){
      	// сохраняем объекты
         int n = Objects.size();
         out << n << " ";
         for(int i=0; i<n; i++) {
            out << StringForSave( &(String)Objects[i]->ClassName() ) << " ";
            Objects[i]->Save( out );
            }
         //  сохраним информацию о связях
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
   // выводим диалог и открываем
   openDlg->Filter = ".psf|*.psf|любой файл|*.*";
   if( openDlg->Execute() ){
      OpenFile(openDlg->FileName);
    }
}
//---------------------------------------------------------------------------
void TfrmEditGrid::OpenFile(String name){
      ifstream in(name.c_str());
      if(in){
         // очищаем существующую сеть
         mnuNewClick(NULL);
      	// сохраняем объекты
         int n;
         in >> n;
         for(int i=0; i<n; i++)  {
            // читаем тип
            char cname[1024];
            in >> cname; String cName = cname;
            // добавляем компонент, указывающий на нулевую модель
            CreateControl( cName, "" );
            // загружаем параметры компонента
            Objects.back()->Read( in );
            }
         // загружаем информацию о связях
         LinkLines.Read( in );
      	// Изменяем заголовок окна
         if( Name.Length() < 32 )
            Caption = name;
         else
            Caption = name.SubString(Name.Length()-32, 32);
         }
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditGrid::mnuNewClick(TObject *Sender)
{
   // очищаем существующую сеть
   for(int i=0, n=Objects.size(); i<n; i++)
      delete Objects[i];
   Objects.clear();
   LinkLines.Clear();
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditGrid::FormClose(TObject *Sender,
      TCloseAction &Action)
{
      // применим изменения в модели
      if(LinkLines.Count()==0 && Objects.size()==1){
         err(0, "EditGrid: подсеть должна содержать более 1 компонента");
         Action = caNone;
         }
      else if( Objects.size() == 0 || LinkLines.Count()!=0){}
      else{
   	   err(0, "EditGrid: не возможно собрать подсеть, т.к. не указано ни одной связи между элементами");
         Action = caNone;
         }
      // проверим уникальность имен в этой подсети
      for(int i=0,n=Objects.size(); i<n; i++){
         String name = Objects[i]->Obj()->Name;
         for(int j=i+1; j<n; j++)
            if(Objects[j]->Obj()->Name == name){
               err(0, "EditGrid: внутри сети объекты должны иметь уникальные имена");
               Action = caNone;
         		}
      	}
}
//---------------------------------------------------------------------------
void __fastcall TfrmEditGrid::FormShow(TObject *Sender)
{
  if(GridCtrl){
  	// изначально окрасим все выводы в серый
         for(int k=0, o=Objects.size(); k<o; k++){
         	// проверим выводы этого компонента
         	PBase *pb = dynamic_cast<PBase*>( Objects[k] );
         	if( pb ){
         		for(int h = 0;h < pb->ComponentCount; h++)
            		if(pb->Components[h]->ClassNameIs("PPinControl")){
               		// если компонент-вывод указывает на нужный нам вывод, то изменяем
                  	// графические характеристики этого компонента
                  	((PPinControl*)pb->Components[h])->Pen->Color = clGray;
                  	}
               }
            }
   // выдели текущие выводы от сети
   for(int i=0, n=GridCtrl->Obj()->Out.Count(); i<n; i++)
      for(int j=0, m=GridCtrl->Obj()->Out[i].Count; j<m; j++){
      	// Этот вывод будем искать
         PPin *pin = &GridCtrl->Obj()->Out[i][j];
         for(int k=0, o=Objects.size(); k<o; k++){
         	// проверим выводы этого компонента
         	PBase *pb = dynamic_cast<PBase*>( Objects[k] );
         	if( pb ){
         		for(int h = 0;h < pb->ComponentCount; h++)
            		if(pb->Components[h]->ClassNameIs("PPinControl")){
               		// если компонент-вывод указывает на нужный нам вывод, то изменяем
                  	// графические характеристики этого компонента
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
