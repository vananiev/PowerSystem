//---------------------------------------------------------------------------

#ifndef mainH
#define mainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <Mask.hpp>
#include <Menus.hpp>
#include <ExtCtrls.hpp>
#include "PBase.h"               // ����� �������� ������������-���������� ��� ������
#include "PGridComponent.h"      // ����� ������������-���������� "����"
#include <Dialogs.hpp>
//---------------------------------------------------------------------------
class TfrmMain : public TForm
{
__published:	// IDE-managed Components
   TStaticText *lblTime;
   TMainMenu *MainMenu1;
   TMenuItem *mnuFile;
   TMenuItem *mnuTools;
   TMenuItem *mnuScope;
   TButton *btnStart;
   TButton *btnPause;
   TButton *btnStop;
   TMenuItem *BuildGrid;
	TMenuItem *mnuObjLib;
	TTimer *Timer;
	TMenuItem *mnuSave;
	TMenuItem *mnuOpen;
	TSaveDialog *saveDlg;
   TOpenDialog *openDlg;
   TMenuItem *mnuNew;
	TLabel *lblSec;
	TMenuItem *mnuHelp;
	TMenuItem *mnuAbout;
   void __fastcall mnuScopeClick(TObject *Sender);
   void __fastcall btnStartClick(TObject *Sender);
   void __fastcall btnPauseClick(TObject *Sender);
   void __fastcall TimerTimer(TObject *Sender);
   void __fastcall FormDestroy(TObject *Sender);
   void __fastcall btnStopClick(TObject *Sender);
   void __fastcall BuildGridClick(TObject *Sender);
   void __fastcall FormDragOver(TObject *Sender, TObject *Source, int X,
          int Y, TDragState State, bool &Accept);
   void __fastcall FormPaint(TObject *Sender);
   void __fastcall FormDragDrop(TObject *Sender, TObject *Source, int X,
          int Y);
	void __fastcall mnuObjLibClick(TObject *Sender);
	void __fastcall FormMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall mnuSaveClick(TObject *Sender);
   void __fastcall mnuOpenClick(TObject *Sender);
   void __fastcall mnuNewClick(TObject *Sender);
	void __fastcall mnuAboutClick(TObject *Sender);
	void __fastcall FormResize(TObject *Sender);
private:	// User declarations
   void AddObject(String TypeName, int X, int Y);
	void CreateControl(String Class, String ModelClass); 	// ������� ����� ���������
   void __fastcall PaintLines();									// ������������� ����������� �����
   void __fastcall UnitChange(TObject *Sender);
   void __fastcall UnitDelete(TObject *Sender);
	void __fastcall LinkDelete(PLinkComponent *Sender);   // ���������� ��� �������� �����
   void __fastcall LinkAdd(PLinkComponent *Sender);		// ���������� ��� ��������� ������
   void __fastcall SubGrid_Accept(PGridComponent *ctrl, Pins_Link &Link);
   void CloseScope();						// �������� �������������
   PGrid *grid;								// ��� ����, ������� ����� ����������
   void __fastcall PinMouseDown(TObject* Sender, TMouseButton Button, Classes::TShiftState Shift, int X, int Y);
   void __fastcall PinDragDrop (System::TObject* Sender, System::TObject* Source, int X, int Y);
   vector<PBase*>       Objects;			// ������ ���� �������� �� �����
   PElement* FindObj(String Name);
   PLinkComponent LinkLines;           // ����� ����� ��������
   void OpenFile(String name);			// ������� ��������� ����
   TComponent* ComponentOwner;         // ���� ������, ���������� ���� ��������� ����������� ���������� ��
   void AddControl(PBase *control);		// �������� ��������� � ������ ������������ � ���������� ���������
public:		// User declarations
	__fastcall TfrmMain(TComponent* Owner);
   };
//---------------------------------------------------------------------------
//extern PACKAGE TfrmMain *frmMain;
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#endif
