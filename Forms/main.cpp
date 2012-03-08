//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "main.h"
#include "Scope.h"
#include "ObectsLibrary.h"
#include "kernel.h"			// хидер для работы с моделями
#include <fstream>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
//TfrmMain *frmMain;
//FFT obj((double)T, (unsigned)N);
//Current_Transformer CT1;
/*PSource C;
PLine w1(2);
PConsumer H;
PTransformer_1 T1;
PTransformer_3 T3;
PSingleTransformer T0;
PCurrent_Transformer_1 TA;
PBreaker Q;
//---------------------------------------------------------------------------
PType fa(PElement* obj, double &time){ return 100*sqrt(2)*(exp(-time/0.1)+sin(w*time));}            //5773*sqrt(2)*sin(w*time);    //93897    //8573
PType fb(PElement* obj, double &time){ return 5773*sqrt(2)*sin(w*time-2*M_PI/3);}
PType fc(PElement* obj, double &time){ return 5773*sqrt(2)*sin(w*time+2*M_PI/3);}       //9859     //900
PType I_F(PType &F){  	return 1e7*pow(F,7)+2*F;} // 3e18*pow(F,9)+10*F
PType I_F_TA(PType &F){ return 2e21*pow(F,7)+1e4*F;} // 3e18*pow(F,9)+10*F
PType U_I(PType &I){  	return pow(I,1.7);} */
//---------------------------------------------------------------------------
void err(unsigned num, String text){
   static unsigned cnt=0;
   cnt++;
   //if(cnt>3)
   //   frmMain->Timer->Enabled = false;
	ShowMessage(text);
}
//---------------------------------------------------------------------------
__fastcall TfrmMain::TfrmMain(TComponent* Owner)
	: TForm(Owner), LinkLines(this)
{
   // инициализируем объекта
   grid = NULL;
   LinkLines.Clear();   
   Objects.clear();
	// компонент отображения связей
   LinkLines.Parent = this;
   LinkLines.OnDelete = LinkDelete;
   LinkLines.OnAdd = LinkAdd;
   // если главная форма и если передали параметр - открываем файл
   if(Name == "frmMain") if( ParamCount()>0 )  OpenFile( ParamStr(1) );

   // владелец создаваемых компонентов (может быть переопределен из-вне)
   ComponentOwner = this;
   // исключим мерцание
   //DoubleBuffered = true;

   /*/ параметры генератора
   C.Name = "G";
   C.Ea = fa;
   C.Eb = fb;
   C.Ec = fc;
   C.R_[_A_] = 1.0;
   C.R_[_N_] = 1e-6;
   C.accept();

   // параметры линии
   w1.length = 10;
   w1.R0[_A_] = 0.5;
   w1.R0[_B_] = 0.5;
   w1.R0[_C_] = 0.5;
   w1.L0[_A_] = 0.667/314 ;
   w1.L0[_B_] = 0.667/314 ;
   w1.L0[_C_] = 0.667/314 ;
   w1.M0[_AB_] = 0.267/314 ;
   w1.M0[_BC_] = 0.267/314 ;
   w1.M0[_CA_] = 0.267/314 ;
   //w1.Gz_( 1e-5 );
   //w1.Cz_ ( 2e-7 );
   w1.accept();

   // параметры нагрузки
   H.R_[_A_] = 1322.5/sqrt(2);         //11.0   //1322.5
   H.R_[_B_] = 1322.5/sqrt(2);
   H.R_[_C_] = 1322.5/sqrt(2);
   H.L_[_A_] = 1322.5/314.16/sqrt(2);
   H.L_[_B_] = 1322.5/314.16/sqrt(2);
   H.L_[_C_] = 1322.5/314.16/sqrt(2);
   H.L_[_N_] = 1e-3;   		            // нейтраль заземляем
   H.accept();

   // параметры трансформатора
   T3.Name = "T";
   T3.U1   = 115000;
   T3.U2   = 10500 ;
   T3.S    = 10e6  ;
   T3.dPk  = 58e3  ;
   T3.Uk   = 10.5  ;
   T3.dPx  = 14e3  ;
   T3.Ix   = 0.9   ;
   T3.F_nom= 0.07  ;
   T3.mType = Three_Rod;   //Armored
   T3.Winding_X1 = wGroundedStar;
   T3.Winding_X2 = wTriangle;
   T3.mCurve.func( I_F );
   T3.accept();
   // трансформатор тока
   TA.R1 = 0;
   TA.L1 = 0;
   TA.R2 =0.96;    // учтем нагрузку
   TA.L2 =2.29e-3;
   TA.n1 = 1;
   TA.n2 = 20;
   TA.I1 = 100;
   TA.S = 30;
   TA.K = 10;  // токовая погрешность (класс точности)
   TA.d = 1;   // угловая погрешность
   //TA.mCurve.func( I_F_TA );
   TA.accept();
   // выключатель
   Q.Rmax = 1e6;
   Q.accept(); */

	/*PBreaker *G = new PBreaker();
   grid = new PGrid(G);
   grid->Make_Equation();
   int i=0;
   while (i<1e3){
      grid->calc();
      TIME += smp;
      i++;
   	}
   btnStart->Enabled = true;*/

   /*
   Преобразование Фурье
   double i[3];
   CT1.get(i);
   vector y = obj.calc(i[0]);
   Out->Text = y.mod;
   static int cnt;
   cnt++;
   lbl->Caption = cnt;*/
   /*/работа с матрицами
   const unsigned n=3, m=3;
   Matrix <double> A(n,m), B(n,m), S(m,n);
   double C[n][m], D[n][m], E[m][n];
   for(unsigned i=0; i<n; i++)
   	for(unsigned  j=0; j<m; j++){
   		C[i][j] = 1.0*random(10);
         A[i][j] = C[i][j];
         }
   for(unsigned i=0; i<n; i++)
   	for(unsigned  j=0; j<m; j++){
   		D[i][j] = 1.0*random(10);
         B[i][j] = D[i][j];
         }
   //double det = B.det();
   S = B.inverse();
   S = S*B;
   for(unsigned i=0; i<m; i++)
   	for(unsigned  j=0; j<n; j++){
         E[i][j] = S[i][j];
         }*/
   // работа с моделью
   //TPL w1[1];
   //PGrid grid(w1);
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::mnuScopeClick(TObject *Sender)
{
	// если рассчет разрешен
   if(grid && btnStart->Enabled){
      frmScope.push_back(new TfrmScope(this));
      frmScope[frmScope.size()-1]->obj = grid;
      frmScope[frmScope.size()-1]->Show();
   }else
      err(1,"PowerSystem: перед вызовом осциллографа выполните сборку сети");
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::btnStartClick(TObject *Sender)
{
   Timer->Enabled = !Timer->Enabled;
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::btnPauseClick(TObject *Sender)
{
   Timer->Enabled = false;
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::TimerTimer(TObject *Sender)
{
   for(unsigned i=0;i<50;i++){
      TIME += smp;
      grid->calc();
      }
   lblTime->Caption=FloatToStr(TIME);
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::FormDestroy(TObject *Sender)
{
   // Owner у grid не выставляется, а значит он чиститься автоматически в model.cpp
   // в структуре PObjectList
   //delete grid;  grid = NULL;
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::btnStopClick(TObject *Sender)
{
   Timer->Enabled=false;
   TIME = 0;
   lblTime->Caption=FloatToStr(TIME);
}
//---------------------------------------------------------------------------
// Закрытие осциллогрфов
void TfrmMain::CloseScope(){
   /*if(frmScope.size()>0){
      err(_NOTICE_, "PowerSystem: перед удаление объекта закройте все осциллографы");
      return;
   	}*/
   for(int i=0, n=frmScope.size(); i<n; i++){
      frmScope[i]->Timer->Enabled = false;
      delete frmScope[i]; 	frmScope[i] = NULL;
   	}
   frmScope.clear();
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::BuildGridClick(TObject *Sender)
{
	Timer->Enabled = false;  // останавливаем рассчет
	if(Objects.size()==0){
   	err(_NOTICE_,"PowerSystem: в сети нет ни одного объекта для сборки");
   	return;
      }
   // закрываем все открытые осциллографы
   CloseScope();

   static bool cntBuild = 0;   // номер сборки сети
   //if(cntBuild!=0)
   //   err(_NOTICE_,"PowerSystem: пересборка сети. Значения потенциалов и токов остаются из предшествующего рассчета.");
   // удаляем старую сеть
   delete grid;  grid = NULL;
	// вызовем метод accept() ля всех объектов
   // это необходимо делать до связывания элементов,
   // т.к. она может изменить информацию о внутренних связях
   Pins_Link Link;
   for(int i=0, n=Objects.size(); i<n; i++){
   	Objects[i]->Obj()->accept();
      // найдем все подсети и спросим у них все соединения
      // и выполним accept() у элементов подсети
      if(   (String)Objects[i]->ClassName() == "PGridComponent" )
         SubGrid_Accept( (PGridComponent*)Objects[i], Link);       // можно сразу выяснить у указанной подсети ее связи (с учетом всех подсетей)
                                                                   // Link.add( Objects[i]->Obg()->Links ) - это должно работать, но пока не оттестировано
                                                                   // поэтому не применено, использовать в будущем.
      }
   // соединяем выводы
	for(int i=0, n=LinkLines.Count(); i<n; i++)
   	Link.add(*LinkLines[i].First->Pin(), *LinkLines[i].Second->Pin());
	// создаем сеть
   if(Link.Number()==0 && Objects.size()==1 && Objects[0]->Obj()->ClassName() != "PSubGrid")
      grid = new PGrid((PUnit *)Objects[0]->Obj());
   else if( Objects.size() != 0 && Link.Number()!=0 ){
   	grid = new PGrid(Link);
      }
   else{
   	err(_ERROR_, "PowerSystem: не возможно собрать сеть, т.к. не указано ни одной связи между элементами");
      return;
      }
   if(  grid->Make_Equation() ){
      // уравнения подготовлены без ошибок
      btnStart->Enabled = true;
      btnStart->SetFocus();
      mnuScope->Enabled = true;
      // увеличиваем номер сборки сети (просто информация)
      cntBuild++;
      }
   else{
      // были ошибки при сборке сети
      delete grid; grid = NULL;
      btnStart->Enabled = false;
      mnuScope->Enabled = false;
      }
}
//---------------------------------------------------------------------------
// рекурчивно опросим подсети
void __fastcall TfrmMain::SubGrid_Accept(PGridComponent *ctrl, Pins_Link &Link){
   Link.add( ctrl->Obj()->Internal_Links );
   // найдем сети внутри этой сети
   for(int i=0, n=ctrl->Objects.size();i < n; i++){
      // выполним accept()
      PElement *obj = ctrl->Objects[i]->Obj();
      if(obj)
         obj->accept();
      PGridComponent *pb = dynamic_cast<PGridComponent*>( ctrl->Objects[i] );
      if( pb )
         SubGrid_Accept( pb , Link);
      }
}
//---------------------------------------------------------------------------
/* Событие происходит при изменении элемента */
void __fastcall TfrmMain::UnitChange(TObject *Sender){
   // при изменении свойств объекта, чтобы запустить расчет надо пересобрать сеть
   // после пересборки кнопка старта активируется
   Timer->Enabled = false;
   btnStart->Enabled = false;
   mnuScope->Enabled = false;
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::UnitDelete(TObject *Sender){
   // закрываем все открытые осциллографы
   CloseScope();
   // остановим рассчет и запретим продолжение
   Timer->Enabled = false;
   btnStart->Enabled = false;
   mnuScope->Enabled = false;
   // удаляем компонент из списка
   /*for(int i=0, n=Objects.size(); i<n; i++)
   	if(Objects[i] == (PBase*)Sender)
      	Objects.erase(Objects.begin() + i);*/
   vector<PBase*>::iterator iter = Objects.begin();
    while( iter != Objects.end() )
    {
      if( *iter == (PBase*)Sender){
        Objects.erase( iter );
        break;}
      else
        ++iter;
    }
   // удаляем все связи с этим объектом
   PElement* obj = ((PBase*)Sender)->Obj();
   for(int i=0, n=obj->Out.Count(); i<n; i++)
   	for(int j=0, m=obj->Out[i].Count; j<m; j++)
      	LinkLines.Delete( &obj->Out[i][j] );			// удалим все связи, связанные с выводом
	// перерисуем связи
   LinkLines.Paint();
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::LinkDelete(PLinkComponent *Sender){
   // остановим рассчет и запретим продолжение
   Timer->Enabled = false;
   btnStart->Enabled = false;
   mnuScope->Enabled = false;
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::LinkAdd(PLinkComponent *Sender){	// вызывается при изменении связей
   // остановим рассчет и запретим продолжение
   Timer->Enabled = false;
   btnStart->Enabled = false;
   mnuScope->Enabled = false;
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::PinMouseDown(TObject* Sender, TMouseButton Button, Classes::TShiftState Shift, int X, int Y){
   if(Shift.Contains(ssLeft)&&Shift.Contains(ssShift))   // был нажат шифр
      // отобразим осциллограф
      if( grid && mnuScope->Enabled ){
         frmScope.push_back(new TfrmScope(this));
         frmScope[frmScope.size()-1]->obj = grid;
         String name = ((PPinControl*)Sender)->Pin()->Obj()->Name + ":" + IntToStr(((PPinControl*)Sender)->Pin()->Node());
         String name2 = ((PPinControl*)Sender)->Pin()->Obj()->Name + ":" + IntToStr( (TObject*)((PPinControl*)Sender) -> Pin() ->Obj() -> Grounded_Node );
         // отобразим потенциал узла относительно заземленной узла
         frmScope[frmScope.size()-1]->cmbPot1->AddItem(name, NULL);
         frmScope[frmScope.size()-1]->cmbPot1->ItemIndex = 0;
         frmScope[frmScope.size()-1]->cmbPot2->AddItem(name2, NULL);
         frmScope[frmScope.size()-1]->cmbPot2->ItemIndex = 0;
         frmScope[frmScope.size()-1]->rbVoltage->Checked = true;
         frmScope[frmScope.size()-1]->Show();
      }else
         err(1,"PowerSystem: перед вызовом осциллографа выполните сборку сети");
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::PinDragDrop (System::TObject* Sender, System::TObject* Source, int X, int Y){
   // если перетащили на самого себя, то не добавляем
   if( Sender == Source ) return;
	// сохраняем информацию графической о связи
   Pair_PinConrtol pcon;
   pcon.First = (PPinControl*)Source;
   pcon.Second= (PPinControl*)Sender;
   LinkLines.Add(pcon);
   // отобразим связь
   LinkLines.Paint();
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::FormDragOver(TObject *Sender, TObject *Source,
      int X, int Y, TDragState State, bool &Accept)
{
   Accept = false;
   // если в процессе соединения выводов
   if(PPinControl *pc = dynamic_cast<PPinControl*>(Source)){
   		int from_X = pc->Left + pc->Width/2;
      	int from_Y = pc->Top  + pc->Height/2;
      	TColor color = Canvas->Pen->Color;
      	Canvas->Pen->Color = clBlack;
      	Repaint();                       // очищаем все временные линии
      	Canvas->MoveTo(from_X, from_Y);
      	Canvas->LineTo(X, Y);
      	Canvas->Pen->Color = color;
         Accept = true;
      }
   // перетащили новый объект из библиоьеки компонентов
   else if(Source->ClassNameIs("TTreeView")){
		Accept = true;
   	}
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::FormPaint(TObject *Sender)
{
	// рисуем линии PaintLines()
   LinkLines.Paint();
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::FormDragDrop(TObject *Sender, TObject *Source,
      int X, int Y)
{
   // очищаем все временные линии созданные при соединении выводов
   if(Source->ClassNameIs("PPinControl"))
   	Repaint();
   // перетащили новый объект из библиоьеки компонентов
   else if(Source->ClassNameIs("TTreeView")){
   	TTreeView *obj = (TTreeView*)Source;
   	int n = obj->Selected->AbsoluteIndex;
   	if ( n != -1){
      	TTreeNode *Node = obj->Items->Item[n];
         if(Node != NULL){// && !Node->HasChildren){
            // создаем элемент
      		// сохраняем его в списке и применяем параметры по умолчанию
            TTreeNode *Prev = obj->Selected;
            String Adress = Prev->Text;
   			while ( Prev ){
     				if ( Prev->HasChildren ) Adress = Prev -> Text + "\\" + Adress;
     				Prev = Prev->Parent;
   				}
            AddObject( Adress, X, Y);
            }
        	}
      }
}
//---------------------------------------------------------------------------
void TfrmMain::AddObject(String TypeName, int X, int Y){    // список TypeName - передается из frmObjLib->tvList
   String cName = "";
   String ClassName = "PBase";
   if(TypeName == "Источники\\Генератор трехфазный")           cName = "PSource";
   else if(TypeName == "Трансформаторы\\Силовые\\Трехфазный")  cName = "PTransformer_3";
   else if(TypeName == "Трансформаторы\\Силовые\\Однофазный")  cName = "PTransformer_1";
   else if(TypeName == "Трансформаторы\\Тока\\Однофазный")     cName = "PCurrent_Transformer_1";
   else if(TypeName == "Линии\\Одноцепная без ГТ")             cName = "PLine";
   else if(TypeName == "Потребители\\Трехфазный")              cName = "PConsumer";
   else if(TypeName == "Коммутационная аппаратура\\Выключатель")cName = "PBreaker";
   else if(TypeName == "Подсеть")										{cName = "PSubGrid";    ClassName = "PGridComponent"; }
	// сохраняем его в списке и применяем параметры по умолчанию
   if(cName!=""){
      // добавить компонент-модель
   	CreateControl(ClassName, cName);
      PBase *control = Objects.back();
      control->Left = X - control->Width/2;
      control->Top  = Y - control->Height/2;
   	}
   else{
      err(_WARNING_, "PowerSystem: не определено действие для создания указанного компонента из Библиотеки объектов");
   	}
}
//---------------------------------------------------------------------------
void TfrmMain::CreateControl(String Class, String ModelClass){
      // добавить компонент-модель
      PBase *control;
      TComponent *owner;
      if(ComponentOwner) owner = ComponentOwner;
      else               owner = this;
      if(Class == "PGridComponent")       control = new PGridComponent( owner );
      else                    	         control = new PBase( owner );
      AddControl( control );              // добавить компонент в список отображаемых и установить параметры
      control->ObjType( ModelClass );     // ассоциируем компонент с моделью типа
}
//---------------------------------------------------------------------------
// добавить компонент в список отображаемых и установить параметры
void TfrmMain::AddControl(PBase *control){
   control->Parent = this;
	control->OnChange = UnitChange;
   control->OnPinMouseDown = PinMouseDown;
   control->OnPinDragDrop = PinDragDrop;
   control->OnDelete = UnitDelete;
   Objects.push_back( control );    		// сохраняем в список компонентов
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::mnuObjLibClick(TObject *Sender)
{
      // вызов библиотеки объектов
      TfrmObjLib *frmOL = new TfrmObjLib(this);
      frmOL->Show();
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::FormMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
   // щелкнули правую кнопку
	if(Button == mbRight){
      // может мы указываем на линию связи?
      LinkLines.ShowMenuFor( X, Y );
   	}
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::mnuSaveClick(TObject *Sender)
{
   // останавливаем рассчет
   Timer->Enabled = false;
   /*/ проверка имен всех объектов
   for(int i=0, n=Objects.size(); i<n; i++){
   	String testName = Objects[i]->Obj()->Name;
   	for(int j=i+1; j<n; j++)
   		if( testName == Objects[j]->Obj()->Name ){
         	err(_ERROR_, "PoserSystem: в пределах сети не должно быть двух объектов с одинаковыми именами ("+ testName+"). Сохранение прервано.");
         	return;
         }
      } */
   // выводим диалог и сохраняем
   saveDlg->Filter = ".psf|*.psf|любой файл|*.*";
   if( saveDlg->Execute() ){
     	AnsiString Name = saveDlg->FileName;
      ofstream out(Name.c_str());
      if(out){
      	// сохраняем объекты
         int n = Objects.size();
         out << n << " ";
         for(int i=0; i<n; i++) {
            out << StringForSave( &(String)Objects[i]->ClassName() ) << " ";
            Objects[i]->Save( out );
            }
         //  сохраним информацию о связях
         LinkLines.Save( out );
      	}
    }
}
//---------------------------------------------------------------------------
PElement* TfrmMain::FindObj(String Name){
   for(int i=0, n=Objects.size(); i<n; i++)
      if( Name == Objects[i]->Obj()->Name )
         return Objects[i]->Obj();
   return NULL;
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::mnuOpenClick(TObject *Sender)
{
   // закрываем все открытые осциллографы
   CloseScope();
   // останавливаем рассчет
   Timer->Enabled = false;
   btnStart->Enabled = false;
   mnuScope->Enabled = false;
   // выводим диалог и открываем
   openDlg->Filter = ".psf|*.psf|любой файл|*.*";
   if( openDlg->Execute() ){
      OpenFile(openDlg->FileName);
    }
}
//---------------------------------------------------------------------------
void TfrmMain::OpenFile(String name){
      ifstream in(name.c_str());
      if(in){
         // очищаем существующую сеть
         mnuNewClick(NULL);
      	// сохраняем объекты
         int n;
         in >> n;
         for(int i=0; i<n; i++)  {
            // читаем тип
            char cname[1024];
            in >> cname; String cName = cname;
            // добавляем компонент, указывающий на нулевую модель
            CreateControl( cName, "" );
            // загружаем параметры компонента
            Objects.back()->Read( in );
            }
         // загружаем информацию о связях
         LinkLines.Read( in );
      	// Изменяем заголовок окна
         if( Name.Length() < 32 )
            Caption = name;
         else
            Caption = name.SubString(Name.Length()-32, 32);
         }
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::mnuNewClick(TObject *Sender)
{
   // закрываем все открытые осциллографы
   CloseScope();
	//обнуляем время
   Timer->Enabled = false;
   btnStart->Enabled = false;
   mnuScope->Enabled = false;
   TIME = 0;
   lblTime->Caption = 0;
   // очищаем существующую сеть
   for(int i=0, n=Objects.size(); i<n; i++)
      delete Objects[i];
   Objects.clear();
   LinkLines.Clear();
   // удаляем сеть
   delete grid;  grid = NULL;
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::mnuAboutClick(TObject *Sender)
{
   MessageBoxA(NULL,
					"Power System	\nВерсия:  0.1 ( Violet )	\nАвторы:  Ананьев В., Бычина Е. (с)2012",
               "About",
               MB_OK | MB_ICONINFORMATION);

}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::FormResize(TObject *Sender)
{
   //Bevel->Left     = Width - 161;
	btnStart->Left = Width - 89;
   btnPause->Left  = Width - 65;
   btnStop->Left   = Width - 41;
   lblTime->Left   = Width - 153;
   lblSec->Left    = Width - 113;
}

