/*
	ƒискретное преобразование фурье
*/
//---------------------------------------------------------------------------
/*все заинклуденные хидеры до #pragma hdrstop будут прекомпилированы и закешены,
если где еще в C/CPP файлах компил€тор встретит ту же последовательность
хидеров до #pragma hdrstop - то заюзаетс€ уже скомпиленый кеш.*/
#pragma hdrstop
#include "dft.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
DSP::DSP(double &T, unsigned int N){
	this->T = T;
   this->N = N;
   this->w = 2*M_PI/(N*T);
   this->q = w*T;
   this->sample = new double[this->N];
   this->M = 0;
   this->oldest = 0;
}
//---------------------------------------------------------------------------
DSP::~DSP(){
	delete [] this->sample;
}
//---------------------------------------------------------------------------
Vector FFT::calc(double &new_samlpe){
	double x=0, y=0;
   double q_m;	//середина выборки в радианах
   if(M < N){
   	sample[M] = new_samlpe;
   	M++;  					//число элементов в очереди
      oldest = 0;
      q_m = (M-1)*q/2;
   	}
   else{
      sample[oldest] = new_samlpe;
      q_m = 0;
      oldest = (++oldest)%N;
      }
   for(unsigned int n=0, i=oldest; n < M; n++, i=(++i)%N)
   	x += sample[i]*sin(q*n-q_m);
   x = 2*x/N;
   //коэфициент дл€ фильтрации посто€нной составл€ющей
   double A_d = sin(q*M/2)/(M*sin(q/2));
   //поправочный коэффициент из-за подавлени€ посто€нной составл€ющей
   double tmp =  1.0*M/N + sin(q*M)/sin(q)/N;
   double C_gd = 1.0/(1.0 - 2.0/N/M*pow(sin(q*M/2.0),2)/pow(sin(q/2.0),2)/tmp );
   for(unsigned int n=0, i=oldest; n < M; n++, i=(++i)%N)
   	y += sample[i]*(cos(q*n-q_m)-A_d); //с учетом подавлени€ посто€нной состовл€ющей;
   y = 2*y/N;
   y = y * C_gd;
   // ѕоправочные коэфициенты, т.к. M может быть меньше N
   if( M<N && M>1 ){
      double C_h = N/(M - sin(2*M_PI*M/N)/sin(q));
      double C_g = N/(M + sin(2*M_PI*M/N)/sin(q));
      x = x * C_h;
      y = y * C_g;
   	}
   Vector ret(x,y);
   // с учетом приведени€ угла
   if(M<=N && x!=0)    // <= т.к. это вверху было M++, т.е. это все еще неполнопериодный
      {double a=-q_m+M*q; ret.rotate(a);}
   return ret;
}




