//---------------------------------------------------------------------------

#ifndef ScopeH
#define ScopeH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Menus.hpp>
#include <Dialogs.hpp>
//---------------------------------------------------------------------------
#include <vector>
#include "model.h"
#include "pngimage.hpp"
//---------------------------------------------------------------------------
class TfrmScope : public TForm
{
__published:	// IDE-managed Components
   TTimer *Timer;
   TPanel *pnlControl;
   TButton *btnStart;
   TButton *btnPause;
   TButton *btnStop;
   TRadioButton *rbCurrent;
   TRadioButton *rbVoltage;
   TComboBox *cmbBranches;
   TComboBox *cmbPot1;
   TComboBox *cmbPot2;
   TPaintBox *Graph;
	TScrollBar *Scroll;
   TPopupMenu *PopupMenu;
   TMenuItem *mnu_Scale;
   TMenuItem *Fit_Graph;
   TMenuItem *Save;
   TSaveDialog *saveDlg;
   TComboBox *cmb_Timer_Value;
   TLabel *Label1;
   void __fastcall TimerTimer(TObject *Sender);
   void __fastcall btnStartClick(TObject *Sender);
   void __fastcall btnPauseClick(TObject *Sender);
   void __fastcall FormResize(TObject *Sender);
   void __fastcall btnStopClick(TObject *Sender);
   void __fastcall FormPaint(TObject *Sender);
   void __fastcall pnlControlResize(TObject *Sender);
	void __fastcall ScrollChange(TObject *Sender);
   void __fastcall FormHide(TObject *Sender);
   void __fastcall FormMouseWheel(TObject *Sender, TShiftState Shift,
          int WheelDelta, TPoint &MousePos, bool &Handled);
   void __fastcall mnu_ScaleClick(TObject *Sender);
   void __fastcall GraphMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
   void __fastcall GraphMouseMove(TObject *Sender, TShiftState Shift,
          int X, int Y);
   void __fastcall GraphMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
   void __fastcall Fit_GraphClick(TObject *Sender);
   void __fastcall SaveClick(TObject *Sender);
   void __fastcall FormDestroy(TObject *Sender);
   void __fastcall FormShow(TObject *Sender);
   void __fastcall cmb_Timer_ValueChange(TObject *Sender);
   void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
private:	// User declarations
public:		// User declarations
   __fastcall TfrmScope(TComponent* Owner);
   PGrid *obj;
   Graphics::TBitmap *out;
   Types::TRect Rect;   // ������ ������� ������������� canvas
   TRect curve_Rect;    // ������������� ��� ������ ������
   // ���������� �������� �������
   PType Xmax;
   PType Xmin;
   PType Ymax;
   PType Ymin;
   vector<PType> X;
   vector<PType> Y;

   bool fromPin;     // ������� ��������� � ������, ����� � ���� ��� �����
   bool fromPinBase; // ������� ��������� � ������, ����� � ���� ��� �����
   PType *Signal;       // ��������� ������
   PType *SignalBase;	// ������������ ������ �������
   PPin* usePin;      // � ����� ������ ������� ���������
   PPin* usePinBase;   // �������� ��������� ������������ ����� ������

   double Grid_Step;        	// ����� �������. ���
   unsigned border_X;         // ���� �� ��� X
   unsigned border_Y;         // ���� �� ��� Y
   bool Scale_To;             // �� X ��� �� Y ����� �������������
   int Mouse_Down_X;          // ���������� ��� ������� �� �����
   int Mouse_Down_Y;          // ���������� ��� ������� �� �����
   PType Mouse_Down_Xmin;    // ���������� ��� ������� �� �����
   PType Mouse_Down_Xmax;    // ���������� ��� ������� �� �����
   PType Mouse_Down_Ymin;    // ���������� ��� ������� �� �����
   PType Mouse_Down_Ymax; // ���������� ��� ������� �� �����
   PType Main_X; PType Main_Y; // ���������� ������� (���������� ������) ����
   bool Move_Graph;     // ������������� ������
   bool Mouse_Scale;    // ������������ ������ ������
};
//---------------------------------------------------------------------------
extern PACKAGE vector<TfrmScope*> frmScope;
//---------------------------------------------------------------------------
#endif
