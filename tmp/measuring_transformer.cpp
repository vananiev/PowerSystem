//---------------------------------------------------------------------------

#pragma hdrstop
#include "measuring_transformer.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
void Current_Transformer::get(double *value){
	value[0] = (*fa)(TIME);
   value[1] = (*fb)(TIME);
   value[2] = (*fc)(TIME);
};
