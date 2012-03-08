//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "frmProp.h"
#include "main.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmProperty *frmProperty;
//---------------------------------------------------------------------------
__fastcall TfrmProperty::TfrmProperty(TComponent* Owner)
   : TForm(Owner)
{

}
//---------------------------------------------------------------------------
void __fastcall TfrmProperty::FormClose(TObject *Sender,
      TCloseAction &Action)
{
   PElement* model = Obj->Obj();
   // сохраняем изменения перед закрытием
   if(model){
      for(int i=0, n=model->Fields.size(); i<n;i++){
         model->Fields[i].Value(   Fields->Values[ model->Fields[i].Name ]   );
         }
      model->accept(); // применяем изменения
      // сохраняем способ рассчета ЭДС - грязный хук
      if(PSource *src = dynamic_cast<PSource*>(model))
      	src->sourceParams.Change(Fields->Values[ "ЭДС" ]);
      }
}
//---------------------------------------------------------------------------
void __fastcall TfrmProperty::FormShow(TObject *Sender)
{
   PElement* model = Obj->Obj();
   // перебираем поля которые надо вывести
   if(Obj){
      for(int i=0, n=model->Fields.size(); i<n;i++){
         Fields->Values[ model->Fields[i].Name ] =  model->Fields[i].Value();
         bool _ro = false;
         if(model->Fields[i].Mode == ro)  _ro = true;
         Fields->ItemProps[model->Fields[i].Name]->ReadOnly = _ro;
         }
   	// сохраняем способ рассчета ЭДС - грязный хук
     	if(PSource *src = dynamic_cast<PSource*>(model)){
      	String p = src->sourceParams.Get();
      	Fields->Values["ЭДС"] = p;
			Valedit::TItemProp *ip = Fields->ItemProps["ЭДС"];
			ip->EditStyle = esPickList;
   		ip->ReadOnly = false;
         if(p!="") ip->PickList->Add( p );
         ip->PickList->Add("неизменно:Значение");
			ip->PickList->Add("sin:Частота:Значение");
   		ip->PickList->Add("нессиметрия:Частота:ПП:ОП:НП");
         }
   	// подгоняем размер
   	Fields->Height = (Fields->DefaultRowHeight+2)*Fields->RowCount;
   	if( Fields->Height > Screen->Height-50) Fields->Height = Screen->Height-50;
      }
}

void __fastcall TfrmProperty::FormResize(TObject *Sender)
{
   Fields->Width  = Width - 16;
   Height = Fields->Height + 40;
}
//---------------------------------------------------------------------------

