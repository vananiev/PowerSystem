#ifndef KernelH
#define KernelH

#define   _ERROR_     0
#define   _WARNING_   3
#define   _NOTICE_    6
#define   _DEBUG_     7


#include <math.h>
//#include "dft.h"
//#include "measuring_transformer.h"
//#include "Matrix.h"  //используем cpp, т.к. в молуле исп. template`s
#include "model.h"
#define sampl_freq 500 			//частота дискретизации
#define FREQ 50						//частота основной гармоники
double T = 1.0/sampl_freq;    	//время между выборками
const unsigned N = 1.0*sampl_freq/FREQ;  //число выборок на период основной частоты
const double w = 2*M_PI*FREQ;		      //круговая частота 1 гармоники
double smp = 1e-5;				// sample - шаг по времени для расчета модели
double TIME=0;                   //текущее время



#endif
 