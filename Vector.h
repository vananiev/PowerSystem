//---------------------------------------------------------------------------

#ifndef VectorH
#define VectorH
//---------------------------------------------------------------------------
#include <math.h>
//---------------------------------------------------------------------------
class Vector{
	public:
   double x;
   double y;
   double mod;
   double arg;
   void set(double &x, double &y);
   Vector(double &x, double &y);
   void set_amp(double &A, double &arg);
   void rotate(double &angle);
};

#endif
