//---------------------------------------------------------------------------

#pragma hdrstop

#include "matrix.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
// умножение матрицы на число
Matrix<Float> operator*(Float op1, Matrix<Float> op2){
	Matrix<Float> temp(op2.rows, op2.cols);
	for(unsigned int i=0; i<op2.rows; i++)
      for(unsigned int j=0; j<op2.cols; j++)
      	temp.A[i][j] = op1 * op2.A[i][j];
   return temp;
}
//---------------------------------------------------------------------------
// перемножение с созврат результата в матрицу N x 1
void Multiply_Matrix(Matrix<Float>  *op1, Float op2[], Float *result[]){
	for(unsigned int i=0; i<op1->rows; i++){
         *result[i] = 0;
      	for(unsigned int k=0; k<op1->cols; k++)
      		(*result[i]) += op1->A[i][k] * op2[k];
         }
}
//---------------------------------------------------------------------------
// копирование одной матрицы в другую
bool Copy_Matrix(Matrix<char> *from,Matrix<Float> *to){
   if(from->Rows!=to->Rows || from->Cols!=to->Cols) {err( 0, "Присваивание матрицы: матрицы имеют разный размер" ); return false;}
	for(unsigned int i=0; i<from->Rows; i++)
      for(unsigned int j=0; j<from->Cols; j++)
      	(*to)[i][j] = (Float)(*from)[i][j];
   return true;
}

