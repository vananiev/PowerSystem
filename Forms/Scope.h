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
   Types::TRect Rect;   // какого размера прямоугольник canvas
   TRect curve_Rect;    // прямоугольник для вывода кривой
   // предельные значения графика
   PType Xmax;
   PType Xmin;
   PType Ymax;
   PType Ymin;
   vector<PType> X;
   vector<PType> Y;

   bool fromPin;     // снимаем показания с вывода, иначе с узла или ветки
   bool fromPinBase; // снимаем показания с вывода, иначе с узла или ветки
   PType *Signal;       // выводимый сигнал
   PType *SignalBase;	// относительно какого сигнала
   PPin* usePin;      // с этого вывода снимаем показания
   PPin* usePinBase;   // измеряем потенциал относительно этого вывода

   double Grid_Step;        	// сетка графика. шаг
   unsigned border_X;         // поле по оси X
   unsigned border_Y;         // поле по оси Y
   bool Scale_To;             // по X или по Y будем мастабировать
   int Mouse_Down_X;          // координаты при нажании по мышке
   int Mouse_Down_Y;          // координаты при нажании по мышке
   PType Mouse_Down_Xmin;    // координаты при нажании по мышке
   PType Mouse_Down_Xmax;    // координаты при нажании по мышке
   PType Mouse_Down_Ymin;    // координаты при нажании по мышке
   PType Mouse_Down_Ymax; // координаты при нажании по мышке
   PType Main_X; PType Main_Y; // координаты главных (выделенных жирным) осей
   bool Move_Graph;     // перетаскиваем график
   bool Mouse_Scale;    // масштабируем график мышкой
};
//---------------------------------------------------------------------------
extern PACKAGE vector<TfrmScope*> frmScope;
//---------------------------------------------------------------------------
#endif
