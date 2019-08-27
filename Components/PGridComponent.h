//---------------------------------------------------------------------------

#ifndef PGridComponentH
#define PGridComponentH
//---------------------------------------------------------------------------
#include "PBase.h"
//---------------------------------------------------------------------------
class PGridComponent : public PBase{
   private:
      typedef PBase sire;                    // ������������ ���������
      virtual void __fastcall MenuItemClick(TObject *Sender);         // ���������� ��� ����� �� ������� ����
   protected:
      PBase* AddControl(String Class, String ModelClass); // �������� ���������, ���������� �������� ������  ModelClass - ����� ������
   public:
      __fastcall PGridComponent(TComponent* Owner);
      virtual void Save(ostream &stream);		// ��������� ������� � ������
      virtual void Read(istream &stream);		// ��������� ������� � ������
      vector<PBase*> Objects;			            // ������ ���� �������� �������
      PLinkComponent LinkLines;                 // ����� ����� ��������
};
//---------------------------------------------------------------------------
#endif
