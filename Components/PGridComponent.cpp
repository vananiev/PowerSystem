//---------------------------------------------------------------------------
#pragma hdrstop

#include "PGridComponent.h"
//---------------------------------------------------------------------------
#include "EditGrid.h"
typedef  TfrmEditGrid      GridEditForm;   // класс формы которая будет вызвана в качестве формы свойств
//---------------------------------------------------------------------------
__fastcall PGridComponent::PGridComponent(TComponent* Owner) :
   PBase(Owner),
   LinkLines(this){
   LinkLines.Parent = NULL;
      // редактировать сеть
   TMenuItem *miGrid = new TMenuItem(Menu);
   miGrid->Name = "EditGrid";
   miGrid->Caption = "Редактировать сеть";
   miGrid->OnClick = MenuItemClick;
   Menu->Items->Add(miGrid);
}
//---------------------------------------------------------------------------
void PGridComponent::Save(ostream &stream){		// сохранить контрол и объект
   // сохраним базовый компонент
   sire::Save(stream);
   // сохраняем компоненты
   int n=Objects.size();
   stream << n << " ";
   for(int i=0; i<n; i++){
      stream << StringForSave( &(String)Objects[i]->ClassName() ) << " ";
      Objects[i]->Save(stream);
      }
   //  сохраним информацию о связях
   LinkLines.Save( stream );
   // сохраним группы выводов
   String Outs = ((PSubGrid*)_Obj)->getOuts();
   stream << StringForSave( &Outs ) << " ";
}
//---------------------------------------------------------------------------
void PGridComponent::Read(istream &stream){		// загрузить контрол и объект
   // очищаем существующую сеть
   Objects.clear();
   LinkLines.Clear();
   // читаем базовый компонент
   sire::Read(stream);
   PSubGrid *grid = dynamic_cast<PSubGrid*>(_Obj);
   if(!grid){err(0, "PGridComponent: при загрузке не создан компонент");}
   // читаем компоненты
   int n=0;
   stream >> n;
   for(int i=0; i<n; i++){
      // читаем тип компонента
      char cname[1024];
      stream >> cname; String cName = cname;
      PBase *ctrl = AddControl(cName, "");// добавляем компонент, указывающий на нулевую модель
      ctrl->Read(stream);                 // прочитаем настройки этого компонента
      }
   //  читаем информацию о связях
   LinkLines.Read( stream );

   // настроим компонент и модель в соответствии со связями
   // информация о связях для передачи в модель
   Pins_Link Link;
	for(int i=0, n=LinkLines.Count(); i<n; i++)
      Link.add(*LinkLines[i].First->Pin(), *LinkLines[i].Second->Pin());
   // найдем все подсети внутри этой сети
   vector<PSubGrid*> sgList;
   PSubGrid *sg;
   for(int i=0, n=Objects.size(); i<n; i++)
   	if(Objects[i]->Obj()->ClassName() == "PSubGrid")
      	sgList.push_back( (PSubGrid*)Objects[i]->Obj() );
   grid->Change( Link, sgList );
   // создадим группы выводов
   char outs[1024];
   stream >> outs; String Outs = outs;
   grid->setOuts( Outs );
   // создадим компоненты-выводы
   createPins();
}
//---------------------------------------------------------------------------
PBase* PGridComponent::AddControl(String Class, String ModelClass){
      // добавить компонент-модель
      PBase *control = NULL;
      if(Class == "PGridComponent")       control = new PGridComponent( this );
      else                    	         control = new PBase( this );
      control->ObjType( ModelClass );        // ассоциируем компонент с моделью типа
      Objects.push_back( control );    		// сохраняем в список компонентов
      return control;
}
//---------------------------------------------------------------------------
void __fastcall PGridComponent::MenuItemClick(TObject *Sender){         // вызывается при клике на элемент меню
   // обрабатываем базовые операции определенные в базовом классе
   sire::MenuItemClick( Sender );
   // операции специфичные для компонента-сети
   String itemName =  ((TMenuItem*)Sender)->Name;
   if(itemName == "EditGrid"){
      PSubGrid *grid = dynamic_cast<PSubGrid*>(_Obj);
      if(!grid) return;
      // вызываем модальное окно редактирования
      GridEditForm *frmGE = new GridEditForm(this);
      // владельцем всех компонентов, созданных на форме, будет этот компонент
      frmGE->ComponentOwner = this;
      // указываеем графический компонент сети
      frmGE->GridCtrl = this;
      // забрасываем на нее компоненты
      frmGE->LinkLines.Clear();
      for(int i=0, n=Objects.size(); i<n; i++)
      	frmGE->AddControl( Objects[i] );      // компоненты должны быть отображены на форме
      // связи между выводами
      frmGE->LinkLines = LinkLines;
      frmGE->LinkLines.Parent = frmGE;    // клмпонент должен быть отображен на форме
      // показываем форму
      frmGE->Caption = _Obj->Name;
      frmGE->ShowModal();
      // сохраним изменения в компонентах и связях
      Objects = frmGE->Objects;
      LinkLines = frmGE->LinkLines;
      // запомним какие выводы были до изменении подсети
      String oldOuts = grid->getOuts();
      // применим изменения в модели
      if(LinkLines.Count()==0 && Objects.size()==1)
         err(0, "PGridComponent: подсеть должна содержать более 1 компонента");
      else if( Objects.size() != 0 && LinkLines.Count()!=0){
   	   // соединяем выводы
         Pins_Link Link;
         Link.clear();
		   for(int i=0, n=LinkLines.Count(); i<n; i++)
   		   Link.add(*LinkLines[i].First->Pin(), *LinkLines[i].Second->Pin());
         // найдем все подсети внутри этой сети
         vector<PSubGrid*> sgList;
         PSubGrid *sg;
         for(int i=0, n=Objects.size(); i<n; i++)
         	if(Objects[i]->Obj()->ClassName() == "PSubGrid")
            	sgList.push_back( (PSubGrid*)Objects[i]->Obj() );
         // опишем сеть
         grid->Change( Link, sgList );
         }
      else if( Objects.size() == 0 ){
   	   // создаем пустую сеть
         Pins_Link Link;
         vector<PSubGrid*> sgList;
         grid->Change( Link, sgList );
      	}
      else
   	   err(0, "PGridComponent: не возможно собрать подсеть, т.к. не указано ни одной связи между элементами");
      // какие выводы необходимы от сети
      String Outs = InputBox("Выводы от сети", "В формате \"Объект1.X; Объект2.X1; [...]\"", grid->getOuts() );
      grid->setOuts( Outs );
      // при изменении сети выводы могли уничтожиться - тогда надо пересобрать компоненты-выводы
      if( grid->getOuts() != oldOuts ){
      	// пересоздаем выводы
      	createPins();
         }
      // чтобы компоненты не удалились
      for(int i=0, n=Objects.size(); i<n; i++) Objects[i]->Parent = NULL;
      LinkLines.Parent = NULL;
      // Owner компонентов сзданных на форме является этот компонент, а Parent==NULL, поэтому удаление формы не приведет к удалению созданных компонентов
      delete frmGE;
      Repaint();  // если произошли изменения внешнего вида, то отобразим
      // событие-уведомление об изменении элемента
      if(OnChange) OnChange(this);
   	}
}
#pragma package(smart_init)
