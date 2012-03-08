//---------------------------------------------------------------------------

#ifndef ObectsLibraryH
#define ObectsLibraryH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------
class TfrmObjLib : public TForm
{
__published:	// IDE-managed Components
	TTreeView *tvList;
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall FormActivate(TObject *Sender);
	void __fastcall FormDeactivate(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TfrmObjLib(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmObjLib *frmObjLib;
//---------------------------------------------------------------------------
#endif
