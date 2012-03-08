//---------------------------------------------------------------------------

#ifndef PGridComponentH
#define PGridComponentH
//---------------------------------------------------------------------------
#include "PBase.h"
//---------------------------------------------------------------------------
class PGridComponent : public PBase{
   private:
      typedef PBase sire;                    // родительский компонент
      virtual void __fastcall MenuItemClick(TObject *Sender);         // вызывается при клике на элемент меню
   protected:
      PBase* AddControl(String Class, String ModelClass); // создадим компонент, владельцем которого станем  ModelClass - класс модели
   public:
      __fastcall PGridComponent(TComponent* Owner);
      virtual void Save(ostream &stream);		// сохранить контрол и объект
      virtual void Read(istream &stream);		// загрузить контрол и объект
      vector<PBase*> Objects;			            // список всех объектов подсети
      PLinkComponent LinkLines;                 // связи между выводами
};
//---------------------------------------------------------------------------
#endif
