//---------------------------------------------------------------------------
#ifndef modelH
#define modelH
#include "Matrix.h"
#include <mem.h>
#include <math.h>
#include <vector.h>
#include <system.hpp>
#include "Dynamic_Array.h"
#include <string.h>
#include <SysUtils.hpp>
#include <fstream>

//using namespace std;
extern double smp;   	// время между пересчетами цепи
extern double TIME;  	// текущее время
extern void err(unsigned, String);
//---------------------------------------------------------------------------
typedef long double PType;		// тип результата вычислений
//---------------------------------------------------------------------------
class PPin;
class Pins;
class PElement;
class Pair_Pin;
class Pins_Link;
class PCurve;
class PElement;
class PGrid;
//---------------------------------------------------------------------------
// экспортируемые функции
char *StringForSave(String *s);   // преобразование строки для вывода в поток
char *StringForSave(char *s);    // преобразование строки для вывода в поток
void SaveObj(PElement *Obj, ostream &stream);               // сохранение объекта в поток
PElement* CreateObj(istream &stream);                       // создание объекта по данным из потока
PElement* CreateObj(String ClassName, vector<void*> Params);// создать модель по имени класса и переданным параметрам, которые передаются в конструктор модели
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/* Обозначения фаз */
enum Phase       {_N_, _A_, _B_, _C_};
enum interPhase  {_AB_, _BC_, _CA_};
//---------------------------------------------------------------------------
/* Типы трансформаторов */
enum Transf_Type {
};
//---------------------------------------------------------------------------
/* Типы магнитопроводов */
enum MagnCircuit_Type {
   Three_Rod = 0,       // трехстержневой
   Armored   = 1        // броневой
};
//---------------------------------------------------------------------------
/* Схемы соединения обмоток */
enum Winding_Connect{
   wStar = 0,           // в звезду
   wTriangle = 1,       // в треугольник
   wGroundedStar = 2    // звезда с заземленной нейтралью
};
//---------------------------------------------------------------------------
/* Power Object List - содержит список всех созданных объектов */
class PObjectList{
      vector<PElement*> Obj;
   public:
      ~PObjectList();
      void Add    (PElement* obj);
      bool Delete (PElement* obj);
      PElement* Find (String Name);             // возвращает объект по имени или (если объект не найден)NULL
      PPin* FindPin(String Name, unsigned pin); // найти вывод объекта (из списка его выводов), заданного именем и номером узла
};
//---------------------------------------------------------------------------
/* Список всех групп выводов - содержит указатели на группы выводов */
class Pins_List{
   private:
   	vector<Pins*> _Pins; // указатель на группы выводов
	public:
   	void add(Pins* X); 						// сохранили группу выводов
   	Pins& operator[](unsigned int i);	// вернули группу выводов
      unsigned Count();                   // возвращаем число групп выводов
      void Clear();								// очистить список
      void Delete(Pins* pins);				// удалить группу выводов

};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/* Модель дросселя -  Power Throttle*/
class Throttle{
   public:
      unsigned EBranch;    // номер электрической ветви
      unsigned MBranch;    // номер магнитной ветви
      int Windings;   // число витков
      PElement *Obj;     // указатель на электрический объект, с которым связываем
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
enum rwMode{ro, rw};   // режим доступа
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/* Класс содержит информацию о свойстве объекта, который могжет быть
отредактировн  */
class PFieldInfo{
   public:
      PFieldInfo();
      String Name;            // имя свойства
      String Description;     // описание поля
      String Value();         // возвращаем значение в виде строки
      void   Value(String newVal);   // установим новое значение
      String Type;            // тип
      rwMode Mode;            // режим чтения записи {ro, rw}
      void  *ptr;             // указатель на свойство
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/* Класс для указания способа соединения элементов */
class Pins_Link{
		vector<Pair_Pin> Link;
	public:
      void add(PPin &First, PPin &Second);	// добавить связь
      void add(Pins& First, Pins& Second);	// добавить пофазную связь
      void add(Pins_Link obj);	            // добавить связи из объекта obj в этот объект
      void clear();                          // очистка
      Pair_Pin& operator[](const unsigned int i);
      //Pins_Link operator=(const Pins_Link &Obj);
      unsigned __fastcall Number();				// число связей
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/* Базовый класс для всех моделей - Power Element*/
class PElement{
   private:
      void Describe_Fields();     // метод который описывает поля этого класса
   protected:
   	unsigned u;				// число узлов
      unsigned v;				// число ветвей
      unsigned k;				// число контуров
   	Dynamic_Array<PType> _Lm; // вектор взаимоиндуктивностей ветвей _Lm[]
   public:
      Matrix<char> *M;	   // первая матрица инцеденций
   	Matrix<char> *N;	   // вторая матрица инцеденций
   	const PType null;   // переменная всегда равная нулю (нужна для создания указателя, указывающего на нулевой элемент)
      const PType MIN;    // минимально возможное значение (число значащих цифр для PType 15. Для погрешности в 5 знаке порядок MIN и MAX не должен отличаться побее чем в 10 раз)
      const PType MAX;    // максимально возможное значение (для погрешности в допустимых пределах)
      unsigned Grounded_Node; // заземляемый узел (должен быть 1)
      PType **R;				// значения сопротивлений в ветвях *R[]
      PType **L;				// значения индуктивностей в ветвях *L[]
      PType **C;				// значения емкостей в ветвях *C[]
      PType ***Lm;         // массив взаимоиндуктивностей ветвей Lm[][]
      Throttle **D;        // магнитные связи между ветвями
   	PType **E;				// вектор-матрица ЭДС в ветвях (E<0 если Е и ток ветви противоположны, напряжение на конденсаторе всегда q/C<0)
      PType **J;           // вектор-матрица задающих токов в узлах (источники тока)
		PType **U_C;  		   // напряжений на емкостях в ветвях (начальные условия)
      PType **U_L;  		   // напряжений на индуктивностях в ветвях (начальные условия)
      PType **U_Lm;  		// напряжений на взаимоиндуктивностях в ветвях (начальные условия)
      PType **U_D;			// напряжения на дроселлях
		PType **Z;           // массив дифференциальных сопротивлений ветвей
      PType **I;		      // указатели на рассчитанние значения токов
      PType **f;				// потенциалы узлов
      PElement();
      PElement(unsigned NumBranches, unsigned NumNodes, unsigned NumCircuit);
      PElement(PElement *obj);
      ~PElement();
      virtual bool update_sources(bool force_update=false){return false;}; // обновляем мгновенные значения токов и напряжений на выходах источников (true-обновились, false-остались прежними)
      virtual bool update_elements(bool force_update=false){return false;}; // учет нелинейных элементов, обновляем начения падений напряжений в ветвях (из-за изменения R,L,C) (true-обновились, false-остались прежними)
      unsigned __fastcall Branches();
      unsigned __fastcall Nodes();
      unsigned __fastcall Circuit();
      //__property unsigned Branches  = { read=GetBranches };
      //__property unsigned Nodes  = { read=GetNodes };
      //__property unsigned Circuit  = { read=GetCircuit };// матрица-вектор напряжений в ветвях
      PPin set_pin(unsigned Node);                       // установка узла в качестве вывода
      String Name;                                       // имя объекта
      void update_differential_resistance();             // установка дифференциальных сопротивлений в ветвяx
      virtual void mutual_inductance(unsigned branch_1, unsigned branch_2, const PType &value);          // установка взаимной индуктивности
      virtual void magnetic_link(unsigned Electric_Branch, unsigned Magnetic_Branch, unsigned Windings); // установка магнитной связи
		Pins_List Out;	            // список групп выводов
      virtual bool accept();     // рассчитать параметры схемы замещения в соответствии с измененными исходными данными
      vector<PFieldInfo>   Fields;
      virtual void Save(ostream &stream);		// сохранение параметров объекта в поток
      virtual void Read(istream &stream);    // загрузка параметров объекта из потока
      virtual String ClassName();				// возвращает имя класса
      void*    Owner;                        // владелец этого объекта. Он ответственен за удаление этого объекта
      Pins_Link Internal_Links; 				   // соединения внутри объекта, меняющиется в зависимости от настройки элемента
		PElement*	Grid;								// в какую сеть входит данный элемент. Если NULL - элемент вернего уровня
      String GridName();							// полное имя сети, в которую входит объект
      String FullName();							// имя объекта с учетом всех подсетей, в которые он входит
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/* Класс предоставляющий доступ к выводам от блоков - Power Pin*/
class PPin{
	private:
   	friend PPin PElement::set_pin(unsigned Node); // установка узла и ветки в качестве вывода
	protected:
   	PElement* obj;		// к какому элементу относимся
   	unsigned node;		// номер выходного узла
      //unsigned branch;  // номер выходной ветки
   public:
   	PElement* __fastcall Obj();
		unsigned  __fastcall Node();
      //__property unsigned Branch  = { read=GetBranch };
      PType __fastcall f();   // потенциал
      PType __fastcall I();   // выходной ток
      void Save(ostream &stream);			// сохранить информацию о выводе
      void Read(istream &stream);			// прочитать информацию о выводе
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/* Пара выводов */
class Pair_Pin{
	public:
   	PPin* First;
      PPin* Second;
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/* Группа выводов - содержит объекты-выводы */
class Pins{
   private:
      PPin* _Pin; 	// массив выводов
	public:
      Pins(unsigned Number);
      Pins(Pins& obj);
      const unsigned Count;
      PPin& operator[](unsigned int i);
      Pins operator=(Pins &op2);
      ~Pins();
      String Name;
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/* Класс на основе которого выводятся классы КОНЕЧНЫХ (более не разложимых на части) силовых установок */
class PUnit : public PElement{
   private:
      typedef PElement sire;  // определяем из какого класса выведены
      void Describe_Fields();     // метод который описывает поля этого класса
   protected:
      PType *_R;				// значения сопротивлений в ветвях
      PType *_L;				// значения индуктивностей в ветвях
      PType *_C;				// значения емкостей в ветвях
      PType *_E;				// вектор-матрица ЭДС в ветвях (E<0 если Е и ток ветви противоположны, напряжение на конденсаторе всегда q/C<0)
      PType *_J;          	// вектор-матрица задающих токов в узлах (источники тока)
		PType *_U_C;  			// напряжений на емкостях в ветвях (начальные условия)
      PType *_U_L;  			// напряжений на индуктивностях в ветвях (начальные условия)
      PType *_U_Lm;  		// напряжений на обузловленные взаимоиндуктивностями в ветвях (начальные условия)
      PType *_U_D;			// напряжения на дроселлях
      PType *_Z;  		   // массив дифференциальных сопротивлений
      PType *_I;  		   // расчитанные значения токов
      PType *_f;				// потенциалы узлов
      ~PUnit();
      PUnit(PUnit *obj);
      PUnit(unsigned NumBranches, unsigned NumNodes, unsigned NumCircuit);
   public:
      // взаимные индуктивности между определенными фазами
      //virtual bool Mutual_Inductance(PType M_ab, PType M_bc, PType M_ca)=0;
      // проводимости между определенными фазами
      //virtual bool Mutual_Conductance(unsigned Phase1, unsigned Phase2, PType Gm, PType Cm) = 0;
      // параметры на 1 фазу
      //virtual bool Phase_Params(Phase phase, PType __R, PType __L, PType __C, PType Gz, PType Cz) = 0;
		//Pins_Link Internal_Links; 				// соединения внутри объекта, меняющиется в зависимости от настройки элемента
		virtual String ClassName();				// возвращает имя класса
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/* Модель линии электропередач - Transmission Power Line */
class PLine: public PUnit{
   private:
      typedef PUnit sire;  // определяем из какого класса выведены
      unsigned z;				// число звеньев в модели
      void set_pins();     // установка соответствия внутренней нумерации ветвей И человекопонятной нумерацией
      void Describe_Fields();                // метод который описывает поля этого класса
      Pins* GetPins_Group(unsigned Index);  // возврат указателя на группу выводов
      /*void setR_ (PType value);  // установка активных сопротивлений на фазу
      void setL_ (PType value);  // установка индуктивности фазы
      void setC_ (PType value);  // установка умкости фазы
      void setGz_(PType value);  // установка активных проводимостей на землю
      void setCz_(PType value);  // установка емкостей на землю
      void setGm_(PType value);  // установка активных междуфазных проводимостей
      void setCm_(PType value);  // установка междуфазных емкостей
      void setLm_(PType value);  // установка междуфазных взаимоиндуктивностей
      // удельные параметры
      void setR0 (PType value); // установка активных сопротивлений на фазу
      void setL0 (PType value); // установка индуктивности фазы
      void setC0 (PType value); // установка умкости фазы
      void setGz0(PType value); // установка активных проводимостей на землю
      void setCz0(PType value); // установка емкостей на землю
      void setGm0(PType value); // установка активных междуфазных проводимостей
      void setCm0(PType value); // установка междуфазных емкостей
      void setLm0(PType value); // установка междуфазных взаимоиндуктивностей */
      // взаимные индуктивности между определенными фазами
      bool Mutual_Inductance(PType M_ab, PType M_bc, PType M_ca);
      bool Mutual_Inductance_0(PType M0_ab, PType M0_bc, PType M0_ca);
      // проводимости между определенными фазами
      bool Mutual_Conductance(unsigned Phase1, unsigned Phase2, PType Gm, PType Cm);
      bool Mutual_Conductance_0(Phase Phase1, Phase Phase2, PType Gm0, PType Cm0);
      // параметры на 1 фазу
      bool Phase_Params(Phase phase, PType __R, PType __L, PType __C, PType Gz, PType Cz);
      bool Phase_Params_0(Phase phase, PType L0, PType R0, PType Gz0, PType Cz0);
	public:
      Pins X1;          // выводы 1ого конца линии (1 группа выводов)
      Pins X2;          // выводы 2ого конца линии (2 группа выводов)
		PLine(unsigned Num_Of_Cells=1);
      PLine(PLine &ob);
      ~PLine();
      PLine operator=(PLine &op2);
      PType length;           // длинна линии
      PType R0[4];
      PType L0[4];
      PType C0[4];
      PType Gz0[4];
      PType Cz0[4];
      PType Gm0[3];
      PType Cm0[3];
      PType M0[3];
      // расчет параметров по специальной методике
      bool set_params(const PType _L0, const PType _R0, const PType _M0,
                      const PType dP_f, const PType dQ_f, const PType U_n_f, const PType U_k_f,
                      const PType dP, const PType dQ, const PType U_n, const PType U_k,
                      const PType freq=50 );
      unsigned Cells();								// возвращает число звеньев
      virtual bool accept();              	// рассчитать параметры схемы замещения в соответствии с измененными исходными данными
      virtual void Save(ostream &stream);		// сохранение параметров объекта в поток
      virtual void Read(istream &stream);    // загрузка параметров объекта из потока
      virtual String ClassName();				// возвращает имя класса
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/*  Хранит информацию для рассчета ЭДС указанной функцией */
class Source_Params{
	public:
   	String Type;						 // способ рассчета
   	vector<PType> Field;			    // Параметры рассчета разбиты по символу ":"
   	void Change(String  description);	// добавим значение
   	String Get();
   	void Save(ostream &stream);			// сохранить  объект
   	void Read(istream &stream);			// прочитать  объект
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
class PSource : public PUnit{
   private:
      typedef PUnit sire;  // определяем из какого класса выведены
      void set_pins();                    // установка соответствия внутренней нумерации ветвей И человекопонятной нумерацией
      void Describe_Fields();             // метод который описывает поля этого класса
      // взаимные индуктивности между определенными фазами
      bool Mutual_Inductance(PType M_ab, PType M_bc, PType M_ca);
      // проводимости между определенными фазами
      bool Mutual_Conductance(unsigned Phase1, unsigned Phase2, PType Gm, PType Cm);
      // параметры на 1 фазу
      bool Phase_Params(Phase phase, PType __R, PType __L, PType __C, PType Gz, PType Cz);
      /*/ установка параметров на все 3 фазы
      void R_(PType value);  // установка активных сопротивлений на фазу
      void L_(PType value);  // установка индуктивности фазы
      void C_(PType value);  // установка умкости фазы
      void Gz_(PType value); // установка активных проводимостей на землю
      void Cz_(PType value); // установка емкостей на землю
      void Gm_(PType value); // установка активных междуфазных проводимостей
      void Cm_(PType value); // установка междуфазных емкостей
      void Lm_(PType value); // установка междуфазных взаимоиндуктивностей  */
   public:
      Pins X;             // выводы генератора
      PSource();
      PType (*Ea)(PElement* Sender, double &time);
      PType (*Eb)(PElement* Sender, double &time);
      PType (*Ec)(PElement* Sender, double &time);
      Source_Params sourceParams;      // информация для рассчета ЭДС
      PType R_[4];
      PType L_[4];
      PType C_[4];
      PType Gz_[4];
      PType Cz_[4];
      PType Gm_[3];
      PType Cm_[3];
      PType M_ [3];
      bool update_sources(bool force_update=false);
      virtual bool accept();              // рассчитать параметры схемы замещения в соответствии с измененными исходными данными
      virtual void Save(ostream &stream);		// сохранение параметров объекта в поток
      virtual void Read(istream &stream);    // загрузка параметров объекта из потока
      virtual String ClassName();				// возвращает имя класса
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/* Потребитель - Power Consumer */
class PConsumer : public PSource{
   private:
      typedef PSource sire;  // определяем из какого класса выведены
   public:
      PConsumer();
      bool Load(const PType P,const PType Q, const PType Unom, bool Grounding, const PType freq );
   /*private:
      PType (*Ea)(double &time);
      PType (*Eb)(double &time);
      PType (*Ec)(double &time);
      void set_pins();     // установка соответствия внутренней нумерации ветвей И человекопонятной нумерацией
      void __fastcall SetR(PType value);  // установка активных сопротивлений на фазу
      void __fastcall SetL(PType value);  // установка индуктивности фазы
      void __fastcall SetC(PType value);  // установка емкости фазы
      void __fastcall SetGz(PType value); // установка активных проводимостей на землю
      void __fastcall SetCz(PType value); // установка емкостей на землю
      void __fastcall SetGm(PType value); // установка активных междуфазных проводимостей
      void __fastcall SetCm(PType value); // установка междуфазных емкостей
      void __fastcall SetLm(PType value); // установка междуфазных взаимоиндуктивностей
   public:
      Pins X;             // выводы потребителя
      PConsumer();
      virtual bool update_sources(bool force_update=false);
      bool Load(const PType P,const PType Q, const PType Unom, bool Grounding, const PType freq );
            // установка параметров на все 3 фазы
      __property PType R_  = { write=SetR };
      __property PType L_  = { write=SetL };
      __property PType C_  = { write=SetC };
      // установка параметров на все 3 маждуфазные проводимости
      __property PType Gz  = { write=SetGz };
      __property PType Cz  = { write=SetCz };
      __property PType Gm  = { write=SetGm };
      __property PType Cm  = { write=SetCm };
      // установка взаимоиндуктивности на все 3 провода
      __property PType Lm_  = { write=SetLm };
      // взаимные индуктивности между определенными фазами
      bool Mutual_Inductance(PType M_ab, PType M_bc, PType M_ca);
      // проводимости между определенными фазами
      bool Mutual_Conductance(unsigned Phase1, unsigned Phase2, PType Gm, PType Cm);
      // параметры на 1 фазу
      bool Phase_Params(Phase phase, PType __R, PType __L, PType __C, PType Gz, PType Cz);
      */
      virtual String ClassName();				// возвращает имя класса
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/* Класс-контейнер для задания кривых - Power curve */
class PCurve{
   private:
      vector<PType> _X;
      vector<PType> _Y;
      bool Symmetry;    // 4 квадрант графика зеркален первому квадранту относительно начала координат
      friend void   PFieldInfo::Value(String newVal);   // необходимо из PFieldInfo получать доступ к private членам этого класса
   protected:
      void _set(PType X, PType Y);
      PType __fastcall X(PType _Y_);
      PType __fastcall Y(PType _X_);
      PType (*Y_X)(PType&);	// указатель на внешнюю функцию рассчета кривой намагничивания
      							   // в форме H=f(B)
   public:
      PCurve();
      void clear();                                   // очищаем ломанную
      unsigned size();                                // число участков (только если задано ломаная)
      void Point(unsigned Index, PType &X, PType &Y); // вернет контрольные точки ломанной
      bool exist;                         // кривая задана (существует)?
      void func( PType (*Func)(PType&) ); // установить функцию для рассчета кривой
      virtual void Save(ostream &stream);	// сохранение параметров объекта в поток
      virtual void Read(istream &stream); // загрузка параметров объекта из потока
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/* Класс-обертка для кривой намагничивания (Вебер-Амперная характеристика) - magnetization curve */
class MagnCurve: public PCurve{
   public:
      void set(PType F, PType I);
      /*PType (__fastcall PCurve::* B)(PType H);         // указатель на функцию поиска индукции
      PType (__fastcall PCurve::* H)(PType B);          // указатель на функцию поиска напряженности*/
      PType __fastcall F(PType _I_);   //
      PType __fastcall I(PType _F_);
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/* Класс-обертка для Вольт-Амперной характеристики - U, I curve */
class UICurve: public PCurve{
   public:
      void set(PType I, PType U);
      PType __fastcall U(PType _I_);
      PType __fastcall I(PType _U_);
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/* Расчетная модель однофазного трансформатора
без учета кривой намагничивания - Power Single Transformer */
class PSingleTransformer : public PUnit{
   	typedef PUnit sire;  	// определяем из какого класса выведены
      typedef PUnit sire;  // определяем из какого класса выведены
      void set_pins();
      PType _U1,  _U2, _S;    // номинальные напряжения и мощность трансформатора
      PType _dPk, _Uk;  		// данные КЗ
      PType _dPx, _Ix;			// данные холостого хода
      PType _Iu;					// ток цепи намагничивания на холостом ходу
      PType _freq;            // номинальная частота
	public:
   	PSingleTransformer();
      Pins X1,  X2;			// группы выводов
   	void Params(PType U1,  PType U2, PType S,  // установка параметров
      	    PType dPk, PType Uk,
             PType dPx, PType Ix,
             PType freq=50);
      virtual String ClassName();				// возвращает имя класса
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/* Базовый класс для трансформаторов */
class PTransformer : public PUnit{
   private:
      typedef PUnit sire;     // определяем из какого класса выведены
      void Describe_Fields(); // метод который описывает поля этого класса
   protected:
      PTransformer(unsigned NumBranches, unsigned NumNodes, unsigned NumCircuit); // объект нельзя создать напрямую, т.к. конструктор protected-член
   public:
      PType n1, n2;           // число витков обмоток
      PType U1,  U2, S;       // номинальные напряжения и мощность трансформатора
      PType dPk, Uk;  		   // данные КЗ
      PType dPx, Ix;			   // данные холостого хода
      PType freq;             // номинальная частота
      PType F_nom;            // магнитный поток на холостом ходу
      virtual void Save(ostream &stream);		// сохранение параметров объекта в поток
      virtual void Read(istream &stream);    // загрузка параметров объекта из потока
      virtual String ClassName();				// возвращает имя класса
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/* Расчетная модель однофазного трансформатора
возможен учет кривой намагничивания - Power Saturable Transformer 1 phase */
class PTransformer_1 : public PTransformer{
      typedef PTransformer sire;  // определяем из какого класса выведены
      void set_pins();
      void Describe_Fields();     // метод который описывает поля этого класса
   public:
      PTransformer_1();
      Pins X1,  X2;			   // группы выводов
      MagnCurve mCurve;       // задаем кривую намагничивания
      virtual bool accept();  // рассчет параметров схемы
      virtual bool update_elements(bool force_update=false);
      void R1(PType value);    // установка активных сопротивлений на фазу
      void L1(PType value);    // установка сопротивление рассеивания фазы
      void R2(PType value);    // установка активных сопротивлений на фазу
      void L2(PType value);    // установка сопротивление рассеивания фазы
      PType R1();
      PType L1();              // сопротивление рассеяния ! (не полное инд сопрот. обмотки, а только рассеяние)
      PType R2();
      PType L2();
      void Gz1(PType value);   // установка активных проводимостей на землю
      void Cz1(PType value);   // установка емкостей на землю
      PType Gz1();             // чтение активных проводимостей на землю
      PType Cz1();             // чтение емкостей на землю
      void Gz2(PType value);   // установка активных проводимостей на землю
      void Cz2(PType value);   // установка емкостей на землю
      PType Gz2();             // чтение активных проводимостей на землю
      PType Cz2();             // чтение емкостей на землю
      virtual void Save(ostream &stream);		// сохранение параметров объекта в поток
      virtual void Read(istream &stream);    // загрузка параметров объекта из потока
      virtual String ClassName();				// возвращает имя класса
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// Расчетная модель трансформатора - Power Transformer
class PTransformer_3 : public PTransformer{
	private:
      typedef PTransformer sire; // определяем из какого класса выведены
   	void set_pins();           // установка соответствия внутренней нумерации ветвей И человекопонятной нумерацией
      bool Link_Windings();      // сборка обмоток в звезду(треугольник)
      void Describe_Fields();    // метод который описывает поля этого класса
	public:
   	PTransformer_3();
      Pins X1,  X2;	         // группы выводов
      Pins X1n, X2n;	         // Выводы со стороны нейтрали
      MagnCircuit_Type  mType;// тип магнитопровода
      MagnCurve mCurve;       // задаем кривую намагничивания
      virtual bool accept();  // рассчет параметров схемы
      virtual bool update_elements(bool force_update=false);
      Winding_Connect Winding_X1, Winding_X2;   // схема соединения обмоток
      // на 1 обмотку
      void __fastcall R1(PType value);    // установка активных сопротивлений на фазу
      void __fastcall L1(PType value);    // установка индуктивности фазы
      void __fastcall C1(PType value);    // установка емкости фазы
      void __fastcall Gz1(PType value);   // установка активных проводимостей на землю
      void __fastcall Cz1(PType value);   // установка емкостей на землю
      PType R1(Phase ph);                 // чтение активных сопротивлений на фазу
      PType L1(Phase ph);                 // чтение сопротивление рассеивания фазы
      PType C1(Phase ph);                 // чтение емкости фазы
      PType Gz1(Phase ph);                // чтение активных проводимостей на землю
      PType Cz1(Phase ph);                // чтение емкостей на землю
      // на 2 обмотку
      void __fastcall R2(PType value);    // установка активных сопротивлений на фазу
      void __fastcall L2(PType value);    // установка индуктивности фазы
      void __fastcall C2(PType value);    // установка умкости фазы
      void __fastcall Gz2(PType value);   // установка активных проводимостей на землю
      void __fastcall Cz2(PType value);   // установка емкостей на землю
      PType R2(Phase ph);                 // чтение активных сопротивлений на фазу
      PType L2(Phase ph);                 // чтение сопротивление рассеивания фазы
      PType C2(Phase ph);                 // чтение емкости фазы
      PType Gz2(Phase ph);                // чтение активных проводимостей на землю
      PType Cz2(Phase ph);                // чтение емкостей на землю
      virtual void Save(ostream &stream);	// сохранение параметров объекта в поток
      virtual void Read(istream &stream); // загрузка параметров объекта из потока
      virtual String ClassName();			// возвращает имя класса
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/* Базовая модель трансформаторов тока
возможен учет кривой намагничивания - Power Current Transformer */
class PCurrent_Transformer : public PUnit{
   private:
   	typedef PUnit sire;  		// определяем из какого класса выведены
      void Describe_Fields();    // метод который описывает поля этого класса
   protected:
      PCurrent_Transformer(unsigned NumBranches, unsigned NumNodes, unsigned NumCircuit);
      PType F_nom;               // магнитный поток
   public:
      PType S;                   // номинальная мощность
      PType I1,  I2;             // номинальные токи первичной и вторичной обмоток
      PType n1, n2;              // число витков обмоток
      PType K;                   // класс точности, в %  (токовая погрешность)
      PType d;                   // угловая погрешность, градусы
      PType freq;                // номинальная частота
      MagnCurve mCurve;          // задаем кривую намагничивания
      virtual void Save(ostream &stream);	// сохранение параметров объекта в поток
      virtual void Read(istream &stream); // загрузка параметров объекта из потока
      virtual String ClassName();			// возвращает имя класса
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/* Расчетная модель однофазного трансформатора тока
возможен учет кривой намагничивания - Power Current Transformer 1 phase */
class PCurrent_Transformer_1 : public PCurrent_Transformer{
   private:
   	typedef PCurrent_Transformer sire;  // определяем из какого класса выведены
      void set_pins();     					// установка соответствия внутренней нумерации ветвей И человекопонятной нумерацией
      void Describe_Fields();             // метод который описывает поля этого класса
   public:
      Pins X1, X2;      // группы выводов
      PType R1, L1;     // сопротивления первичной обмотки
      PType R2, L2;     // сопротивления вторичной обмотки
      PCurrent_Transformer_1();
      virtual bool accept();     // рассчет параметров схемы
      virtual bool update_elements(bool force_update=false);
      virtual void Save(ostream &stream);	// сохранение параметров объекта в поток
      virtual void Read(istream &stream); // загрузка параметров объекта из потока
      virtual String ClassName();			// возвращает имя класса
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/* Трехфазный выключатель  -  Power Breaker */
class PBreaker : public PUnit{
   private:
   	typedef PUnit sire;  	// определяем из какого класса выведены
      void set_pins();        // установка соответствия внутренней нумерации ветвей И человекопонятной нумерацией
      void Describe_Fields(); // метод который описывает поля этого класса
      PType T_on[4], T_off[4];// момент подачи сигнала на включение (выключение)
   public:
      PType Rmax;             // сопротивление в фазе в отключенном состоянии
      PType Rmin;             // в фазе вo включенном состоянии
      PType t_on;             // собственное время включения
      PType t_off;            // собственное время отключения
      PType t_full_off;       // полное время отключения
      bool IsOn;					// текущее состояние (true - включен)
      Pins X1, X2;            // выводы для выключателя
      PBreaker();
      bool on(Phase phase);   // включить фазу
      bool off(Phase phase);  // отключить фазу
      bool on();              // одновременно включить все фазы
      bool off();             // одновременно отключить все фазы
      virtual bool accept();     // рассчет параметров схемы
      virtual bool update_elements(bool force_update=false);
    	virtual void Save(ostream &stream);	// сохранение параметров объекта в поток
      virtual void Read(istream &stream); // загрузка параметров объекта из потока
      virtual String ClassName();			// возвращает имя класса
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/* Расчетная модель подсети - Power SubGrid */
class PSubGrid : public PElement{
   private:
   	typedef PElement sire;  	// определяем из какого класса выведены
      vector<PElement*> Element; // список элементов
      void Describe_Fields();		// опишем поля
      void LinksForSubGrid(PSubGrid *grid, Pins_Link &Link);   // векурсивный возврат связей (с учетом связей во вложенных подсетях) для указанной сети
   public:
      PSubGrid();                // функция-пустышка, просто обнуляет указатели. Используется в производных классах
      //~SubPGrid();
      PElement* __fastcall Unit(int Index);
      unsigned  __fastcall num_Units();
      PElement* obj_from_name(String Name);     // возвращает объект по имени
      void setOuts  (String strOut);				// переназначение групп выводов
      String getOuts(); 								// получение групп выводов
      bool Change( Pins_Link &InLink,
      				vector<PSubGrid*> SubGrids);  // установить описание сети на основе соединения объектов и передачи списка подсетей
      virtual bool accept(){return true;};      // рассчет параметров схемы
      Pins_Link Links();                        // возвращает все связи внутри себя. включая связи подсетей
                                                // (Internal_Links включает связи внутри сети без связей в подсетях этой сети)
    	virtual void Save(ostream &stream);			// сохранение параметров объекта в поток
      virtual void Read(istream &stream); 		// загрузка параметров объекта из потока
      virtual String ClassName();					// возвращает имя класса
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/* Расчетная модель электрической сети - Power Grid */
class PGrid : public PElement{
	private:
   	typedef PElement sire;  	// определяем из какого класса выведены
      void Describe_Fields(); 	// метод который описывает поля этого класса
      void set_pins(PElement* Obj,  Pins* pins); // установка соответствия внутренней нумерации ветвей И человекопонятной нумерацией
      PType *_J;          	// вектор-матрица задающих токов в узлах (источники тока)
      Matrix<PType> *A;		// матрица из уравнения A*I=B
      PType *B;   			// матрица из уравнения A*I=B
      Matrix<PType> *F;		// Матрица для нахождения потенциалов узлов f = F*( I*Z - U_L + U_C - U_Lm )
      PElement **Element;     // указатели на элементы используемые в PGrid
      unsigned numElement;    // число элементов
      unsigned numLinks;	// число связей, использованных при создании сети (сеобходимо для сохранения)
      unsigned *Links;		// связи, использованные при создании сети (необходимо для сохранения)
      unsigned **Branch; 	// таблица соответствий номеров вервей в результирующей матрице и [№_блока][№_ветви] = №_ветви_результ_матрицы, переданных блоков (==-1 - нет соответствия)
      unsigned **Node;   	// таблица соответствий номеров узлов в результирующей матрице и [№_блока][№_узла]=№_узла_результ_матрицы, переданных блоков (==-1 - нет соответствия)
      PType *U;				// разность потенциалов по концам ветви (учет падений напряжений и ЭДС)
      PType *_U_Lm2;  		// падения напряжений на взаимоиндуктивнотях от текущего тока
      PType **U_Lm2;  		// падения напряжений на взаимоиндуктивнотях от текущего тока
      PType *_I2;          // предшествующие значения тока
      void linked_nodes(const unsigned numLink, const unsigned Link[][2][2], bool link_use[],const unsigned i_node,const unsigned obj,const unsigned node);//, unsigned &tmp);
      unsigned __fastcall Node_From_LocalNode(unsigned Obj, unsigned LocalNode);
      unsigned __fastcall Branch_From_LocalBranch(unsigned Obj, unsigned LocalBranch);
      unsigned ObjNum_From_Object(PElement *obj);
      unsigned __fastcall Node_From_LocalNode(PElement* Obj, unsigned LocalNode);
      unsigned __fastcall Branch_From_LocalBranch(PElement* Obj, unsigned LocalBranch);
   	bool Link_Elements(unsigned num, PElement *Block[], unsigned numLink,unsigned Link[][2][2]);	// создание сети из подэлементов и указания связанных узлов
      void Update_Equation();
      virtual bool update_sources(bool force_update=false);
      virtual bool update_elements(bool force_update=false);
      bool BuildOK;           // установится в false, если сеть создана с ошибками
   public:
      PGrid(Pins_Link Link);  // сеть создается из группы объектов
      PGrid(PUnit *obj);      // сеть создается из одного объекта
      PGrid();                // функция-пустышка, просто обнуляет указатели. Используется в производных классах
      ~PGrid();
      bool Make_Equation();   // если PGrid конечна ярассчетная сеть, создаем уравнения для рассчета
      void calc();
      //__property PElement* Unit[int Index]={read=GetUnit};
      //__property unsigned num_Units  = { read=Get_num_Units };
      PElement* __fastcall Unit(int Index);
      unsigned  __fastcall num_Units();
      PElement* obj_from_name(String Name);     // возвращает объект по имени
      void setOuts  (String strOut);	 			// переназначение групп выводов
      String getOuts();				 					// получение групп выводов
      virtual bool accept(){return true;};      // рассчет параметров схемы
    	virtual void Save(ostream &stream);			// сохранение параметров объекта в поток
      virtual void Read(istream &stream); 		// загрузка параметров объекта из потока
      virtual String ClassName();					// возвращает имя класса
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/* Тестовая модель А */
class tstA: public PUnit{
private:
      /*virtual void __fastcall SetR(PType value);  // установка активных сопротивлений на фазу
      virtual void __fastcall SetL(PType value);  // установка индуктивности фазы
      virtual void __fastcall SetC(PType value);  // установка умкости фазы
      virtual void __fastcall SetGz(PType value); // установка активных проводимостей на землю
      virtual void __fastcall SetCz(PType value); // установка емкостей на землю
      virtual void __fastcall SetGm(PType value); // установка активных междуфазных проводимостей
      virtual void __fastcall SetCm(PType value); // установка междуфазных емкостей
      virtual void __fastcall SetLm(PType value); // установка междуфазных взаимоиндуктивностей
      virtual bool Mutual_Inductance(PType M_ab, PType M_bc, PType M_ca);
      virtual bool Mutual_Conductance(unsigned Phase1, unsigned Phase2, PType Gm, PType Cm);
      virtual bool Phase_Params(Phase phase, PType __R, PType __L, PType __C, PType Gz, PType Cz);*/
      //MCircuit *_Magn;  // магнитопровода
public:
		tstA();
      ~tstA();
      Pins X;
      UICurve BAX;
      //PGrid *Magn;   // магнитная цепь
      virtual bool update_sources(bool force_update=false);
      virtual bool update_elements(bool force_update=false);
};
/* Тестовая модель В */
class tstB: public PUnit{
private:
      virtual void __fastcall SetR(PType value);  // установка активных сопротивлений на фазу
      virtual void __fastcall SetL(PType value);  // установка индуктивности фазы
      virtual void __fastcall SetC(PType value);  // установка умкости фазы
      virtual void __fastcall SetGz(PType value); // установка активных проводимостей на землю
      virtual void __fastcall SetCz(PType value); // установка емкостей на землю
      virtual void __fastcall SetGm(PType value); // установка активных междуфазных проводимостей
      virtual void __fastcall SetCm(PType value); // установка междуфазных емкостей
      virtual void __fastcall SetLm(PType value); // установка междуфазных взаимоиндуктивностей
      virtual bool Mutual_Inductance(PType M_ab, PType M_bc, PType M_ca);
      virtual bool Mutual_Conductance(unsigned Phase1, unsigned Phase2, PType Gm, PType Cm);
      virtual bool Phase_Params(Phase phase, PType __R, PType __L, PType __C, PType Gz, PType Cz);
public:
		tstB();
      Pins X;
      virtual bool update_sources(bool force_update=false);
      virtual bool update_elements(bool force_update=false);
};
#endif
