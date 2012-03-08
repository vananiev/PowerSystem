//---------------------------------------------------------------------------
#pragma hdrstop
#include "Vector.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
/* --- Конструктор --- */
Vector::Vector(double &x, double &y){set(x,y);}
//---------------------------------------------------------------------------
void Vector::set(double &x, double &y){
	this->x = x;
   this->y = y;
   this->mod = sqrt(x*x + y*y);
   if(x!=0){
   	this->arg = atan(y/x);
      if(x<0) this->arg+=M_PI;
      }
   else
   	this->arg = 0;
}
//---------------------------------------------------------------------------
void Vector::set_amp(double &A, double &arg){
   this->mod = A;
   this->arg = arg;
   this->x = A*cos(arg);
   this->y = A*sin(arg);
}
//---------------------------------------------------------------------------
void Vector::rotate(double &angle){
	arg +=angle;
   x = mod*cos(arg);
   y = mod*sin(arg);
   arg = atan(y/x);
   if(x<0) arg+=M_PI;// приводим угол
}


