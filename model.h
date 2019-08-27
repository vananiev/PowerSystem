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
extern double smp;   	// ����� ����� ����������� ����
extern double TIME;  	// ������� �����
extern void err(unsigned, String);
//---------------------------------------------------------------------------
typedef long double PType;		// ��� ���������� ����������
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
// �������������� �������
char *StringForSave(String *s);   // �������������� ������ ��� ������ � �����
char *StringForSave(char *s);    // �������������� ������ ��� ������ � �����
void SaveObj(PElement *Obj, ostream &stream);               // ���������� ������� � �����
PElement* CreateObj(istream &stream);                       // �������� ������� �� ������ �� ������
PElement* CreateObj(String ClassName, vector<void*> Params);// ������� ������ �� ����� ������ � ���������� ����������, ������� ���������� � ����������� ������
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/* ����������� ��� */
enum Phase       {_N_, _A_, _B_, _C_};
enum interPhase  {_AB_, _BC_, _CA_};
//---------------------------------------------------------------------------
/* ���� ��������������� */
enum Transf_Type {
};
//---------------------------------------------------------------------------
/* ���� ��������������� */
enum MagnCircuit_Type {
   Three_Rod = 0,       // ��������������
   Armored   = 1        // ��������
};
//---------------------------------------------------------------------------
/* ����� ���������� ������� */
enum Winding_Connect{
   wStar = 0,           // � ������
   wTriangle = 1,       // � �����������
   wGroundedStar = 2    // ������ � ����������� ���������
};
//---------------------------------------------------------------------------
/* Power Object List - �������� ������ ���� ��������� �������� */
class PObjectList{
      vector<PElement*> Obj;
   public:
      ~PObjectList();
      void Add    (PElement* obj);
      bool Delete (PElement* obj);
      PElement* Find (String Name);             // ���������� ������ �� ����� ��� (���� ������ �� ������)NULL
      PPin* FindPin(String Name, unsigned pin); // ����� ����� ������� (�� ������ ��� �������), ��������� ������ � ������� ����
};
//---------------------------------------------------------------------------
/* ������ ���� ����� ������� - �������� ��������� �� ������ ������� */
class Pins_List{
   private:
   	vector<Pins*> _Pins; // ��������� �� ������ �������
	public:
   	void add(Pins* X); 						// ��������� ������ �������
   	Pins& operator[](unsigned int i);	// ������� ������ �������
      unsigned Count();                   // ���������� ����� ����� �������
      void Clear();								// �������� ������
      void Delete(Pins* pins);				// ������� ������ �������

};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/* ������ �������� -  Power Throttle*/
class Throttle{
   public:
      unsigned EBranch;    // ����� ������������� �����
      unsigned MBranch;    // ����� ��������� �����
      int Windings;   // ����� ������
      PElement *Obj;     // ��������� �� ������������� ������, � ������� ���������
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
enum rwMode{ro, rw};   // ����� �������
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/* ����� �������� ���������� � �������� �������, ������� ������ ����
�������������  */
class PFieldInfo{
   public:
      PFieldInfo();
      String Name;            // ��� ��������
      String Description;     // �������� ����
      String Value();         // ���������� �������� � ���� ������
      void   Value(String newVal);   // ��������� ����� ��������
      String Type;            // ���
      rwMode Mode;            // ����� ������ ������ {ro, rw}
      void  *ptr;             // ��������� �� ��������
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/* ����� ��� �������� ������� ���������� ��������� */
class Pins_Link{
		vector<Pair_Pin> Link;
	public:
      void add(PPin &First, PPin &Second);	// �������� �����
      void add(Pins& First, Pins& Second);	// �������� �������� �����
      void add(Pins_Link obj);	            // �������� ����� �� ������� obj � ���� ������
      void clear();                          // �������
      Pair_Pin& operator[](const unsigned int i);
      //Pins_Link operator=(const Pins_Link &Obj);
      unsigned __fastcall Number();				// ����� ������
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/* ������� ����� ��� ���� ������� - Power Element*/
class PElement{
   private:
      void Describe_Fields();     // ����� ������� ��������� ���� ����� ������
   protected:
   	unsigned u;				// ����� �����
      unsigned v;				// ����� ������
      unsigned k;				// ����� ��������
   	Dynamic_Array<PType> _Lm; // ������ �������������������� ������ _Lm[]
   public:
      Matrix<char> *M;	   // ������ ������� ����������
   	Matrix<char> *N;	   // ������ ������� ����������
   	const PType null;   // ���������� ������ ������ ���� (����� ��� �������� ���������, ������������ �� ������� �������)
      const PType MIN;    // ���������� ��������� �������� (����� �������� ���� ��� PType 15. ��� ����������� � 5 ����� ������� MIN � MAX �� ������ ���������� ����� ��� � 10 ���)
      const PType MAX;    // ����������� ��������� �������� (��� ����������� � ���������� ��������)
      unsigned Grounded_Node; // ����������� ���� (������ ���� 1)
      PType **R;				// �������� ������������� � ������ *R[]
      PType **L;				// �������� �������������� � ������ *L[]
      PType **C;				// �������� �������� � ������ *C[]
      PType ***Lm;         // ������ �������������������� ������ Lm[][]
      Throttle **D;        // ��������� ����� ����� �������
   	PType **E;				// ������-������� ��� � ������ (E<0 ���� � � ��� ����� ��������������, ���������� �� ������������ ������ q/C<0)
      PType **J;           // ������-������� �������� ����� � ����� (��������� ����)
		PType **U_C;  		   // ���������� �� �������� � ������ (��������� �������)
      PType **U_L;  		   // ���������� �� �������������� � ������ (��������� �������)
      PType **U_Lm;  		// ���������� �� �������������������� � ������ (��������� �������)
      PType **U_D;			// ���������� �� ���������
		PType **Z;           // ������ ���������������� ������������� ������
      PType **I;		      // ��������� �� ������������ �������� �����
      PType **f;				// ���������� �����
      PElement();
      PElement(unsigned NumBranches, unsigned NumNodes, unsigned NumCircuit);
      PElement(PElement *obj);
      ~PElement();
      virtual bool update_sources(bool force_update=false){return false;}; // ��������� ���������� �������� ����� � ���������� �� ������� ���������� (true-����������, false-�������� ��������)
      virtual bool update_elements(bool force_update=false){return false;}; // ���� ���������� ���������, ��������� ������� ������� ���������� � ������ (��-�� ��������� R,L,C) (true-����������, false-�������� ��������)
      unsigned __fastcall Branches();
      unsigned __fastcall Nodes();
      unsigned __fastcall Circuit();
      //__property unsigned Branches  = { read=GetBranches };
      //__property unsigned Nodes  = { read=GetNodes };
      //__property unsigned Circuit  = { read=GetCircuit };// �������-������ ���������� � ������
      PPin set_pin(unsigned Node);                       // ��������� ���� � �������� ������
      String Name;                                       // ��� �������
      void update_differential_resistance();             // ��������� ���������������� ������������� � �����x
      virtual void mutual_inductance(unsigned branch_1, unsigned branch_2, const PType &value);          // ��������� �������� �������������
      virtual void magnetic_link(unsigned Electric_Branch, unsigned Magnetic_Branch, unsigned Windings); // ��������� ��������� �����
		Pins_List Out;	            // ������ ����� �������
      virtual bool accept();     // ���������� ��������� ����� ��������� � ������������ � ����������� ��������� �������
      vector<PFieldInfo>   Fields;
      virtual void Save(ostream &stream);		// ���������� ���������� ������� � �����
      virtual void Read(istream &stream);    // �������� ���������� ������� �� ������
      virtual String ClassName();				// ���������� ��� ������
      void*    Owner;                        // �������� ����� �������. �� ������������ �� �������� ����� �������
      Pins_Link Internal_Links; 				   // ���������� ������ �������, ����������� � ����������� �� ��������� ��������
		PElement*	Grid;								// � ����� ���� ������ ������ �������. ���� NULL - ������� ������� ������
      String GridName();							// ������ ��� ����, � ������� ������ ������
      String FullName();							// ��� ������� � ������ ���� ��������, � ������� �� ������
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/* ����� ��������������� ������ � ������� �� ������ - Power Pin*/
class PPin{
	private:
   	friend PPin PElement::set_pin(unsigned Node); // ��������� ���� � ����� � �������� ������
	protected:
   	PElement* obj;		// � ������ �������� ���������
   	unsigned node;		// ����� ��������� ����
      //unsigned branch;  // ����� �������� �����
   public:
   	PElement* __fastcall Obj();
		unsigned  __fastcall Node();
      //__property unsigned Branch  = { read=GetBranch };
      PType __fastcall f();   // ���������
      PType __fastcall I();   // �������� ���
      void Save(ostream &stream);			// ��������� ���������� � ������
      void Read(istream &stream);			// ��������� ���������� � ������
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/* ���� ������� */
class Pair_Pin{
	public:
   	PPin* First;
      PPin* Second;
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/* ������ ������� - �������� �������-������ */
class Pins{
   private:
      PPin* _Pin; 	// ������ �������
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
/* ����� �� ������ �������� ��������� ������ �������� (����� �� ���������� �� �����) ������� ��������� */
class PUnit : public PElement{
   private:
      typedef PElement sire;  // ���������� �� ������ ������ ��������
      void Describe_Fields();     // ����� ������� ��������� ���� ����� ������
   protected:
      PType *_R;				// �������� ������������� � ������
      PType *_L;				// �������� �������������� � ������
      PType *_C;				// �������� �������� � ������
      PType *_E;				// ������-������� ��� � ������ (E<0 ���� � � ��� ����� ��������������, ���������� �� ������������ ������ q/C<0)
      PType *_J;          	// ������-������� �������� ����� � ����� (��������� ����)
		PType *_U_C;  			// ���������� �� �������� � ������ (��������� �������)
      PType *_U_L;  			// ���������� �� �������������� � ������ (��������� �������)
      PType *_U_Lm;  		// ���������� �� ������������� ��������������������� � ������ (��������� �������)
      PType *_U_D;			// ���������� �� ���������
      PType *_Z;  		   // ������ ���������������� �������������
      PType *_I;  		   // ����������� �������� �����
      PType *_f;				// ���������� �����
      ~PUnit();
      PUnit(PUnit *obj);
      PUnit(unsigned NumBranches, unsigned NumNodes, unsigned NumCircuit);
   public:
      // �������� ������������� ����� ������������� ������
      //virtual bool Mutual_Inductance(PType M_ab, PType M_bc, PType M_ca)=0;
      // ������������ ����� ������������� ������
      //virtual bool Mutual_Conductance(unsigned Phase1, unsigned Phase2, PType Gm, PType Cm) = 0;
      // ��������� �� 1 ����
      //virtual bool Phase_Params(Phase phase, PType __R, PType __L, PType __C, PType Gz, PType Cz) = 0;
		//Pins_Link Internal_Links; 				// ���������� ������ �������, ����������� � ����������� �� ��������� ��������
		virtual String ClassName();				// ���������� ��� ������
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/* ������ ����� �������������� - Transmission Power Line */
class PLine: public PUnit{
   private:
      typedef PUnit sire;  // ���������� �� ������ ������ ��������
      unsigned z;				// ����� ������� � ������
      void set_pins();     // ��������� ������������ ���������� ��������� ������ � ���������������� ����������
      void Describe_Fields();                // ����� ������� ��������� ���� ����� ������
      Pins* GetPins_Group(unsigned Index);  // ������� ��������� �� ������ �������
      /*void setR_ (PType value);  // ��������� �������� ������������� �� ����
      void setL_ (PType value);  // ��������� ������������� ����
      void setC_ (PType value);  // ��������� ������� ����
      void setGz_(PType value);  // ��������� �������� ������������� �� �����
      void setCz_(PType value);  // ��������� �������� �� �����
      void setGm_(PType value);  // ��������� �������� ����������� �������������
      void setCm_(PType value);  // ��������� ����������� ��������
      void setLm_(PType value);  // ��������� ����������� ��������������������
      // �������� ���������
      void setR0 (PType value); // ��������� �������� ������������� �� ����
      void setL0 (PType value); // ��������� ������������� ����
      void setC0 (PType value); // ��������� ������� ����
      void setGz0(PType value); // ��������� �������� ������������� �� �����
      void setCz0(PType value); // ��������� �������� �� �����
      void setGm0(PType value); // ��������� �������� ����������� �������������
      void setCm0(PType value); // ��������� ����������� ��������
      void setLm0(PType value); // ��������� ����������� �������������������� */
      // �������� ������������� ����� ������������� ������
      bool Mutual_Inductance(PType M_ab, PType M_bc, PType M_ca);
      bool Mutual_Inductance_0(PType M0_ab, PType M0_bc, PType M0_ca);
      // ������������ ����� ������������� ������
      bool Mutual_Conductance(unsigned Phase1, unsigned Phase2, PType Gm, PType Cm);
      bool Mutual_Conductance_0(Phase Phase1, Phase Phase2, PType Gm0, PType Cm0);
      // ��������� �� 1 ����
      bool Phase_Params(Phase phase, PType __R, PType __L, PType __C, PType Gz, PType Cz);
      bool Phase_Params_0(Phase phase, PType L0, PType R0, PType Gz0, PType Cz0);
	public:
      Pins X1;          // ������ 1��� ����� ����� (1 ������ �������)
      Pins X2;          // ������ 2��� ����� ����� (2 ������ �������)
		PLine(unsigned Num_Of_Cells=1);
      PLine(PLine &ob);
      ~PLine();
      PLine operator=(PLine &op2);
      PType length;           // ������ �����
      PType R0[4];
      PType L0[4];
      PType C0[4];
      PType Gz0[4];
      PType Cz0[4];
      PType Gm0[3];
      PType Cm0[3];
      PType M0[3];
      // ������ ���������� �� ����������� ��������
      bool set_params(const PType _L0, const PType _R0, const PType _M0,
                      const PType dP_f, const PType dQ_f, const PType U_n_f, const PType U_k_f,
                      const PType dP, const PType dQ, const PType U_n, const PType U_k,
                      const PType freq=50 );
      unsigned Cells();								// ���������� ����� �������
      virtual bool accept();              	// ���������� ��������� ����� ��������� � ������������ � ����������� ��������� �������
      virtual void Save(ostream &stream);		// ���������� ���������� ������� � �����
      virtual void Read(istream &stream);    // �������� ���������� ������� �� ������
      virtual String ClassName();				// ���������� ��� ������
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/*  ������ ���������� ��� �������� ��� ��������� �������� */
class Source_Params{
	public:
   	String Type;						 // ������ ��������
   	vector<PType> Field;			    // ��������� �������� ������� �� ������� ":"
   	void Change(String  description);	// ������� ��������
   	String Get();
   	void Save(ostream &stream);			// ���������  ������
   	void Read(istream &stream);			// ���������  ������
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
class PSource : public PUnit{
   private:
      typedef PUnit sire;  // ���������� �� ������ ������ ��������
      void set_pins();                    // ��������� ������������ ���������� ��������� ������ � ���������������� ����������
      void Describe_Fields();             // ����� ������� ��������� ���� ����� ������
      // �������� ������������� ����� ������������� ������
      bool Mutual_Inductance(PType M_ab, PType M_bc, PType M_ca);
      // ������������ ����� ������������� ������
      bool Mutual_Conductance(unsigned Phase1, unsigned Phase2, PType Gm, PType Cm);
      // ��������� �� 1 ����
      bool Phase_Params(Phase phase, PType __R, PType __L, PType __C, PType Gz, PType Cz);
      /*/ ��������� ���������� �� ��� 3 ����
      void R_(PType value);  // ��������� �������� ������������� �� ����
      void L_(PType value);  // ��������� ������������� ����
      void C_(PType value);  // ��������� ������� ����
      void Gz_(PType value); // ��������� �������� ������������� �� �����
      void Cz_(PType value); // ��������� �������� �� �����
      void Gm_(PType value); // ��������� �������� ����������� �������������
      void Cm_(PType value); // ��������� ����������� ��������
      void Lm_(PType value); // ��������� ����������� ��������������������  */
   public:
      Pins X;             // ������ ����������
      PSource();
      PType (*Ea)(PElement* Sender, double &time);
      PType (*Eb)(PElement* Sender, double &time);
      PType (*Ec)(PElement* Sender, double &time);
      Source_Params sourceParams;      // ���������� ��� �������� ���
      PType R_[4];
      PType L_[4];
      PType C_[4];
      PType Gz_[4];
      PType Cz_[4];
      PType Gm_[3];
      PType Cm_[3];
      PType M_ [3];
      bool update_sources(bool force_update=false);
      virtual bool accept();              // ���������� ��������� ����� ��������� � ������������ � ����������� ��������� �������
      virtual void Save(ostream &stream);		// ���������� ���������� ������� � �����
      virtual void Read(istream &stream);    // �������� ���������� ������� �� ������
      virtual String ClassName();				// ���������� ��� ������
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/* ����������� - Power Consumer */
class PConsumer : public PSource{
   private:
      typedef PSource sire;  // ���������� �� ������ ������ ��������
   public:
      PConsumer();
      bool Load(const PType P,const PType Q, const PType Unom, bool Grounding, const PType freq );
   /*private:
      PType (*Ea)(double &time);
      PType (*Eb)(double &time);
      PType (*Ec)(double &time);
      void set_pins();     // ��������� ������������ ���������� ��������� ������ � ���������������� ����������
      void __fastcall SetR(PType value);  // ��������� �������� ������������� �� ����
      void __fastcall SetL(PType value);  // ��������� ������������� ����
      void __fastcall SetC(PType value);  // ��������� ������� ����
      void __fastcall SetGz(PType value); // ��������� �������� ������������� �� �����
      void __fastcall SetCz(PType value); // ��������� �������� �� �����
      void __fastcall SetGm(PType value); // ��������� �������� ����������� �������������
      void __fastcall SetCm(PType value); // ��������� ����������� ��������
      void __fastcall SetLm(PType value); // ��������� ����������� ��������������������
   public:
      Pins X;             // ������ �����������
      PConsumer();
      virtual bool update_sources(bool force_update=false);
      bool Load(const PType P,const PType Q, const PType Unom, bool Grounding, const PType freq );
            // ��������� ���������� �� ��� 3 ����
      __property PType R_  = { write=SetR };
      __property PType L_  = { write=SetL };
      __property PType C_  = { write=SetC };
      // ��������� ���������� �� ��� 3 ����������� ������������
      __property PType Gz  = { write=SetGz };
      __property PType Cz  = { write=SetCz };
      __property PType Gm  = { write=SetGm };
      __property PType Cm  = { write=SetCm };
      // ��������� ������������������� �� ��� 3 �������
      __property PType Lm_  = { write=SetLm };
      // �������� ������������� ����� ������������� ������
      bool Mutual_Inductance(PType M_ab, PType M_bc, PType M_ca);
      // ������������ ����� ������������� ������
      bool Mutual_Conductance(unsigned Phase1, unsigned Phase2, PType Gm, PType Cm);
      // ��������� �� 1 ����
      bool Phase_Params(Phase phase, PType __R, PType __L, PType __C, PType Gz, PType Cz);
      */
      virtual String ClassName();				// ���������� ��� ������
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/* �����-��������� ��� ������� ������ - Power curve */
class PCurve{
   private:
      vector<PType> _X;
      vector<PType> _Y;
      bool Symmetry;    // 4 �������� ������� �������� ������� ��������� ������������ ������ ���������
      friend void   PFieldInfo::Value(String newVal);   // ���������� �� PFieldInfo �������� ������ � private ������ ����� ������
   protected:
      void _set(PType X, PType Y);
      PType __fastcall X(PType _Y_);
      PType __fastcall Y(PType _X_);
      PType (*Y_X)(PType&);	// ��������� �� ������� ������� �������� ������ ��������������
      							   // � ����� H=f(B)
   public:
      PCurve();
      void clear();                                   // ������� ��������
      unsigned size();                                // ����� �������� (������ ���� ������ �������)
      void Point(unsigned Index, PType &X, PType &Y); // ������ ����������� ����� ��������
      bool exist;                         // ������ ������ (����������)?
      void func( PType (*Func)(PType&) ); // ���������� ������� ��� �������� ������
      virtual void Save(ostream &stream);	// ���������� ���������� ������� � �����
      virtual void Read(istream &stream); // �������� ���������� ������� �� ������
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/* �����-������� ��� ������ �������������� (�����-�������� ��������������) - magnetization curve */
class MagnCurve: public PCurve{
   public:
      void set(PType F, PType I);
      /*PType (__fastcall PCurve::* B)(PType H);         // ��������� �� ������� ������ ��������
      PType (__fastcall PCurve::* H)(PType B);          // ��������� �� ������� ������ �������������*/
      PType __fastcall F(PType _I_);   //
      PType __fastcall I(PType _F_);
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/* �����-������� ��� �����-�������� �������������� - U, I curve */
class UICurve: public PCurve{
   public:
      void set(PType I, PType U);
      PType __fastcall U(PType _I_);
      PType __fastcall I(PType _U_);
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/* ��������� ������ ����������� ��������������
��� ����� ������ �������������� - Power Single Transformer */
class PSingleTransformer : public PUnit{
   	typedef PUnit sire;  	// ���������� �� ������ ������ ��������
      typedef PUnit sire;  // ���������� �� ������ ������ ��������
      void set_pins();
      PType _U1,  _U2, _S;    // ����������� ���������� � �������� ��������������
      PType _dPk, _Uk;  		// ������ ��
      PType _dPx, _Ix;			// ������ ��������� ����
      PType _Iu;					// ��� ���� �������������� �� �������� ����
      PType _freq;            // ����������� �������
	public:
   	PSingleTransformer();
      Pins X1,  X2;			// ������ �������
   	void Params(PType U1,  PType U2, PType S,  // ��������� ����������
      	    PType dPk, PType Uk,
             PType dPx, PType Ix,
             PType freq=50);
      virtual String ClassName();				// ���������� ��� ������
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/* ������� ����� ��� ��������������� */
class PTransformer : public PUnit{
   private:
      typedef PUnit sire;     // ���������� �� ������ ������ ��������
      void Describe_Fields(); // ����� ������� ��������� ���� ����� ������
   protected:
      PTransformer(unsigned NumBranches, unsigned NumNodes, unsigned NumCircuit); // ������ ������ ������� ��������, �.�. ����������� protected-����
   public:
      PType n1, n2;           // ����� ������ �������
      PType U1,  U2, S;       // ����������� ���������� � �������� ��������������
      PType dPk, Uk;  		   // ������ ��
      PType dPx, Ix;			   // ������ ��������� ����
      PType freq;             // ����������� �������
      PType F_nom;            // ��������� ����� �� �������� ����
      virtual void Save(ostream &stream);		// ���������� ���������� ������� � �����
      virtual void Read(istream &stream);    // �������� ���������� ������� �� ������
      virtual String ClassName();				// ���������� ��� ������
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/* ��������� ������ ����������� ��������������
�������� ���� ������ �������������� - Power Saturable Transformer 1 phase */
class PTransformer_1 : public PTransformer{
      typedef PTransformer sire;  // ���������� �� ������ ������ ��������
      void set_pins();
      void Describe_Fields();     // ����� ������� ��������� ���� ����� ������
   public:
      PTransformer_1();
      Pins X1,  X2;			   // ������ �������
      MagnCurve mCurve;       // ������ ������ ��������������
      virtual bool accept();  // ������� ���������� �����
      virtual bool update_elements(bool force_update=false);
      void R1(PType value);    // ��������� �������� ������������� �� ����
      void L1(PType value);    // ��������� ������������� ����������� ����
      void R2(PType value);    // ��������� �������� ������������� �� ����
      void L2(PType value);    // ��������� ������������� ����������� ����
      PType R1();
      PType L1();              // ������������� ��������� ! (�� ������ ��� ������. �������, � ������ ���������)
      PType R2();
      PType L2();
      void Gz1(PType value);   // ��������� �������� ������������� �� �����
      void Cz1(PType value);   // ��������� �������� �� �����
      PType Gz1();             // ������ �������� ������������� �� �����
      PType Cz1();             // ������ �������� �� �����
      void Gz2(PType value);   // ��������� �������� ������������� �� �����
      void Cz2(PType value);   // ��������� �������� �� �����
      PType Gz2();             // ������ �������� ������������� �� �����
      PType Cz2();             // ������ �������� �� �����
      virtual void Save(ostream &stream);		// ���������� ���������� ������� � �����
      virtual void Read(istream &stream);    // �������� ���������� ������� �� ������
      virtual String ClassName();				// ���������� ��� ������
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// ��������� ������ �������������� - Power Transformer
class PTransformer_3 : public PTransformer{
	private:
      typedef PTransformer sire; // ���������� �� ������ ������ ��������
   	void set_pins();           // ��������� ������������ ���������� ��������� ������ � ���������������� ����������
      bool Link_Windings();      // ������ ������� � ������(�����������)
      void Describe_Fields();    // ����� ������� ��������� ���� ����� ������
	public:
   	PTransformer_3();
      Pins X1,  X2;	         // ������ �������
      Pins X1n, X2n;	         // ������ �� ������� ��������
      MagnCircuit_Type  mType;// ��� ��������������
      MagnCurve mCurve;       // ������ ������ ��������������
      virtual bool accept();  // ������� ���������� �����
      virtual bool update_elements(bool force_update=false);
      Winding_Connect Winding_X1, Winding_X2;   // ����� ���������� �������
      // �� 1 �������
      void __fastcall R1(PType value);    // ��������� �������� ������������� �� ����
      void __fastcall L1(PType value);    // ��������� ������������� ����
      void __fastcall C1(PType value);    // ��������� ������� ����
      void __fastcall Gz1(PType value);   // ��������� �������� ������������� �� �����
      void __fastcall Cz1(PType value);   // ��������� �������� �� �����
      PType R1(Phase ph);                 // ������ �������� ������������� �� ����
      PType L1(Phase ph);                 // ������ ������������� ����������� ����
      PType C1(Phase ph);                 // ������ ������� ����
      PType Gz1(Phase ph);                // ������ �������� ������������� �� �����
      PType Cz1(Phase ph);                // ������ �������� �� �����
      // �� 2 �������
      void __fastcall R2(PType value);    // ��������� �������� ������������� �� ����
      void __fastcall L2(PType value);    // ��������� ������������� ����
      void __fastcall C2(PType value);    // ��������� ������� ����
      void __fastcall Gz2(PType value);   // ��������� �������� ������������� �� �����
      void __fastcall Cz2(PType value);   // ��������� �������� �� �����
      PType R2(Phase ph);                 // ������ �������� ������������� �� ����
      PType L2(Phase ph);                 // ������ ������������� ����������� ����
      PType C2(Phase ph);                 // ������ ������� ����
      PType Gz2(Phase ph);                // ������ �������� ������������� �� �����
      PType Cz2(Phase ph);                // ������ �������� �� �����
      virtual void Save(ostream &stream);	// ���������� ���������� ������� � �����
      virtual void Read(istream &stream); // �������� ���������� ������� �� ������
      virtual String ClassName();			// ���������� ��� ������
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/* ������� ������ ��������������� ����
�������� ���� ������ �������������� - Power Current Transformer */
class PCurrent_Transformer : public PUnit{
   private:
   	typedef PUnit sire;  		// ���������� �� ������ ������ ��������
      void Describe_Fields();    // ����� ������� ��������� ���� ����� ������
   protected:
      PCurrent_Transformer(unsigned NumBranches, unsigned NumNodes, unsigned NumCircuit);
      PType F_nom;               // ��������� �����
   public:
      PType S;                   // ����������� ��������
      PType I1,  I2;             // ����������� ���� ��������� � ��������� �������
      PType n1, n2;              // ����� ������ �������
      PType K;                   // ����� ��������, � %  (������� �����������)
      PType d;                   // ������� �����������, �������
      PType freq;                // ����������� �������
      MagnCurve mCurve;          // ������ ������ ��������������
      virtual void Save(ostream &stream);	// ���������� ���������� ������� � �����
      virtual void Read(istream &stream); // �������� ���������� ������� �� ������
      virtual String ClassName();			// ���������� ��� ������
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/* ��������� ������ ����������� �������������� ����
�������� ���� ������ �������������� - Power Current Transformer 1 phase */
class PCurrent_Transformer_1 : public PCurrent_Transformer{
   private:
   	typedef PCurrent_Transformer sire;  // ���������� �� ������ ������ ��������
      void set_pins();     					// ��������� ������������ ���������� ��������� ������ � ���������������� ����������
      void Describe_Fields();             // ����� ������� ��������� ���� ����� ������
   public:
      Pins X1, X2;      // ������ �������
      PType R1, L1;     // ������������� ��������� �������
      PType R2, L2;     // ������������� ��������� �������
      PCurrent_Transformer_1();
      virtual bool accept();     // ������� ���������� �����
      virtual bool update_elements(bool force_update=false);
      virtual void Save(ostream &stream);	// ���������� ���������� ������� � �����
      virtual void Read(istream &stream); // �������� ���������� ������� �� ������
      virtual String ClassName();			// ���������� ��� ������
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/* ���������� �����������  -  Power Breaker */
class PBreaker : public PUnit{
   private:
   	typedef PUnit sire;  	// ���������� �� ������ ������ ��������
      void set_pins();        // ��������� ������������ ���������� ��������� ������ � ���������������� ����������
      void Describe_Fields(); // ����� ������� ��������� ���� ����� ������
      PType T_on[4], T_off[4];// ������ ������ ������� �� ��������� (����������)
   public:
      PType Rmax;             // ������������� � ���� � ����������� ���������
      PType Rmin;             // � ���� �o ���������� ���������
      PType t_on;             // ����������� ����� ���������
      PType t_off;            // ����������� ����� ����������
      PType t_full_off;       // ������ ����� ����������
      bool IsOn;					// ������� ��������� (true - �������)
      Pins X1, X2;            // ������ ��� �����������
      PBreaker();
      bool on(Phase phase);   // �������� ����
      bool off(Phase phase);  // ��������� ����
      bool on();              // ������������ �������� ��� ����
      bool off();             // ������������ ��������� ��� ����
      virtual bool accept();     // ������� ���������� �����
      virtual bool update_elements(bool force_update=false);
    	virtual void Save(ostream &stream);	// ���������� ���������� ������� � �����
      virtual void Read(istream &stream); // �������� ���������� ������� �� ������
      virtual String ClassName();			// ���������� ��� ������
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/* ��������� ������ ������� - Power SubGrid */
class PSubGrid : public PElement{
   private:
   	typedef PElement sire;  	// ���������� �� ������ ������ ��������
      vector<PElement*> Element; // ������ ���������
      void Describe_Fields();		// ������ ����
      void LinksForSubGrid(PSubGrid *grid, Pins_Link &Link);   // ����������� ������� ������ (� ������ ������ �� ��������� ��������) ��� ��������� ����
   public:
      PSubGrid();                // �������-��������, ������ �������� ���������. ������������ � ����������� �������
      //~SubPGrid();
      PElement* __fastcall Unit(int Index);
      unsigned  __fastcall num_Units();
      PElement* obj_from_name(String Name);     // ���������� ������ �� �����
      void setOuts  (String strOut);				// �������������� ����� �������
      String getOuts(); 								// ��������� ����� �������
      bool Change( Pins_Link &InLink,
      				vector<PSubGrid*> SubGrids);  // ���������� �������� ���� �� ������ ���������� �������� � �������� ������ ��������
      virtual bool accept(){return true;};      // ������� ���������� �����
      Pins_Link Links();                        // ���������� ��� ����� ������ ����. ������� ����� ��������
                                                // (Internal_Links �������� ����� ������ ���� ��� ������ � �������� ���� ����)
    	virtual void Save(ostream &stream);			// ���������� ���������� ������� � �����
      virtual void Read(istream &stream); 		// �������� ���������� ������� �� ������
      virtual String ClassName();					// ���������� ��� ������
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/* ��������� ������ ������������� ���� - Power Grid */
class PGrid : public PElement{
	private:
   	typedef PElement sire;  	// ���������� �� ������ ������ ��������
      void Describe_Fields(); 	// ����� ������� ��������� ���� ����� ������
      void set_pins(PElement* Obj,  Pins* pins); // ��������� ������������ ���������� ��������� ������ � ���������������� ����������
      PType *_J;          	// ������-������� �������� ����� � ����� (��������� ����)
      Matrix<PType> *A;		// ������� �� ��������� A*I=B
      PType *B;   			// ������� �� ��������� A*I=B
      Matrix<PType> *F;		// ������� ��� ���������� ����������� ����� f = F*( I*Z - U_L + U_C - U_Lm )
      PElement **Element;     // ��������� �� �������� ������������ � PGrid
      unsigned numElement;    // ����� ���������
      unsigned numLinks;	// ����� ������, �������������� ��� �������� ���� (���������� ��� ����������)
      unsigned *Links;		// �����, �������������� ��� �������� ���� (���������� ��� ����������)
      unsigned **Branch; 	// ������� ������������ ������� ������ � �������������� ������� � [�_�����][�_�����] = �_�����_�������_�������, ���������� ������ (==-1 - ��� ������������)
      unsigned **Node;   	// ������� ������������ ������� ����� � �������������� ������� � [�_�����][�_����]=�_����_�������_�������, ���������� ������ (==-1 - ��� ������������)
      PType *U;				// �������� ����������� �� ������ ����� (���� ������� ���������� � ���)
      PType *_U_Lm2;  		// ������� ���������� �� ������������������� �� �������� ����
      PType **U_Lm2;  		// ������� ���������� �� ������������������� �� �������� ����
      PType *_I2;          // �������������� �������� ����
      void linked_nodes(const unsigned numLink, const unsigned Link[][2][2], bool link_use[],const unsigned i_node,const unsigned obj,const unsigned node);//, unsigned &tmp);
      unsigned __fastcall Node_From_LocalNode(unsigned Obj, unsigned LocalNode);
      unsigned __fastcall Branch_From_LocalBranch(unsigned Obj, unsigned LocalBranch);
      unsigned ObjNum_From_Object(PElement *obj);
      unsigned __fastcall Node_From_LocalNode(PElement* Obj, unsigned LocalNode);
      unsigned __fastcall Branch_From_LocalBranch(PElement* Obj, unsigned LocalBranch);
   	bool Link_Elements(unsigned num, PElement *Block[], unsigned numLink,unsigned Link[][2][2]);	// �������� ���� �� ������������ � �������� ��������� �����
      void Update_Equation();
      virtual bool update_sources(bool force_update=false);
      virtual bool update_elements(bool force_update=false);
      bool BuildOK;           // ����������� � false, ���� ���� ������� � ��������
   public:
      PGrid(Pins_Link Link);  // ���� ��������� �� ������ ��������
      PGrid(PUnit *obj);      // ���� ��������� �� ������ �������
      PGrid();                // �������-��������, ������ �������� ���������. ������������ � ����������� �������
      ~PGrid();
      bool Make_Equation();   // ���� PGrid ������� ����������� ����, ������� ��������� ��� ��������
      void calc();
      //__property PElement* Unit[int Index]={read=GetUnit};
      //__property unsigned num_Units  = { read=Get_num_Units };
      PElement* __fastcall Unit(int Index);
      unsigned  __fastcall num_Units();
      PElement* obj_from_name(String Name);     // ���������� ������ �� �����
      void setOuts  (String strOut);	 			// �������������� ����� �������
      String getOuts();				 					// ��������� ����� �������
      virtual bool accept(){return true;};      // ������� ���������� �����
    	virtual void Save(ostream &stream);			// ���������� ���������� ������� � �����
      virtual void Read(istream &stream); 		// �������� ���������� ������� �� ������
      virtual String ClassName();					// ���������� ��� ������
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/* �������� ������ � */
class tstA: public PUnit{
private:
      /*virtual void __fastcall SetR(PType value);  // ��������� �������� ������������� �� ����
      virtual void __fastcall SetL(PType value);  // ��������� ������������� ����
      virtual void __fastcall SetC(PType value);  // ��������� ������� ����
      virtual void __fastcall SetGz(PType value); // ��������� �������� ������������� �� �����
      virtual void __fastcall SetCz(PType value); // ��������� �������� �� �����
      virtual void __fastcall SetGm(PType value); // ��������� �������� ����������� �������������
      virtual void __fastcall SetCm(PType value); // ��������� ����������� ��������
      virtual void __fastcall SetLm(PType value); // ��������� ����������� ��������������������
      virtual bool Mutual_Inductance(PType M_ab, PType M_bc, PType M_ca);
      virtual bool Mutual_Conductance(unsigned Phase1, unsigned Phase2, PType Gm, PType Cm);
      virtual bool Phase_Params(Phase phase, PType __R, PType __L, PType __C, PType Gz, PType Cz);*/
      //MCircuit *_Magn;  // ��������������
public:
		tstA();
      ~tstA();
      Pins X;
      UICurve BAX;
      //PGrid *Magn;   // ��������� ����
      virtual bool update_sources(bool force_update=false);
      virtual bool update_elements(bool force_update=false);
};
/* �������� ������ � */
class tstB: public PUnit{
private:
      virtual void __fastcall SetR(PType value);  // ��������� �������� ������������� �� ����
      virtual void __fastcall SetL(PType value);  // ��������� ������������� ����
      virtual void __fastcall SetC(PType value);  // ��������� ������� ����
      virtual void __fastcall SetGz(PType value); // ��������� �������� ������������� �� �����
      virtual void __fastcall SetCz(PType value); // ��������� �������� �� �����
      virtual void __fastcall SetGm(PType value); // ��������� �������� ����������� �������������
      virtual void __fastcall SetCm(PType value); // ��������� ����������� ��������
      virtual void __fastcall SetLm(PType value); // ��������� ����������� ��������������������
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
