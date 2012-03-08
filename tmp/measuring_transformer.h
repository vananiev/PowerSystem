//---------------------------------------------------------------------------
#ifndef measuring_transformerH
#define measuring_transformerH
#include <math.h>
extern double T;
extern const double w;
extern double TIME;
//---------------------------------------------------------------------------
class Meas_Transformer{
	//далее она должна сама опознавать величину тока в зависимости от точки установки
   //должны задаваться кривая намагничивания и коэф трансформации
   public:
   	virtual void get(double *value) = 0;
      double (*fa)(double &time);
      double (*fb)(double &time);
      double (*fc)(double &time);
};
class Current_Transformer: public Meas_Transformer{
   public:
		void get(double *value);
};
#endif
