/*
	���������� �������������� �����
*/
//---------------------------------------------------------------------------
/*��� ������������� ������ �� #pragma hdrstop ����� ���������������� � ��������,
���� ��� ��� � C/CPP ������ ���������� �������� �� �� ������������������
������� �� #pragma hdrstop - �� ��������� ��� ����������� ���.*/
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
   double q_m;	//�������� ������� � ��������
   if(M < N){
   	sample[M] = new_samlpe;
   	M++;  					//����� ��������� � �������
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
   //���������� ��� ���������� ���������� ������������
   double A_d = sin(q*M/2)/(M*sin(q/2));
   //����������� ����������� ��-�� ���������� ���������� ������������
   double tmp =  1.0*M/N + sin(q*M)/sin(q)/N;
   double C_gd = 1.0/(1.0 - 2.0/N/M*pow(sin(q*M/2.0),2)/pow(sin(q/2.0),2)/tmp );
   for(unsigned int n=0, i=oldest; n < M; n++, i=(++i)%N)
   	y += sample[i]*(cos(q*n-q_m)-A_d); //� ������ ���������� ���������� ������������;
   y = 2*y/N;
   y = y * C_gd;
   // ����������� �����������, �.�. M ����� ���� ������ N
   if( M<N && M>1 ){
      double C_h = N/(M - sin(2*M_PI*M/N)/sin(q));
      double C_g = N/(M + sin(2*M_PI*M/N)/sin(q));
      x = x * C_h;
      y = y * C_g;
   	}
   Vector ret(x,y);
   // � ������ ���������� ����
   if(M<=N && x!=0)    // <= �.�. ��� ������ ���� M++, �.�. ��� ��� ��� ����������������
      {double a=-q_m+M*q; ret.rotate(a);}
   return ret;
}




