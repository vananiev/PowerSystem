//---------------------------------------------------------------------------
#ifndef Dynamic_ArrayH
#define Dynamic_ArrayH
#include <vector>
using namespace std;
//---------------------------------------------------------------------------
#define DYNAMIC_ARRAY_BLOCK_LEN 64   // длинна каждого блока (блок выделяется по мере необходимости в дополнительной памяти)
//using namespace std;
//---------------------------------------------------------------------------
// обеспечивается валидность узазателей на элементы массива (в отличие от вектора)
template<class T> class Dynamic_Array{
   private:
      const unsigned block_len;        // длинна блоков
      unsigned elem_cnt;               // число сохраненных элементов в текущем блоке
      vector <T*> Block;                // Массив указателей на блоки
   public:
      Dynamic_Array();
      Dynamic_Array(unsigned Num_Elements);
      Dynamic_Array(unsigned Blocks_num, unsigned Block_Len);
      ~Dynamic_Array();
      T& push_back(const T &value);          //  сохранить значение
      void clear();                       // очистка содержимого
      unsigned size();                    // размер массива
      T* begin();                         // указатель на первый элемент
      T& operator[](unsigned int i);      // возвращаем элемент массива
      Dynamic_Array operator=(Dynamic_Array op2);
      int index(const T* ptr);            // получить индекс по указателю (возвращает -1 если элемент не содержится в массиве)
   };
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
template<class T> Dynamic_Array<T>::Dynamic_Array():
                                    block_len(DYNAMIC_ARRAY_BLOCK_LEN){
   Block.push_back( new T [block_len] );
   elem_cnt = 0;  // элементов сохранено в текущем блоке
}
//---------------------------------------------------------------------------
/*/ создаем Num_Elements элементов и гарантируем что поместится Max_Num_Elements
template<class T> Dynamic_Array<T>::Dynamic_Array(unsigned Num_Elements, unsigned Max_Num_Elements):
                                    blocks_num(1 + Max_Num_Elements/(Num_Elements+1)),
                                    block_len(Num_Elements+1){
   Block = new T*[blocks_num];
   for(unsigned i=0; i<blocks_num; i++) Block[i] = NULL;
   i_block = 0;   // используемый блок
   Block[ i_block ] = new T [block_len];
   elem_cnt = 0;  // элементов сохранено в текущем блоке
}*/
//---------------------------------------------------------------------------
template<class T> Dynamic_Array<T>::Dynamic_Array(unsigned Num_Elements):
                                    block_len(Num_Elements+1){  // число элементов в блоке равно числу элементов + 1 (иначе вдруг Num_Elements==0)
   Block.push_back( new T[block_len] );
   elem_cnt = Num_Elements;  // элементов сохранено в текущем блоке
}
//---------------------------------------------------------------------------
template<class T> Dynamic_Array<T>::~Dynamic_Array(){
   for(unsigned i=0, n=Block.size(); i<n; i++)
      delete [] Block[i];
}
//---------------------------------------------------------------------------
template<class T> T& Dynamic_Array<T>::push_back(const T &value){
   // сохраняем
   unsigned i_block = Block.size()-1;
   *(Block[ i_block ] + elem_cnt) = value;
   unsigned blk = i_block;
   unsigned ind = elem_cnt;
   elem_cnt++;
   // нужно ли создать новый блок
   if(elem_cnt >= block_len){
      Block.push_back( new T [block_len] );
      elem_cnt = 0;  // элементов сохранено в текущем блоке
      }
   return Block[blk][ind];
}
//---------------------------------------------------------------------------
template<class T> void Dynamic_Array<T>::clear(){
   // все очищаем
   for(unsigned i=0, n=Block.size(); i<n; i++)
      delete [] Block[i];
   Block.clear();
   // создаем по новой
   Block.push_back( new T [block_len] );
   elem_cnt = 0;  // элементов сохранено в текущем блоке
}
//---------------------------------------------------------------------------
template<class T> T& Dynamic_Array<T>::operator[](unsigned int i){
   if( i > size() ) {err(0, "Доступ к несуществующему элементу"); i = i%size();}
   unsigned blk = i / block_len;
   unsigned index = i - blk*block_len;
   return Block[ blk ][ index ];
}
//---------------------------------------------------------------------------
template<class T> unsigned Dynamic_Array<T>::size(){
   return (Block.size()-1) * block_len + elem_cnt;
}
//---------------------------------------------------------------------------
template<class T> Dynamic_Array<T> Dynamic_Array<T>::operator=(Dynamic_Array op2){
   this->clear();
   unsigned n = op2.size();
   for(unsigned i=0, blk1=0, blk2=0, index1=0, index2=0; i<n; i++, index1++, index2++){
      if(index1 >= block_len)      { blk1++; index1=0; }
      if(index2 >= op2.block_len)  { blk2++; index2=0; }
      Block[ blk1 ][ index1 ] = Block[ blk2 ][ index2 ];
      }
   return *this;
}
//---------------------------------------------------------------------------
template<class T> int Dynamic_Array<T>::index(const T* ptr){
   for(unsigned i=0, n=Block.size(); i<n; i++){
      int ind = ptr - Block[i];
      if( ind >= 0 && ind < (signed)block_len  )
         return ( i * block_len + ind );
      }
   return -1;
}
//---------------------------------------------------------------------------
template<class T> T* Dynamic_Array<T>::begin(){  return &Block[0][0]; }
#endif
