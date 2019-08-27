//---------------------------------------------------------------------------
#ifndef matrixH
#define matrixH
#include <vector>
#include <math.h>
#include <system.hpp>
using namespace std;
extern void err(unsigned, String);
//---------------------------------------------------------------------------
typedef long double Float;		// ������������ ���������
//---------------------------------------------------------------------------
template<class T> class Matrix{
   private:
		unsigned __fastcall GetRows(){return rows;};
      unsigned __fastcall GetCols(){return cols;};
      T **A;	// �������� �������
      unsigned int rows; // ����� �����
      unsigned int cols; // ����� ��������
      void Connected_Nodes(unsigned Node, bool used_node[], Matrix &Tree, unsigned &NumWritedBranch); // ����� �������� ����� ��� ���������� ������
   public:
  		Matrix(unsigned int num_rows, unsigned int num_columns); 		// ������� �������
      Matrix(unsigned int num_rows, unsigned int num_columns, T *B);// �������������� �������
      Matrix(Matrix &M);	// ����� ������� - ����� ������
      Matrix(const Matrix<T> &ob);//����������� �����
   	~Matrix();
      T *operator[](unsigned int i);
      Matrix<T> operator=(Matrix<T> op2);
      Matrix<T> operator=(T *op2);
	   Matrix<T> operator+(Matrix<T> &op2);
      Matrix<T> operator-(Matrix<T> &op2);
      Matrix<T> operator*(Matrix<T> &op2);
      void link_to_massiv(T** to);
      friend Matrix<T> operator*(T op1, Matrix<T> op2);
      Matrix<T> transpose();
      unsigned triangular(Matrix &temp);
      Matrix<T> triangular();
      T det();
      Matrix<Float> inverse();
      void Delete_Row(unsigned row);
      Matrix tree(bool deleted_branch[]);	// �� ������� ���������� ����� �������� ���.���. ������, ���������� �������� ����� (deleted_branch[i]==true, ���� �������)
      Matrix tree_with_null_branch(); // �� ������� ���������� ����� �������� ���.���. ������, �������� �� ������ �����
		__property unsigned Rows  = { read=GetRows };
      __property unsigned Cols  = { read=GetCols };
      void Distance_From_Node(unsigned Start_Node, unsigned Distance[]);	// ����� ����������� ���������� �� ���� �� ����
		Matrix<char> Way(unsigned Start_Node, unsigned End_Node); // ���������� ���� �� ���� �� ����
		friend void Multiply_Matrix(Matrix<Float>  *op1, Float op2[], Float *result[]);
};
//-------------------------------------------------------------------------
template < class T > Matrix<T>::Matrix(unsigned int num_rows, unsigned int num_columns){
   rows = num_rows;
   cols = num_columns;
	A = new T*[rows];
   for(unsigned k=0; k<rows;k++)
   	A[k] = new T[cols];
   for(unsigned int i=0; i<rows; i++)
   	for(unsigned int j=0; j<cols; j++)
      	A[i][j]=0;
}
//-------------------------------------------------------------------------
template < class T > Matrix<T>::Matrix(unsigned int num_rows, unsigned int num_columns, T *B){
   rows = num_rows;
   cols = num_columns;
	A = new T*[rows];
   for(unsigned k=0; k<rows;k++)
   	A[k] = new T[cols];
   for(unsigned int i=0; i<rows; i++){
   	T *p = (B+i*cols);
   	for(unsigned int j=0; j<cols; j++)
      	A[i][j]=p[j];
      }
}
//---------------------------------------------------------------------------
template < class T > Matrix<T>::Matrix(Matrix &M){
   rows = M.rows;
   cols = M.cols;
	A = new T*[rows];
   for(unsigned k=0; k<rows;k++)
   	A[k] = new T[cols];
   for(unsigned int i=0; i<rows; i++)
   	for(unsigned int j=0; j<cols; j++)
      	A[i][j]=M.A[i][j];
}
//---------------------------------------------------------------------------
// ��� ����������� �����
template < class T > Matrix<T>::Matrix(const Matrix<T> &ob){
   rows = ob.rows;
   cols = ob.cols;
   A = new T*[rows];
   for(unsigned k=0; k<rows;k++){
   	try{ A[k] = new T[cols];}
      catch(std::bad_alloc xa) {err(0, "�������� ������");}
      }
   for(unsigned int i=0; i<rows; i++)
   	for(unsigned int j=0; j<cols; j++)
      	A[i][j]=ob.A[i][j];
}
//---------------------------------------------------------------------------
template <class T>  Matrix<T>::~Matrix(){
   for(unsigned k=0; k<rows;k++)
   	{delete [] A[k];  A[k] = NULL;}
   delete [] A; A = NULL;
}
//---------------------------------------------------------------------------
template <class T> Matrix<T>  Matrix<T>::operator+(Matrix<T> &op2){
	Matrix<T> temp(rows, cols);
   if(rows!=op2.rows || cols!=op2.cols) {err( 0, "�������� ������: ������� ����� ������ ������ __FILE__:__LINE__" ); return temp;}
	for(unsigned int i=0; i<rows; i++)
      for(unsigned int j=0; j<cols; j++)
      	temp.A[i][j] = A[i][j] + op2.A[i][j];
   return temp;
}
//---------------------------------------------------------------------------
template <class T> T* Matrix<T>::operator[](unsigned int i){
	return A[i];
}
//---------------------------------------------------------------------------
template<class T> Matrix<T> Matrix<T>::operator=(Matrix<T> op2){
   if(rows!=op2.rows || cols!=op2.cols) {err( 0, "������������ �������: ������� ����� ������ ������" ); return *this;}
	for(unsigned int i=0; i<rows; i++)
      for(unsigned int j=0; j<cols; j++)
      	A[i][j] = op2.A[i][j];
   return *this;
}
//---------------------------------------------------------------------------
template<class T> Matrix<T> Matrix<T>::operator=(T *op2){
   for(unsigned int i=0; i<rows; i++)
   	for(unsigned int j=0; j<cols; j++)
      	A[i][j]=*(op2+i*cols+j);
   return *this;
}
//---------------------------------------------------------------------------
template<class T> Matrix<T>  Matrix<T>::operator-(Matrix<T> &op2){
	Matrix<T> temp(rows, cols);
   if(rows!=op2.rows || cols!=op2.cols) {err( 0, "��������� ������: ������� ����� ������ ������"); return temp;}
	for(unsigned int i=0; i<rows; i++)
      for(unsigned int j=0; j<cols; j++)
      	temp.A[i][j] = A[i][j] - op2.A[i][j];
   return temp;
}
//---------------------------------------------------------------------------
template<class T> Matrix<T>  Matrix<T>::operator*(Matrix<T> &op2){
	Matrix<T> temp(rows, op2.cols);
   if(cols!=op2.rows) {err( 0, "��������� ������: ����� ����� � �������� �� ���������"); return temp;} 
	for(unsigned int i=0; i<rows; i++)
      for(unsigned int j=0; j<op2.cols; j++){
         temp.A[i][j] = 0;
      	for(unsigned int k=0; k<cols; k++)
      		temp.A[i][j] += A[i][k] * op2.A[k][j];
         }
   return temp;
}
//---------------------------------------------------------------------------
// ������ ������ ��������� �� �������-�������
template<class T> void Matrix<T>::link_to_massiv(T** to){
   if(cols!=1) {err( 0, "������������� ������� ������� � ������: ����� �������� � ������� �� ����� 1"); return;}
   for(unsigned int j=0; j<rows; j++)
   	to[j] = &A[j][0];
}
//---------------------------------------------------------------------------
// ������������� �������
template<class T> Matrix<T> Matrix<T>::transpose(){
	Matrix<T> temp(cols, rows);
	for(unsigned int i=0; i<rows; i++)
      for(unsigned int j=0; j<cols; j++)
      	temp.A[j][i] = A[i][j];
   return temp;
}
//---------------------------------------------------------------------------
/* ���������� ������� � ������������ ���� (���������� ����� ������������)*/
template<class T> unsigned Matrix<T>::triangular(Matrix &temp){
   if(temp.cols != temp.rows) {err( 0, "���������� ������� � ������������ ����: ����� ����� � �������� �� ���������"); return 0;}
   // �� ������ ������ �� ��� ����?
   unsigned h;
   for(unsigned h=0; h< rows; h++) if(fabs(temp.A[h][0])>1e-300) break;
   if(h==rows){err( 0, "��������� �������� �������: ��� �������� � ������ ������� ������� �������"); return 0;}
   unsigned u=0;	// ����� ������������ �����
   for(unsigned i=0; i<rows-1; i++){
      // ���� ������������ ������� ���� ������ ������ �������
      if(fabs(temp.A[i][i]) < 1e-300){
         //����� ������ � ��������� ������������ ���������
         unsigned NotNull; for(NotNull=i+1; NotNull<rows; NotNull++) if(fabs(temp.A[NotNull][i])>1e-300) break;
         if(NotNull==rows) {err( 0, "���������� ������� � ������������ ����: ������ � ������ ���������� ������������� ��������"); return 0;}
         T *tmp = temp.A[i];
         temp.A[i] = temp.A[NotNull];
      	temp.A[NotNull] = tmp;
         u++;
         }
      for(unsigned k=i+1; k<rows; k++){
      	if(temp.A[k][i] != 0){
         	// ��������� ���������
         	T d = -1.0*temp.A[k][i]/temp.A[i][i];
         	// ���������� ���������� ������
         	temp.A[k][i] = 0;
         	for(unsigned j=i+1; j<cols; j++)
            	temp.A[k][j] = d*temp.A[i][j] + temp.A[k][j];
            }
      	}
   	}
   return u;
}
//---------------------------------------------------------------------------
/* ���������� ������� � ������������ ���� */
template<class T> Matrix<T> Matrix<T>::triangular(){
	Matrix<T> temp(*this);
   triangular(temp);
   return temp;
}
//---------------------------------------------------------------------------
/* ������� ������������ ������� */
template<class T> T Matrix<T>::det(){
	T d;
   if(cols != rows) {err( 0, "����� ������������: ������� �� ����������"); return 0;}
   Matrix temp(*this);
   unsigned u = triangular(temp);
   if( u%2==0 ) d=1; else d=-1;
   for(unsigned i=0; i<rows; i++)
   	d *= temp.A[i][i];
   return d;
}
//---------------------------------------------------------------------------
/* ������� �������� ������� ������� ������ � ������� */
template<class T> Matrix<Float> Matrix<T>::inverse(){
   Matrix temp(*this);
   Matrix E(rows, cols);
   if(temp.cols != temp.rows) {err( 0, "��������� �������� �������: �������� ������� �� ����������"); return E;}
   // �� ������ ������ �� ��� ����?
   unsigned h;
   for(h=0; h< rows; h++) if(fabs(A[h][0])>1e-300) break;
   if(h==rows){err( 0, "��������� �������� �������: ��� �������� � ������ ������� ������� �������"); return E;}
   // ��������� ��������� �������
   for(unsigned i=0; i<rows; i++) E.A[i][i] = 1;
   unsigned u=0;	// ����� ������������ �����
   // � ������ �����������
   for(unsigned i=0; i<rows-1; i++){
      // ������� ������������ ������� � �������
      unsigned max_i = i;
      Float max = fabs(temp.A[i][i]);
      for(unsigned r=i+1; r<rows-1; r++)
         if(fabs(temp.A[r][i]) > max){
            max = fabs(temp.A[r][i]);
            max_i = r;
            }
      if(max_i != i){ // ������ ������ �������
         Float *tmp = temp.A[i];
         temp.A[i] = temp.A[max_i];
      	temp.A[max_i] = tmp;
         u++;
         // � ��� ���������
         tmp = E.A[i];
      	E.A[i] = E.A[max_i];
      	E.A[max_i] = tmp;
         }
      // ���� ������������ ������� ���� ������ ������ �������
      if(fabs(temp.A[i][i]) < 1e-300){
         //����� ������ � ��������� ������������ ���������
         unsigned NotNull; for(NotNull=i+1; NotNull<rows; NotNull++) if(fabs(temp.A[NotNull][i])>1e-300) break;
         if(NotNull==rows) {err( 0, "���������� ������� � ������������ ����: ������ � ������ �� �������� ������������� ��������"); return E;}
         Float *tmp = temp.A[i];
         temp.A[i] = temp.A[NotNull];
      	temp.A[NotNull] = tmp;
         u++;
         // � ��� ���������
         tmp = E.A[i];
      	E.A[i] = E.A[NotNull];
      	E.A[NotNull] = tmp;
         }
      // �������� �� ������������ �������� "1"
      Float d = temp.A[i][i];
      for(unsigned j=i; j<cols; j++)
      	temp.A[i][j] = temp.A[i][j]/d;
      for(unsigned j=0; j<cols; j++)
      	E.A[i][j] = E.A[i][j]/d;
      for(unsigned k=i+1; k<rows; k++){
         	// ��������� ���������
         	Float d = -temp.A[k][i];
         	// ���������� ���������� ������
         	temp.A[k][i] = 0;
         	for(unsigned j=i+1; j<cols; j++)
            	temp.A[k][j] = d*temp.A[i][j] + temp.A[k][j];
            for(unsigned j=0; j<cols; j++)
            	E.A[k][j] = d*E.A[i][j] + E.A[k][j];
      	}
   	}
   // �������� �� ������� ������������ �������� "1"
	Float d = 1.0*temp.A[rows-1][rows-1];
   if(d==0){
      err(0,"Matrix::inverse(): ������� �� ����");
      d=1e-100;}
   temp.A[rows-1][rows-1] = 1;
   for(unsigned j=0; j<cols; j++)
      E.A[rows-1][j] = E.A[rows-1][j]/d;

   //� �������� �����������
   for(unsigned i=rows-1; i>0; i--){
      for(unsigned k=0; k<i; k++){
      	if(fabs(temp.A[k][i])>1e-300){
         	// ��������� ���������
         	Float d = -temp.A[k][i];
         	// ���������� ���������� ������
         	temp.A[k][i] = 0;
            for(unsigned j=0; j<cols; j++)
            	E.A[k][j] = d*E.A[i][j] + E.A[k][j];
            }
      	}
   	}
   return E;
}
//---------------------------------------------------------------------------
template<class T> Matrix<char> Matrix<T>::Way(unsigned Start_Node, unsigned End_Node){
   Matrix<char> way(1,cols);
   if(Start_Node==End_Node) return way;
	unsigned *Distance = new unsigned[cols];
   for(unsigned i=0;i<cols;i++) Distance[i] = -1;
   // ����� ���������� �� ����
   Distance[Start_Node] = 0;
   Distance_From_Node(Start_Node, Distance);
   //unsigned g[200];
   //for(unsigned i=0; i<cols;i++)g[i]=Distance[i];
   // ���������� ����
   unsigned nB = cols;
   unsigned nN = rows;
   // ���� �� �������� �������
   unsigned s = End_Node;
   while( s != Start_Node){
   		unsigned Dis = Distance[s]-1; // ���� �������� ���� �� ����� ���������� �� ����������
   		// ���� ����� �� �������� �������
         unsigned i;
   		for(i=0;i<nB;i++){
   			if(A[s][i]!=0){
               // � ����� ���������������� ������� ��������
               unsigned j;
               for(j=0;j<nN;j++)
               	if(A[j][i]!=0 && Dis == Distance[j]){
                     way[0][i] = A[s][i];	// 1 ��� -1 -� ����������� �� ����������� ��� ������ �����
               		s = j;
               		break;
                     }
               if(j<nN) break; // ������� � ������ ����, ����� ������ ����������
            	}
            }
         // ��� ����
         if(i >= nB){
            for(unsigned x=0;x<nB;x++) way[0][x]=0;
            break;
         	}
      	}
   delete [] Distance;
   return way;
}
//---------------------------------------------------------------------------
// ����������� ����� ���������� �� ����
template<class T> void Matrix<T>::Distance_From_Node(unsigned Start_Node, unsigned Distance[]){ //, End_Node
   vector<unsigned> Cocedi;
   Float Find_Dist = Distance[Start_Node]+1;
   //���� �������� �� �������������� ����
   for(unsigned j=0; j<cols; j++)
   	if(fabs(A[Start_Node][j])>1e-300) // ���� ���� �����, ������ �� ������ ��� ����
      	for(unsigned i=0; i<rows; i++)
         	if(fabs(A[i][j])>1e-300 && Find_Dist<Distance[i]){	// ���� �� �� ���� ���������� ������
               Distance[i] = Find_Dist;			// �������� ����
               Cocedi.push_back(i);
            	}
   // ���� ����������
   for(unsigned i=0, n=Cocedi.size(); i<n; i++)
   	Distance_From_Node(Cocedi[i], Distance);//, End_Node ����� �������� �����
}
//---------------------------------------------------------------------------
// ����������� ����� ���������� �� ����
template<> void Matrix<char>::Distance_From_Node(unsigned Start_Node, unsigned Distance[]){
   std::vector <unsigned> Cocedi;  // std::vector
   unsigned Find_Dist = Distance[Start_Node]+1;
   //���� �������� �� �������������� ����
   for(unsigned j=0; j<cols; j++)
   	if(A[Start_Node][j]!=0) // ���� ���� �����, ������ �� ������ ��� ����
      	for(unsigned i=0; i<rows; i++)
         	if(A[i][j]!=0 && Find_Dist<Distance[i]){	// ���� �� �� ���� ���������� ������
               Distance[i] = Find_Dist;			// �������� ����
               Cocedi.push_back(i);
            	}
   // ���� ����������
   for(unsigned i=0, n=Cocedi.size(); i<n; i++)
   	Distance_From_Node(Cocedi[i], Distance);// ����� �������� �����
}
//---------------------------------------------------------------------------
// ����������� ����� �������� �����
template<class T> void Matrix<T>::Connected_Nodes(unsigned Node, bool used_node[], Matrix &Tree, unsigned &NumWritedBranch){
   //���� �������� �� �������������� ����
   for(unsigned j=0; j<cols; j++)
   	if(fabs(A[Node][j])>1e-300) // ���� ���� �����, ������ �� ������ ��� ����
      	for(unsigned i=0; i<rows; i++)
         	if(fabs(A[i][j])>1e-300 && !used_node[i]){	// ���� �� �� ��������������� ����
               used_node[i] = true;			// �������� ����
               for(unsigned s=0; s<rows; s++) Tree.A[s][j] = A[s][j]; // �������� ����������
               NumWritedBranch++;
               if(NumWritedBranch < (Tree.rows-1))	// ����� �� ����������
               	Connected_Nodes(i, used_node, Tree, NumWritedBranch);// ����� �������� �����
            	break;
            	}
}
//---------------------------------------------------------------------------
template<class T> Matrix<T> Matrix<T>::tree(bool deleted_branch[]){
// ����� ����� � ������ ������ ���� �� 1 ��� ������
	Matrix Res(rows, rows-1);
   Matrix Tree(rows, cols);
   // ���� ������
   Tree = tree_with_null_branch();
   // ������� ������� �������
   for(unsigned i=0; i<cols;i++) deleted_branch[i]=true;	// ���������� �������, ��� ��� ����� �������
   unsigned write_col=0;
   for(unsigned h=0;h<Tree.cols;h++){
   	unsigned i;
   	for(i=0;i<Tree.rows;i++)if(fabs(Tree.A[i][h])>1e-300) break;
      if(i!=Tree.rows){    // � ������� �� ��� ����
         deleted_branch[h] = false;	// ����� �� �������
         for(unsigned r=0;r<Tree.rows;r++) Res.A[r][write_col]=Tree.A[r][h];
         write_col++;
         }
      }
   if(write_col != Res.Cols){err(0, "Matrix::tree: ��� ���������� ������ �� ����������� �����_������ == (�����_�����-1)");}
   return Res;
   /*
   // ��������� ������ �����
   unsigned k=0;  // ����� ����������� ����� � ������
   unsigned j;    // ������� �����
   for(j=0; j<cols; j++){
   	unsigned h=0;
   	// �� ��� �� �������� ������� == 0 ?
   	for(h=0; h<rows; h++) if(fabs(A[h][j])>1e-300) break;
   	if(h!=rows){
      	for(unsigned i=0; i<rows; i++){
            if(fabs(A[i][j])>1e-300) used_node[i]=true;
         	temp.A[i][k] = A[i][j];
            }
         deleted_branch[j] = false; // ��������� j-�����
         k++;
      	break;
         }
      }
   // ��������� ����� ����������� �� ����� ����
   for(j++; j<cols; j++){
     if(k<temp.cols) // ���� ����� ��� �����
   	for(unsigned i=0; i<rows; i++)
      	if(fabs(A[i][j])>1e-300 && !used_node[i]){
         	// ���������
            for(unsigned s=0; s<rows; s++) temp.A[s][k] = A[s][j];
            deleted_branch[j] = false; // ��������� j-�����
            used_node[i] = true;			// ������� ��� ������������ �������
            k++; // � ����� ������ ����� �� ���� ����� ������
            break;
         	}
   	} */
}
//---------------------------------------------------------------------------
template<class T> Matrix<T> Matrix<T>::tree_with_null_branch(){
   // ����� ����� � ������ ������ ���� �� 1 ��� ������
   Matrix Tree(rows, cols);
   bool *used_node = new bool[Tree.rows];
   for(unsigned i=0; i<Tree.rows;i++) used_node[i]=false;
   // ��������� ������� ����
   static unsigned Uz=0; // ������� ����
   used_node[Uz]=true;
   unsigned k=0;	 // ����� ������� ������ � ������
   //���� �������� �� �������������� ����
   Connected_Nodes(Uz, used_node, Tree, k);
   delete [] used_node;
   return Tree;
	/*/ ����� ����� � ������ ������ ���� �� 1 ��� ������
	Matrix temp(rows, cols);
   bool *used_node = new bool[temp.rows];
   for(unsigned i=0; i<temp.rows;i++) used_node[i]=false;
   // ��������� ����� ����������� �� ����� ����
   for(unsigned j=0; j<cols; j++){
   	unsigned i;
   	for(i=0; i<rows; i++)
      	if(fabs(A[i][j])>1e-300 && !used_node[i]){ // ��������� �� ����� ����� ����?
         	// ��
            for(unsigned s=0; s<rows; s++) temp.A[s][j] = A[s][j];
            break;
         	}
      if(i==rows) // ���
          for(unsigned s=0; s<rows; s++) temp.A[s][j] = 0;
   	}
   return temp;*/
}
//---------------------------------------------------------------------------
template<class T> void Matrix<T>::Delete_Row(unsigned row){
	for(unsigned i=row; i<(rows-1);i++) A[i] = A[i+1];
   delete A[rows-1];
   rows--;
}
//---------------------------------------------------------------------------
template<class T> Matrix<Float> Multiply_Matrix(Matrix<T>  *op1, Float ***op2, unsigned op2_rows, unsigned op2_cols)
{
   Matrix<Float> temp(op1->Rows, op2_cols);
   if(op1->Cols!=op2_rows) {err( 0, "��������� ������: ����� ����� � �������� �� ���������"); return temp;}
   for(unsigned int i=0; i<op1->Rows; i++)
      for(unsigned int j=0; j<op2_cols; j++){
         temp[i][j] = 0;
      	for(unsigned int k=0; k<op1->Cols; k++)
      		temp[i][j] += (Float)(*op1)[i][k] * (*op2[k][j]);
         }
   return temp;
}
//---------------------------------------------------------------------------
bool Copy_Matrix(Matrix<char> *from, Matrix<Float> *to);
#endif

