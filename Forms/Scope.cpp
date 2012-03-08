//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Scope.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
vector<TfrmScope*> frmScope;
extern double TIME;
//---------------------------------------------------------------------------
int scale_X(TfrmScope* Owner, PType x){ return Owner->out->Width*((x - Owner->Xmin)/(Owner->Xmax-Owner->Xmin+1e-300)); }
int scale_Y(TfrmScope* Owner, PType y){ return Owner->out->Height*(1 - (y - Owner->Ymin)/(Owner->Ymax-Owner->Ymin+1e-300)); }
PType scale_X(TfrmScope* Owner, int x){ return Owner->Xmin+(Owner->Xmax-Owner->Xmin)*x/Owner->out->Width; }
PType scale_Y(TfrmScope* Owner, int y){ return Owner->Ymax+(Owner->Ymin-Owner->Ymax)*y/Owner->out->Height; }
//---------------------------------------------------------------------------
// Округление к ближайшему с заданным количеством знаков после запятой.
// value - округляемое значение
// digits - количество знаков после запятой
String RoundTo(const PType &value, int digits) {
   int exp=0;
   PType t = value;
   if(fabs(t)<1e-100) return "0";
   while(fabs(t) < 1){
	   t *= 10;
	   exp--;
      }
   while(fabs(t) > 10){
	   t = t/10;
	   exp++;
      }
   PType temp = pow10(digits-1);
   if(exp != 0)
      return FloatToStr((floor(t*temp+0.5))/temp) + "e" + IntToStr(exp);
   else
      return FloatToStr((floor(t*temp+0.5))/temp);
}
//---------------------------------------------------------------------------
// очистка поля для графика
void Graph_Clear(TfrmScope* Owner){
   static bool Working=false;
   if(Working) return;
   Working=true;
   if(Owner->out->Width <= 0 || Owner->out->Height<=0) return;
   // очищаем
   Owner->out->Canvas->Rectangle(0,0,Owner->out->Width+1,Owner->out->Height+1);
   // запоминаем настройки перьев
   TColor PenColor = Owner->out->Canvas->Pen->Color;
   int PenWidth =  Owner->out->Canvas->Pen->Width;
   TPenStyle PenStyle = Owner->out->Canvas->Pen->Style;
   Owner->out->Canvas->Pen->Color=(TColor)RGB(128,128,128); //Задаем цвет пера
   /*psSolid Сплошная линия
   psDash Штриховая линия
   psDot Пунктирная линия
   psDashDot Штрих-пунктирная линия
   psDashDotDot Линия, чередующая штрих и два пунктира
   psClear Отсутствие линии
   psInsideFrame Сплошная линия, но при Width > 1 допускающая цвета, отличные от палитры Windows*/
   // границы графика
   unsigned border_X = Owner->border_X;
   unsigned border_Y = Owner->border_Y;
   int min_X = border_X,
            max_X = Owner->out->Width-border_Y, // дада- Y
            min_Y = border_Y,
            max_Y = Owner->out->Height-border_Y;
   Owner->out->Canvas->Pen->Width=1;
   Owner->out->Canvas->MoveTo(min_X, min_Y);
   Owner->out->Canvas->LineTo(max_X, min_Y);
   Owner->out->Canvas->LineTo(max_X, max_Y);
   Owner->out->Canvas->LineTo(min_X, max_Y);
   Owner->out->Canvas->LineTo(min_X, min_Y);
   // вывод сетки
   unsigned step =  Owner->Grid_Step;        // шаг сетки
   int X_axis = scale_X(Owner, Owner->Main_Y);      // координаты главной вертикальной оси
   int Y_axis = scale_Y(Owner, Owner->Main_X);      // координаты главной горизонтальной оси
   //второстепенные оси
   Owner->out->Canvas->Pen->Width=1;
   Owner->out->Canvas->Pen->Style = psDot;
   int w = X_axis - step*((X_axis-min_X)/step);
   for(int cnt=0; w < max_X; w=w+step, cnt++){
      Owner->out->Canvas->MoveTo(w, min_Y);
      Owner->out->Canvas->LineTo(w, max_Y);
      if(cnt%2)Owner->out->Canvas->TextOutA(w-16,Owner->out->Height-16, RoundTo((Owner->Xmax-Owner->Xmin)*(w-X_axis)/Owner->out->Width,3).c_str()); //Owner->Xmin+(Owner->Xmax-Owner->Xmin)*((double)w/Owner->out->Width)
      }
   int h = Y_axis - step*((Y_axis-min_Y)/step);
   for(; h < max_Y; h=h+step){
      Owner->out->Canvas->MoveTo(min_X, h);
      Owner->out->Canvas->LineTo(max_X, h);
      Owner->out->Canvas->TextOutA(3,h-8, RoundTo((Owner->Ymin-Owner->Ymax)*(h-Y_axis)/Owner->out->Height, 3).c_str());  //Owner->Ymax-(Owner->Ymax-Owner->Ymin)*((double)h)/Owner->out->Height
      }
   // вывод цены деления по шкале
   TColor FColor = Owner->out->Canvas->Font->Color;
   unsigned FSize = Owner->out->Canvas->Font->Size;
   Owner->out->Canvas->Font->Color=(TColor)RGB(128,128,128);   // цвет шрифта подписей
   Owner->out->Canvas->Font->Size= 0.8 * FSize;
   Owner->out->Canvas->TextOutA(max_X-50,max_Y-FSize-4, RoundTo(scale_X(Owner,(int)step)-scale_X(Owner,(int)0),3).c_str());
   Owner->out->Canvas->TextOutA(min_X+4,min_Y+4, RoundTo(scale_Y(Owner,(int)0)-scale_Y(Owner,(int)step),3).c_str());
   // Минимальные значения
   Owner->out->Canvas->Font->Color=FColor;   // цвет шрифта подписей
   Owner->out->Canvas->Font->Size= FSize;
   //главные оси
   Owner->out->Canvas->Pen->Width=2;   //Задаем ширину  пера
   Owner->out->Canvas->Pen->Style = psSolid;
   if(X_axis>min_X && X_axis<max_X){
      Owner->out->Canvas->MoveTo(X_axis, min_Y);
      Owner->out->Canvas->LineTo(X_axis, max_Y);
      }
   if(Y_axis>min_Y && Y_axis<max_Y){
      Owner->out->Canvas->MoveTo(min_X, Y_axis);
      Owner->out->Canvas->LineTo(max_X, Y_axis);
      }
   // восстанавливаем параметры пера
   Owner->out->Canvas->Pen->Color=PenColor; //Задаем цвет пера
   Owner->out->Canvas->Pen->Width=PenWidth;   //Задаем ширину  пера
   Owner->out->Canvas->Pen->Style = PenStyle;
   Working=false;
}
//---------------------------------------------------------------------------
void Graph_Refresh(TfrmScope* Owner){
   static bool Working=false;
   if(Working) return;
   Working=true;
   if(Owner->out->Width <= 0 || Owner->out->Height<=0) return;
   // прорисовываем сетку
   Graph_Clear(Owner);
   // очищаем
   if(Owner) Owner->Graph->Canvas -> CopyRect(Owner->Rect, Owner->out->Canvas, Owner->Rect); // выводим сетку
   //прорисовываем график
   unsigned n=Owner->X.size();
   unsigned i;
   for (i=0; i<n;i++) if(Owner->X[i] > Owner->Xmin) break;
   if((i-1)<n && i>0)
      Owner->out->Canvas->MoveTo(scale_X(Owner, Owner->X[i-1]), scale_Y(Owner, Owner->Y[i-1]));
   else if(i<n)
      Owner->out->Canvas->MoveTo(scale_X(Owner, Owner->X[i]), scale_Y(Owner, Owner->Y[i]));
   for (; i<n;i++) Owner->out->Canvas->LineTo(scale_X(Owner, Owner->X[i]), scale_Y(Owner, Owner->Y[i]));
   if(Owner) Owner->Graph->Canvas -> CopyRect(Owner->curve_Rect, Owner->out->Canvas, Owner->curve_Rect); // выводим график+сетку
   Working=false;
}
//---------------------------------------------------------------------------
__fastcall TfrmScope::TfrmScope(TComponent* Owner)
   : TForm(Owner)
{
   Xmax=0.01;
   Xmin=0;
   Ymax=5;
   Ymin=-5;
   Signal=NULL;       // выводимый сигнал
   SignalBase=NULL;	// относительно какого сигнала
   out = new Graphics::TBitmap();      // здесь содержится график
   //Задаем цвет пера
   out->Canvas->Pen->Color=clActiveCaption;    // цвет графика
   //Задаем ширину  пера
   out->Canvas->Pen->Width=2;
   // Цвет кисти
   out->Canvas->Brush->Color=clBtnFace;  // цвет фона
   //Цвет текста
   out->Canvas->Font->Color=clBlack;   // цвет шрифта подписей
   //Размер шрифта в точках
   out->Canvas->Font->Size=10;
   //Стиль шрифта
   TFontStyles tFontStyle;
   //Зачеркнутый, наклонный, жирный, подчепкнутый
   //tFontStyle << fsStrikeOut << fsItalic << fsBold << fsUnderline;
   tFontStyle << fsBold;
   out->Canvas->Font->Style =tFontStyle;
   //Имя шрифта
   out->Canvas->Font->Name="Times";
   // сетка графика. шаг
   Grid_Step=40;
   Scroll->Min = 0;
   Scroll->Max = 0;
   // поля графика
   border_X = 50;
   border_Y = 20;
   // масштабируем по X
   Scale_To=true;
   mnu_Scale->Caption = "Масштаб по Y";
   Move_Graph = false;     // перетаскиваем график - нет
   Mouse_Scale = false;    // масштабируем график мышкой - нет
   Main_X = 0; Main_Y = 0; // координаты главных осей
   this->Constraints->MinHeight = 300;
   this->Constraints->MinWidth = 550;
   // время между обновлениями графика
   cmb_Timer_Value->ItemIndex = 1;
}
//---------------------------------------------------------------------------
void __fastcall TfrmScope::FormDestroy(TObject *Sender)
{
   delete out;
}
//---------------------------------------------------------------------------
PType get_signal(TfrmScope* Owner){
   if(Owner->rbCurrent->Checked){ // мерием ток
      if(Owner->fromPin) return Owner->usePin->I();
      else               return *Owner->Signal;
      }
   else{ // напряжение
      PType signal, signal_base;
      if(Owner->fromPin)      signal = Owner->usePin->f();
      else                    signal = *Owner->Signal;
      if(Owner->fromPinBase)  signal_base = Owner->usePinBase->f();
      else                    signal_base = *Owner->SignalBase;
      return (signal - signal_base);
      }
}
//---------------------------------------------------------------------------
void __fastcall TfrmScope::TimerTimer(TObject *Sender)
{
	if(fabs(Xmax-Xmin)>1e-6) Scroll->Max = TIME/(Xmax-Xmin)*100;
   X.push_back(TIME);
   PType signal = get_signal(this);
   Y.push_back(signal);
   out->Canvas->LineTo(scale_X(this, TIME), scale_Y(this, signal));
   Graph->Canvas -> CopyRect(curve_Rect, out -> Canvas, curve_Rect); // обновляем только область графика (обрезая его)
}
//---------------------------------------------------------------------------
void __fastcall TfrmScope::btnStartClick(TObject *Sender)
{
   // если уже был запущен вывод графика то останавливаем
   if( Timer->Enabled ){
      btnPauseClick(Sender);
      return;
      }
   // запустим рассчет
	int tmp;
	if(rbCurrent->Checked){ // мерием ток
      if(cmbBranches->ItemIndex != -1){   // c какого вывода брать значения
         usePin = (PPin*)cmbBranches->Items->Objects[cmbBranches->ItemIndex];
         fromPin = true;
         }
   	else if(TryStrToInt(cmbBranches->Text,tmp) && StrToInt(cmbBranches->Text)>=0 && StrToInt(cmbBranches->Text)<(signed)obj->Branches()){
   	   Signal = obj->I[StrToInt(cmbBranches->Text)];  // график чего строим
         fromPin = false;
         }
      else{ // пытаемся найти объект по имени
         int point=cmbBranches->Text.Pos(":");
         PElement* child = ((PGrid*)obj)->obj_from_name( cmbBranches->Text.SubString(1,point-1) );
         String Number = cmbBranches->Text.SubString(point+1, cmbBranches->Text.Length()-point);
         if(TryStrToInt(Number,tmp) && child!=NULL ){
         	if(tmp>=0 && tmp<(int)child->Branches()){
            	Signal = child->I[tmp];  // график чего строим
            	fromPin = false;
               }
            }
         else
            {ShowMessage("Не верная ветка");return;}
         }
      }
   else if(rbVoltage->Checked){  // Напряжение
      // первый узел
   	if(cmbPot1->ItemIndex != -1){   // c какого вывода брать значения
         usePin = (PPin*)cmbPot1->Items->Objects[cmbPot1->ItemIndex];
         fromPin = true;
         }
   	else if(TryStrToInt(cmbPot1->Text,tmp) && StrToInt(cmbPot1->Text)>=0 && StrToInt(cmbPot1->Text)<(signed)obj->Nodes()){
   	   Signal = obj->f[tmp];
         fromPin = false;
   	   }
      else{ // пытаемся найти объект по имени
         int point=cmbPot1->Text.Pos(":");
         PElement* child = ((PGrid*)obj)->obj_from_name( cmbPot1->Text.SubString(1,point-1) );
         String Number = cmbPot1->Text.SubString(point+1, cmbPot1->Text.Length()-point);
         if(TryStrToInt(Number,tmp) && child!=NULL){
         	if(tmp>=0 && tmp<(int)child->Nodes()){
            	Signal = child->f[tmp];  // график чего строим
            	fromPin = false;
               }
            }
         else
            {ShowMessage("Не верный узел 1");return;}
         }
      // второй узел
      if(cmbPot2->ItemIndex != -1){   // c какого вывода брать значения
         usePinBase = (PPin*)cmbPot2->Items->Objects[cmbPot2->ItemIndex];
         fromPinBase = true;
         }
      else if(TryStrToInt(cmbPot2->Text,tmp) && StrToInt(cmbPot2->Text)>=0 && StrToInt(cmbPot2->Text)<(signed)obj->Nodes()){
   	   SignalBase = obj->f[StrToInt(cmbPot2->Text)];
         fromPinBase = false;
         }
      else{ // пытаемся найти объект по имени
         int point=cmbPot2->Text.Pos(":") ;
         PElement* child = ((PGrid*)obj)->obj_from_name( cmbPot2->Text.SubString(1,point-1) );
         String Number = cmbPot2->Text.SubString(point+1, cmbPot2->Text.Length()-point);
         if(TryStrToInt(Number,tmp) && child!=NULL){
         	if(tmp>=0 && tmp<(int)child->Nodes()){
            	SignalBase = child->f[tmp];  // график чего строим
            	fromPinBase = false;
               }
            }
         else
            {ShowMessage("Не верный узел 2");return;}
         }
   	}
   // обновить график
   Graph_Refresh(this);
   PType signal = get_signal(this);
   out->Canvas->MoveTo(scale_X(this,TIME), scale_Y(this,signal));
   Timer->Interval = StrToInt(cmb_Timer_Value->Text); // время между обновлениями графика
   Timer->Enabled=true;
   rbCurrent->Enabled = false;
   rbVoltage->Enabled = false;
   btnStart->SetFocus();			// установить фокус на кнопке старта
   // оставим видимым то для чего ведется рассчет графика
   if( rbCurrent->Checked ){
      cmbPot1->Visible = false;
      cmbPot2->Visible = false;
      rbVoltage->Visible = false;
      }
   else if( rbVoltage->Checked ){
      cmbBranches->Visible = false;
      rbCurrent->Visible = false;
      }
}
//---------------------------------------------------------------------------
void __fastcall TfrmScope::btnPauseClick(TObject *Sender)
{
   Timer->Enabled=false;
   rbCurrent->Enabled = true;
   rbVoltage->Enabled = true;
   // отобразим нужные компоненты
   cmbBranches->Visible = true;
   rbCurrent->Visible = true;
   cmbPot1->Visible = true;
   cmbPot2->Visible = true;
   rbVoltage->Visible = true;
}
//---------------------------------------------------------------------------
void __fastcall TfrmScope::FormResize(TObject *Sender)
{
   bool tmr=Timer->Enabled;
   Timer->Enabled=false;
   Graph->Top=0; Graph->Left=0;
   Graph->Height=ClientHeight-pnlControl->Height - Scroll->Height;//-47;
   Graph->Width=ClientWidth;//-18; //9
   pnlControl->Left=0;
   pnlControl->Width=ClientWidth;//-18; //9
   pnlControl->Top=ClientHeight-pnlControl->Height;//-40;
   Scroll->Left = 0;
   Scroll->Width=ClientWidth;// - 18;   //10
   Scroll->Top=ClientHeight-pnlControl->Height - Scroll->Height;//-47;
   out->Width = Graph->Width;
   out->Height = Graph->Height;
   Rect.left=0;
   Rect.bottom=0;
   Rect.right=Rect.left+out->Width;
   Rect.top=out->Height;

   curve_Rect.left = border_X;
   curve_Rect.bottom = border_Y;
   curve_Rect.top = out->Height-border_Y;
   curve_Rect.right = out->Width-border_Y;

   // Обновить график
   Graph_Refresh(this);
   Timer->Enabled=tmr;
}
//---------------------------------------------------------------------------
void __fastcall TfrmScope::btnStopClick(TObject *Sender)
{
   Timer->Enabled=false;
   X.clear();
   Y.clear();
   Scroll->Position=0;
   Scroll->Min=0;
   Scroll->Max=0;
   rbCurrent->Enabled = true;
   rbVoltage->Enabled = true;
   // Обновить график
   Graph_Refresh(this);
}
//---------------------------------------------------------------------------
void __fastcall TfrmScope::FormPaint(TObject *Sender)
{
   Graph_Refresh(this);
}
//---------------------------------------------------------------------------
void __fastcall TfrmScope::pnlControlResize(TObject *Sender)
{
   //btnStop->Left = pnlControl->Width-btnStop->Width-5;
   //btnPause->Left = pnlControl->Width-btnStop->Width-btnPause->Width-5;
   //btnStart->Left = pnlControl->Width-btnStop->Width-btnPause->Width-btnStart->Width-5;
}
//---------------------------------------------------------------------------
void __fastcall TfrmScope::ScrollChange(TObject *Sender)
{
   static int ScrollPos=0;   // предыдущее положение
   int delta = Scroll->Position - ScrollPos;
   PType wd = Xmax-Xmin;
   Xmin += delta*wd/100;
   Xmax = Xmin+wd;
	Graph_Refresh(this);
   ScrollPos = Scroll->Position;
}
//---------------------------------------------------------------------------
void __fastcall TfrmScope::FormHide(TObject *Sender)
{
	/*/ остановим вывод графика
   Timer->Enabled = false;
   // ищем свой указатель в векторе и удаляем его
   unsigned n = frmScope.size();
   for (unsigned i=0;i<n;i++)
      if(frmScope[i] == this){
         frmScope.erase(frmScope.begin()+i);
         break;
         }
   /*
   caNone   The form is not allowed to close, so nothing happens.
   caHide   The form is not closed, but just hidden. Your application can still access a hidden form.
   caFree   The form is closed and all allocated memory for the form is freed.
   caMinimize   The form is minimized, rather than closed. This is the default action for MDI child forms.
   */
   //caFree;  
}
//---------------------------------------------------------------------------
void __fastcall TfrmScope::FormMouseWheel(TObject *Sender,
      TShiftState Shift, int WheelDelta, TPoint &MousePos, bool &Handled)
{
   if(Scale_To){
   	PType dX = 0.1*(Xmax-Xmin);
      PType percent = (double)(MousePos.x - Left)/Graph->Width;
      Xmax = Xmax - (1-percent) *dX*WheelDelta/abs(WheelDelta);
      Xmin = Xmin + percent   *dX*WheelDelta/abs(WheelDelta);
   }else{
   	PType dY = 0.1*(Ymax-Ymin);
      PType percent = (double)(MousePos.y - Top)/Graph->Height;
      Ymax = Ymax - (percent)*dY*WheelDelta/abs(WheelDelta);
      Ymin = Ymin + (1-percent)   *dY*WheelDelta/abs(WheelDelta);
      }
   Graph_Refresh(this);
}
//---------------------------------------------------------------------------
void __fastcall TfrmScope::mnu_ScaleClick(TObject *Sender)
{
   Scale_To = !Scale_To;
   if(Scale_To) mnu_Scale->Caption = "Масштаб по Y";
   else mnu_Scale->Caption = "Масштаб по X";
}
//---------------------------------------------------------------------------
void __fastcall TfrmScope::GraphMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
   if(Button == mbLeft){ // правой кнопкой мышки перетаскиваем график
      Mouse_Down_X = X;
      Mouse_Down_Y = Y;
      Mouse_Down_Xmin = Xmin;
      Mouse_Down_Xmax = Xmax;
      Mouse_Down_Ymin = Ymin;
      Mouse_Down_Ymax = Ymax;
      Mouse_Scale = false;
      Move_Graph = true;     // перетаскиваем график
      Graph->Cursor = crHandPoint;  // появляется курсор-рука
      }
   else if (Button == mbMiddle){  // по нажатию на колесико - мастшабируем мышью
      Mouse_Down_X = X;
      Mouse_Down_Y = Y;
      Move_Graph = false;
      Mouse_Scale = true;
      }
}
//---------------------------------------------------------------------------
void __fastcall TfrmScope::GraphMouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{
   if(Move_Graph){    // если передвигаем график
      PType delta_X = (Xmax-Xmin)*(X-Mouse_Down_X)/Graph->Width;
      Xmin = Mouse_Down_Xmin - delta_X;
      Xmax = Mouse_Down_Xmax - delta_X;
      PType delta_Y = (Ymax-Ymin)*(Y-Mouse_Down_Y)/Graph->Height;
      Ymin = Mouse_Down_Ymin + delta_Y;
      Ymax = Mouse_Down_Ymax + delta_Y;
      Graph_Refresh(this);
      }
   if(Mouse_Scale){   //Если масштабируем
      // вырисовываем прямоугольник
      TColor C = Graph->Canvas->Pen->Color;
      int W = Graph->Canvas->Pen->Width;
      Graph->Canvas->Pen->Color = (TColor)RGB(200,200,200);
      Graph->Canvas->Pen->Width = 1;
      Graph_Refresh(this);
      Graph->Canvas->MoveTo(Mouse_Down_X, Mouse_Down_Y);
      Graph->Canvas->LineTo(Mouse_Down_X, Y);
      Graph->Canvas->LineTo(X, Y);
      Graph->Canvas->LineTo(X, Mouse_Down_Y);
      Graph->Canvas->LineTo(Mouse_Down_X, Mouse_Down_Y);
      Graph->Canvas->Pen->Color = C;
      Graph->Canvas->Pen->Width = W;
      }

}
//---------------------------------------------------------------------------
void __fastcall TfrmScope::GraphMouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
   if(Button == mbLeft){ // правой кнопкой мышки перетаскиваем график
      Move_Graph = false;     // перетаскиваем график
      Graph->Cursor = crDefault;  // появляется курсор-стрелка
      }
   else if (Button == mbMiddle){  // по нажатию на колесико - мастшабируем мышью
      if(Mouse_Down_X > X) {int t=X; X=Mouse_Down_X; Mouse_Down_X=t;}
      if(Mouse_Down_Y > Y) {int t=Y; Y=Mouse_Down_Y; Mouse_Down_Y=t;}
      PType X_min = scale_X(this,(int) Mouse_Down_X); // Xmin+(Xmax-Xmin)*Mouse_Down_X/Graph->Width;
      PType X_max = scale_X(this,(int) X );            //Xmin+(Xmax-Xmin)*X/Graph->Width;
      PType Y_max = scale_Y(this,(int) Mouse_Down_Y); //Ymax+(Ymin-Ymax)*Mouse_Down_Y/Graph->Height;
      PType Y_min = scale_Y(this,(int) Y);            //Ymax+(Ymin-Ymax)*Y/Graph->Height;
      // добавим поля
      X_min -= 0.15*(X_max-X_min);
      X_max += 0.15*(X_max-X_min);
      Y_min -= 0.15*(Y_max-Y_min);
      Y_max += 0.15*(Y_max-Y_min);
      Xmin = X_min;
      Xmax = X_max;
      Ymin = Y_min;
      Ymax = Y_max;
      Mouse_Scale = false;
      Graph_Refresh(this);
      }
}
//---------------------------------------------------------------------------
void __fastcall TfrmScope::Fit_GraphClick(TObject *Sender)
{
   PType x_min, x_max, y_min, y_max;
   unsigned n = X.size();
   if(n>0){
      // найдем максимумы и минимумы
      x_min=X[0];x_max=X[0]; y_min=Y[0];y_max=Y[0];
      for(unsigned i=0; i<n; i++){
         if(X[i]<x_min)
            x_min=X[i];
         if(X[i]>x_max)
            x_max=X[i];
         if(Y[i]<y_min)
            y_min=Y[i];
         if(Y[i]>y_max)
            y_max=Y[i];
         }
      // добавим поля
      x_min -= 0.15*(x_max-x_min);
      x_max += 0.15*(x_max-x_min);
      y_min -= 0.15*(y_max-y_min);
      y_max += 0.15*(y_max-y_min);
      if(x_min == x_max) x_max += 1e-100;
      if(y_min == y_max) y_max += 1e-100;
      // установим
      Xmin = x_min;
      Xmax = x_max;
      Ymin = y_min;
      Ymax = y_max;
      // перерисуем график
      Graph_Refresh(this);
      }
}
//---------------------------------------------------------------------------
void __fastcall TfrmScope::SaveClick(TObject *Sender)
{
   saveDlg->Filter = ".png|*.png|.bmp|*.bmp|любой файл|*.*";
   if( saveDlg->Execute() ){
     Graphics::TBitmap*  bmp = new Graphics::TBitmap();
     bmp->Width = out->Width;
     bmp->Height = out->Height;
     bmp->Canvas->CopyRect(Rect, out->Canvas, Rect);
     AnsiString Name = saveDlg->FileName;
     if(saveDlg->FilterIndex==2)   // .bmp
      bmp->SaveToFile(Name);
     else if(saveDlg->FilterIndex==1  || saveDlg->FilterIndex==3){ // .png
      TPNGObject* png = new TPNGObject;
      png->Assign(bmp);
      png->SaveToFile(Name);
      delete png;
      }
     delete bmp;
    }

}
//---------------------------------------------------------------------------

void __fastcall TfrmScope::FormShow(TObject *Sender)
{
   cmbBranches->Items->Clear();
   cmbPot1->Items->Clear();
   cmbPot2->Items->Clear();
   // набиваем списки ветвей и узлов значениями
   for(unsigned i=0, n=obj->Out.Count(); i<n; i++){ // цикл по выводам
      Pins* pins = &obj->Out[i];
      for(unsigned k=0, cnt_pins = pins->Count; k<cnt_pins; k++){  // цикл по выводам объекта
      	PPin *pin = &(*pins)[k];
         String Name = pins->Name + ":";
         switch(k){
         	case _N_:    Name += "N"; break;
            case _A_:    Name += "A"; break;
            case _B_:    Name += "B"; break;
            case _C_:    Name += "C"; break;
            }
         cmbBranches->AddItem(Name, (TObject*)pin);
      	cmbPot1->AddItem(Name, (TObject*)pin);
         cmbPot2->AddItem(Name, (TObject*)pin);
         }
      }
}
//---------------------------------------------------------------------------

void __fastcall TfrmScope::cmb_Timer_ValueChange(TObject *Sender)
{
   Timer->Interval = StrToInt(cmb_Timer_Value->Text); // время между обновлениями графика   
}
//---------------------------------------------------------------------------

void __fastcall TfrmScope::FormClose(TObject *Sender, TCloseAction &Action)
{
	// остановим вывод графика
   Timer->Enabled = false;
   // ищем свой указатель в векторе и удаляем его
   unsigned n = frmScope.size();
   for (unsigned i=0;i<n;i++)
      if(frmScope[i] == this){
         frmScope.erase(frmScope.begin()+i);
         break;
         }
   /*
   caNone   The form is not allowed to close, so nothing happens.
   caHide   The form is not closed, but just hidden. Your application can still access a hidden form.
   caFree   The form is closed and all allocated memory for the form is freed.
   caMinimize   The form is minimized, rather than closed. This is the default action for MDI child forms.
   */
   Action = caFree;   
}
//---------------------------------------------------------------------------

