//---------------------------------------------------------------------------

#ifndef frmPropH
#define frmPropH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Grids.hpp>
#include <ValEdit.hpp>
#include <PBase.h>
#include <Grids.hpp>
#include <ValEdit.hpp>
//---------------------------------------------------------------------------
class TfrmProperty : public TForm
{
__published:	// IDE-managed Components
   TValueListEditor *Fields;
   void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
   void __fastcall FormShow(TObject *Sender);
   void __fastcall FormResize(TObject *Sender);
private:	// User declarations
public:		// User declarations
   __fastcall TfrmProperty(TComponent* Owner);
   PBase *Obj;
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmProperty *frmProperty;
//---------------------------------------------------------------------------
#endif
