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
// формулы рассчета ЭДС источников
PType Ef(PElement* obj, double &time);
PType Eb(PElement* obj, double &time);
PType Ec(PElement* obj, double &time);
//---------------------------------------------------------------------------
//typedef void __fastcall (__closure *TNotifyEvent)(System::TObject* Sender);
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
class PPinControl;
class PACKAGE PBase : public TGraphicControl  // TGraphicControl позволяет сделать компонент прозрачным
{
private:
   typedef TGraphicControl inherited;
	DYNAMIC void __fastcall MouseDown(TMouseButton Button, Classes::TShiftState Shift, int X, int Y);
	DYNAMIC void __fastcall MouseMove(Classes::TShiftState Shift, int X, int Y);
	DYNAMIC void __fastcall MouseUp(TMouseButton Button, Classes::TShiftState Shift, int X, int Y);
   std::vector<int> _Pins_Cnt;
   int MouseDownPos_X, MouseDownPos_Y; // где была нажата кнопка мыши
   bool _Drag; // флаг нереноса
   int _LEFT_, _RIGHT_, _TOP_,_BOTTOM_;   // номер какой группы выводов с какой
                                          // стороны должен быть отображен
                                          // (необходим для поворота компонента)
   PPinControl* PinControl (PPin *pin);   // возвращает граф.объект ассоциированный с выводом
   void __fastcall PinMouseDown(TObject* Sender, TMouseButton Button, Classes::TShiftState Shift, int X, int Y);
   void __fastcall PinDragDrop (System::TObject* Sender, System::TObject* Source, int X, int Y);
   void __fastcall PinDragOver(System::TObject* Sender, System::TObject* Source, int X, int Y, TDragState State, bool &Accept);
   int __fastcall XtoParent(int x);
   int __fastcall YtoParent(int y);
protected:
   TPopupMenu *Menu; // контектное меню
   virtual void __fastcall MenuItemClick(TObject *Sender);         // вызывается при клике на элемент меню
   PElement *_Obj;
   virtual void __fastcall SetParent(TWinControl* AParent);
   void createPins();                     // создать объекты граф.представления выводов
   vector<PPinControl*> pList;            // список графических объектов выводов
public:
    //typedef TCustomControl inherited;
    //void __fastcall CreateParams(TCreateParams &Params);                  // overriding TWinControl::CreateParams
    //void __fastcall SetBounds( int left, int top, int width, int height); // overriding TWinControl::SetBounds
    //void __fastcall WndProc( Messages::TMessage &message);
   __fastcall PBase(TComponent* Owner);
   __fastcall ~PBase();
   virtual void __fastcall Paint(void);   // вызывается родителем при просисовке компонента
   __property DragKind  = {default=0};
	__property DragMode  = {default=0};
   __property OnMouseDown ;
	__property OnMouseMove ;
	__property OnMouseUp ;
   __property OnEndDrag = {default=0};
   __property OnStartDrag = {default=0};
   TNotifyEvent OnChange;
   TMouseEvent OnPinMouseDown;            // событие возникает при клике по выводу
   TDragDropEvent OnPinDragDrop;          // возникает в конце перетаскивания на объект
   TDragOverEvent OnPinDragOver;          // возникает в конце перетаскивания на объект
   void Obj(PElement *obj);               // ассоцииируем силовой объект с компонентом
   PElement * Obj();           		      // ассоцииируем силовой объект с компонентом
   PElement* ObjType(String ClassName);   // создаем объект и ассоциируем его с компонентом
   TNotifyEvent OnDelete;						// Вызывается при удалении
   //virtual void SaveComponentOnly(ostream &stream);		// сохраняется только компонент без модели
   //virtual void ReadComponentOnly(istream &stream);      // читаем только компонент без модели
   virtual void Save(ostream &stream);		// сохранить контрол и объект (saveModel=false сохраняется только компонент без модели )
   virtual void Read(istream &stream);		// загрузить контрол и объект
__published:
};
//---------------------------------------------------------------------------
// Power Pin Control - Графическое представление вывода от устройства
class PPinControl : public TShape{
private:
   PPin* _Pin;
   DYNAMIC void __fastcall MouseDown(TMouseButton Button, Classes::TShiftState Shift, int X, int Y);
   DYNAMIC void __fastcall DragOver(System::TObject* Source, int X, int Y, TDragState State, bool &Accept);
   DYNAMIC void __fastcall DragDrop (System::TObject* Source, int X, int Y);
   int savedWidth, savedHeight; // для запоминания размеров
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
   PPin* Pin();            // возвращаем вывод
   void  Pin(PPin* pin);   //  установим вывод
   TMouseEvent    OnMouseDown;         // событие возникает при клике по выводу
   TDragOverEvent OnDragOver;          // возникает в конце перетаскивания на объект
   TDragDropEvent OnDragDrop;          // возникает в конце перетаскивания на объект
   typedef void __fastcall (__closure *PinNotifyEvent)(PPinControl* Sender);
   PinNotifyEvent 	OnDelete;				// Вызывается при удалении
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
      TPopupMenu* Menu;							// меню данной связи
      int menuFor;								// для какого элемента обображено меню
      void __fastcall PinDelete(PPinControl *pc);	// вызывается при удалении компонента вывода
      void Change();								// вызывается при изменении компонентов связи
   public:
      void Add   (Pair_PinConrtol link);  // добавить новую пару
      void Delete(PPinControl* pc);			// удалить связи, где присутствует указанный вывод
 		void Delete(PPin* pin);					// удалить связи, где присутствует указанный вывод
      Pair_PinConrtol& operator[](int i);
      int Count();
      int FindAtPos(int X, int Y);	      // поиск линии по координатам
      PLinkComponent(TComponent *owner);
      PLinkComponent operator=(const PLinkComponent &Obj);// присваивание
      ~PLinkComponent();
      void ShowMenuFor(int LinkNumber);	// показать меню для заданной связи
      void ShowMenuFor(int X, int Y);		// показать меню для связи с заданными координатами
      void __fastcall Paint();            // прорисовывает связи
      TForm* Parent;							   // компонент на котором рисуем
      TComponent* Owner;							   // компонент-владелец
      void Save(ostream &stream);			// сохранить контрол и связи
      void Read(istream &stream);			// загрузить контрол и связи
      void __fastcall (__closure *OnDelete)(PLinkComponent* Sender);  // Вызывается при удалении связи
      void __fastcall (__closure *OnAdd)(PLinkComponent* Sender);  // вызывается при изменении связей
      void Clear();
};
//---------------------------------------------------------------------------
#endif
