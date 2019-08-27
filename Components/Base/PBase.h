//---------------------------------------------------------------------------
#ifndef PBaseH
#define PBaseH
//---------------------------------------------------------------------------
#include <SysUtils.hpp>
#include <Classes.hpp>
#include <Controls.hpp>
#include <ExtCtrls.hpp>
#include <vector>
#include "model.h"
//---------------------------------------------------------------------------
// ������� �������� ��� ����������
PType Ef(PElement* obj, double &time);
PType Eb(PElement* obj, double &time);
PType Ec(PElement* obj, double &time);
//---------------------------------------------------------------------------
//typedef void __fastcall (__closure *TNotifyEvent)(System::TObject* Sender);
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
class PPinControl;
class PACKAGE PBase : public TGraphicControl  // TGraphicControl ��������� ������� ��������� ����������
{
private:
   typedef TGraphicControl inherited;
	DYNAMIC void __fastcall MouseDown(TMouseButton Button, Classes::TShiftState Shift, int X, int Y);
	DYNAMIC void __fastcall MouseMove(Classes::TShiftState Shift, int X, int Y);
	DYNAMIC void __fastcall MouseUp(TMouseButton Button, Classes::TShiftState Shift, int X, int Y);
   std::vector<int> _Pins_Cnt;
   int MouseDownPos_X, MouseDownPos_Y; // ��� ���� ������ ������ ����
   bool _Drag; // ���� ��������
   int _LEFT_, _RIGHT_, _TOP_,_BOTTOM_;   // ����� ����� ������ ������� � �����
                                          // ������� ������ ���� ���������
                                          // (��������� ��� �������� ����������)
   PPinControl* PinControl (PPin *pin);   // ���������� ����.������ ��������������� � �������
   void __fastcall PinMouseDown(TObject* Sender, TMouseButton Button, Classes::TShiftState Shift, int X, int Y);
   void __fastcall PinDragDrop (System::TObject* Sender, System::TObject* Source, int X, int Y);
   void __fastcall PinDragOver(System::TObject* Sender, System::TObject* Source, int X, int Y, TDragState State, bool &Accept);
   int __fastcall XtoParent(int x);
   int __fastcall YtoParent(int y);
protected:
   TPopupMenu *Menu; // ���������� ����
   virtual void __fastcall MenuItemClick(TObject *Sender);         // ���������� ��� ����� �� ������� ����
   PElement *_Obj;
   virtual void __fastcall SetParent(TWinControl* AParent);
   void createPins();                     // ������� ������� ����.������������� �������
   vector<PPinControl*> pList;            // ������ ����������� �������� �������
public:
    //typedef TCustomControl inherited;
    //void __fastcall CreateParams(TCreateParams &Params);                  // overriding TWinControl::CreateParams
    //void __fastcall SetBounds( int left, int top, int width, int height); // overriding TWinControl::SetBounds
    //void __fastcall WndProc( Messages::TMessage &message);
   __fastcall PBase(TComponent* Owner);
   __fastcall ~PBase();
   virtual void __fastcall Paint(void);   // ���������� ��������� ��� ���������� ����������
   __property DragKind  = {default=0};
	__property DragMode  = {default=0};
   __property OnMouseDown ;
	__property OnMouseMove ;
	__property OnMouseUp ;
   __property OnEndDrag = {default=0};
   __property OnStartDrag = {default=0};
   TNotifyEvent OnChange;
   TMouseEvent OnPinMouseDown;            // ������� ��������� ��� ����� �� ������
   TDragDropEvent OnPinDragDrop;          // ��������� � ����� �������������� �� ������
   TDragOverEvent OnPinDragOver;          // ��������� � ����� �������������� �� ������
   void Obj(PElement *obj);               // ������������ ������� ������ � �����������
   PElement * Obj();           		      // ������������ ������� ������ � �����������
   PElement* ObjType(String ClassName);   // ������� ������ � ����������� ��� � �����������
   TNotifyEvent OnDelete;						// ���������� ��� ��������
   //virtual void SaveComponentOnly(ostream &stream);		// ����������� ������ ��������� ��� ������
   //virtual void ReadComponentOnly(istream &stream);      // ������ ������ ��������� ��� ������
   virtual void Save(ostream &stream);		// ��������� ������� � ������ (saveModel=false ����������� ������ ��������� ��� ������ )
   virtual void Read(istream &stream);		// ��������� ������� � ������
__published:
};
//---------------------------------------------------------------------------
// Power Pin Control - ����������� ������������� ������ �� ����������
class PPinControl : public TShape{
private:
   PPin* _Pin;
   DYNAMIC void __fastcall MouseDown(TMouseButton Button, Classes::TShiftState Shift, int X, int Y);
   DYNAMIC void __fastcall DragOver(System::TObject* Source, int X, int Y, TDragState State, bool &Accept);
   DYNAMIC void __fastcall DragDrop (System::TObject* Source, int X, int Y);
   int savedWidth, savedHeight; // ��� ����������� ��������
   TColor savedColor;
protected:
   void __fastcall MouseEnter(TMessage& Msg);
   void __fastcall MouseLeave(TMessage& Msg);
   #pragma warn -8027
   BEGIN_MESSAGE_MAP
      VCL_MESSAGE_HANDLER(CM_MOUSEENTER,TMessage,MouseEnter);
      VCL_MESSAGE_HANDLER(CM_MOUSELEAVE,TMessage,MouseLeave);
   END_MESSAGE_MAP(TShape);
   #pragma warn -8027
public:
   __fastcall PPinControl(TComponent* Owner);
   __fastcall ~PPinControl();
   PPin* Pin();            // ���������� �����
   void  Pin(PPin* pin);   //  ��������� �����
   TMouseEvent    OnMouseDown;         // ������� ��������� ��� ����� �� ������
   TDragOverEvent OnDragOver;          // ��������� � ����� �������������� �� ������
   TDragDropEvent OnDragDrop;          // ��������� � ����� �������������� �� ������
   typedef void __fastcall (__closure *PinNotifyEvent)(PPinControl* Sender);
   PinNotifyEvent 	OnDelete;				// ���������� ��� ��������
};
//---------------------------------------------------------------------------
class Pair_PinConrtol{
	public:
      PPinControl* First;
      PPinControl* Second;
};
//---------------------------------------------------------------------------
class PLinkComponent{
		vector<Pair_PinConrtol> Links;
      void __fastcall MenuItemClick(TObject *Sender);
      TPopupMenu* Menu;							// ���� ������ �����
      int menuFor;								// ��� ������ �������� ���������� ����
      void __fastcall PinDelete(PPinControl *pc);	// ���������� ��� �������� ���������� ������
      void Change();								// ���������� ��� ��������� ����������� �����
   public:
      void Add   (Pair_PinConrtol link);  // �������� ����� ����
      void Delete(PPinControl* pc);			// ������� �����, ��� ������������ ��������� �����
 		void Delete(PPin* pin);					// ������� �����, ��� ������������ ��������� �����
      Pair_PinConrtol& operator[](int i);
      int Count();
      int FindAtPos(int X, int Y);	      // ����� ����� �� �����������
      PLinkComponent(TComponent *owner);
      PLinkComponent operator=(const PLinkComponent &Obj);// ������������
      ~PLinkComponent();
      void ShowMenuFor(int LinkNumber);	// �������� ���� ��� �������� �����
      void ShowMenuFor(int X, int Y);		// �������� ���� ��� ����� � ��������� ������������
      void __fastcall Paint();            // ������������� �����
      TForm* Parent;							   // ��������� �� ������� ������
      TComponent* Owner;							   // ���������-��������
      void Save(ostream &stream);			// ��������� ������� � �����
      void Read(istream &stream);			// ��������� ������� � �����
      void __fastcall (__closure *OnDelete)(PLinkComponent* Sender);  // ���������� ��� �������� �����
      void __fastcall (__closure *OnAdd)(PLinkComponent* Sender);  // ���������� ��� ��������� ������
      void Clear();
};
//---------------------------------------------------------------------------
#endif
