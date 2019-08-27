#ifndef KernelH
#define KernelH

#define   _ERROR_     0
#define   _WARNING_   3
#define   _NOTICE_    6
#define   _DEBUG_     7


#include <math.h>
//#include "dft.h"
//#include "measuring_transformer.h"
//#include "Matrix.h"  //���������� cpp, �.�. � ������ ���. template`s
#include "model.h"
#define sampl_freq 500 			//������� �������������
#define FREQ 50						//������� �������� ���������
double T = 1.0/sampl_freq;    	//����� ����� ���������
const unsigned N = 1.0*sampl_freq/FREQ;  //����� ������� �� ������ �������� �������
const double w = 2*M_PI*FREQ;		      //�������� ������� 1 ���������
double smp = 1e-5;				// sample - ��� �� ������� ��� ������� ������
double TIME=0;                   //������� �����



#endif
 