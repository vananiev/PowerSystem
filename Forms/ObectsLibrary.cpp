//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ObectsLibrary.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmObjLib *frmObjLib;
//---------------------------------------------------------------------------
__fastcall TfrmObjLib::TfrmObjLib(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TfrmObjLib::FormClose(TObject *Sender,
      TCloseAction &Action)
{
   /*
   caNone   The form is not allowed to close, so nothing happens.
   caHide   The form is not closed, but just hidden. Your application can still access a hidden form.
   caFree   The form is closed and all allocated memory for the form is freed.
   caMinimize   The form is minimized, rather than closed. This is the default action for MDI child forms.
   */
   caFree;
}
//---------------------------------------------------------------------------
void __fastcall TfrmObjLib::FormActivate(TObject *Sender)
{
	AlphaBlend = false;	
}
//---------------------------------------------------------------------------

void __fastcall TfrmObjLib::FormDeactivate(TObject *Sender)
{
	AlphaBlend = true;		
}
//---------------------------------------------------------------------------

