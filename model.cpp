//---------------------------------------------------------------------------
#pragma hdrstop

#include "model.h"
//---------------------------------------------------------------------------
PObjectList ObjectList;    // содержит список всех созданных объектов
//---------------------------------------------------------------------------
PObjectList::~PObjectList(){
   // находим все объекты из списка, у которых нет владельца
   vector<PElement*> toDel;
   vector<PElement*>::iterator iter = Obj.begin();
    while( iter != Obj.end() ){
      if((*iter)->Owner == NULL) // удаляем объекты без владельца, остальные должны быть удалены владельцами
         toDel.push_back(*iter);
      iter++;
      }
   // удаляем все объекты из списка, у которых нет владельца
   for(int i=0, n=toDel.size(); i<n; i++)
      delete toDel[i];
   Obj.clear();
}
//---------------------------------------------------------------------------
void PObjectList::Add    (PElement* obj){
   Obj.push_back(obj);
}
//---------------------------------------------------------------------------
bool PObjectList::Delete (PElement* obj){
   vector<PElement*>::iterator iter = Obj.begin();
    while( iter != Obj.end() )
    {
      if( *iter == obj){
        Obj.erase( iter );
        return true;
        }
      else
        ++iter;
    }
   return false;
}
//---------------------------------------------------------------------------
// имя может быть указано через с указанием сетей, в которые входит объект
// например ОЭС_Ценрта.Самарские_РЭС.Т1
// имя объекта (сети или оборудования) в своей подсети должно быть уникально
PElement* PObjectList::Find (String Name){
   String obj, grid;   // объект и объект который в него входит, например ОЭС.Сеть2.T2
   // зазделяем имя и полное имя сети
   int point = 0;
   for(int i=Name.Length(); i>0; i--) if(Name[i]=='.') {point=i;break;}
   if(point!=0){
      grid = Name.SubString(1,point-1);
      obj = Name.SubString(point+1, Name.Length()-point);
   }else{
      grid = "";
      obj = Name;
      }
   // найдем этот объект
   for(int i=0,n=Obj.size(); i<n; i++)
   	if( Obj[i]->Name == obj && Obj[i]->GridName() == grid)
      	return Obj[i];
   return NULL;
   /*/ зазделяем имя до первой точки
   int point=Name.Pos(".");
   if(point!=0){
      grid = Name.SubString(1,point-1);
      obj = Name.SubString(point+1, Name.Length()-point);
   }else{
      grid = "";
      obj = Name;
      }
   // вернем объект
   if(grid!=""){
   	// ищем внутри указанной сети
      // имя сети первого уровня должно быть уникально
      PSubGrid *gr = dynamic_cast<PSubGrid*>(  Find( grid ) );
      if ( gr ) return gr->obj_from_name( obj );
      else      return NULL;
      }
	else
		{
      // указано имя без родителя
      PElement *ret = NULL;
      int cnt=0;
   	for(int i=0,n=Obj.size(); i<n; i++)
      	if( Obj[i]->Name == Name && Obj[i]->Grid == NULL){  // ищем в сети верхнего уровня
         	ret = Obj[i];
            cnt++;
            }
      if( cnt>1 ){
      	err(0, "PObjectList: имется "+ IntToStr(cnt) +" объекта с именем: "+Name);
         return NULL;}
      else
      	return ret;
		} */
}
//---------------------------------------------------------------------------
PPin* PObjectList::FindPin(String Name, unsigned pin){
   PElement* obj = Find(Name);
   if(obj){
      for(int i=0, n=obj->Out.Count(); i<n; i++)
         for(int j=0, m=obj->Out[i].Count; j<m; j++)
            if(obj->Out[i][j].Node() == pin)
               return &obj->Out[i][j];
      }
   return NULL;
}
//---------------------------------------------------------------------------
PElement::PElement():null(0),MIN(1e-3),MAX(1e8){
   M = NULL;
   N = NULL;
   R = NULL;
   L = NULL;
   C = NULL;
   Lm = NULL;
   D = NULL;
   E = NULL;
   J = NULL;
   U_C = NULL;
   U_L = NULL;
   U_Lm = NULL;
   U_D = NULL;
   Z = NULL;
   I = NULL;
   f = NULL;
   Owner = NULL;
   Grid = NULL;
   Grounded_Node = 0;   // заземляемый узел
   // сохраняем объект в список
   ObjectList.Add( this );
   // опишем поля класса
   Describe_Fields();
}
//---------------------------------------------------------------------------
PElement::PElement(unsigned NumBranches, unsigned NumNodes, unsigned NumCircuit):null(0),MIN(1e-3),MAX(1e8){
   v = NumBranches;
   u = NumNodes;
   k = NumCircuit;
   E = new PType*[v];
   U_C= new PType*[v];
   U_L= new PType*[v];
   U_Lm= new PType*[v];
   D = new Throttle*[v];  for(unsigned i=0;i<v;i++) D[i] = NULL; // дроссели создаются по мере необходимости
   U_D= new PType*[v];
   R = new PType*[v];
   L = new PType*[v];
   C = new PType*[v];
   Z = new PType*[v];
   I = new PType*[v];
   J = new PType*[u];
   f = new PType*[u];
   //Lm = new vector<PType>::iterator*[v]; for(unsigned i=0;i<v;i++)Lm[i] = new vector<PType>::iterator [v];
   Lm = new PType**[v]; for(unsigned i=0;i<v;i++)Lm[i] = new PType*[v];
   M = new Matrix<char>(u, v);
   N = new Matrix<char>(k, v);
   Owner = NULL;
   Grid = NULL;
   Grounded_Node = 0;   // заземляемый узел
   // сохраняем объект в список
   ObjectList.Add( this );
   // опишем поля класса
   Describe_Fields();
}
//---------------------------------------------------------------------------
PElement::PElement(PElement *obj):null(0),MIN(1e-3),MAX(1e8){
   if(obj->M->Cols != obj->N->Cols){err(0,"Создание объекта Element число строк матриц инцеденций не равны между собой");}
   v = obj->M->Cols;
   u = obj->M->Rows;
   k = obj->N->Rows;
   _Lm = obj->_Lm;
   E = new PType*[v];
   U_C= new PType*[v];
   U_L= new PType*[v];
   U_Lm= new PType*[v];
   D = new Throttle*[v];   for(unsigned i=0;i<v;i++) D[i] = obj->D[i];  // копируем указатели на дроссели
   U_D= new PType*[v];
   R = new PType*[v];
   L = new PType*[v];
   C = new PType*[v];
   Z = new PType*[v];
   I = new PType*[v];
   J = new PType*[u];
   f = new PType*[u];
   //Lm = new vector<PType>::iterator*[v]; for(unsigned i=0;i<v;i++)Lm[i] = new vector<PType>::iterator [v];
   Lm = new PType**[v]; for(unsigned i=0;i<v;i++)Lm[i] = new PType*[v];
   M = new Matrix<char>(*obj->M);
   N = new Matrix<char>(*obj->N);
   Owner = NULL;
   Grid = NULL;
   Grounded_Node = obj->Grounded_Node;   // заземляемый узел
   // сохраняем объект в список
   ObjectList.Add( this );
   // опишем поля класса
   Describe_Fields();
}
//---------------------------------------------------------------------------
void PElement::Describe_Fields(){
   // Сохраняем информацию о свойствах класса
   PFieldInfo fi;
   fi.Name = "Имя";
   fi.Type = "String";
   fi.ptr = (void*) &Name;
   Fields.push_back(fi);
}
//---------------------------------------------------------------------------
PElement::~PElement(){
   delete M;
   delete N;
   delete [] R;
   delete [] L;
   delete [] C;
   if(Lm!=NULL)
      for(unsigned j=0;j<v;j++)
         delete [] Lm[j];
   delete [] Lm;
   delete [] D;
   delete [] E;
   delete [] J;
   delete [] U_C;
   delete [] U_L;
   delete [] U_Lm;
   delete [] U_D;
   delete [] Z;
   delete [] I;
   delete [] f;
   // удаляем объект из списока
   ObjectList.Delete( this );
}
//---------------------------------------------------------------------------
unsigned __fastcall PElement::Branches(){ return v; }
unsigned __fastcall PElement::Nodes(){ return u; }
unsigned __fastcall PElement::Circuit(){ return k; }
//---------------------------------------------------------------------------
void PElement::mutual_inductance(unsigned branch_1, unsigned branch_2, const PType &value){
   if(value==0) return;
   // взаимоиндуктивность не может быть больше корня произведения индуктивностей
   if((fabs(value) - sqrt((*L[branch_1])*(*L[branch_2])))>1e-300){
   	err(0, "mutual_inductance: установленная взаимоиндуктивность больше корня произведения индуктивностей. Физически это не возможно");
      }
   PType *p = &_Lm.push_back(value);
   //Lm[ branch_1 ][ branch_2 ] = _Lm.end()-1;
   //Lm[ branch_2 ][ branch_1 ] = _Lm.end()-1;
   Lm[ branch_1 ][ branch_2 ] = p;
   Lm[ branch_2 ][ branch_1 ] = p;
}
//---------------------------------------------------------------------------
void PElement::magnetic_link(unsigned Electric_Branch, unsigned Magnetic_Branch, unsigned Windings){
   // создаем новый дроссель
   D[ Electric_Branch ] = new Throttle;
   D[ Electric_Branch ]->EBranch = Electric_Branch;
   D[ Electric_Branch ]->MBranch = Magnetic_Branch;
   D[ Electric_Branch ]->Windings = Windings;
   D[ Electric_Branch ]->Obj = this;
}
//---------------------------------------------------------------------------
PPin PElement::set_pin(unsigned Node){
	PPin pin;
   pin.obj = this;
   pin.node = Node;
   //pin.branch = -1;
   return pin;
}
//---------------------------------------------------------------------------
/* Пересчет дифференциального сопротивления для метода примоугольников при нахождении интеграла /
void PElement::update_differential_resistance_rect(){
   for(unsigned i=0; i<v; i++)
   	if(*C[i])
      	*Z[i] = *R[i] + *L[i]/smp + smp/(*C[i]);
      else
      	*Z[i] = *R[i] + *L[i]/smp;
} */
//---------------------------------------------------------------------------
/* Пересчет дифференциального сопротивления для метода трапеций*/
void PElement::update_differential_resistance(){
   for(unsigned i=0; i<v; i++)
   	if(*C[i])
      	*Z[i] = *R[i] + *L[i]/smp + smp/(*C[i])/2;
      else
      	*Z[i] = *R[i] + *L[i]/smp;
}
//---------------------------------------------------------------------------
/* Рассчет параметров схемы по измененным начальным данным */
bool PElement::accept(){
   update_differential_resistance();
   return true;
};
//---------------------------------------------------------------------------
void PElement::Save(ostream &stream){		// сохранение параметров объекта в поток
   // имя
   stream << StringForSave( &Name ) << " ";
	// сеть, в которую входим
   String grName = GridName(); if( grName == "" ) grName = "N";
   stream << StringForSave( &grName ) << " ";
}
//---------------------------------------------------------------------------
void PElement::Read(istream &stream){    // загрузка параметров объекта из потока
   char name[1024];
   // имя
	stream >> name;    Name = name;
   // сеть, в которую входим
   stream >> name;    String grName = name;
   if(grName == "N")		Grid = NULL;
   else						Grid = ObjectList.Find( grName );
}
//---------------------------------------------------------------------------
String PElement::ClassName(){ return "PElement";}
//---------------------------------------------------------------------------
// полное имя сети, в которую входит объект
String PElement::GridName(){
	if( Grid == NULL ) return "";
   else					 return Grid->FullName();
}
//---------------------------------------------------------------------------
// имя объекта с учетом всех подсетей, в которые он входит
String PElement::FullName(){
	if( Grid == NULL ) return Name;
   else					 return Grid->FullName() + "." + Name;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------








unsigned __fastcall PPin::Node(){ return node;}
//unsigned __fastcall PPin::GetBranch(){ return branch;}
PElement* __fastcall PPin::Obj(){ return obj; }
PType __fastcall PPin::f(){ return *obj->f[node]; }
PType __fastcall PPin::I(){
	// вычислим ток всех ветвей, подходящих к узлу
   PType I=0;
   unsigned n=obj->Branches();
   for(unsigned j=0; j<n;j++){
   	I += (*obj->M)[node][j] * (*obj->I[j]);
      }
   return I;
}
//---------------------------------------------------------------------------
void PPin::Save(ostream &stream){			// сохранить информацию о выводе
   stream << StringForSave(&obj->FullName()) << " ";
   stream << node << " ";
}
//---------------------------------------------------------------------------
void PPin::Read(istream &stream){			// прочитать информацию о выводе
   char n[1024];
   stream >> n;
   obj = ObjectList.Find( (String)n );
   if( !obj ){
      err(0, "PPin: чтение из потока. Имя объекта ("+ (String)n +") не найдено в списке объектов");
      }
   stream >> node;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
/* Группа выводов */
Pins::Pins(unsigned Number):
   Count(Number){
   _Pin = new PPin[Count];
}
//---------------------------------------------------------------------------
Pins::Pins(Pins& obj):
	Count(obj.Count){
   _Pin = new PPin[Count];
   Name = obj.Name;
   for(unsigned i=0; i<Count; i++)
   	_Pin[i] = obj[i];
}
//---------------------------------------------------------------------------
PPin& Pins::operator[](unsigned int i){
   if(i<Count)
      return _Pin[i];
   else{
      err(0,"Pins: Доступ к несуществующему элементу. Вернули 0 элемент");
      return _Pin[0];
      }
}
//---------------------------------------------------------------------------
Pins::~Pins(){
   delete [] _Pin;
}
//---------------------------------------------------------------------------
Pins Pins::operator=(Pins &op2){
	if(Count != op2.Count){
      err(0, "Pins::operator=: ошибка при присваивании. Число выводов в Выходных модулях не равны");
   	return *this;
   	}
   Name = op2.Name;
	for(unsigned i=0; i<Count; i++)
   	_Pin[i] = op2[i];
   return *this;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------






void Pins_Link::add(PPin &First, PPin &Second){
   Pair_Pin pair;
   pair.First = &First;
   pair.Second = &Second;
	Link.push_back(pair);
}
//---------------------------------------------------------------------------
void Pins_Link::add(Pins& First, Pins& Second){
   Pair_Pin pair;
   if(First.Count != Second.Count) {err(0, "Pins_Link::add: число выводов в группах выводов не одинаково. Не все выводы одного устройста соединены со вторым");}
   for(unsigned i=0, n=First.Count; i<n; i++){
   	pair.First = &First[i];
   	pair.Second = &Second[i];
		Link.push_back(pair);
      }
}
//---------------------------------------------------------------------------
// добавить связи из объекта obj в этот объект
void Pins_Link::add(Pins_Link obj){
   for(unsigned i=0, n=obj.Link.size(); i<n; i++)
      Link.push_back(obj.Link[i]);
}
//---------------------------------------------------------------------------
unsigned __fastcall Pins_Link::Number(){ return Link.size(); }
//---------------------------------------------------------------------------
Pair_Pin& Pins_Link::operator[](const unsigned int i){ return Link[i]; }
//---------------------------------------------------------------------------
void Pins_Link::clear(){ Link.clear(); };
/*/---------------------------------------------------------------------------
Pins_Link Pins_Link::operator=(const Pins_Link &Obj){
   Link.clear();
   Link = Obj.Link;
   return *this;
} */
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
void Pins_List::add(Pins* X){ _Pins.push_back(X); }   /* Сохранили группу выводов */
Pins& Pins_List::operator[](unsigned int i){          // Вернули группу выводов
	if(i<_Pins.size()) return *_Pins[i];
   else{
      err(0, "Pins_List::operator[]: обратились к несуществующей группе выводов.");
   	return *_Pins[0];
   	}
}
unsigned Pins_List::Count(){ return _Pins.size();} // число групп выводов
//---------------------------------------------------------------------------
void Pins_List::Clear(){_Pins.clear();}
//---------------------------------------------------------------------------
void Pins_List::Delete(Pins* pins){
	for(int i=0, n=_Pins.size(); i<n; i++)
   	if( _Pins[i] == pins ){
      	_Pins.erase(_Pins.begin() + i);
         break;
         }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------








PUnit::PUnit(unsigned NumBranches, unsigned NumNodes, unsigned NumCircuit):PElement(NumBranches, NumNodes, NumCircuit){
   _E = new PType[v];
   _U_C= new PType[v];
   _U_L= new PType[v];
   _U_Lm= new PType[v];
   _U_D= new PType[v];
   _R = new PType[v];
   _L = new PType[v];
   _C = new PType[v];
   _Z = new PType[v];
   _I = new PType[v];
   _J = new PType[u];
   _f = new PType[u];
   for(unsigned i=0;i<v;i++){
      _E[i]=0;
      _U_C[i]=0;
      _U_L[i]=0;
      _U_Lm[i]=0;
      _U_D[i]=0;
      _R[i]=MIN;     // чтобы не было короткозамкнутых ветвей, при которых весь объект превращается в точку
      _L[i]=0;
      _C[i]=0;
      _Z[i]=0;
      _I[i]=0;
      }
   for(unsigned i=0;i<u;i++){
      _J[i]=0;
      _f[i]=0;
      }
   // указатели указывают на эти значения
   _Lm.push_back(0); const PType *first = _Lm.begin();
   for(unsigned i=0;i<v;i++){
      E[i]=&_E[i];
      U_C[i]=&_U_C[i];
      U_L[i]=&_U_L[i];
      U_Lm[i]=&_U_Lm[i];
      U_D[i]=&_U_D[i];
      R[i]=&_R[i];
      L[i]=&_L[i];
      C[i]=&_C[i];
      Z[i]=&_Z[i];
      I[i]=&_I[i];
      for(unsigned j=0;j<v;j++) Lm[i][j] = (PType*)first;
      }
   for(unsigned i=0;i<u;i++){
      J[i]=&_J[i];
      f[i]=&_f[i];
      }
   // опишем поля класса
   Describe_Fields();
}
//---------------------------------------------------------------------------
void PUnit::Describe_Fields(){
}
//---------------------------------------------------------------------------
PUnit::PUnit(PUnit *obj):PElement(obj){
   _E = new PType[v];
   _U_C= new PType[v];
   _U_L= new PType[v];
   _U_Lm= new PType[v];
   _U_D= new PType[v];
   _R = new PType[v];
   _L = new PType[v];
   _C = new PType[v];
   _Z = new PType[v];
   _I = new PType[v];
   _J = new PType[u];
   _f = new PType[u];
   for(unsigned i=0;i<v;i++){
      _E[i]=obj->_E[i];
      _U_C[i]=obj->_U_C[i];
      _U_L[i]=obj->_U_L[i];
      _U_Lm[i]=obj-> _U_Lm[i];
      _U_D[i]=obj-> _U_D[i];
      _R[i]=obj->_R[i];
      _L[i]=obj->_L[i];
      _C[i]=obj->_C[i];
      _Z[i]=obj->_Z[i];
      _I[i]=obj->_I[i];
      }
   for(unsigned i=0;i<u;i++){
      _J[i]=obj->_J[i];
      _f[i]=obj->_I[i];
      }
   // указатели указывают на эти значения
   _Lm.push_back(0);
   for(unsigned i=0;i<v;i++){
      E[i]=&_E[i];
      U_C[i]=&_U_C[i];
      U_L[i]=&_U_L[i];
      U_Lm[i]=&_U_Lm[i];
      U_D[i]=&_U_D[i];
      R[i]=&_R[i];
      L[i]=&_L[i];
      C[i]=&_C[i];
      Z[i]=&_Z[i];
      I[i]=&_I[i];
      //for(unsigned j=0;j<v;j++) Lm[i][j] = _Lm.begin() + distance(obj->_Lm.begin(), obj->Lm[i][j]);
      for(unsigned j=0;j<v;j++) Lm[i][j] = &_Lm[  obj->_Lm.index( obj->Lm[i][j] )  ];
      }
   for(unsigned i=0;i<u;i++){
      J[i]=&_J[i];
      f[i]=&_f[i];
      }
   Fields = obj->Fields;   // теже свойства полей
}
//---------------------------------------------------------------------------
PUnit::~PUnit(){
   delete [] _R;
   delete [] _L;
   delete [] _C;
   delete [] _E;
   delete [] _J;
   delete [] _U_C;
   delete [] _U_L;
   delete [] _U_Lm;
   delete [] _U_D;
   delete [] _Z;
   delete [] _I;
   delete [] _f;
}
//---------------------------------------------------------------------------
String PUnit::ClassName(){return "PUnit";}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
















PLine::PLine(unsigned Num_Of_Cells):
   PUnit(15*Num_Of_Cells+12, 1+3*Num_Of_Cells+3, 12*Num_Of_Cells+9),
   X1(4),               // выводы первой группы
   X2(4){               // выводы второй группы
   z=Num_Of_Cells;	   // число звеньев
   set_pins();          // обозначение выводов от устройства
   Describe_Fields();   // Сохраняем информацию о свойствах класса
	/*/ заполняем матрицы
   u= 1 + 3*z + 3;
   v=15*z + 12;
   k=4*z + 2;	// число узлов, ветвей, незав. контуров*/
   char _M[4][15] = {{ 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0},
   	               {-1, 0, 1,-1, 0, 1,-1, 0, 0,-1, 0, 0,-1, 0, 0},
                     { 1,-1, 0, 1,-1, 0, 0,-1, 0, 0,-1, 0, 0,-1, 0},
                     { 0, 1,-1, 0, 1,-1, 0, 0,-1, 0, 0,-1, 0, 0,-1}};
   for(unsigned k=0;k<z;k++){
      for(unsigned j=0;j<15;j++)
      		(*M)[0][15*k+j] = _M[0][j];
   	for(unsigned i=1;i<4;i++)
      	for(unsigned j=0;j<15;j++)
      		(*M)[3*k+i][15*k+j] = _M[i][j];
      (*M)[3*k+4][15*k+12]=1;
      (*M)[3*k+5][15*k+13]=1;
      (*M)[3*k+6][15*k+14]=1;
      }
   for(unsigned i=1;i<4;i++)
     	for(unsigned j=0;j<12;j++)
     		(*M)[u-4+i][v-12+j] = _M[i][j];
   (*M)[0][v-6]=1;
   (*M)[0][v-5]=1;
   (*M)[0][v-4]=1;
   (*M)[0][v-3]=1;
   (*M)[0][v-2]=1;
   (*M)[0][v-1]=1;
   char _N[12][15] = {{ 1, 0, 0, 0, 0, 0,-1, 1, 0, 0, 0, 0, 0, 0, 0},
   						 { 0, 1, 0, 0, 0, 0, 0,-1, 1, 0, 0, 0, 0, 0, 0},
                      { 0, 0,-1, 0, 0, 0,-1, 0, 1, 0, 0, 0, 0, 0, 0},
                      { 1, 0, 0,-1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                      { 0, 1, 0, 0,-1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                      { 0, 0, 1, 0, 0,-1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                      { 0, 0, 0, 0, 0, 0, 1, 0, 0,-1, 0, 0, 0, 0, 0},
                      { 0, 0, 0, 0, 0, 0, 0, 1, 0, 0,-1, 0, 0, 0, 0},
                      { 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0,-1, 0, 0, 0},
                      { 0, 0, 0, 0, 0, 0,-1, 0, 0, 0, 0, 0, 1, 0, 0},
                      { 0, 0, 0, 0, 0, 0, 0,-1, 0, 0, 0, 0, 0, 1, 0},
                      { 0, 0, 0, 0, 0, 0, 0, 0,-1, 0, 0, 0, 0, 0, 1}};
   for(unsigned n=0;n<z;n++){
   	for(unsigned i=0;i<12;i++)
      	for(unsigned j=0;j<15;j++)
      		(*N)[12*n+i][15*n+j] = _N[i][j];
      (*N)[12*n+9][15*(n+1)+6]=1;
      (*N)[12*n+10][15*(n+1)+7]=1;
      (*N)[12*n+11][15*(n+1)+8]=1;
      }
   for(unsigned i=0;i<9;i++)
     	for(unsigned j=0;j<12;j++)
     		(*N)[k-9+i][v-12+j] = _N[i][j];
   Name = "ЛЭП";        // имя объекта
   Grounded_Node = 0;   // заземляемый узел
   // исходные  параметры линии
   for(int i=_N_;i<=_C_;i++){
      R0[i] = MIN;
      C0[i] = 0;
      L0[i] = 0 ;
      Cz0[i] = 0 ;
      Gz0[i] = 1/MAX;
      }
   for(int i=_AB_;i<=_CA_;i++){
      Cm0[i] = 0;
      Gm0[i] = 1/MAX;
      M0 [i] = 0 ;
      }
   /*/ большие междуфазные активные сопротивления
   for(unsigned i=0; i<(z+1); i++)
      *R[i*15]   = *R[i*15+1] = *R[i*15+2] =
      *R[i*15+3] = *R[i*15+4] = *R[i*15+5] =
      *R[i*15+6] = *R[i*15+7] = *R[i*15+8] =
      *R[i*15+9] = *R[i*15+10]= *R[i*15+11]= MAX;
   // минимальные продольные сопротивления G (необходимы для возможности записи 2 закона Кирхгофа)
   for(unsigned i=0; i<z; i++)
      *R[i*15+12] = *R[i*15+13] = *R[i*15+14] = MIN; */
   length = 1;  // длинна линии
}
//---------------------------------------------------------------------------
PLine::PLine(PLine &ob):
   PUnit(&ob),
   X1(4),
   X2(4){
   z=ob.z;	      // число звеньев
   set_pins();    // обозначение выводов от устройства
}
//---------------------------------------------------------------------------
PLine::~PLine(){ ;}
//---------------------------------------------------------------------------
/* Установка человеко-понятных выводов от устройства */
void PLine::set_pins(){
   Out.add(&X1);  // указатели на группы элементов   Pins_Group[0] = &X1
   Out.add(&X2);
   X1.Name = "X1";
   X2.Name = "X2";
	X1[ _N_ ] = set_pin(0);
   X1[ _A_ ] = set_pin(1);
   X1[ _B_ ] = set_pin(2);
   X1[ _C_ ] = set_pin(3);
   X2[ _N_ ] = set_pin(0);
   X2[ _A_ ] = set_pin(u-3); //, 12+15*(z-1) );
   X2[ _B_ ] = set_pin(u-2); //, 13+15*(z-1) );
   X2[ _C_ ] = set_pin(u-1); //, 14+15*(z-1) );
}
//---------------------------------------------------------------------------
void PLine::Describe_Fields(){
   PFieldInfo fi;
   String name;
   // длинна
   fi.Name = "Длинна";
   fi.Type = "PType";
   fi.ptr = (void*) &length;
   Fields.push_back(fi);
   // число звеньев
   fi.Name = "Звеньев";
   fi.Type = "int";
   fi.ptr = (void*) &z;
   fi.Mode = ro;        // только чтение
   Fields.push_back(fi);
   // возврящаем режим записи для последующих полей
   fi.Mode = rw;
   // активные сопротивления
   for(int i=0; i<3; i++){
      fi.Name = "R0[_" + (String)((char)('A'+i)) + "_]";
      fi.Type = "PType";
      fi.ptr = (void*) &R0[_A_+i];
      Fields.push_back(fi);
      }
   // индуктивности
   for(int i=0; i<3; i++){
      fi.Name = "L0[_" + (String)((char)('A'+i)) + "_]";
      fi.Type = "PType";
      fi.ptr = (void*) &L0[_A_+i];
      Fields.push_back(fi);
      }
   /*/ емкости в линии
   for(int i=0; i<3; i++){
      fi.Name = "C0[_" + (String)((char)('A'+i)) + "_]";
      fi.Type = "PType";
      fi.ptr = (void*) &C0[_A_+i];
      Fields.push_back(fi);
      } */
   // взаимная индуктивность
   for(int i=0; i<3; i++){
      switch(i){
         case _AB_: name = "AB"; break;
         case _BC_: name = "ВС"; break;
         case _CA_: name = "СА"; break;
         default:   name = "";
      }
      fi.Name = "M_[_" + name + "_]";
      fi.Type = "PType";
      fi.ptr = (void*) &M0[_AB_+i];
      Fields.push_back(fi);
      }
   // активная проводимость на землю
   for(int i=0; i<3; i++){
      fi.Name = "Gz0[_" + (String)((char)('A'+i)) + "_]";
      fi.Type = "PType";
      fi.ptr = (void*) &Gz0[_A_+i];
      Fields.push_back(fi);
      }
   // емкостная проводимость на землю
   for(int i=0; i<3; i++){
      fi.Name = "Cz0[_" + (String)((char)('A'+i)) + "_]";
      fi.Type = "PType";
      fi.ptr = (void*) &Cz0[_A_+i];
      Fields.push_back(fi);
      }
   // активная междуфазная проводимость
   for(int i=0; i<3; i++){
      switch(i){
         case _AB_: name = "AB"; break;
         case _BC_: name = "ВС"; break;
         case _CA_: name = "СА"; break;
         default:   name = "";
      }
      fi.Name = "Gm0[_"+ name +"_]";
      fi.Type = "PType";
      fi.ptr = (void*) &Gm0[_AB_+i];
      Fields.push_back(fi);
      }
   // емкостная междуфазная проводимость
   for(int i=0; i<3; i++){
      switch(i){
         case _AB_: name = "AB"; break;
         case _BC_: name = "ВС"; break;
         case _CA_: name = "СА"; break;
         default:   name = "";
      }
      fi.Name = "Cm0[_"+ name +"_]";
      fi.Type = "PType";
      fi.ptr = (void*) &Cm0[_AB_+i];
      Fields.push_back(fi);
      }   
}
//---------------------------------------------------------------------------
PLine PLine::operator=(PLine &ob){
   if(z != ob.z ||
      u != ob.u ||
      v != ob.v ||
      k != ob.k) {err(0,"Присваивание объекта линия: число звеньев разное (TPL::operator=).");return *this; }
   length = ob.length;  // длинна линии
   *M = *ob.M;
   *N = *ob.N;
   // значения параметров
   for(unsigned i=0;i<v;i++){
      _E[i]=*ob.E[i];
      _U_C[i]=*ob.U_C[i];
      _U_L[i]=*ob.U_L[i];
      _U_Lm[i]=*ob.U_Lm[i];
      _R[i]=*ob.R[i];
      _L[i]=*ob.L[i];
      _C[i]=*ob.C[i];
      _Z[i]=*ob.Z[i];
      _I[i]=*ob.I[i];
      }
   _Lm = ob._Lm;
   for(unsigned i=0;i<v;i++)
      for(unsigned j=0;j<v;j++)
         //Lm[i][j] = _Lm.begin() + distance(ob._Lm.begin(), ob.Lm[i][j]);
           Lm[i][j] = &_Lm[  ob._Lm.index( ob.Lm[i][j] )  ];
   for(unsigned i=0;i<u;i++){
      _J[i]=*ob.J[i];
      _f[i]=*ob.f[i];
      }
   return *this;
}
//---------------------------------------------------------------------------
/* Параметры линии 
void PLine::setR_(PType value){  // установка активных сопротивлений на фазу
   PType R_zvena = value / z;
   for(unsigned i=0;i<z;i++){
      *R[i*15+12] = R_zvena;
      *R[i*15+13] = R_zvena;
      *R[i*15+14] = R_zvena;
      }
}
void PLine::setL_(PType value){  // установка индуктивности фазы
   PType L_zvena = value / z;
   for(unsigned i=0;i<z;i++){
      *L[i*15+12] = L_zvena;
      *L[i*15+13] = L_zvena;
      *L[i*15+14] = L_zvena;
      }
}
void PLine::setC_(PType value){  // установка умкости фазы
   PType C_zvena = value * z;   // обрати внимание! умножить
   for(unsigned i=0;i<z;i++){
      *C[i*15+12] = C_zvena;
      *C[i*15+13] = C_zvena;
      *C[i*15+14] = C_zvena;
      }
}
void PLine::setGz_(PType value){ // установка активных проводимостей на землю
   PType Rz_zvena = (z+1) / value;   // число проводимостей на 1 больше, чем число звеньев
   for(unsigned i=0, cnt=z+1; i<cnt; i++){
      *R[i*15+6] = Rz_zvena;
      *R[i*15+7] = Rz_zvena;
      *R[i*15+8] = Rz_zvena;
      }
}
void PLine::setCz_(PType value){ // установка емкостей на землю
   PType Cz_zvena = value / (z+1);   // да-да! именно делить
   for(unsigned i=0, cnt=z+1; i<cnt; i++){
      *C[i*15+9 ] = Cz_zvena;
      *C[i*15+10] = Cz_zvena;
      *C[i*15+11] = Cz_zvena;
      }
   // обнуляем активное сопротивление в этих ветвях
   if(value!=0)
      for(unsigned i=0, cnt=z+1; i<cnt; i++){
         *R[i*15+9 ] = 0;
         *R[i*15+10] = 0;
         *R[i*15+11] = 0;
         }
   else
      for(unsigned i=0, cnt=z+1; i<cnt; i++){
         *R[i*15+9 ] = MAX;
         *R[i*15+10] = MAX;
         *R[i*15+11] = MAX;
         }

}
void PLine::setGm_(PType value){ // установка активных междуфазных проводимостей
   PType Rm_zvena = (z+1) / value;   // число проводимостей на 1 больше, чем число звеньев
   for(unsigned i=0, cnt=z+1; i<cnt; i++){
      *R[i*15+0] = Rm_zvena;
      *R[i*15+1] = Rm_zvena;
      *R[i*15+2] = Rm_zvena;
      }
}
void PLine::setCm_(PType value){ // установка междуфазных емкостей
   PType Cm_zvena = value / (z+1);   // да-да! именно делить
   for(unsigned i=0, cnt=z+1; i<cnt; i++){
      *C[i*15+3] = Cm_zvena;
      *C[i*15+4] = Cm_zvena;
      *C[i*15+5] = Cm_zvena;
      }
   // обнуляем активное сопротивление в этих ветвях
   if(value!=0)
      for(unsigned i=0, cnt=z+1; i<cnt; i++){
         *R[i*15+3] = 0;
         *R[i*15+4] = 0;
         *R[i*15+5] = 0;
         }
   else
      for(unsigned i=0, cnt=z+1; i<cnt; i++){
         *R[i*15+3] = MAX;
         *R[i*15+4] = MAX;
         *R[i*15+5] = MAX;
         }
}
void PLine::setLm_(PType value){ // установка междуфазных взаимоиндуктивностей
   PType Lm_zvena = value / z;
   for(unsigned i=0;i<z;i++){
   	mutual_inductance(i*15+12, i*15+13, Lm_zvena);
      mutual_inductance(i*15+12, i*15+14, Lm_zvena);
      mutual_inductance(i*15+13, i*15+14, Lm_zvena);
   	}
}
//---------------------------------------------------------------------------
// Удельные параметры
void PLine::setR0(PType value){  // установка активных сопротивлений на фазу
   PType R_zvena = value * length / z;
   for(unsigned i=0;i<z;i++){
      *R[i*15+12] = R_zvena;
      *R[i*15+13] = R_zvena;
      *R[i*15+14] = R_zvena;
      }
}
void PLine::setL0(PType value){  // установка индуктивности фазы
   PType L_zvena = value * length / z;
   for(unsigned i=0;i<z;i++){
      *L[i*15+12] = L_zvena;
      *L[i*15+13] = L_zvena;
      *L[i*15+14] = L_zvena;
      }
}
void PLine::setC0(PType value){  // установка умкости фазы
   PType C_zvena = (value / length) * z;   // обрати внимание! умножить
   for(unsigned i=0;i<z;i++){
      *C[i*15+12] = C_zvena;
      *C[i*15+13] = C_zvena;
      *C[i*15+14] = C_zvena;
      }
}
void PLine::setGz0(PType value){ // установка активных проводимостей на землю
   PType Rz_zvena = (z+1) / (value * length);   // число проводимостей на 1 больше, чем число звеньев
   for(unsigned i=0, cnt=z+1; i<cnt; i++){
      *R[i*15+6] = Rz_zvena;
      *R[i*15+7] = Rz_zvena;
      *R[i*15+8] = Rz_zvena;
      }
}
void PLine::setCz0(PType value){ // установка емкостей на землю
   PType Cz_zvena = (value * length ) / (z+1);   // да-да! именно делить
   for(unsigned i=0, cnt=z+1; i<cnt; i++){
      *C[i*15+9 ] = Cz_zvena;
      *C[i*15+10] = Cz_zvena;
      *C[i*15+11] = Cz_zvena;
      }
   // обнуляем активное сопротивление в этих ветвях
   if(value!=0)
      for(unsigned i=0, cnt=z+1; i<cnt; i++){
         *R[i*15+9 ] = 0;
         *R[i*15+10] = 0;
         *R[i*15+11] = 0;
         }
   else
      for(unsigned i=0, cnt=z+1; i<cnt; i++){
         *R[i*15+9 ] = MAX;
         *R[i*15+10] = MAX;
         *R[i*15+11] = MAX;
         }
}
void PLine::setGm0(PType value){ // установка активных междуфазных проводимостей
   PType Rm_zvena = (z+1) / (value * length);   // число проводимостей на 1 больше, чем число звеньев
   for(unsigned i=0, cnt=z+1; i<cnt; i++){
      *R[i*15+0] = Rm_zvena;
      *R[i*15+1] = Rm_zvena;
      *R[i*15+2] = Rm_zvena;
      }
}
void PLine::setCm0(PType value){ // установка междуфазных емкостей
   PType Cm_zvena = (value * length) / (z+1);   // да-да! именно делить
   for(unsigned i=0, cnt=z+1; i<cnt; i++){
      *C[i*15+3] = Cm_zvena;
      *C[i*15+4] = Cm_zvena;
      *C[i*15+5] = Cm_zvena;
      }
   // обнуляем активное сопротивление в этих ветвях
   if(value!=0)
      for(unsigned i=0, cnt=z+1; i<cnt; i++){
         *R[i*15+3] = 0;
         *R[i*15+4] = 0;
         *R[i*15+5] = 0;
         }
   else
      for(unsigned i=0, cnt=z+1; i<cnt; i++){
         *R[i*15+3] = MAX;
         *R[i*15+4] = MAX;
         *R[i*15+5] = MAX;
         }
}
void PLine::setLm0(PType value){ // установка междуфазных взаимоиндуктивностей
   PType Lm_zvena = (value * length) / z;
   for(unsigned i=0;i<z;i++){
   	mutual_inductance(i*15+12, i*15+13, Lm_zvena);
      mutual_inductance(i*15+12, i*15+14, Lm_zvena);
      mutual_inductance(i*15+13, i*15+14, Lm_zvena);
   	}
}*/
//---------------------------------------------------------------------------
/* определение параметров схемы замещения линии по данных холостого хода */
bool PLine::set_params(const PType _L0, const PType _R0, const PType _M0, const PType dP_f, const PType dQ_f, const PType U_n_f, const PType U_k_f, const PType dP, const PType dQ, const PType U_n, const PType U_k, const PType freq ){
	// поиск сопротивлений продольных ветвей
   PType Xz = 2*M_PI*freq*(_L0*length/z);
   //PType Lz = L0*length/z;
   PType Rz = 2*M_PI*freq*(_R0*length/z);
	// поиск проводимостей поперечных ветвей на землю
	PType a = (U_k_f-U_n_f)/(z*U_k_f);
   PType C1 = z*(z-1)*(3*a*a*pow(z,3) - z*z*(12*a*a+15*a) + z*(13*a*a+35*a+20) - 2*a*a - 10*a - 10 )/60;
	PType C2 = (z+1)*(2*a*a*z*z + z*(a*a-6*a) + 6 )/6;
   PType k = (dP_f-Rz*dQ_f/Xz)/(U_k_f*U_k_f*C2);
   PType A1 = Xz*U_k_f*U_k_f*C1*(1+pow(Rz/Xz,2));
   PType B1 = 2*k*Rz*U_k_f*U_k_f*C1 + U_k_f*U_k_f*C2;
   PType D1 = Xz*U_k_f*U_k_f*k*k*C1 - dQ_f;
   PType b2z = (sqrt(B1*B1-4*A1*D1) - B1)/(2*A1);
   PType C2z = b2z/(2*M_PI*freq);
   PType g2z = (k + Rz/Xz*b2z);
   // поиск междуфазных проводимостей
   PType Xmz = 2*M_PI*freq*((_L0-_M0)*length/z);
   //PType Mz = M0*length/z;
   a = (U_k-U_n)/(z*U_k);
   C1 = z*(z-1)*(3*a*a*pow(z,3) - z*z*(12*a*a+15*a) + z*(13*a*a+35*a+20) - 2*a*a - 10*a - 10 )/60;
	C2 = (z+1)*(2*a*a*z*z + z*(a*a-6*a) + 6 )/6;
   k = (dP-Rz*dQ/Xmz)/(3*U_k*U_k*C2);
	A1 = Xmz*U_k*U_k*C1*(1+pow(Rz/Xmz,2));
   B1 = 2*k*Rz*U_k*U_k*C1 + U_k*U_k*C2;
   D1 = Xmz*U_k*U_k*k*k*C1 - dQ/3;
   PType bz = (sqrt(B1*B1-4*A1*D1) - B1)/(2*A1);
   PType gz = k + Rz/Xmz*bz;
   PType b1z = (bz - b2z)/3;
   PType C1z = b1z/(2*M_PI*freq);
   PType g1z = (gz - g2z)/3;
   // установка значений
   for(int i=_A_; i<=_C_; i++){
      R0[i] = _R0;
      L0[i] = _L0;
      C0[i] = 0;
      M0[i] = _M0;
      Gz0[i] = g2z * (z+1) / length;
      Cz0[i] = C2z * (z+1) / length;
      }
   for(int i=_AB_; i<=_CA_; i++){
      Gm0[i] = g1z * (z+1) / length;
      Cm0[i] = C1z * (z+1) / length;
      }
   /*setR_ ( _R0 * length );
   setL_ ( _L0 * length );
   setC_ ( 0 );
   setLm_ ( _M0 * length );
   setGz_ ( g2z * (z+1) );
   setCz_ ( C2z * (z+1) );
   setGm_ ( g1z * (z+1) );
   setCm_ ( C1z * (z+1) ); */
   return true;
}
//---------------------------------------------------------------------------
/* определение параметров схемы замещения фазы для ЛЭП*/
bool PLine::Phase_Params(Phase phase, PType __R, PType __L, PType __C, PType Gz, PType Cz){
   PType R2;	   // сопротивления на землю
   if(Gz) R2 = (z+1)/Gz;
   else R2 = MAX;
   PType C2 = Cz/(z+1);
   PType L_zvena  = __L/z;
   PType R_zvena  = __R/z;
   PType C_zvena  = __C/z;
   // определяем фазу
   unsigned p;
   switch (phase){
   	case _A_: p=0; break;
      case _B_: p=1; break;
      case _C_: p=2; break;
      default:	 return false;
      }
   // установка значений
   for(unsigned i=0;i<(z+1);i++){
      // ветка активной проводимости
      *R[i*15+6+p] = R2;
      *L[i*15+6+p] = 0;
      *C[i*15+6+p] = 0;
      // ветка емкостной проводимости
      if(C2){
         *C[i*15+9+p] = C2;
         *R[i*15+9+p] = 0;
         *L[i*15+9+p] = 0;
         }
      else
         *R[i*15+9+p] = MAX;
      }
   for(unsigned i=0;i<z;i++){
      *R[i*15+12+p] = R_zvena;
      *L[i*15+12+p]	= L_zvena;
      *C[i*15+12+p]	= C_zvena;
      }
   return true;
}
//---------------------------------------------------------------------------
/* определение параметров схемы замещения фазы для ЛЭП*/
bool PLine::Phase_Params_0(Phase phase, PType R0, PType L0, PType Gz0, PType Cz0){
   return  Phase_Params(phase, R0*length, L0*length, 0, Gz0*length, Cz0*length);
}
//---------------------------------------------------------------------------
bool PLine::Mutual_Inductance(PType M_ab, PType M_bc, PType M_ca){
   for(unsigned i=0;i<z;i++){
   	mutual_inductance(i*15+12, i*15+13, M_ab/z);
      mutual_inductance(i*15+13, i*15+14, M_bc/z);
      mutual_inductance(i*15+14, i*15+12, M_ca/z);
   	}
   return true;
}
//---------------------------------------------------------------------------
/* определение параметров схемы замещения линии - взаимоиндуктивности */
bool PLine::Mutual_Inductance_0(PType M0_ab, PType M0_bc, PType M0_ca){
   return Mutual_Inductance(M0_ab*length, M0_bc*length, M0_ca*length);
}
//---------------------------------------------------------------------------
/* определение параметров схемы замещения для ЛЭП - междуфазные проводимости */
bool PLine::Mutual_Conductance(unsigned Phase1, unsigned Phase2, PType Gm, PType Cm){
   PType R1 = (z+1)/Gm;	// междуфазные сопротивления
   PType C1 = Cm/(z+1);
   // определяем фазу
   unsigned p;
   switch (Phase1 + Phase2){
   	case (_A_ + _B_): p=0; break;
      case (_B_ + _C_): p=1; break;
      case (_C_ + _A_): p=2; break;
      default:	 return false;
      }
   // установка значений
   for(unsigned i=0;i<(z+1);i++){
      // активная проводимость
      *R[i*15+p] = R1;
      *L[i*15+p] = 0;
      *C[i*15+p] = 0;
      // емкостная проводимость
      if(C1){
         *C[i*15+3+p] = C1;
         *R[i*15+3+p] = 0;
         *L[i*15+3+p] = 0;
         }
      else
         *R[i*15+3+p] = MAX;
      }
   return true;
}
//---------------------------------------------------------------------------
/* определение параметров схемы замещения для ЛЭП - междуфазные проводимости */
bool PLine::Mutual_Conductance_0(Phase Phase1, Phase Phase2, PType Gm0, PType Cm0){
   return Mutual_Conductance(Phase1, Phase2, Gm0*length, Cm0*length);
}
//---------------------------------------------------------------------------
bool PLine::accept(){
   bool ret = true;
   // фазные параметры
   for(int i=_A_; i<=_C_; i++)
      ret = ret & Phase_Params_0((Phase) i, R0[i], L0[i], Gz0[i], Cz0[i]);
   // междуфазные индуктивности
   ret = ret & Mutual_Inductance_0(M0[_AB_], M0[_BC_], M0[_CA_]);
   // междуфазные проводимости
   ret = ret & Mutual_Conductance_0(_A_, _B_, Gm0[_AB_], Cm0[_AB_]);
   ret = ret & Mutual_Conductance_0(_B_, _C_, Gm0[_BC_], Cm0[_BC_]);
   ret = ret & Mutual_Conductance_0(_C_, _A_, Gm0[_CA_], Cm0[_CA_]);
   update_differential_resistance();
   return ret;
}
//---------------------------------------------------------------------------
void PLine::Save(ostream &stream){		// сохранение параметров объекта в поток
   // сначала сохранится базовый класс
   sire::Save(stream);
   // теперь сохраняем этот класс
	stream << length << " ";
   for(int i=0; i<4; i++){
      stream << R0[i] << " ";
      stream << L0[i] << " ";
      stream << C0[i] << " ";
      stream << Gz0[i] << " ";
      stream << Cz0[i] << " ";
   	}
   for(int i=0; i<3; i++){
      stream << M0[i] << " ";
      stream << Gm0[i] << " ";
      stream << Cm0[i] << " ";
   	}
}
//---------------------------------------------------------------------------
void PLine::Read(istream &stream){    // загрузка параметров объекта из потока
   // сначала читает данные базовый класс
   sire::Read(stream);
   // теперь читает этот класс
	stream >> length;
   for(int i=0; i<4; i++){
      stream >> R0[i];
      stream >> L0[i];
      stream >> C0[i];
      stream >> Gz0[i];
      stream >> Cz0[i];
   	}
   for(int i=0; i<3; i++){
      stream >> M0[i];
      stream >> Gm0[i];
      stream >> Cm0[i];
   	}
}
//---------------------------------------------------------------------------
String PLine::ClassName(){ return "PLine";}
//---------------------------------------------------------------------------
unsigned PLine::Cells(){return z;};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------












//---------------------------------------------------------------------------
PGrid::PGrid(){
	// устанавливаем нулевые указатели
   A=NULL;
   B=NULL;
   F=NULL;
   _J=NULL;
   Branch=NULL;
   Node=NULL;
   Element=NULL;
   Links = NULL;
   U=NULL;
   _U_Lm2=NULL;
   U_Lm2=NULL;
   _I2=NULL;
   // задаем имя
   Name = "Сеть";
   Describe_Fields();	// задать описание полей
   BuildOK = false;      // сеть нельзя использовать для решения
}
//---------------------------------------------------------------------------
/* Создаем сеть из одного объекта */
PGrid::PGrid(PUnit *obj){
   PElement **Block = new  PElement*[1];
   Block[0] = obj;
   if(obj->Internal_Links.Number() == 0)   // между узлами нет соединенй
      Link_Elements(1, Block, 0, NULL);
   else{     // между узлами есть связи
      Pins_Link Link;
      Link.add(obj->Internal_Links);
	   unsigned *link = new unsigned [Link.Number() * 4];
      for(unsigned i=0,m=Link.Number();i<m;i++) {
         // первый узел
         link[4*i] = 0;
         link[4*i + 1] = Link[i].First->Node();
         // второй объект
         link[4*i + 2] = 0;
         link[4*i + 3] = Link[i].Second->Node();
         }
	   BuildOK = Link_Elements(1, Block, Link.Number(), (unsigned (*)[2][2])link);
      delete [] link;
      }
   delete [] Block;
   // задаем имя
   Name = "Сеть";
   Describe_Fields();	// задать описание полей
}
//---------------------------------------------------------------------------
/* Поузловое соединений элементов */
PGrid::PGrid(Pins_Link Link){
   if(Link.Number() <=0 ){
      err(0, "PGrid: не возможно собрать сеть, т.к. не указано ни одной связи между элементами");
   	return;
   	}
   /*/ в сети имена всех элементов должны быть уникальны
   // доделать позже
   for(unsigned i=0, n=Link.Number(); i<n; i++){
      String nameA = Block[i]->Name;
      for(unsigned j=i+1; j<num; j++)
         if( name == Block[j]->Name ){
            err(0, "PGrid: внутри сети имена всех объектов должны быть уникальны. Сеть не собрана");
            return false;
         	} */
   // подготовка данных для сборки сети
   vector<PElement*> Block;
   vector<int> link;
   for(unsigned i=0;i<Link.Number();i++) {
      // первый объект
      unsigned j, n=Block.size(); // уже добавили ?
      PElement *e = Link[i].First->Obj();
         for(j=0;j<n;j++)
      	   if(e == Block[j]) break;
         if(j==n){
            Block.push_back(Link[i].First->Obj());
            Link.add(((PUnit*)Link[i].First->Obj())->Internal_Links);   // учтем внутренние соединения между узлами одного объекта
            }
         link.push_back( j );
         link.push_back( Link[i].First->Node() );
      // второй объект
      n=Block.size(); // уже добавили ?
      e = Link[i].Second->Obj();
         for(j=0;j<n;j++)
      	   if(e == Block[j]) break;
         if(j==n) {
            Block.push_back(Link[i].Second->Obj());
            Link.add(((PUnit*)Link[i].Second->Obj())->Internal_Links);   // учтем внутренние соединения между узлами одного объекта
            }
         link.push_back( j );
         link.push_back( Link[i].Second->Node() );
      }
	BuildOK = Link_Elements(Block.size(), Block.begin(), Link.Number(), (unsigned (*)[2][2])link.begin());
   // задаем имя
   Name = "Сеть";
   Describe_Fields();	// задать описание полей
}
//---------------------------------------------------------------------------
void PGrid::Describe_Fields(){
   /*PFieldInfo fi;
   // сопротивление во включ. состоянии
   fi.Name = "Группы выводов";
   fi.Type = "Pins_List";
   fi.ptr = (void*)this;
   Fields.push_back(fi);*/
}
//---------------------------------------------------------------------------
/* num - число блоков для построения сети
   Block - массив из этих элементов
   Link  - массив, указывающий как связаны блоки {  { {№_блока_1, вершина_блока_1},{№_блока_2, вершина_блока_2} }, ...}
*/
bool PGrid::Link_Elements(unsigned num, PElement *Block[], unsigned numLink,unsigned Link[][2][2]){
	// устанавливаем нулевые указатели
   A=NULL;
   B=NULL;
   F=NULL;
   _J=NULL;
   Branch=NULL;
   Node=NULL;
   Element=NULL;
   Links = NULL;
   U=NULL;
   _U_Lm2=NULL;
   U_Lm2=NULL;
   _I2=NULL;
   if(num==0){err(0,"PGrid::Link_Elements(): При создании сети использовано 0 элементов. Сеть не создана."); return false;}
   // запоминаем переданные элементы
   numElement = num;
   Element = new PElement*[numElement];
   for(unsigned i=0;i<numElement;i++)
      Element[i] = Block[i];
   // запоминаем связи
   numLinks = numLink;
   Links = new unsigned [numLink * 4];
   for(unsigned i=0; i<numLink; i++){
      Links[0+4*i] = Link[i][0][0];
      Links[1+4*i] = Link[i][0][1];
      Links[2+4*i] = Link[i][1][0];
      Links[3+4*i] = Link[i][1][1];
   	}
   // проверим уникальны ли имена элементов объектов
   for(unsigned i=0; i<numElement; i++){
      String name = Element[i]->FullName();
      for(unsigned j=i+1; j<numElement; j++)
         if( name == Element[j]->FullName() ){
            err(0, "PSubGrid: внутри сети имена всех объектов должны быть уникальны, сеть не собрана");
            return false;
         	}
      }
   // создаем и заполняем первую матрицу связей ветвей в узлах
   u = 0;
   v = 0;
   k = 0;
   for(unsigned i=0; i<num;i++){
      PElement *p = Block[i];
      u += p->M->Rows;     // общее число узлов
      v += p->M->Cols;    // общее число ветвей
      k += p->N->Rows;    // общее число контуров
      }
   //numNode = u;
   //numBranch = v;
   if(u<numLink) {err(0, "PGrid::Link_Elements(): отрицательное значение вершин"); return false;};
   if(v==0) {err(0, "PGrid::Link_Elements(): число вервей равно 0"); return false;};
   //u = u - numLink;  // c учетом объединяемых узлов

   Branch = new unsigned*[numElement];
   for(unsigned i=0; i<num;i++){
      unsigned tmp_v = Block[i]->Branches();
      Branch[i] = new unsigned[tmp_v];
      for(unsigned j=0;j<tmp_v;j++)
         Branch[i][j] = -1;
      }
   Node = new unsigned*[numElement];
   for(unsigned i=0; i<num;i++){
      unsigned tmp_u = Block[i]->Nodes();
      Node[i] = new unsigned[tmp_u];
      for(unsigned j=0;j<tmp_u;j++)
         Node[i][j] = -1;
      }
   // сохраняем соответствие ветвей
   unsigned i_branch=0; // текущее число ветвей в результирующей матрице
   for(unsigned i=0; i<num;i++){
      PElement *p = Block[i];
      unsigned tmp_rows = p->M->Cols;
      for(unsigned j=0; j<tmp_rows; j++){
         Branch[i][j] = i_branch;
         i_branch++;
         }
      }
   // сохраняем соответствия узлов (с учетом связей)
   unsigned i_node=0;         // текущее число узлов в результирующей матрице
   Grounded_Node = i_node;    // объявляем этот узел как заземленный
   bool *link_use = new bool[numLink];
   for(unsigned i=0; i<numLink;i++) link_use[i]=false;
   // а) объединяем заземляемые узлы
   for(unsigned b=0; b<num;b++){
   	PElement *p = Block[b];
      Node[  b  ][  p->Grounded_Node  ] = Grounded_Node;
      // найдем связанные узлы
      for(unsigned i=0; i<numLink; i++)
   	   if( ! link_use[i])
            if( Link[i][0][0]==b &&   Link[i][0][1]==p->Grounded_Node )
			      linked_nodes(numLink, Link, link_use, Grounded_Node, Link[i][0][0], Link[i][0][1]);
            else if( Link[i][1][0]==b &&  Link[i][1][1]==p->Grounded_Node )
			      linked_nodes(numLink, Link, link_use, Grounded_Node, Link[i][1][0], Link[i][1][1]);
      }
   i_node++;
   // b) создаем один новый узел для нескольких объединяемых
   for(unsigned i=0; i<numLink; i++)
   	if( ! link_use[i] ){
      	// сам узел
         Node[  Link[i][0][0]  ][  Link[i][0][1]  ] = i_node;
         // c ним связанные
			linked_nodes(numLink, Link, link_use, i_node, Link[i][0][0], Link[i][0][1]);//, i_node);//tmp);
      	// узлов в результирующей матрице стало на 1 больше
      	i_node++;
      	}
   delete [] link_use;
   // c) создаем один новый узел на один НЕ связанный узел
   for(unsigned i=0; i<num;i++){
      unsigned tmp_rows = Block[i]->M->Rows;
      for(unsigned j=0; j<tmp_rows; j++){
      	// мы не записывали этот узел как объединенный?
         if( -1 == Node_From_LocalNode(i,j) ){
         	// узел еще не был учтен
            Node[i][j] = i_node;
            i_node++;
         	}
      	}
      }
   u = i_node; // всего столько узлов насчитали
   // заполняем матрицу M связей ветвей с узлами
   M = new Matrix<char>(u, v);
   unsigned *Short_Branch = new unsigned[v];  // коротко-замкнутые ветви, начало и конец которых в одном узле
   unsigned num_Short_Branch = 0; // число коротко-замкнутых ветвей
   for(unsigned b=0; b<num;b++){
   	PElement *p = Block[b];
      unsigned tmp_rows = p->M->Rows;
      unsigned tmp_cols = p->M->Cols;
      for(unsigned i=0; i<tmp_rows; i++)
      	for(unsigned j=0; j<tmp_cols; j++){
         unsigned br = Branch_From_LocalBranch(b,j);
         unsigned nd = Node_From_LocalNode(b,i);
         if(br>=v || nd>=u)
            {err(0,"PGrid::Link_Elements(): номер узла(ветви) больше числа узлов(вейтвей)");return false;}
         (*M)[nd][br] += (*p->M)[i][j];	// если ветка начинается там же где заканчивается, то получим 0 в итоге
         if(fabs((*M)[nd][br]) < 0.01 && fabs((*p->M)[i][j])>0.01){  // если так, то это коротко-замкнутая ветка
            Short_Branch[num_Short_Branch]=br;
            num_Short_Branch++;}
      	}
      }

   // создаем 2 матрицу инцеденций N (2 закон кирхгофа)
   k = v-u+1;
   N = new Matrix<char>(k,v);
   //if(k != N->Rows){err(0, "Создание сети: число контуров не соответствует неоходимому количеству для решения уравнений ( PGrid() )");}
   unsigned nd = 0;  // число уже записанных уравнений
   for(unsigned b=0; b<num;b++){
   	PElement *p = Block[b];
      unsigned tmp_rows = p->N->Rows;
      unsigned tmp_cols = p->N->Cols;
      for(unsigned i=0; i<tmp_rows; i++){
      	for(unsigned j=0; j<tmp_cols; j++){
            unsigned br = Branch_From_LocalBranch(b,j);
            (*N)[nd][br] = (*p->N)[i][j];
      	   }
         nd++;   // запишем следующее уравнение
         }
      }
   // 2 закон Кирхгофа из равенства потенциалов объединенных узлов
   if( nd < N->Rows ){
   	/* При объединении n узлов (2 объекта) можно составить n-1 независимое уравнение следующим образом:
      приравнивать потенциалы 2х объединяемых узлов относительно 2х других объединяемых узлов.
      Уравнения будем записывать для узлов, которые объединяются при соединении двух разных
      объектов.
      	При объединении 2х узлов одного объекта, на каждую объединяемую пару можно составить
      хакон кирхгофа.
      */
      // Ищем узлы образуемые при объединении 2x объектов
      bool *Used_Link = new bool[numLink]; for(unsigned i=0;i<numLink;i++) Used_Link[i] = false;
      bool Loop = true; // продолжаем цикл
      while(Loop){
      	unsigned i;
      	for(i=0; i<numLink;i++) if( !Used_Link[i] )break; // ищем не использованные связи
         if (i!=numLink){ // нашли
         	unsigned Obj1 = Link[i][0][0];	// первый объект
         	unsigned Obj2 = Link[i][1][0];   // второй объект
      		vector<unsigned>  Node_For_Obj1; // сохраняем использованные узлы 1 объекта
      		vector<unsigned>  Node_For_Obj2; // сохраняем использованные узлы 2 объекта
            Node_For_Obj1.push_back(Link[i][0][1]);
            Node_For_Obj2.push_back(Link[i][1][1]);
            Used_Link[i] = true;
            if( Obj1 == Obj2 ){ //объединяем узлы одного объекта
               Matrix<char> Way(1,v);
               Matrix<char> Way1 = Element[Obj1]->M->Way(Node_For_Obj1[0], Node_For_Obj2[0]);
               for(unsigned s=0, n=Way1.Cols; s<n; s++)
                  Way[0][Branch_From_LocalBranch(Obj1, s)] += Way1[0][s];    // Way = Way1
               /*/ если точек больше 2
               if( Element[Obj1]->Nodes > 2){
                  // выбираем третью точку
               	unsigned node_A = (Node_For_Obj1[0] + Node_For_Obj2[0])/2;
               	if( node_A == Node_For_Obj1[0] ) node_A = (node_A++)%Element[Obj1]->Nodes;
               	if( node_A == Node_For_Obj2[0] ) node_A = (node_A++)%Element[Obj1]->Nodes;
                  Matrix<char> Way1 = Element[Obj1]->M->Way(node_A, Node_For_Obj1[0]);
                  Matrix<char> Way2 = Element[Obj2]->M->Way(node_A, Node_For_Obj2[0]);
                  for(unsigned s=0, n=Way1.Cols; s<n; s++)
            			Way[0][Branch_From_LocalBranch(Obj1, s)] += Way1[0][s];    // Way = Way1
                  for(unsigned s=0, n=Way2.Cols; s<n; s++)
            			Way[0][Branch_From_LocalBranch(Obj2, s)] -= Way2[0][s];    // Way = Way1 - Way2
                  }
               else if( Element[Obj1]->Nodes == 2){ // узлов только 2
                  Matrix<char> Way1 = Element[Obj1]->M->Way(Node_For_Obj1[0], Node_For_Obj2[0]);
               	for(unsigned s=0, n=Way1.Cols; s<n; s++)
            			Way[0][ Branch_From_LocalBranch(Obj1, s) ] += Way1[0][s];    // Way = Way2
               	}*/
               // считаем число ветвей в контуре (должно быть более 0)
               unsigned branches=0; for(unsigned s=0;s<v;s++) if(Way[0][s]!=0) branches++;
               // сохраняем закон
   //PType t0[2000];
               if(branches > 0){    // >1
            		for(unsigned s=0;s<v;s++) {(*N)[nd][s] = Way[0][s];}//t0[s] = Way[0][s];}
         			nd++;	// запишем еще одно уравнение по 2 закону Кирхгофа
                  if(nd>=N->Rows) Loop = false; // уравнений достаточно
         			}
            	}
            else{  // объединяем 2 разных объекта
             // ищем другие объединяемые узлы при объединении этих объектов друг с другом
             for(unsigned j=0; j<numLink;j++)
            	if( !Used_Link[j] ){
               	if( Link[j][0][0] == Obj1 && Link[j][1][0] == Obj2){
            			Node_For_Obj1.push_back(Link[j][0][1]);
            			Node_For_Obj2.push_back(Link[j][1][1]);
                     Used_Link[j] = true;
                  	}
                  else if( Link[j][1][0] == Obj1 && Link[j][0][0] == Obj2){
            			Node_For_Obj1.push_back(Link[j][1][1]);
            			Node_For_Obj2.push_back(Link[j][0][1]);
                     Used_Link[j] = true;
                  	}
                  }
            // В Node_For_Obj1 и Node_For_Obj2 под одинаковыми индексами лежат объединяемые узлы
            // составим 2 закон кирхгофа (приравняем потенциалы этих узлов)
            // пути будем составлять от первых узлов (у=0) до остальных
            unsigned node_A = Block[Obj1]->Grounded_Node; //Node_For_Obj1[0];
            unsigned node_B = Block[Obj2]->Grounded_Node; //Node_For_Obj2[0];
            Matrix<char> Way_AB = M->Way( Node_From_LocalNode(Obj1, node_A), Node_From_LocalNode(Obj2, node_B));  // node_A и node_В один и тот же узел - путь д.б. нулевым
            for(unsigned y=0, n=Node_For_Obj1.size(); y<n; y++){  // y=1
               if( Node_For_Obj1[y] != node_A || Node_For_Obj2[y] != node_B ){
            	   // кратчайшие пути
      			   Matrix<char> Way1 = Element[Obj1]->M->Way(node_A, Node_For_Obj1[y]);
         		   Matrix<char> Way2 = Element[Obj2]->M->Way(node_B, Node_For_Obj2[y]);
                  // составляем закон
                  Matrix<char> Way(Way_AB);  // Way = way_AB
                  for(unsigned s=0, n=Way2.Cols; s<n; s++){
            		   unsigned br = Branch_From_LocalBranch(Obj2, s);
            		   Way[0][br] += Way2[0][s];    // Way = ay_AB + way2
               	   }
            	   for(unsigned s=0, n=Way1.Cols; s<n; s++){
               	   unsigned br = Branch_From_LocalBranch(Obj1, s);
            		   Way[0][br] -=  Way1[0][s];                      // Way = Way_AB + way2 - way1
                     }
                  // считаем число ветвей в контуре (должно быть более 0)
                  unsigned branches=0;
                  for(unsigned s=0;s<v;s++) {if(Way[0][s]!=0) branches++;}
                  // если число ветвей больше 0 сохраняем закон
                  //PType t0[3200];
         		   if(branches > 0){    // >1
            		   for(unsigned s=0;s<v;s++) {(*N)[nd][s] = Way[0][s];}//t0[s] = Way[0][s];}
         			   nd++;	// запишем еще одно уравнение по 2 закону Кирхгофа
         			   }
                  if(nd>=N->Rows) {Loop = false; break; } // уравнений достаточно
                  } // условие if
            	}  // конец цикла быбора пар
             } // конец для 2х объединяемых узлов
            }
         else
         	Loop = false; // просмотрели все связи
         }
      delete [] Used_Link;
      /* Берем произвольный узел и считаем по имеющейся 1 матрице инциденций M
      два развых пути до объединяемых узлов. И так как эти узлы объединены получим контур.
      Нужна функция нахождения пути между узлами:
      1) берем узел A 1 объекта и считаем путь до объединяемого узла U1 по матрице М1
      2) считаем путь от узла A до любого узла В 2 объекта по матрице М
      3) считаем путь от В до объединяемого узла U2 по матрице М2
      3.1) запоминаем пары {A,U1} {B,U2} и больше их не используем для заданных объектов
      4) сотавляем 2 закон Кирхгофа
      5) этот закон может быть законом для короткозамкнутой ветви, тогда его не сохраняем,
      	и ищем другой узел В и повторим п.3-5
      6) этот закон может иметь 0 ветвей, тогда его не сохраняем,
      	и ищем другой узел В и повторим п.3-5
      7) этот закон может содержать ветви только одного объекта - не сохраняем, повторим п.3-5
      //
      vector< vector<unsigned> >  First_Node_For_Obj(numElement); // сохраняем использованные пары узлов - Pairs_Node_For_Obj[Obj][№][0/1]
      vector< vector<unsigned> >  Second_Node_For_Obj(numElement);// сохраняем использованные узлы
      for(unsigned i=0; i<numLink;i++){
      	unsigned Obj1 = Link[i][0][0];
         unsigned Obj2 = Link[i][1][0];
      	// A = 0, B = 0, U1 = Link[i][0][1], U2 = Link[i][1][1]
         unsigned node_U1 = Link[i][0][1];
         unsigned node_U2 = Link[i][1][1];
         unsigned node_A = 0;
         unsigned node_B = 0;
         //*Pairs_Node_For_Obj = new vector<unsigned[2]>[numElement];
         Matrix<char> Way(1, v); // найденный контур
         unsigned branches = 0;	// число ветвей в результирующем законе
         bool Loop = false;		// повторить расчет для других узлов A, B
//PType t0[200];
         do{
            branches=0;
            // Не использовали ли мы раньше этой пары узлов
            bool use_1, use_2;
            do{
         	   if(node_A == node_U1)  node_A = node_A++;
               if(node_B == node_U2)  node_B = node_B++;
                  // по 1 объекту
               use_1=false;
               for(unsigned i=0, n=First_Node_For_Obj[ Obj1 ].size(); i<n; i++)
                  if( First_Node_For_Obj[ Obj1 ][i] == node_A  && Second_Node_For_Obj[ Obj1 ][i] == node_U1) {use_1 = true; break;}
                  else if( First_Node_For_Obj[ Obj1 ][i] == node_U1  && Second_Node_For_Obj[ Obj1 ][i] == node_A  ) {use_1 = true; break;}
                  // по 2 объекту
               use_2=false;
               for(unsigned i=0, n=First_Node_For_Obj[ Obj2 ].size(); i<n; i++)
                  if( First_Node_For_Obj[ Obj2 ][i] == node_B  && Second_Node_For_Obj[ Obj2 ][i] == node_U2) {use_2 = true; break;}
                  else if( First_Node_For_Obj[ Obj2 ][i] == node_U2  && Second_Node_For_Obj[ Obj2 ][i] == node_B  ) {use_2 = true; break;}
               if(use_1) node_A = node_A++;
               if(use_2) node_B = node_B++;
               if(   node_A >= Element[Obj1]->Nodes  ||
                     node_B >= Element[Obj2]->Nodes     ){
                     use_1 = false; // и далее прервем процесс поиска
                     use_2 = false;
                     }
               }while( use_1 || use_2);
            if(   node_A >= Element[Obj1]->Nodes  ||
                  node_B >= Element[Obj2]->Nodes     ){ break;}
            // запоминаем эти узлы
            First_Node_For_Obj[ Obj1 ].push_back(node_U1);
            Second_Node_For_Obj[ Obj1 ].push_back(node_A);
            First_Node_For_Obj[ Obj2 ].push_back(node_U2);
            Second_Node_For_Obj[ Obj2 ].push_back(node_B);
            // кратчайшие пути
      		Matrix<char> Way1 = Element[Obj1]->M->Way(node_A, node_U1);
         	Matrix<char> Way2 = Element[Obj2]->M->Way(node_B, node_U2);
            Matrix<char> Way_AB = M->Way( Node_From_LocalNode(Obj1, node_A), Node_From_LocalNode(Obj2, node_B));
            // проверка: а не принадлечатли ветви конечного закона одному объекту
            unsigned branches_1 = 0;// число ветвей от первого объекта
            unsigned branches_2 = 0;// число ветвей от второго объекта
            for(unsigned i=0, n=Way1.Cols; i<n; i++){
            	unsigned br = Branch_From_LocalBranch(Obj1, i);
               if( (Way1[0][i] + Way_AB[0][ br ])!=0 ) branches_1++;
               }
            if( branches_1>0 ){ // есть ветви от первого объекта
               for(unsigned i=0; i<v; i++)Way[0][i]=Way_AB[0][i]; // Way = way_AB
            	for(unsigned i=0, n=Way2.Cols; i<n; i++){
            		unsigned br = Branch_From_LocalBranch(Obj2, i);
            		Way[0][ br ] = Way2[0][i] + Way_AB[0][ br ];    // Way = ay_AB + way2
               	if( Way[0][ br ]!=0 ) branches_2++;
               	}
               if( branches_2>0){ // есть ветви от второго объекта
            		// составляем уравнение
         			for(unsigned i=0, n=Way1.Cols; i<n; i++){
                  	unsigned br = Branch_From_LocalBranch(Obj1, i);
            			Way[0][br] -=  Way1[0][i];                      // Way = ay_AB + way2 - way1
                     }
         			// считаем число ветвей в контуре (должно быть более 2х)
            		branches=0;
         			for(unsigned i=0;i<v;i++) if(Way[0][i]!=0) branches++;
               	}
               }
            if(branches <= 1 ||	// нашли закон для короткозамкнутой ветви,нашли закон из 0 ветвей
            	branches_1==0 || branches_2==0){// или закон включающий ветви только одного оъекта
               Loop = true; // повторим расчет для этих объединяемых ветвей изменив узла А и/или В
            	node_B++;
               if(node_B >= Element[Obj2]->Nodes){
               	node_B = 0;
               	node_A++;
               	}
               }
            else
            	Loop = false;	// найденный закон удовлетворяет всем условиям, идем к другой паре объединяемых узлов
         	}while( Loop && node_A<Element[Obj1]->Nodes);  // <=1
         //PType t1[8000];
         if(branches > 1){    // >1
            for(unsigned i=0;i<v;i++) {(*N)[nd][i] = Way[0][i];}//  t1[i]= Way[0][i];}
         	nd++;	// запишем еще одно уравнение по 2 закону Кирхгофа
         	}
         if (nd == N->Rows) break; // уравнений достаточно
         } */
   	}
   // 2 закон кирхгофа для короткозамкнутых ветвей (если не хватает уравнений) - в нормальном режиме должно хватать
   if( nd < N->Rows ){
   	unsigned num_use_SB = N->Rows - nd;
      if( num_use_SB>num_Short_Branch ) num_use_SB = num_Short_Branch;
   	for(unsigned i=0; i<num_use_SB;i++){
      	(*N)[nd][ Short_Branch[i] ] = 1;
      	nd++; // запишем следующее уравнение
      	}
   	}
   delete [] Short_Branch;
   if(nd < N->Rows) {err(0, "PGrid::Link_Elements: не удалось составить достаточное количество уравнений по 2 закону Кирхгофа");}
   // матрица узловых токов и матрица потенциалов
   _J = new PType[u];
   J = new PType*[u];
   for(unsigned j=0;j<u;j++) {_J[j]=0; J[j]=&_J[j];}
   // обнуляем матрицу потенциалов
   f = new PType*[u];
   for(unsigned j=0;j<u;j++) {f[j]=NULL;}
   for(unsigned b=0; b<numElement; b++){
      PElement *p = Element[b];
      unsigned tmp_rows = p->Nodes();
      for(unsigned i=0; i<tmp_rows; i++){
         unsigned nd = Node_From_LocalNode(b,i);
         (*J[nd]) += *(p->J[i]);	// если узлы были объединены, то их задающие токи складываются
         if( f[nd]==NULL )
            f[nd] = p->f[i];
         else  // i-ый узел элемента p уже указывает на узел другого элемента_2 (они связаны)
         	p->f[i] = f[nd];	// тогда потенциал i-ого узела элемента p, будет равен потенциалу узла элемента_2
      	}
      }

   // матрица дифференциальных сопротивлений, начальных условий
   // матрица эдс в ветвях
   E = new PType*[v];
   // матрица дифференциальых сопротивлений
   Z = new PType*[v];
   // напряжений на емкостях в ветвях (начальные условия)
   U_C= new PType*[v];
   // напряжений на индуктивностях в ветвях (начальные условия)
   U_L= new PType*[v];
   // напряжений на обузловленные взаимоиндуктивностями в ветвях (начальные условия)
   U_Lm= new PType*[v];
   // дроссели в ветвях
   D = new Throttle*[v];
   // напряжений на дроселлях в ветвях
   U_D= new PType*[v];
   // матрица результатов
   I = new PType*[v];
   R = new PType*[v];
   L = new PType*[v];
   C = new PType*[v];
   _Lm.clear();
	_Lm.push_back(0);
   //Lm = new vector<PType>::iterator *[v];  for(unsigned i=0;i<v;i++)Lm[i] = new vector<PType>::iterator [v];
   Lm = new PType **[v];  for(unsigned i=0;i<v;i++)Lm[i] = new PType*[v];
      const PType *first = _Lm.begin();
      for(unsigned i=0;i<v;i++)
         for(unsigned j=0;j<v;j++)
            Lm[i][j] = (PType*)first;  // по умолчанию ссылаемся на локальную матрицу взаимоиндуктивностей
   for(unsigned b=0; b<numElement;b++){
   	PElement *p = Element[b];
      //p->accept(); // <-- она должна вызываться до сборки сети, т.к. она может
      					// изменять внутренние связи, необходимые при сборке сети
                     // (напр трехфазный транс. PTransformer_3). Здесь вызывать нет ее смысла
      ((PUnit*)p)->update_differential_resistance(); // пересчет матрицы Z - ато не дай бог забыли
      unsigned tmp_cols = p->Branches();
      for(unsigned j=0; j<tmp_cols; j++){
         unsigned br = Branch_From_LocalBranch(b,j);
         E[br] = p->E[j];
         Z[br] = p->Z[j];      // матрица дифференциальных сопротивлений
         U_C[br] = p->U_C[j];  // напряжений на емкостях в ветвях (начальные условия)
         U_L[br] = p->U_L[j];  // напряжений на индуктивностях в ветвях (начальные условия)
         U_Lm[br] = p->U_Lm[j];// напряжений на обузловленные взаимоиндуктивностями в ветвях (начальные условия)
         D[br] = p->D[j];      // дроссель в ветке
         U_D[br] = p->U_D[j];  // напряжений на дроселлях в ветвях
         I[br] = p->I[j];      // результаты расчета токов
         R[br] = p->R[j];      // активное сопротивоение ветви
         L[br] = p->L[j];      // индуктивность ветви
         C[br] = p->C[j];      // емкость ветви
         for(unsigned k=0;k<tmp_cols;k++){
            unsigned br2 = Branch_From_LocalBranch(b,k);
            Lm[br][br2] = p->Lm[j][k];  // взаимоиндуктивность между ветвями (ссылаемся на _Lm первоначального объекта, но при использовании mutual_inductence будем создавать значение своем векторе _Lm и ссылаться на него)
            }
      	}
      }
   // значения предшествующего тока
   _I2= new PType[v];
   for(unsigned i=0;i<v;i++)
        _I2[i] = *I[i];

   update_sources(true); // принудительно обновляем значения
   update_elements(true);  // принудительно обновляем значения
   // Создадим группы выводов
   for(unsigned b=0; b<numElement;b++){
   	PElement *p = Element[b];
      for(unsigned j=0, n=p->Out.Count(); j<n; j++){
         Pins *unit_pins = &p->Out[j];                	// это группа выводов исходного объекта
         Pins *grid_pins = new Pins(*unit_pins);     		// создаем Группу выводов для сети как копию
         grid_pins->Name = p->FullName() + "." + unit_pins->Name; // меняем имя
         Out.add(grid_pins); 										// добавляем группу выводов в список
      	}
      }
   return true;
}
//---------------------------------------------------------------------------
PGrid::~PGrid(){
   // чистим память, выделенную под переменные
   delete A;      A = NULL;
   delete [] B;   B = NULL;
   delete F;      F = NULL;
   delete [] _J;  _J= NULL;
   if(Branch!=NULL)
      for(unsigned j=0;j<numElement;j++)
         delete [] Branch[j];
   delete [] Branch; Branch = NULL;
   if(Node!=NULL)
      for(unsigned j=0;j<numElement;j++)
         delete [] Node[j];
   delete [] Node;      Node = NULL;
   delete [] Element;   Element = NULL;
   delete [] Links;     Links = NULL;
   delete [] U;         U = NULL;
   delete [] _U_Lm2;    _U_Lm2 = NULL;
   delete [] U_Lm2;      U_Lm2 = NULL;
   delete [] _I2;       _I2 = NULL;
   // удаляем динамически созданные группы выводов
   for(unsigned i=0, n=Out.Count(); i<n; i++)
   	delete &Out[i];
}
//---------------------------------------------------------------------------
/* Находит все узлы объединенные с заданным
   numLink  - число связей
   Link[][2][2] - описание связей
   link_use[] - связь обработана или нет?
   i_node - номер узла сети, который присваивается всем объединяемым узлам (текущее число вершин в результирующей матрице)
   obj, node - связь с какой вершиной ищем
*/
void PGrid::linked_nodes(const unsigned numLink, const unsigned Link[][2][2], bool link_use[],const unsigned i_node,const unsigned obj,const unsigned node){//, unsigned &tmp){
   for(unsigned j=0; j<numLink; j++){
   	if( ! link_use[j] ){
      	if( Link[j][0][0] == obj && Link[j][0][1] == node ){ // если текущий узел объединяем с ним
         	// с ним связанный
            Node[  Link[j][1][0]  ][  Link[j][1][1]  ] = i_node;
            link_use[j]=true;  // связь обработана
            // какие с этим связанны?
            linked_nodes(numLink, Link, link_use, i_node, Link[j][1][0], Link[j][1][1]);
            }
         else if( Link[j][1][0] == obj && Link[j][1][1] == node ){ // если текущий узел объединяем с ним
         	// с ним связанный
            Node[  Link[j][0][0]  ][  Link[j][0][1]  ] = i_node;
            link_use[j]=true;  // связь обработана
            // какие с этим связанны?
            linked_nodes(numLink, Link, link_use, i_node, Link[j][0][0], Link[j][0][1]);
            }
         }
      }
}
//---------------------------------------------------------------------------
/* Возвращает номер глобальной вершины по номеру объекта в сети и номеру его локальной вершины   */
unsigned __fastcall PGrid::Node_From_LocalNode(unsigned Obj, unsigned LocalNode) { return Node[Obj][LocalNode]; }
//---------------------------------------------------------------------------
/*  Возвращает номер глобальной ветви по номеру объекта в сети и номеру его локальной ветви  */
unsigned __fastcall PGrid::Branch_From_LocalBranch(unsigned Obj, unsigned LocalBranch){ return Branch[Obj][LocalBranch]; }
//---------------------------------------------------------------------------
unsigned PGrid::ObjNum_From_Object(PElement *obj){
   for(unsigned i=0; i<numElement; i++)
      if( Element[i] == obj )
         return i;
   return -1;  // нет объекта в списке
}
//---------------------------------------------------------------------------
/* Возвращает номер глобальной вершины по номеру объекта в сети и номеру его локальной вершины   */
unsigned __fastcall PGrid::Node_From_LocalNode(PElement* Obj, unsigned LocalNode) {
   unsigned obj_num = ObjNum_From_Object(Obj);
   if(obj_num == -1) return -1;
   return Node[obj_num][LocalNode];
}
//---------------------------------------------------------------------------
/*  Возвращает номер глобальной ветви по номеру объекта в сети и номеру его локальной ветви  */
unsigned __fastcall PGrid::Branch_From_LocalBranch(PElement* Obj, unsigned LocalBranch){
   unsigned obj_num = ObjNum_From_Object(Obj);
   if(obj_num == -1) return -1;
   return Branch[obj_num][LocalBranch];
}
//---------------------------------------------------------------------------
bool PGrid::Make_Equation(){
   if( !BuildOK ){
      err(0, "PGrid: уравнения для сети не составлены, т.к. сеть была собрана с ошибками");
      return false;
      }
   // инициализируем матрицы по уравнениям кирхгофа
   unsigned n = u-1+k;
   if( n != v) {err(0, "PGrid::Make_Equation: число уравнений по законам Кирхгофа не равно числу ветвей. Ошибка в коде.");}
   A = new Matrix<PType>(n, v);
   B = new PType[v];  	// по числу уравнений == числу ветвей
   U = new PType[v];	// падения напряжений в ветвях
   F = new Matrix<PType>( M->Rows-1, M->Cols );
   _U_Lm2 = new PType[v];  		// падения напряжений на взаимоиндуктивнотях от текущего тока
   U_Lm2 = new PType*[v];  		// падения напряжений на взаимоиндуктивнотях от текущего тока
   for(unsigned i=0;i<v;i++) _U_Lm2[i]=0;
   for(unsigned i=0;i<v;i++) U_Lm2[i]=&_U_Lm2[i];

   // заполняем матрицу A по уравнениям состояния
   Update_Equation();

   // получаем матрицу для нахождения потенциалов
   Matrix<PType> *tmp = new Matrix<PType>( M->Rows, M->Rows-1 );
   bool *deleted_branch = new bool[M->Cols];
   Copy_Matrix( & M->tree(deleted_branch), tmp);
   //bool as[8];for(int i=0;i<M->Cols;i++) as[i]=deleted_branch[i];
   tmp->Delete_Row((unsigned)(tmp->Rows-1));
   *tmp = -1.0*(tmp->transpose().inverse());

   for(unsigned j=0, k=0; j<F->Cols; j++)
   	if( deleted_branch[j] ){
         // ветка была удалена при составлении матрицы tmp
         for(unsigned i=0; i<F->Rows; i++) (*F)[i][j] = 0;
      	}
      else{ // ветка была сохранена при построении матрицы tmp
   		for(unsigned i=0; i<F->Rows; i++) (*F)[i][j] = (*tmp)[i][k];
         k++; // берем следующую ветку в матрице tmp
         }
   delete tmp;
   delete [] deleted_branch;
   return true;
}
//---------------------------------------------------------------------------
void PGrid::Update_Equation(){
   // инициализируем матрицы по уравнениям кирхгофа
   unsigned n = u-1+k;
   if( n != v) {err(0, "PGrid::Update_Equation: число уравнений по законам Кирхгофа не равно числу ветвей. Ошибка в коде.");}
   // заполняем матрицы по уравнениям Кирхгофа
   // 1 закон Кирхгофа
   for(unsigned i=0; i<u-1; i++)
   	for(unsigned j=0; j<v; j++)
      	(*A)[i][j]= (*M)[i][j];
   // учет взаимоиндуктивности для 2 закона Кирхгофа
   Matrix<PType> *Nm = new Matrix<PType>(k,v);
   *Nm = (1/smp) * Multiply_Matrix(N, Lm, v, v);
/*PType t0[3][3];
for(int i=0;i<2;i++)
	for(int j=0;j<3;j++)
   	t0[i][j] = (*N)[i][j];*/

   // 2 закон Кирхгофа
   for(unsigned i=0; i<k; i++)
   	for(unsigned j=0; j<v; j++)
     	(*A)[i+u-1][j]= (*N)[i][j]*(*Z[j]) + (*Nm)[i][j];
   delete Nm;

   // учет дросселей
   for(unsigned j=0; j<v; j++)
      if( D[j]!=NULL ){ // ветвь в которой установлен дроссель
      // магнитная ветка
      unsigned br = Branch_From_LocalBranch(D[j]->Obj, D[j]->MBranch);
      // поиск уравнений в которые входит ветка с дросселем
      for(unsigned i=0; i<k; i++){
         if( (*N)[i][j] != 0 ){  // уравнение в которую входит ветка с дросселем
            int sgn_for_branch = (*N)[i][j];               // ветка сонаправлена или противонаправлена с контуром
            (*A)[i+u-1][br] += sgn_for_branch * D[j]->Windings / smp;  // учет влияния потока в магнитопроводе на эту ветку
            }
      	// поиск уравнений, в которые входит магнитная ветвь
         if( (*N)[i][br] != 0 ){  // уравнение в которую входит ветка с дросселем
            int sgn_for_branch = (*N)[i][br];      // ветка сонаправлена или противонаправлена с контуром
            (*A)[i+u-1][j] += - sgn_for_branch * D[j]->Windings;	// учет МДС
         	}
         }
      }
   //Matrix<PType> R1(*A);
   *A = A->inverse();
   /*
   R1=(*A) * R1;
   unsigned NotNull=0;
   //PType Sum=0;
   //PType t0[3][3];
   for(unsigned i=0;i<A->Cols; i++)
      for (unsigned j=0; j<A->Rows; j++){
      	if(R1[i][j]>0.01)
            NotNull++;
         //Sum += fabs(R1[i][j]);
         //t0[i][j] = R1[i][j];
         }
   if(NotNull != A->Rows)
      err(0,"PGrid::Update_Equation(): Обратная матрица найдена с погрешностью более 1%.");
   */
}
//---------------------------------------------------------------------------
// обновляем информацию об источника тока и ЗДС в ветвях
bool PGrid::update_sources(bool force_update){
   bool ret=false;
   //PType i = *Element[0]->I[1];
   for(unsigned i=0; i<numElement; i++){
      bool ret2 = Element[i]->update_sources(force_update);
      ret = ret | ret2;
      }
   // если обновились
   if(ret || force_update){
      // матрица узловых токов
      for(unsigned j=0;j<u;j++) *J[j]=0;
      for(unsigned b=0; b<numElement; b++){
         PElement *p = Element[b];
         unsigned tmp_rows = p->Nodes();
         for(unsigned i=0; i<tmp_rows; i++){
            unsigned nd = Node_From_LocalNode(b,i);
            (*J[nd]) += *(p->J[i]);	// если узлы были объединены, то их задающие токи складываются
      	   }
         }
      }
   return ret;
}
//---------------------------------------------------------------------------
// обновляем информацию о падении напряжений в ветвях (учет нелинейных элементов)
bool PGrid::update_elements(bool force_update){
   bool ret=false;
   for(unsigned i=0; i<numElement; i++){
      bool  ret_i = Element[i]->update_elements(force_update);
      if(ret_i) Element[i]->update_differential_resistance();  // если поменялись сопротивления, изменим значения дифференциальных сопротивлений
                                                               // у элемента. А так как сеть использует указатели на значения сопротивлений у
                                                               // объектов, то поменяются дифференциальные сопротивления и ветвей сети
      ret = ret | ret_i;
      }
   return ret;
}
//---------------------------------------------------------------------------
PElement* PGrid::obj_from_name(String Name){
   String obj, Child;   // объект и объект который в него входит, например ОЭС.Сеть2.T2
   // зазделяем имя до первой точки
   int point=Name.Pos(".");
   if(point!=0){
      obj = Name.SubString(1,point-1);
      Child = Name.SubString(point+1, Name.Length()-point);
   }else{
      obj = Name;
      Child = "";
      }
   for(unsigned i=0; i<numElement;i++)
      if(Element[i]->Name == obj){   // нашли объект
         if(Child!="")  // ищем объект который в него входит
            return ((PGrid*)Element[i])->obj_from_name(Child);
         else
            return Element[i];
         }
   return NULL;   // не нашли объект с этим именем
}
/*/---------------------------------------------------------------------------
void PGrid::calc(){
   // обновляем информацию об источника тока и ЗДС в ветвях
   update_sources();
   // обновляем информацию о сопротивлении ветвей
   bool elm = update_elements();
   // если изменились сопротивления элементов, то меняются уравнения состояния
   if( elm ) Update_Equation();
	// на основании о токах предшествующего режима рассчитаем эдс на емкостях (начальные условия)
   for(unsigned i=0; i<v; i++)
 		if(*C[i])*U_C[i] = *U_C[i] + *I[i]*smp/(*C[i]);
		else *U_C[i]=0;
   // на основании предшествующего режима рассчитаем эдс в индуктивностях (начальные условия)
   for(unsigned i=0; i<v; i++)
		*U_L[i] = *L[i]*(*I[i])/smp;
   // на основании предшествующего режима рассчитаем дополнительную эдс во взаимоиндуктивных элементах(начальные условия)
   for(unsigned int j=0; j<v; j++)
   	(*U_Lm[j]) = *U_Lm2[j];
   // рассчет матрицы B из уравнения A*I=B
   for(unsigned i=0; i<u-1; i++)
      B[i] = *J[i];	// правые части уравнений по 1 закону Кирхгофа
	for(unsigned i=u-1; i<v; i++){
   	B[i] = 0;
   	for(unsigned j=0; j<v; j++)
			B[i] += (*N)[i-u+1][j]*((*E[j])+(*U_L[j])-(*U_C[j])+(*U_Lm[j])); // правые части по 2 закону Кирхгофа
      }

   // расчет новых значений токов I=A*B
   Multiply_Matrix( A, B, I);

   // Разность потенциалов в ветвях
   for(unsigned i=0; i<v; i++){
   	U[i] = (*Z[i])*(*I[i]) - (*E[i]) - (*U_L[i]) + (*U_C[i]) - (*U_Lm[i]); // Z включает падения напряжения на емк и инд, но не включает падение напряжения на взаимоинд (учтем ниже)
      // падения напряжения на взаимоиндуктивностях от текущего тока
      *U_Lm2[i]=0;
   	for(unsigned int j=0; j<v; j++)
			(*U_Lm2[i]) += (*Lm[i][j])/smp * (*I[j]);
      // учет этого падения напряжения
      U[i] += *U_Lm2[i];
      }
	// расчет потенциалов узлов
   Multiply_Matrix( F, U, f);
   *f[u-1]=0; // потенциал самого последнего узла == 0
}*/
//---------------------------------------------------------------------------
void PGrid::calc(){
   // обновляем информацию об источника тока и ЗДС в ветвях
   update_sources();
   // обновляем информацию о сопротивлении ветвей
   bool elm = update_elements();
   // если изменились сопротивления элементов, то меняются уравнения состояния
   if( elm ) Update_Equation();
	// на основании о токах предшествующего режима рассчитаем эдс на емкостях (начальные условия)
   for(unsigned i=0; i<v; i++)
 		if(*C[i])*U_C[i] = *U_C[i] + smp/(*C[i])*(_I2[i] + *I[i])/2;
		else *U_C[i]=0;
   // на основании предшествующего режима рассчитаем эдс в индуктивностях (начальные условия)
   for(unsigned i=0; i<v; i++)
		*U_L[i] = *L[i]*(*I[i])/smp;
   // на основании предшествующего режима рассчитаем дополнительную эдс во взаимоиндуктивных элементах(начальные условия)
   for(unsigned int j=0; j<v; j++)
   	(*U_Lm[j]) = *U_Lm2[j];
   // на основании предшествующего режима рассчитаем ЭДС на дросселях
   for(unsigned i=0; i<v; i++)
      if( D[i]!=NULL ){ // ветка, в которой установлен дроссель
         // магнитная ветка, на которую намотан дроссель
         unsigned br = Branch_From_LocalBranch(D[i]->Obj, D[i]->MBranch);
         if(br!=-1) *U_D[i] = *I[br] * D[i]->Windings / smp;
         }
   // рассчет матрицы B из уравнения A*I=B
   for(unsigned i=0; i<u-1; i++)
      B[i] = *J[i];	// правые части уравнений по 1 закону Кирхгофа
	for(unsigned i=u-1; i<v; i++){
   	B[i] = 0;
   	for(unsigned j=0; j<v; j++)
         B[i] += (*N)[i-u+1][j]*( (*E[j])+(*U_L[j])-(*U_C[j])+(*U_Lm[j])+(*U_D[j]) ); // правые части по 2 закону Кирхгофа
         if(*C[i])    B[i] = B[i] - smp/(*C[i])/2*(*I[i]);
      }

   // сохраняем предшествующие значения тока
   for(unsigned i=0; i<v;i++)
        _I2[i] = *I[i];

   // расчет новых значений токов I=A*B
   Multiply_Matrix( A, B, I);

   // Разность потенциалов в ветвях
   for(unsigned i=0; i<v; i++){
   	U[i] = (*Z[i])*(*I[i]) - (*E[i]) - (*U_L[i]) + (*U_C[i]) - (*U_Lm[i]) - (*U_D[i]); // Z включает падения напряжения на емк, инд и дросселе, но не включает падение напряжения на взаимоинд (учтем ниже)
      if(*C[i])    U[i] = U[i] + smp/(*C[i])/2*_I2[i];
      // падения напряжения на взаимоиндуктивностях от текущего тока
      *U_Lm2[i]=0;
   	for(unsigned int j=0; j<v; j++)
			(*U_Lm2[i]) += (*Lm[i][j])/smp * (*I[j]);
      // учет этого падения напряжения
      U[i] += *U_Lm2[i];
      // падения напряжения на дросселе от текущего тока
      if(D[i]!=NULL){
         unsigned br = Branch_From_LocalBranch(D[i]->Obj, D[i]->MBranch);
         U[i] += D[i]->Windings * (*I[br]) / smp;
         }
      }
	// расчет потенциалов узлов
   Multiply_Matrix( F, U, f);
   *f[u-1]=0; // потенциал самого последнего узла == 0

   /*/ выбор оптимального шага
   PType d = fabs(*I[0] - _I2[0]);
   if(d>0.001){
   	smp = smp/2;
      update_elements(true);
      Update_Equation();
      } */
   /*if(d<1e-17){
   	smp = smp*2;
      update_elements(true);
      Update_Equation();
      } */
}
//---------------------------------------------------------------------------
PElement* __fastcall PGrid::Unit(int Index) { return Element[Index];};
//---------------------------------------------------------------------------
unsigned  __fastcall PGrid::num_Units(){ return numElement; }
//---------------------------------------------------------------------------
void PGrid::Save(ostream &stream){
   // сохраняем данные базового класса
	sire::Save(stream);
   // число объектов
   stream << numElement << " ";
   // сохраняем объекты
   for(unsigned i=0; i<numElement; i++)
      SaveObj(Element[i], stream);
   // число связей
   stream << numLinks << " ";
   // сохраняем информацию о связях
   for(unsigned i=0; i<numLinks; i++){
      stream << Links[0+4*i] << " ";
      stream << Links[1+4*i] << " ";
      stream << Links[2+4*i] << " ";
      stream << Links[3+4*i] << " ";
   	}
   // сохраняем информацию о группах выводов
   String outs = getOuts();
   stream << StringForSave( &outs )  << " ";
}
//---------------------------------------------------------------------------
void PGrid::Read(istream &stream){
   // читаем данные базового класса
	sire::Read(stream);
   PElement **el;
   unsigned *lnk;
   unsigned numEl;
   unsigned numLnk;
   // число объектов
   stream >> numEl;
   el = new PElement*[numEl];
   // создаем объекты
   char cname[1024]; String ClassName;
   for(unsigned i=0; i<numEl; i++)
      el[i] = CreateObj( stream );
   // число связей
   stream >> numLnk;
   lnk = new unsigned[4*numLnk];
   // сохраняем информацию о связях
   for(unsigned i=0; i<numLnk; i++){
      stream >> lnk[0+4*i];
      stream >> lnk[1+4*i];
      stream >> lnk[2+4*i];
      stream >> lnk[3+4*i];
   	}
   // перед созданием сети применим параметры
   for(unsigned i=0; i< numEl; i++ ) el[i]->accept();
   // создаем эту сеть
   Link_Elements(numEl, el, numLnk, (unsigned (*)[2][2])lnk );
   // устанавливаем группы выводов
   char outs[1024];
   stream >> outs;
   setOuts( outs );
}
//---------------------------------------------------------------------------
String PGrid::ClassName(){ return "PGrid";}
//---------------------------------------------------------------------------
String PGrid::getOuts(){
	String strOuts = "";
   for(int i=0, n=Out.Count(); i<n; i++){
   	strOuts +=  Out[i].Name; //Out[i][0].Obj()->Name + "." + Out[i].Name;
      if( (i+1)<n ) strOuts += ";";
      }
	return strOuts;
}
//---------------------------------------------------------------------------
// стороку содержащую группу выводов преобразуем в группы выводов
void PGrid::setOuts(String strOut){
   // очищаем
   Out.Clear();    // список групп выводов
   // очищаем группы выводов
   //for(int i=0, n=X.size(); i<n; i++) delete X[i];
   //X.clear();
   // находим группы выводов
	vector<String> Point;
	// возможно было более одного пробела в начале
   int numSpace, m;
   for(numSpace=1, m=strOut.Length(); numSpace<=m; numSpace++) if(strOut[numSpace]!=' ') break;
   strOut = strOut.SubString(numSpace, strOut.Length() - numSpace + 1);
   int pos = strOut.Pos(";");
   while( pos ){
   	Point.push_back( strOut.SubString(1,pos-1) );
      strOut = strOut.SubString(pos+1, strOut.Length()-pos);
      // возможно было более одного пробела
      for(numSpace=1, m=strOut.Length(); numSpace<=m; numSpace++) if(strOut[numSpace]!=' ') break;
      strOut = strOut.SubString(numSpace, strOut.Length() - numSpace + 1);
      pos = strOut.Pos(";");
      }
   Point.push_back( strOut.SubString(1,strOut.Length()) );
   // перебираем каждую группу выводов
   String obj, outName;   // объект и объект который в него входит, например ОЭС.Сеть2.T2
   for(unsigned i=0, n=Point.size(); i<n; i++){
   	// зазделяем имя до первой точки
   	int point=Point[i].Pos(".");
   	if(point!=0){
      	obj =Point[i].SubString(1,point-1);
      	outName = Point[i].SubString(point+1, Point[i].Length()-point);
         PElement *Obj = obj_from_name(obj);
         if(Obj){
         	// добавляем группу выводов этого объекта
            for(unsigned j=0, m=Obj->Out.Count(); j<m; j++)
               if(Obj->Out[j].Name == outName){
                  set_pins( Obj, &Obj->Out[j] );
            		}
            }
			}
      }
}
//---------------------------------------------------------------------------
void PGrid::set_pins(PElement* Obj, Pins* pins){
   // создадим группу выводов
   Pins *x = new Pins( pins->Count );
   x->Name = Obj->Name + "." + pins->Name;
   // создадим выводы указывающие на сеть
   for(int i=0, n=pins->Count; i<n; i++){
      PPin tmp_pin;
      unsigned node = Node_From_LocalNode( (*pins)[i].Obj(), (*pins)[i].Node() );
      // если такой узел найден
      if(node != -1)
         tmp_pin = set_pin( node );
      else{
         err(0, "PGrid: узел не найден в сети");
         tmp_pin = set_pin( Grounded_Node );
         }
      (*x)[i] = tmp_pin;
      }
   Out.add( x );
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
PSubGrid::PSubGrid(){
	Describe_Fields(); // опишем поля
   Name = "Сеть";
}
//---------------------------------------------------------------------------
void PSubGrid::Describe_Fields(){
   PFieldInfo fi;
   // сопротивление во включ. состоянии
   fi.Name = "Группы выводов";
   fi.Type = "Pins_List";
   fi.ptr = (void*)this;
   fi.Mode = ro;
   Fields.push_back(fi);
}
//---------------------------------------------------------------------------
PElement* __fastcall PSubGrid::Unit(int Index) { return Element.at(Index);};
//---------------------------------------------------------------------------
unsigned  __fastcall PSubGrid::num_Units(){ return Element.size(); } 
//---------------------------------------------------------------------------
void PSubGrid::Save(ostream &stream){
   // сохраняем данные базового класса
	sire::Save(stream);
}
//---------------------------------------------------------------------------
void PSubGrid::Read(istream &stream){
   // читаем данные базового класса
	sire::Read(stream);
}
//---------------------------------------------------------------------------
String PSubGrid::ClassName(){ return "PSubGrid";}
//---------------------------------------------------------------------------
String PSubGrid::getOuts(){
	String strOuts = "";
   for(int i=0, n=Out.Count(); i<n; i++){
   	strOuts +=  Out[i][0].Obj()->Name + "." + Out[i].Name;
      if( (i+1)<n ) strOuts += ";";
      }
	return strOuts;
}
//---------------------------------------------------------------------------
// стороку содержащую группу выводов преобразуем в группы выводов
void PSubGrid::setOuts(String strOut){
   // очищаем
   Out.Clear();    // список групп выводов
   // находим группы выводов
	vector<String> Point;
	// возможно было более одного пробела в начале
   int numSpace, m;
   for(numSpace=1, m=strOut.Length(); numSpace<=m; numSpace++) if(strOut[numSpace]!=' ') break;
   strOut = strOut.SubString(numSpace, strOut.Length() - numSpace + 1);
   int pos = strOut.Pos(";");
   while( pos ){
   	Point.push_back( strOut.SubString(1,pos-1) );
      strOut = strOut.SubString(pos+1, strOut.Length()-pos);
      // возможно было более одного пробела
      for(numSpace=1, m=strOut.Length(); numSpace<=m; numSpace++) if(strOut[numSpace]!=' ') break;
      strOut = strOut.SubString(numSpace, strOut.Length() - numSpace + 1);
      pos = strOut.Pos(";");
      }
   Point.push_back( strOut.SubString(1,strOut.Length()) );
   // перебираем каждую группу выводов
   String obj, outName;   // объект и объект который в него входит, например ОЭС.Сеть2.T2
   for(unsigned i=0, n=Point.size(); i<n; i++){
   	// зазделяем имя до последней точки
      int point=Point[i].Pos(".");
      //for(int i=Point[i].Pos("."); i!=0; i=Point[i].Pos(".")) point = i;
   	if(point!=0){
      	obj =Point[i].SubString(1,point-1);
      	outName = Point[i].SubString(point+1, Point[i].Length()-point);
         PElement *Obj = obj_from_name(obj);
         if(Obj){
         	// добавляем группу выводов этого объекта
            for(unsigned j=0, m=Obj->Out.Count(); j<m; j++)
               if(Obj->Out[j].Name == outName)
                  Out.add( &Obj->Out[j] );
            }
			}
      }
}
//---------------------------------------------------------------------------
PElement* PSubGrid::obj_from_name(String Name){
   String obj, Child;   // объект и объект который в него входит, например ОЭС.Сеть2.T2
   // зазделяем имя до первой точки
   int point=Name.Pos(".");
   if(point!=0){
      obj = Name.SubString(1,point-1);
      Child = Name.SubString(point+1, Name.Length()-point);
   }else{
      obj = Name;
      Child = "";
      }
   for(unsigned i=0, n=Element.size(); i<n; i++)
      if(Element[i]->Name == obj){   // нашли объект
         if(Child!="")  // ищем объект который в него входит
            return ((PSubGrid*)Element[i])->obj_from_name(Child);
         else
            return Element[i];
         }
   return NULL;   // не нашли объект с этим именем
}
//---------------------------------------------------------------------------
// установить описание сети
bool PSubGrid::Change( Pins_Link &InLink, vector<PSubGrid*> SubGrids ){
   // мы не являемся сетью для элементов, которые мы "забываем"
   for(unsigned i=0, n=Element.size(); i<n; i++)
   	if(Element[i]->Grid==this)
   		Element[i]->Grid = NULL;
   // "забудем" предшествующие элементы
   Element.clear();
   // запомним связи
   Internal_Links = InLink;
   // запомним подсети
   for(int i=0, n=SubGrids.size(); i<n; i++){
      PSubGrid *sg = SubGrids[i];
      // добавим, если элемент уже не добавлен
      int i;
      int n=Element.size();
      for(i=0; i<n; i++) if( Element[i]==sg ) break;
      if(i==n){
         if(sg->Grid == NULL )
         	sg->Grid = this;			// добавляемый элемент будет относиться к этой сети
      	Element.push_back( sg );}
   	}
   // запомним объекты подсети
   for(int i=0, n=InLink.Number(); i<n; i++){
      PElement * A = InLink[i].First->Obj();
      PElement * B = InLink[i].Second->Obj();
      // запомним элемент А, если он не повторяющийся
      int i;
      int n=Element.size();
      for(i=0; i<n; i++) if( Element[i]==A ) break;
      if(i==n){
      	Element.push_back( A );
         // сеть видит всю аппаратуру, входящую во все его подсети
         // поэтому, если какая-то подсеть сказала, что это оборудование его,
         // то не перензначаем подсеть.
         if(A->Grid == NULL )
         	A->Grid = this;			// добавляемый элемент будет относиться к этой сети
         }
      // запомним элемент B, если он не повторяющийся
      n=Element.size();
      for(i=0; i<n; i++) if( Element[i]==B ) break;
      if(i==n){
      	Element.push_back( B );
         // сеть видит всю аппаратуру, входящую во все его подсети
         // поэтому, если какая-то подсеть сказала, что это оборудование его,
         // то не перензначаем подсеть.
         if(B->Grid == NULL )
         	B->Grid = this;			// добавляемый элемент будет относиться к этой сети
         }
      }
   // проверим уникальны ли имена элементов
   for(unsigned i=0, n=Element.size(); i<n; i++){
      String name = Element[i]->Name;
      for(unsigned j=i+1; j<n; j++)
         if( name == Element[j]->Name ){
            err(0, "PSubGrid: внутри сети имена всех объектов должны быть уникальны, сеть не собрана");
            return false;
         	}
      }
   // удалим несуществующие выводы от сети
   for(unsigned i=0, n=Out.Count(); i<n; i++){
      PElement *Obj = Out[i][0].Obj();
      int j, m=Element.size();
      for(j=0; j<m; j++) if(Element[j] == Obj) break;
      if(j==m) // объект, на который ссылаются выводы не найден
      	Out.Delete( &Out[i] );	// удалим группу выводов	
   	}
   return true;
}
//---------------------------------------------------------------------------
// возвращает все связи внутри себя. включая связи подсетей
Pins_Link PSubGrid::Links(){
   Pins_Link   Link;
   LinksForSubGrid( this, Link );
   return Link;
}
//---------------------------------------------------------------------------
// рекурчивно опросим подсети
void PSubGrid::LinksForSubGrid(PSubGrid *grid, Pins_Link &Link){
   // связи объектов внутри этой сети получив их связи
   Link.add( grid->Internal_Links );
   // добавим также связи объектов внутри подсетей, которые расположены внутри этой сети
   for(int i=0, n=Element.size(); i<n; i++){
      PSubGrid *g = dynamic_cast<PSubGrid*>( Element[i] );
      if( g ) LinksForSubGrid( g , Link);
      }
}













tstA::tstA():PUnit(5, 4, 2),X(2){
	char _M[4][5] =   {{-1, 0,-1, 0, 0},
                      {+1,-1, 0, 0, 0},
                      { 0,+1,+1,-1,-1},
                      { 0, 0, 0, 1, 1},
                       };
   char _N[2][5] = {{ 1, 1,-1, 0, 0},
                    { 0, 0, 0,-1, 1}};
	M->operator =(&_M[0][0]);
   N->operator =(&_N[0][0]);
   /*/ должно быть задано из вне
   _U_L[0]=0;_U_L[1]=0; _U_L[2]=0;
   _U_C[0]=0;_U_C[1]=0; _U_C[2]=0;
   _R[0]=1; _R[1]=1; _R[2]=3;
   _L[0]=0.001; _L[1]=0.001; _L[2]=0;
   _C[0]=0; _C[1]=0; _C[2]=0;
   _E[0]=0; _E[1]=0; _E[2]=-10;
   _J[0]=0; _J[1]=0; _J[2]=0;*/

   // расчет матрицы Z
	for(unsigned i=0; i<v; i++)
   	if(*C[i])
      	*Z[i] = *R[i] + *L[i]/smp + smp/(*C[i]);
      else
      	*Z[i] = *R[i] + *L[i]/smp;

   // установка взаимоиндуктивностей
   mutual_inductance(0,1,-1*sqrt(_L[0]*_L[1]));
   // установка выводов
  Out.add(&X);  // указатели на группы элементов
   X[0] = set_pin(0);
   X[1] = set_pin(1);

   /*/ моделируем магнитопровод
   _Magn = new MCircuit(2,2,1);
   char magn_M[2][2] ={{-1,-1},
                       { 1, 1}};
   char magn_N[1][2] = { 1,-1};
   *_Magn->M = (char*)magn_M;
   *_Magn->N = (char*)magn_N;
   *_Magn->R[0] = *_Magn->R[1] = 1e3/2;
   Magn = new PGrid(_Magn);
   Magn->Make_Equation();

   // привязываем магнитопровод к дросселю
   _Magn->add_D(this, 1, 0, 1000);*/

   magnetic_link(1,3,2);
   //*R[3] = 500; *R[4]=500;
   *L[4] = 2.257;
   *R[4] = 709;
}
//---------------------------------------------------------------------------
tstA::~tstA(){
}
//---------------------------------------------------------------------------
// обновляем информацию об источника тока и ЗДС в ветвях
bool tstA::update_sources(bool force_update){
   *E[2] = sin(100*M_PI*TIME);
	return true;
   }
bool tstB::update_sources(bool force_update){ return false;}
bool tstA::update_elements(bool force_update){
   // *C[1] = 6e-4 + 0.003*exp(-20*pow(fabs(*U_C[1]),4))
   //Magn->calc();
   return false;
}
bool tstB::update_elements(bool force_update){ return false;}

//---------------------------------------------------------------------------

tstB::tstB():PUnit(5, 4, 2),X(2){
	char _M[5][5] = {{-1, 0, 0,-1,-1},
                      {+1,-1, 0, 0, 0},
                      { 0,+1,-1, 0,+1},
                      { 0, 0,+1,+1, 0}};
   char _N[2][5] = {{+1,+1, 0, 0,-1},
                      { 0, 0,+1,-1,+1}};
	M->operator =(&_M[0][0]);
   N->operator =(&_N[0][0]);
   // должно быть задано из вне
   _U_L[0]=0;_U_L[1]=0; _U_L[2]=0; _U_L[3]=0; _U_L[4]=0;
   _U_C[0]=0;_U_C[1]=0; _U_C[2]=0; _U_C[4]=0; _U_C[5]=0;
   _R[0]=2; _R[1]=1; _R[2]=2; _R[3]=0; _R[4]=2;
   _L[0]=0; _L[1]=0; _L[2]=0.002; _L[3]=0; _L[4]=0;
   _C[0]=0; _C[1]=0; _C[2]=0; _C[3]=0.0001; _C[4]=0;
   _E[0]=0; _E[1]=1; _E[2]=0; _E[3]=1; _E[4]=0;
   _J[0]=0; _J[1]=0; _J[2]=0; _J[3]=0;

   // расчет матрицы Z
	for(unsigned i=0; i<v; i++)
   	if(*C[i])
      	*Z[i] = *R[i] + *L[i]/smp + smp/(*C[i]);
      else
      	*Z[i] = *R[i] + *L[i]/smp;
   // установка выводов
   Out.add(&X);  // указатели на группы элементов
   X[0] = set_pin(0);
   X[1] = set_pin(3);
}
//---------------------------------------------------------------------------
void __fastcall tstB::SetR(PType value){  // установка активных сопротивлений на фазу
}
void __fastcall tstB::SetL(PType value){  // установка индуктивности фазы
}
void __fastcall tstB::SetC(PType value){  // установка умкости фазы
}
void __fastcall tstB::SetGz(PType value){ // установка активных проводимостей на землю
}
void __fastcall tstB::SetCz(PType value){ // установка емкостей на землю
}
void __fastcall tstB::SetGm(PType value){ // установка активных междуфазных проводимостей
}
void __fastcall tstB::SetCm(PType value){ // установка междуфазных емкостей
}
void __fastcall tstB::SetLm(PType value){ // установка междуфазных взаимоиндуктивностей
}
bool tstB::Mutual_Inductance(PType M_ab, PType M_bc, PType M_ca){// взаимные индуктивности между определенными фазами
   return true;
}
bool tstB::Mutual_Conductance(unsigned Phase1, unsigned Phase2, PType Gm, PType Cm){ // проводимости между определенными фазами
   return true;
}
bool tstB::Phase_Params(Phase phase, PType __R, PType __L, PType __C, PType Gz, PType Cz){return true;};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------











/*  Хранит информацию для рассчета ЭДС указанной функцией */
//---------------------------------------------------------------------------
void Source_Params::Change(String  description){	// добавим значение
   Field.clear();
	// разделяем строку по :
   PType t=0;
   String param = description;
   int pos = param.Pos(":");
   if(pos!=0){
      Type = param.SubString(1,pos-1);
      param = param.SubString(pos+1,param.Length()-pos+1);
      pos = param.Pos(":");
      while(pos!=0){
   	   t=0; TryStrToFloat(param.SubString(1,pos-1), t);
         Field.push_back(t);
         param = param.SubString(pos+1,param.Length()-pos+1);
         pos = param.Pos(":");
   	   }
      t=0;TryStrToFloat(param, t);
      Field.push_back(t);
      }
   else
      Type = param;
}
//---------------------------------------------------------------------------
String Source_Params::Get(){
      	String desc = Type;
      	for(int j=0, m=Field.size(); j<m;j++)
         	desc += ":" + FloatToStr(Field[j]);
         return desc;
}
//---------------------------------------------------------------------------
void Source_Params::Save(ostream &stream){			// сохранить  объект
   if(Type != ""){
      String param = Get();
	   stream << StringForSave( &param ) << " ";
      }
   else
      stream << "no" << " ";
}
//---------------------------------------------------------------------------
void Source_Params::Read(istream &stream){			// прочитать  объект
   char tmp[2048];
   String param;
   stream >> tmp;							// загрузили
   param = tmp;
   if( param!="no" ) Change(param);
}
//---------------------------------------------------------------------------
// функция рассчета ЭДС по заданным параметрам из sourceParams
PType _Ea(PElement* obj, double &time){
	// найдем параметры рассчета ЭДС
	Source_Params *sp = &((PSource*)obj)->sourceParams;
   if(sp->Type == "неизменно")
   	return sp->Field[0];
	else if(sp->Type == "sin"){
   	PType w = 2*M_PI*sp->Field[0];
      return sqrt(2)/sqrt(3)*sp->Field[1]*sin( w*time );
      }
   else if (sp->Type == "нессиметрия"){
      PType w = 2*M_PI*sp->Field[0];
      return sqrt(2)/sqrt(3)*( 	sp->Field[1]*sin( w*time ) +
      						         sp->Field[2]*sin( w*time ) +
                                 sp->Field[3]*sin( w*time ) );
      }
   else
   	return 0;
}
//---------------------------------------------------------------------------
PType _Eb(PElement* obj, double &time){
	// найдем параметры рассчета ЭДС
	Source_Params *sp = &((PSource*)obj)->sourceParams;
   if(sp->Type == "неизменно")
   	return sp->Field[0];
	else if(sp->Type == "sin"){
   	PType w = 2*M_PI*sp->Field[0];
      return sqrt(2)/sqrt(3)*sp->Field[1]*sin( w*time - 2*M_PI/3);
      }
   else if (sp->Type == "нессиметрия"){
      PType w = 2*M_PI*sp->Field[0];
      return sqrt(2)/sqrt(3)*( 	sp->Field[1]*sin( w*time - 2*M_PI/3) +
      						         sp->Field[2]*sin( w*time + 2*M_PI/3) +
                                 sp->Field[3]*sin( w*time ) );
      }
   else
   	return 0;
}
//---------------------------------------------------------------------------
PType _Ec(PElement* obj, double &time){
	// найдем параметры рассчета ЭДС
	Source_Params *sp = &((PSource*)obj)->sourceParams;
   if(sp->Type == "неизменно")
   	return sp->Field[0];
	else if(sp->Type == "sin"){
   	PType w = 2*M_PI*sp->Field[0];
      return sqrt(2)/sqrt(3)*sp->Field[1]*sin( w*time + 2*M_PI/3);
      }
   else if (sp->Type == "нессиметрия"){
      PType w = 2*M_PI*sp->Field[0];
      return sqrt(2)/sqrt(3)*( 	sp->Field[1]*sin( w*time + 2*M_PI/3) +
      						         sp->Field[2]*sin( w*time - 2*M_PI/3) +
                                 sp->Field[3]*sin( w*time ) );
      }
   else
   	return 0;
}








// идеальный источник бесконечной мощности (глухозаземленный)
PSource::PSource():
   PUnit(16,5,12),
   X(4){
   Ea=_Ea;  // функции по умолчанию для рассчета ЭДС
   Eb=_Eb;
   Ec=_Ec;
   set_pins();          // обозначение выводов от источника
   Describe_Fields();   // описываем свойства класса
	// заполняем матрицы
   char _M[5][16] =  {{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1},
   	                { 0, 1, 0, 0,-1, 0, 1,-1, 0, 1,-1, 0, 0,-1, 0, 0},
                      { 0, 0, 1, 0, 1,-1, 0, 1,-1, 0, 0,-1, 0, 0,-1, 0},
                      { 0, 0, 0, 1, 0, 1,-1, 0, 1,-1, 0, 0,-1, 0, 0,-1},
                      {-1,-1,-1,-1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};
	*M = (char*)_M;
   char _N[12][16] = {{ 0, 0, 0, 0, 1, 0, 0, 0, 0, 0,-1, 1, 0, 0, 0, 0},
   						 { 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0,-1, 1, 0, 0, 0},
                      { 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,-1, 0, 1, 0, 0, 0},
                      { 0, 0, 0, 0, 1, 0, 0,-1, 0, 0, 0, 0, 0, 0, 0, 0},
                      { 0, 0, 0, 0, 0, 1, 0, 0,-1, 0, 0, 0, 0, 0, 0, 0},
                      { 0, 0, 0, 0, 0, 0, 1, 0, 0,-1, 0, 0, 0, 0, 0, 0},
                      { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0,-1, 0, 0},
                      { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0,-1, 0},
                      { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0,-1},
                      {-1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0},
                      {-1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
                      {-1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0}};
   *N = (char*)_N;
   Name = "G";  // имя объекта
   Grounded_Node = 0;   // заземляемый узел
   // исходные  параметры
   for(int i=_N_;i<=_C_;i++){
      R_[i] = MIN;
      L_[i] = 0 ;
      C_[i] = 0;
      Cz_[i] = 0 ;
      Gz_[i] = 1/MAX;
      }
   for(int i=_AB_;i<=_CA_;i++){
      Cm_[i] = 0;
      Gm_[i] = 1/MAX;
      M_ [i] = 0 ;
      }
   *R[0] = MIN;   // нейтраль заземлена
   /*/ большие междуфазные сопротивления
   *R[4] = *R[5] = *R[6] = *R[7] = *R[8] = *R[9] = *R[10] = *R[11] = *R[12] = *R[13] = *R[14] = *R[15] = MAX;
   // минимальные сопротивления G (необходимы для возможности записи 2 закона Кирхгофа)
   *R[0] = *R[1] = *R[2] = *R[3] = MIN;*/
}
//---------------------------------------------------------------------------
bool PSource::update_sources(bool force_update){
   if(Ea) *E[1] = (*Ea)(this, TIME);
   else *E[1]=0;
   if(Eb) *E[2] = (*Eb)(this, TIME);
   else *E[2]=0;
   if(Ec) *E[3] = (*Ec)(this, TIME);
   else *E[3]=0;
   return true;
}
//---------------------------------------------------------------------------
/* Установка человеко-понятных выводов от устройства */
void PSource::set_pins(){
   Out.add(&X);  // указатели на группы элементов
   X.Name = "X";
   X[ _N_ ] = set_pin(0);
   X[ _A_ ] = set_pin(1);
   X[ _B_ ] = set_pin(2);
   X[ _C_ ] = set_pin(3);
}
//---------------------------------------------------------------------------
void PSource::Describe_Fields(){
   PFieldInfo fi;
   String name;
   // активные сопротивления
   for(int i=_N_; i<=_C_; i++){
      if(i==_N_) name = "N";
      else       name = (String)((char)('A'+i-_A_));
      fi.Name = "R_[_" + name + "_]";
      fi.Type = "PType";
      fi.ptr = (void*) &R_[i];
      Fields.push_back(fi);
      }
   // индуктивности
   for(int i=_N_; i<=_C_; i++){
      if(i==_N_) name = "N";
      else       name = (String)((char)('A'+i-_A_));
      fi.Name = "L_[_" + name + "_]";
      fi.Type = "PType";
      fi.ptr = (void*) &L_[i];
      Fields.push_back(fi);
      }
   // емкости
   for(int i=_N_; i<=_C_; i++){
      if(i==_N_) name = "N";
      else       name = (String)((char)('A'+i-_A_));
      fi.Name = "C_[_" + name + "_]";
      fi.Type = "PType";
      fi.ptr = (void*) &C_[i];
      Fields.push_back(fi);
      }
   // взаимная индуктивность
   for(int i=0; i<3; i++){
      switch(i){
         case _AB_: name = "AB"; break;
         case _BC_: name = "ВС"; break;
         case _CA_: name = "СА"; break;
         default:   name = "";
      }
      fi.Name = "M_[_" + name + "_]";
      fi.Type = "PType";
      fi.ptr = (void*) &M_[i];
      Fields.push_back(fi);
      }
   // активная проводимость на землю
   for(int i=_A_; i<=_C_; i++){
      fi.Name = "Gz_[_" + (String)((char)('A'+i-_A_)) + "_]";
      fi.Type = "PType";
      fi.ptr = (void*) &Gz_[i];
      Fields.push_back(fi);
      }
   // емкостная проводимость на землю
   for(int i=_A_; i<=_C_; i++){
      fi.Name = "Cz_[_" + (String)((char)('A'+i-_A_)) + "_]";
      fi.Type = "PType";
      fi.ptr = (void*) &Cz_[i];
      Fields.push_back(fi);
      }
   // активная междуфазная проводимость
   for(int i=0; i<3; i++){
      switch(i){
         case _AB_: name = "AB"; break;
         case _BC_: name = "ВС"; break;
         case _CA_: name = "СА"; break;
         default:   name = "";
      }
      fi.Name = "Gm_[_"+ name +"_]";
      fi.Type = "PType";
      fi.ptr = (void*) &Gm_[i];
      Fields.push_back(fi);
      }
   // емкостная междуфазная проводимость
   for(int i=0; i<3; i++){
      switch(i){
         case _AB_: name = "AB"; break;
         case _BC_: name = "ВС"; break;
         case _CA_: name = "СА"; break;
         default:   name = "";
      }
      fi.Name = "Cm_[_"+ name +"_]";
      fi.Type = "PType";
      fi.ptr = (void*) &Cm_[i];
      Fields.push_back(fi);
      }
}
/*/---------------------------------------------------------------------------
void PSource::R_(PType value){  // установка активных сопротивлений на фазу
   *R[1] = value;
   *R[2] = value;
   *R[3] = value;
}
void PSource::L_(PType value){  // установка индуктивности фазы
   *L[1] = value;
   *L[2] = value;
   *L[3] = value;
}
void PSource::C_(PType value){  // установка умкости фазы
   *C[1] = value;
   *C[2] = value;
   *C[3] = value;
}
void PSource::Gz_(PType value){ // установка активных проводимостей на землю
   *R[10] = 1/value;
   *R[11] = 1/value;
   *R[12] = 1/value;
}
void PSource::Cz_(PType value){ // установка емкостей на землю
   *C[13] = value;
   *C[14] = value;
   *C[15] = value;
   // обнуляем активное сопротивление в этих ветвях
   if(value!=0)
      *R[13] = *R[14] = *R[15] = 0;
   else
      *R[13] = *R[14] = *R[15] = MAX;
}
void PSource::Gm_(PType value){ // установка активных междуфазных проводимостей
   *R[4] = 1/value;
   *R[5] = 1/value;
   *R[6] = 1/value;
}
void PSource::Cm_(PType value){ // установка междуфазных емкостей
   *C[7] = value;
   *C[8] = value;
   *C[9] = value;
   // обнуляем активное сопротивление в этих ветвях
   if(value!=0)
      *R[7] = *R[8] = *R[9] = 0;
   else
      *R[7] = *R[8] = *R[9] = MAX;
}
void PSource::Lm_(PType value){ // установка междуфазных взаимоиндуктивностей
   mutual_inductance(1, 2, value);
   mutual_inductance(1, 3, value);
   mutual_inductance(2, 3, value);
}*/
bool PSource::Mutual_Inductance(PType M_ab, PType M_bc, PType M_ca){// взаимные индуктивности между определенными фазами
   mutual_inductance(1, 2, M_ab);
   mutual_inductance(2, 3, M_bc);
   mutual_inductance(1, 3, M_ca);
   return true;
}
bool PSource::Mutual_Conductance(unsigned Phase1, unsigned Phase2, PType Gm, PType Cm){ // проводимости между определенными фазами
   PType Rm;	// междуфазные сопротивления
   if(Gm) Rm = 1/Gm;
   else   Rm = MAX;
   // определяем фазу
   unsigned p;
   switch (Phase1 + Phase2){
   	case (_A_ + _B_): p=0; break;
      case (_B_ + _C_): p=1; break;
      case (_C_ + _A_): p=2; break;
      default:	 return false;
      }
   // установка значений
   *R[4+p] = Rm;  *L[4+p] = 0;   *C[4+p] = 0;
   if(Cm) {*C[7+p] = Cm; *R[7+p] = 0;  *L[7+p] = 0;}
   else   {*C[7+p] = Cm; *R[7+p] =MAX; *L[7+p] = 0;}
   return true;
}
bool PSource::Phase_Params(Phase phase, PType __R, PType __L, PType __C, PType Gz, PType Cz){ // параметры на 1 фазу
   // определяем фазу
   unsigned p;
   switch (phase){
   	case _A_: p=0; break;
      case _B_: p=1; break;
      case _C_: p=2; break;
      default:	 return false;
      }
   // установка значений
   *R[1+p] = __R;
   *L[1+p] = __L;
   *C[1+p] = __C;
   if(Gz) *R[10+p] = 1/Gz;
   else   *R[10+p] = MAX;
   if(Cz) {*C[13+p] = Cz;  *R[13+p] = 0;  *L[13+p] = 0;}
   else   {*C[13+p] = Cz;  *R[13+p] =MAX; *L[13+p] = 0;}
   return true;
}
//---------------------------------------------------------------------------
bool PSource::accept(){
   bool ret = true;
   // фазные параметры
   for(int i=_A_; i<=_C_; i++)
      ret = ret & Phase_Params((Phase) i, R_[i], L_[i], C_[i], Gz_[i], Cz_[i]);
   // параметры нейтрали
   *R[0] = R_[_N_];
   *L[0] = L_[_N_];
   *C[0] = C_[_N_];
   // междуфазные индуктивности
   ret = ret & Mutual_Inductance(M_[_AB_], M_[_BC_], M_[_CA_]);
   // междуфазные проводимости
   ret = ret & Mutual_Conductance(_A_, _B_, Gm_[_AB_], Cm_[_AB_]);
   ret = ret & Mutual_Conductance(_B_, _C_, Gm_[_BC_], Cm_[_BC_]);
   ret = ret & Mutual_Conductance(_C_, _A_, Gm_[_CA_], Cm_[_CA_]);
   update_differential_resistance();
   return ret;
}
//---------------------------------------------------------------------------
void PSource::Save(ostream &stream){		// сохранение параметров объекта в поток
   // сначала сохранится базовый класс
   sire::Save(stream);
   // теперь сохраняем этот класс
   for(int i=0; i<4; i++){
      stream << R_[i] << " ";
      stream << L_[i] << " ";
      stream << C_[i] << " ";
      stream << Gz_[i] << " ";
      stream << Cz_[i] << " ";
   	}
   for(int i=0; i<3; i++){
      stream << M_[i] << " ";
      stream << Gm_[i] << " ";
      stream << Cm_[i] << " ";
   	}
   // сохраним параметры источников
   sourceParams.Save( stream );
}
//---------------------------------------------------------------------------
void PSource::Read(istream &stream){    // загрузка параметров объекта из потока
   // сначала читает данные базовый класс
   sire::Read(stream);
   // теперь читаем этот класс
   for(int i=0; i<4; i++){
      stream >> R_[i];
      stream >> L_[i];
      stream >> C_[i];
      stream >> Gz_[i];
      stream >> Cz_[i];
   	}
   for(int i=0; i<3; i++){
      stream >> M_[i];
      stream >> Gm_[i];
      stream >> Cm_[i];
   	}
   // читаем параметры источников
   sourceParams.Read( stream );
}
//---------------------------------------------------------------------------
String PSource::ClassName(){return "PSource";};				// возвращает имя класса
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------













// Потребитель (глухозаземленный по умолчанию)
PConsumer::PConsumer(){
   Name = "Нагр";  // имя объекта
   Grounded_Node = 0;   // заземляемый узел
   // исходные  параметры
   for(int i=_N_;i<=_C_;i++){
      R_[i] = MAX;            // по умолчанию нагрузка не потребляет
      L_[i] = 0 ;
      C_[i] = 0 ;
      Cz_[i] = 0 ;
      Gz_[i] = 1/MAX;
      }
   for(int i=_AB_;i<=_CA_;i++){
      Cm_[i] = 0;
      Gm_[i] = 1/MAX;
      M_[i] = 0 ;
      }
   *R[0] = MIN;   // нейтраль заземлена
   /*/ большие междуфазные сопротивления
   *R[4] = *R[5] = *R[6] = *R[7] = *R[8] = *R[9] = *R[10] = *R[11] = *R[12] = *R[13] = *R[14] = *R[15] = MAX;
   // нагрузка заземлена (!=0 для записи 2 закона Кирхгофа для данной ветви)
   *R[0] = MIN;
   // по умолчанию нагрузка ничего не потребляет
   *R[1] = *R[2] = *R[3] = MAX;*/
   // принимаем характеристики
   accept();
}
/*/---------------------------------------------------------------------------
bool PConsumer::update_sources(bool force_update){
   if(Ea) *E[1] = (*Ea)(TIME);
   else *E[1]=0;
   if(Eb) *E[2] = (*Eb)(TIME);
   else *E[2]=0;
   if(Ec) *E[3] = (*Ec)(TIME);
   else *E[3]=0;
   return true;
}*/
//---------------------------------------------------------------------------
/* Установка человеко-понятных выводов от устройства /
void PConsumer::set_pins(){
   Out.add(&X);  // указатели на группы элементов
   X.Name = "X";
   X[ _N_ ] = set_pin(0); //, 0);
   X[ _A_ ] = set_pin(1); //, 1);
   X[ _B_ ] = set_pin(2); //, 2);
   X[ _C_ ] = set_pin(3); //, 3);
}
//---------------------------------------------------------------------------
void __fastcall PConsumer::SetR(PType value){  // установка активных сопротивлений на фазу
   *R[1] = value;
   *R[2] = value;
   *R[3] = value;
}
void __fastcall PConsumer::SetL(PType value){  // установка индуктивности фазы
   *L[1] = value;
   *L[2] = value;
   *L[3] = value;
}
void __fastcall PConsumer::SetC(PType value){  // установка умкости фазы
   *C[1] = value;
   *C[2] = value;
   *C[3] = value;
}
void __fastcall PConsumer::SetGz(PType value){ // установка активных проводимостей на землю
   *R[10] = 1/value;
   *R[11] = 1/value;
   *R[12] = 1/value;
}
void __fastcall PConsumer::SetCz(PType value){ // установка емкостей на землю
   *C[13] = value;
   *C[14] = value;
   *C[15] = value;
   // обнуляем активное сопротивление в этих ветвях
   *R[13] = *R[14] = *R[15] = 0;
}
void __fastcall PConsumer::SetGm(PType value){ // установка активных междуфазных проводимостей
   *R[4] = 1/value;
   *R[5] = 1/value;
   *R[6] = 1/value;
}
void __fastcall PConsumer::SetCm(PType value){ // установка междуфазных емкостей
   *C[7] = value;
   *C[8] = value;
   *C[9] = value;
   // обнуляем активное сопротивление в этих ветвях
   *R[7] = *R[8] = *R[9] = 0;
}
void __fastcall PConsumer::SetLm(PType value){ // установка междуфазных взаимоиндуктивностей
   mutual_inductance(1, 2, value);
   mutual_inductance(1, 3, value);
   mutual_inductance(2, 3, value);
}
bool PConsumer::Mutual_Inductance(PType M_ab, PType M_bc, PType M_ca){// взаимные индуктивности между определенными фазами
   mutual_inductance(1, 2, M_ab);
   mutual_inductance(2, 3, M_bc);
   mutual_inductance(3, 1, M_ca);
   return true;
}
bool PConsumer::Mutual_Conductance(unsigned Phase1, unsigned Phase2, PType Gm, PType Cm){ // проводимости между определенными фазами
   PType Rm = 1/Gm;	// междуфазные сопротивления
   // определяем фазу
   unsigned p=0;
   switch (Phase1 + Phase2){
   	case (_A_ + _B_): p=0; break;
      case (_B_ + _C_): p=1; break;
      case (_C_ + _A_): p=2; break;
      default:	 return false;
      }
   // установка значений
   *R[4+p] = Rm;
   *C[7+p] = Cm;
   return true;
}
bool PConsumer::Phase_Params(Phase phase, PType __R, PType __L, PType __C, PType Gz, PType Cz){ // параметры на 1 фазу
   // определяем фазу
   unsigned p=0;
   switch (phase){
   	case _A_: p=0; break;
      case _B_: p=1; break;
      case _C_: p=2; break;
      default:	 return false;
      }
   // установка значений
   *R[1+p] = __R;
   *L[1+p] = __L;
   *C[1+p] = __C;
   *R[10+p] = 1/Gz;
   *C[13+p] = Cz;
   return true;
}*/
//---------------------------------------------------------------------------
/* определение параметров схемы замещения линии по данным нагрузки */
bool PConsumer::Load(const PType P,const PType Q, const PType Unom, bool Grounding, const PType freq ){
   PType P_1 = P/3;
   PType r = Unom*Unom/P_1;
   PType Q_1 = Q/3;
   PType l=0, c=0;
   if(Q_1>0) l = Unom*Unom/(Q_1*2*M_PI*freq);
   else c = Q_1/(Unom*Unom*2*M_PI*freq);
   PType m=0;
   if(!Grounding) // потребительль с изолированной нейтралью
      *R[0] = MAX;
   else
      *R[0] = MIN;
   // установка значений
   for(int i=_A_;i<=_C_;i++){
      R_[i] = r;            // по умолчанию нагрузка не потребляет
      L_[i] = l ;
      C_[i] = c ;
      M_[i] = m ;
      Cz_[i] = 0 ;
      Gz_[i] = 1/MAX;
      }
   for(int i=_AB_;i<=_CA_;i++){
      Cm_[i] = 0;
      Gm_[i] = 1/MAX;
      }
   /*
   R_( r );
   L_( l );
   C_( c );
   Lm_( m );
   Gm_( 1/MAX );
   Cm_( 0 );
   Gz_( 1/MAX );
   Cz_( 0 ); */
   return true;
}
//---------------------------------------------------------------------------
String PConsumer::ClassName(){return "PConsumer";};				// возвращает имя класса
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------















// трансформатор однофазные без учета цепи намагничивания
PSingleTransformer ::PSingleTransformer():
					PUnit(6, 6, 1),
               X1(2), X2(2){	// указаны 4, но используются только с индексами 1-3, Х1[0] == X1[_N_] не используется
   set_pins();
  	// заполняем матрицы
   char _M[6][6] =   {{-1, 0, 0,-1, 0, 0},
                      { 1, 0, 0, 0, 0, 0},
                      { 0,-1, 0, 0,-1, 0},
                      { 0, 1, 0, 0, 0, 0},
                      { 0, 0, 1, 1, 1, 1},
                      { 0, 0,-1, 0, 0,-1}};
	*M = (char*)_M;
   char _N[1][6] =    {{0, 0,-1, 0, 0, 1}};
   *N = (char*)_N;
   /* зададим трансф с коэф трансформации 1 */
   _U1 = 1;
   _U2 = 1;
   _S  = 1;
   _dPk= 0;
   _Uk = 10.5;
   _dPx= 0;
   _Ix = 0.01;
   _freq = 50;
   // сопротвление утечки обмоток
   *R[3] = *R[4] = MAX;
   Name = "ОТр";  // имя объекта
   Grounded_Node = 4;   // заземляемый узел
}
//---------------------------------------------------------------------------
void PSingleTransformer :: set_pins(){
   X1 [_N_] = set_pin(1);
   X1 [_A_] = set_pin(0);
   X2 [_N_] = set_pin(3);
   X2 [_A_] = set_pin(2);
   // имена
   X1.Name  = "X1";
   X2.Name  = "X2";
	// добавим в список групп выводов
	Out.add(&X1);
   Out.add(&X2);
}
//---------------------------------------------------------------------------
void PSingleTransformer :: Params(PType U1,  PType U2, PType S,  // установка параметров
      	                         PType dPk, PType Uk,
                                  PType dPx, PType Ix,
                                  PType freq){
   // Примем
   PType Km = 1;				  // коэффициент магнитной связи
   _Iu = 1e3;               // , А выбираем ток в цепи намагничивания (магнитопроводе)
                             // заметил: чем больше Iu тем меньше погрешность 2 зак. Кирхгофа для цепи намагничивания
   // запомним
	_U1 = U1;
   _U2 = U2;
   _S  = S;
   _dPk= dPk;
   _Uk = Uk;
   _dPx= dPx;
   _Ix = Ix;
   _freq = freq;

   // козффициенты
   PType k = U1/U2;        // коэффициент трансформации
   PType w = 2*freq*M_PI;  // угловая частота

   // пошли рассчитывать
   // активное сопротивление обмоток
   PType R_sum = dPk * pow(U1,2) / pow(S,2);
   PType R1 = 0.5 * R_sum;
   PType R2 = R1 / pow(k,2);
   // величина магнитной связи (при учете приближенного равенства ЭДСxx = Uxx)
   PType Xm1 = U1 / _Iu ;
   PType Xm2 = U2 / _Iu ;
   // ток холостого хода
   PType __Ix = Ix/100 * S/U1;   // в Амперах
   //PType Ru = (dPx - R1 * pow(__Ix,2)) / pow(_Iu,2);
   // реактивные потери холостого хода
   PType dQ2 = pow(__Ix*U1,2) - pow(dPx,2);
   if(dQ2<0){
      err(0, "PSingleTransformer::Params(): параметры трансформатора заданы не верно.Слишком маленький ток х.х. для заданных активн. потерь х.х. Установлено новое значение тока х.х.");
      __Ix = dPx/U1;
      _Ix = __Ix / (S/U1) * 100;
      //Ix = _Ix;
      dQ2 = 0;
      }
   PType dQx = sqrt( dQ2 );
   PType q_xx;	// угол между током и напряжением  холостого хода
   if(dPx)
      q_xx = atan(dQx/dPx);
   else
      q_xx = M_PI/2;
   // активное сопротивление цепи намагничивания (учет потерь)
   PType Ru = __Ix * Xm1 * cos(q_xx)/_Iu;
   PType X_pot = __Ix * Xm1 * sin(q_xx)/_Iu;
   // реактивное сопротивление цепи намагничивания (учет потерь)
   //PType X_pot  = (dQx - X1 * pow(__Ix,2) - 2*Xm1*__Ix*_Iu*sin(atan(dQx/dPx))) / pow(_Iu,2);	// определяет реактивные потери в магнитопроводе
   // реактивное сопротивление обмоток
   PType Z_sum =  Uk/100 * pow(U1,2) / S;
   PType X_sum2 = pow(Z_sum,2) - pow(R_sum,2);
   if(X_sum2<0){
      err(0, "PSingleTransformer::Params(): параметры трансформатора заданы не верно.Слишком маленький ток Uk для заданных активн. потерь к.з. Установлено новое значение Uk");
      _Uk = R_sum * S / pow(U1,2) * 100;
      //Uk = _Uk;
      //Z_sum =  _Uk/100 * pow(U1,2) / S;
      X_sum2 = 0;
      }
   PType X_sum =  sqrt( X_sum2 );
   PType X1 = 0.5*X_sum ;
   //PType X1 = 0.5 * X_sum + Xm1*(1+k)/2;  // хорош на холостой ход
   PType X2 = X1 / pow(k,2);
   // индуктивность в цепи намагничивания
   PType Xu1 = pow(Xm1/Km,2) / X1;    		// оно скомпенсируется емкостью
   PType Xu2 = pow(Xm2/Km,2) / X2;
   PType Xu  = Xu1 + Xu2;
   PType X_Cu = Xu + X_pot;  // необходимая емкость
   /*
   PType Z_sum =  Uk/100 * pow(U1,2) / S;
   PType X_sum2 = pow(Z_sum,2) - pow(R_sum,2);
   if(X_sum2<0){
      err(0, "PSingleTransformer::Params(): параметры трансформатора заданы не верно.Слишком маленький ток Uk для заданных активн. потерь к.з. Установлено новое значение Uk");
      _Uk = R_sum * S / pow(U1,2) * 100;
      Uk = _Uk;
      Z_sum =  _Uk/100 * pow(U1,2) / S;
      X_sum2 = 0;
      }
   PType X_sum =  sqrt( X_sum2 );
   PType Zxx = U1 / __Ix;

   Km = sqrt (  (2*Zxx - X_sum)/(2*Zxx - 0.5*X_sum)  );

   PType X1 = Zxx / (1 - 0.5*pow(Km,2));
   Xm1 = (k+1)*(X1 - X_sum/2); // на сколько домножим X1 во столько раз ток Iu будет меньше

   PType Xu = 2 * pow(Xm1,2)/pow(Km,2)/X1 ;    // 2*X1

   PType X2 = X1 / pow(k,2);
   Xm2 = Xm1 / k;

   _Iu = U1 / Xm1;
   PType Ru = dPx / pow(_Iu,2);

   *R[0] = R1; *L[0] = X1/w;
   *R[1] = R2; *L[1] = X2/w;
	*R[2] = Ru/2; *L[2] = Xu/w/2;
   *R[5] = Ru/2; *L[5] = Xu/w/2;
   mutual_inductance(0, 2, Xm1/w);
   mutual_inductance(1, 5, Xm2/w);
   */
   // устанавливаем параметры
   *R[0] = R1; *L[0] = X1/w;
   *R[1] = R2; *L[1] = X2/w;
   *R[2] = Ru; *L[2] = Xu/w;  *C[2] = 1/(X_Cu*w);
   *R[5] = 0;  *L[5] = 0;
   //_Ru_nom = *C[2];  // запоминаем номинальное сопротивление

   mutual_inductance(0, 2, Xm1/w);
   mutual_inductance(1, 2, Xm2/w);
   update_differential_resistance();
}
//---------------------------------------------------------------------------
String PSingleTransformer::ClassName(){return "PSingleTransformer";}				// возвращает имя класса








//---------------------------------------------------------------------------
PCurve :: PCurve(){
   Y_X = NULL;
   Symmetry = true;
   exist = false;
}
//---------------------------------------------------------------------------
void PCurve :: _set(PType X, PType Y){
   _X.push_back(X);
   _Y.push_back(Y);
   if(Y<0) Symmetry = false;
   if(size() > 0) exist = true; 
}
//---------------------------------------------------------------------------
void PCurve :: clear(){
   _X.clear();
   _Y.clear();
   if(Y_X == NULL) exist = false;  // аналитич кривая не задана, и ломаная удалена = График Не Задан
}
//---------------------------------------------------------------------------
unsigned PCurve :: size(){
   int n = _X.size()-1;
   return (n==-1)? 0 : n;
}
//---------------------------------------------------------------------------
void PCurve :: Point(unsigned Index, PType &X, PType &Y){
   if( Index < _X.size() && Index < _Y.size() ){
      X = _X[Index];
      Y = _Y[Index];
      }
}
//---------------------------------------------------------------------------
void PCurve :: func( PType (*Func)(PType&) ){
   Y_X = Func;
   if(Y_X != NULL)
      exist = true;
   else if( size() < 1 ) // и функция обнулена и ломоной кривой нет
      exist = false;
}
//---------------------------------------------------------------------------
PType __fastcall PCurve :: X(PType _Y_){
 if (Y_X==NULL){  // если не задана кривая
   int sgn = 1;
   if( Symmetry ){ // кривая симметрична относительно начала координат
      sgn = (_Y_>0)?1:-1;
      _Y_ = sgn*_Y_;
      }
   PType dY, dX;
   if(_Y.size() == 0) {
      err(0, "PCurve::X(): кривая не задана. вернули значение -1.");
      return -1;}
   int n=_Y.size(); // число участков
   int i;
   for(i=0;i<n;i++)
      if( _Y_ < _Y[i]) break;
   if (i==n || i==0){ // мы попали на последний участок
      err(0, "PCurve::X(): кривая не задана для искомого Y="+ FloatToStr(_Y_)+". вернули значение -1.");
      return -1;
      }
   else{
      i--;  // участок кривой намагничивания, на которой находится рабочая точка
      dY = _Y[i+1] - _Y[i]; // приращение на участке
      dX = _X[i+1] - _X[i]; // приращение на участке
      }
   // считая кривую симметричной относительно начала координат
   return sgn*(_X[i] + dX/dY * (_Y_-_Y[i]));
 }
 else{
 	// рассчет по аналитической кривой
   // по условия H(B) = 0
   PType _Ymin=0, _Ymax = 1e10;
   PType e=1e-3; 			// допустимые погрешности рассчета
   PType a = _Ymin, b = _Ymax, c = _Ymin/2 + _Ymax/2;
   PType Y_c;
   do{
   	Y_c = (*Y_X)(c);
   	if( (*Y_X)(a)<_Y_ &&  Y_c>_Y_){
         b = c;
         c = (a+b)/2;
      	}
      else if ( Y_c<_Y_ &&  (*Y_X)(b)>_Y_){
      	a = c;
         c = (a+b)/2;
      	}
      else{
         err(0,"PCurve :: X(): Не найдено значение X по Y="+FloatToStr(_Y_)+". Не верна аналитическая функция.");
         return c;
      	}
   }while(fabs(a - b) > e || fabs(Y_c - _Y_) > e);
   return c;	// возвращаем значение индукции
 }
}
//---------------------------------------------------------------------------
PType __fastcall PCurve :: Y(PType _X_){
 if(Y_X==NULL){    // если не задана кривая
   int sgn = 1;
   if( Symmetry ){ // кривая симметрична относительно начала координат
      sgn = (_X_>0)?1:-1;
      _X_ = sgn*_X_;
      }
   PType dY, dX;
   if(_Y.size() == 0) {
      err(0, "PCurve::Y(): кривая не задана. вернули значение -1.");
      return -1;}
   int n=_X.size(); // число участков
   int i;
   for(i=0;i<n;i++)
      if( _X_ < _X[i]) break;
   if (i==n || i==0){ // мы попали на последний участок
      err(0, "PCurve::Y(): кривая не задана для искомого X="+FloatToStr(_X_)+". вернули значение -1.");
      return -1;
      }
   else{
      i--;  // участок кривой намагничивания, на которой находится рабочая точка
      dY = _Y[i+1] - _Y[i]; // приращение на участке
      dX = _X[i+1] - _X[i]; // приращение на участке
      }
   // считая кривую симметричной относительно начала координат
   return sgn*(_Y[i] + dY/dX * (_X_-_X[i]));
 	}
 else{
 	// рассчет по аналитической кривой
   return (*Y_X)(_X_);
 }
}
//---------------------------------------------------------------------------
void PCurve::Save(ostream &stream){		// сохранение параметров объекта в поток
   /* кривую заданую функцией можно сохранить как набор точек
   и при загрузке получит ломанную кривую, но этот функционал не заложен */
   // сохраняем кривую намагничивания заданную ломанной
   stream << _X.size() << " ";
   for(int i=0, n=_X.size(); i<n; i++){
      stream << _X[i] << " ";
      stream << _Y[i] << " ";
      }
}
//---------------------------------------------------------------------------
void PCurve::Read(istream &stream){    // загрузка параметров объекта из потока
   // читаем кривую намагничивания заданную ломанной
   int n=0;
   stream >> n;
   PType X, Y;
   for(int i=0; i<n; i++){
      stream >> X;   _X.push_back(X);
      stream >> Y;   _Y.push_back(Y);
      }
   //  если ломоной кривая задана
   if( size() > 0 ) exist = true;
   //else             exist = false;  // не факт, т.к. может быть задана аналитическая функция - оставляем как есть 
}














//---------------------------------------------------------------------------
void MagnCurve :: set(PType F, PType I){
   _set(F,I);
}
PType __fastcall MagnCurve :: F(PType _I_){
   return X(_I_);
}
PType __fastcall MagnCurve :: I(PType _F_){
   return Y(_F_);
}
//---------------------------------------------------------------------------
void UICurve :: set(PType I, PType U){
   _set(I,U);
}
PType __fastcall UICurve :: I(PType _U_){
   return X(_U_);
}
PType __fastcall UICurve :: U(PType _I_){
   return Y(_I_);
}





//---------------------------------------------------------------------------
/* Базовый класс для силовых трансформаторов */
PTransformer::PTransformer(unsigned NumBranches, unsigned NumNodes, unsigned NumCircuit) :
					PUnit(NumBranches, NumNodes, NumCircuit){
   Describe_Fields();   // опишем свойства класса
   /* зададим трансф с коэф трансформации 1 */
   U1 = 1;
   U2 = 1;
   S  = 1;
   dPk= 0;
   Uk = 10.5;
   dPx= 0;
   Ix = MIN;
   freq = 50;
   F_nom = 0.01;
   Name = "Тр";  // имя объекта
}
//---------------------------------------------------------------------------
void PTransformer::Describe_Fields(){
   PFieldInfo fi;
   // мощность
   fi.Name = "Мощность";
   fi.Type = "PType";
   fi.ptr = (void*) &S;
   Fields.push_back(fi);
   // первичное напряжение
   fi.Name = "U1";
   fi.Type = "PType";
   fi.ptr = (void*) &U1;
   Fields.push_back(fi);
   // вторичное напряжение
   fi.Name = "U2";
   fi.Type = "PType";
   fi.ptr = (void*) &U2;
   Fields.push_back(fi);
   // Напряжение Кз
   fi.Name = "Напр. к.з.";
   fi.Type = "PType";
   fi.ptr = (void*) &Uk;
   Fields.push_back(fi);
   // Потери Кз
   fi.Name = "Акт.потери к.з.";
   fi.Type = "PType";
   fi.ptr = (void*) &dPk;
   Fields.push_back(fi);
   // Ток xx
   fi.Name = "Ток x.x.";
   fi.Type = "PType";
   fi.ptr = (void*) &Ix;
   Fields.push_back(fi);
   // Потери xx
   fi.Name = "Акт.потери x.x.";
   fi.Type = "PType";
   fi.ptr = (void*) &dPx;
   Fields.push_back(fi);
   // Номинальная частота
   fi.Name = "Ном.частота";
   fi.Type = "PType";
   fi.ptr = (void*) &freq;
   Fields.push_back(fi);
   // Номинальный поток
   fi.Name = "Ном.поток";
   fi.Type = "PType";
   fi.ptr = (void*) &F_nom;
   Fields.push_back(fi);
}
/*/---------------------------------------------------------------------------
PType PTransformer :: U1()  {return _U1;}                // чтение значения напряжения первичнйо обмотки
PType PTransformer :: U2()  {return _U2;}
PType PTransformer :: S()   {return _S;}
PType PTransformer :: dPk() {return _dPk;}
PType PTransformer :: Uk()  {return _Uk;}
PType PTransformer :: dPx() {return _dPx;}
PType PTransformer :: Ix()  {return _Ix;}
PType PTransformer :: freq(){return _freq;}
PType PTransformer :: F_nom(){return _F_nom;}
//---------------------------------------------------------------------------
void PTransformer  :: U1(PType value)  {_U1   = value;}      // установка напряжения
void PTransformer  :: U2(PType value)  {_U2   = value;}
void PTransformer  :: S(PType value)   {_S    = value;}
void PTransformer  :: dPk(PType value) {_dPk  = value;}
void PTransformer  :: Uk(PType value)  {_Uk   = value;}
void PTransformer  :: dPx(PType value) {_dPx  = value;}
void PTransformer  :: Ix(PType value)  {_Ix   = value;}
void PTransformer  :: freq(PType value){_freq = value;}
void PTransformer  :: F_nom(PType value){_F_nom= value;}  */
//---------------------------------------------------------------------------
void PTransformer::Save(ostream &stream){		// сохранение параметров объекта в поток
   // сначала сохранится базовый класс
   sire::Save(stream);
   // теперь сохраняем этот класс
   stream << n1 << " ";
   stream << n2 << " ";
   stream << U1 << " ";
   stream << U2 << " ";
   stream << S  << " ";
   stream << dPk<< " ";
   stream << Uk << " ";
   stream << dPx<< " ";
   stream << Ix << " ";
   stream <<freq<< " ";
   stream <<F_nom<< " ";
}
//---------------------------------------------------------------------------
void PTransformer::Read(istream &stream){    // загрузка параметров объекта из потока
   // сначала читает данные базовый класс
   sire::Read(stream);
   // теперь читаем этот класс
   stream >> n1;
   stream >> n2 ;
   stream >> U1 ;
   stream >> U2 ;
   stream >> S  ;
   stream >> dPk;
   stream >> Uk ;
   stream >> dPx;
   stream >> Ix ;
   stream >>freq;
   stream >>F_nom;
}
//---------------------------------------------------------------------------
String PTransformer::ClassName(){return "PTransformer";};				// возвращает имя класса









PTransformer_1::PTransformer_1() :
					PTransformer(5, 5, 1),
               X1(2), X2(2){	// указаны 4, но используются только с индексами 1-3, Х1[0] == X1[_N_] не используется
   set_pins();          // установим выводы
   Describe_Fields();   // опишем поля
  	// заполняем матрицы
   char _M[5][5] =   {{-1, 0, 0,-1, 0},
                      { 1, 0, 0, 0, 0},
                      { 0,-1, 0, 0,-1},
                      { 0, 1, 0, 0, 0},
                      { 0, 0, 0, 1, 1}};
	*M = (char*)_M;
   char _N[1][5] =    {{0, 0, 1, 0, 0}};
   *N = (char*)_N;
   //mCurve.set(0,0);
   //mCurve.set(1e3,1e6);  // линейная кривая намагничивания с насыщением при грамандой индукции
   //_Bnom = 1.2; 		// установка параметров трансформатора для данной номинальной индукции
   //Params(_U1,_U2,_S,_dPk,_Uk,_dPx,_Ix,_freq);
   // сопротвление утечки обмоток
   *R[3] = *R[4] = MAX;
   // рассчитаем параметры схемы на основе установленных данных
   Name = "ОТр";  // имя объекта
   Grounded_Node = 4;   // заземляемый узел
}
//---------------------------------------------------------------------------
void PTransformer_1 :: set_pins(){
   X1 [_N_] = set_pin(1);
   X1 [_A_] = set_pin(0);
   X2 [_N_] = set_pin(3);
   X2 [_A_] = set_pin(2);
   // имена
   X1.Name  = "X1";
   X2.Name  = "X2";
	// добавим в список групп выводов
	Out.add(&X1);
   Out.add(&X2);
}
//---------------------------------------------------------------------------
void PTransformer_1::Describe_Fields(){
   PFieldInfo fi;
   // Кривая намагничивания
   fi.Name = "I(Ф)";
   fi.Type = "PCurve";
   fi.ptr = (void*) &mCurve;
   Fields.push_back(fi);
}
//---------------------------------------------------------------------------
bool PTransformer_1 :: accept(){
   if(Ix==0) {
   	err(0, "PTransformer_1: ток х.х. не может быть равным нулю");
      Ix=MIN;
      return false;
   	}
   // коэффициенты
   PType k = U1 / U2;        // действительный коэффициент трансформации
   PType w = 2*freq*M_PI;     // угловая частота

   // активное сопротивление обмоток
   PType R_sum = dPk * pow(U1,2) / pow(S,2);
   PType R1 = 0.5 * R_sum;
   PType R2 = R1 / pow(k,2);
   // ток холостого хода (в Амперах)
   PType __Ix = Ix/100 * S/U1;   // в Амперах
   // реактивные потери холостого хода
   PType dQ2 = pow(__Ix*U1,2) - pow(dPx,2);
   if(dQ2<0){
      err(0, "PTransformer_1::accept(): параметры трансформатора заданы не верно.Слишком маленький ток х.х. для заданных активн. потерь х.х.");
      return false;
      }
   PType dQx = sqrt( dQ2 );
   PType q_xx;	// угол между током и напряжением  холостого хода
   if(dPx) q_xx = atan(dQx/dPx);
   else    q_xx = M_PI/2;
   // число витков обмоток
   n1 = U1 / (w*F_nom);
   n2 = n1 / k;
   // сопротивление цепи намагничивания (учет потерь)
   PType Zu = __Ix * n1 / F_nom;
   PType Ru = Zu * sin(q_xx);
   PType Xu = Zu * cos(q_xx);
   // реактивное сопротивление обмоток
   PType Z_sum =  Uk/100 * pow(U1,2) / S;
   PType X_sum2 = pow(Z_sum,2) - pow(R_sum,2);
   if(X_sum2<0){
      err(0, "PTransformer_1::accept(): параметры трансформатора заданы не верно.Слишком маленький ток Uk для заданных активн. потерь к.з.");
      return false;
      }
   PType X_sum =  sqrt( X_sum2 );
   PType dX1 = X_sum / 2;   // сопротивление рассеяния первичной обмотки
   PType dX2 = dX1 / pow(k,2);      // сопротивление рассеяния вторичной обмотки */


   // устанавливаем параметры
   *R[0] = R1; *L[0] = dX1/w;
   *R[1] = R2; *L[1] = dX2/w;
   *R[2] = Ru; *L[2] = Xu/w;
   magnetic_link(0,2,n1);
   magnetic_link(1,2,n2);
   update_differential_resistance();   // пересчитываем дифференциальные сопротивления
   return true;
}
//---------------------------------------------------------------------------
bool PTransformer_1 :: update_elements(bool force_update){
   // обновляем параметры схемы в соответствии с кривой намагничивания
   if( !mCurve.exist )  // задана ли кривая намагничивания
      return false;
   else{
   	PType Ru;
      PType _F = *I[2];          	// значение потока
      if(!_F) return false;
      PType _Ip = mCurve.I(_F);   	// какой должена быть мгнов. реактивная составляющая тока холостого хода
      PType _U = *f[0] - *f[1];		// напряжение на обмотке
      PType dRxx = pow(U1,2)/dPx;	// коэф. пропорциональности между напряж на обмотке и акт сост тока хх.
      PType _Ia = _U / dRxx;			// мгновенное активно составл тока хх
      PType _I = _Ia + _Ip; 			// мгнов. значение тока хх

      static PType _F_1 = 0;
      PType d_F = (_F - _F_1)/smp;
      Ru = (n1*_I - *L[2]*d_F)/_F; // устанавливаем активное сопротивление магнитопровода, которое влияет на потери
      if(Ru<0)	Ru=0;						// сопротивл. не д.б. отриц. - это погрешность расчета
      _F_1 = _F;
      *R[2] = Ru;
      return true;
      }
}
//---------------------------------------------------------------------------
void PTransformer_1 :: R1(PType value){ *R[0] = value; }
void PTransformer_1 :: L1(PType value){ *L[0] = value; }
void PTransformer_1 :: R2(PType value){ *R[1] = value; }
void PTransformer_1 :: L2(PType value){ *L[1] = value; }
void PTransformer_1 :: Gz1(PType value){   *R[3] = 1/value; }
void PTransformer_1 :: Cz1(PType value){   *C[3] = value;   }
void PTransformer_1 :: Gz2(PType value){   *R[4] = 1/value; }
void PTransformer_1 :: Cz2(PType value){   *C[4] = value;   }
PType PTransformer_1 :: R1()   { return *R[0];      }
PType PTransformer_1 :: L1()   { return *L[0];      }
PType PTransformer_1 :: R2()   { return *R[1];      }
PType PTransformer_1 :: L2()   { return *L[1];      }
PType PTransformer_1 :: Gz1()  { return 1/(*R[3]);  }
PType PTransformer_1 :: Cz1()  { return *C[3];      }
PType PTransformer_1 :: Gz2()  { return 1/(*R[4]);  }
PType PTransformer_1 :: Cz2()  { return *C[4];      }
//---------------------------------------------------------------------------
void PTransformer_1::Save(ostream &stream){		// сохранение параметров объекта в поток
   // сначала сохранится базовый класс
   sire::Save(stream);
   // сохраняем кривую намагничивания
   mCurve.Save(stream);
}
//---------------------------------------------------------------------------
void PTransformer_1::Read(istream &stream){    // загрузка параметров объекта из потока
   // сначала читает данные базовый класс
   sire::Read(stream);
   // теперь читаем этот класс
   // читаем кривую намагничивания
   mCurve.Read(stream);
}
//---------------------------------------------------------------------------
String PTransformer_1::ClassName(){return "PTransformer_1";};				// возвращает имя класса
















// трансформатор (по умолчанию трехстрержневой)
PTransformer_3 ::PTransformer_3():
					PTransformer(23, 18, 6),
               X1(4), X2(4), X1n(4), X2n(4){	// указаны 4, но используются только с индексами 1-3, Х1[0] == X1[_N_] не используется
	set_pins();	         // установка выводов
   Describe_Fields();   // описываем поля
   	// заполняем матрицы
   char _M[18][23] = {{-1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-1, 0, 0, 0, 0, 0, 0, 0},
                      { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-1, 0},
                      { 0,-1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-1, 0, 0, 0, 0, 0, 0},
                      { 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-1},
                      { 0, 0,-1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-1, 0, 0, 0, 0, 0},
                      { 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                      { 0, 0, 0,-1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-1, 0, 0, 0, 0},
                      { 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                      { 0, 0, 0, 0,-1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-1, 0, 0, 0},
                      { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                      { 0, 0, 0, 0, 0,-1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-1, 0, 0},
                      { 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                      { 0, 0, 0, 0, 0, 0,-1, 0, 0,-1, 0,-1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                      { 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0,-1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                      { 0, 0, 0, 0, 0, 0, 0,-1, 0, 0, 0, 1, 0,-1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                      { 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0,-1, 0, 0, 0, 0, 0, 0, 0, 0},
                      { 0, 0, 0, 0, 0, 0, 0, 0,-1, 0,-1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                      { 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1}};

	*M = (char*)_M;
   char _N[6][23] =  {{ 0, 0, 0, 0, 0, 0, 1, 0, 0,-1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                      { 0, 0, 0, 0, 0, 0,-1, 1, 0, 0, 0, 1,-1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                      { 0, 0, 0, 0, 0, 0, 0,-1, 1, 0, 0, 0, 0, 1,-1, 0, 0, 0, 0, 0, 0, 0, 0},
                      { 0, 0, 0, 0, 0, 0, 0, 0,-1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                      { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-1, 0, 0, 0, 0, 0, 1, 0},   // сопротивление в нейтрали первой обмотки
                      { 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-1, 0, 0, 0, 0, 0, 1}};  // сопротивление в нейтрали второй обмотки
   *N = (char*)_N;
   mType = Armored;           // тип магнитопровода - броневой
   Winding_X1 = Winding_X2 = wStar;   // схема соединения обмоток - звезда
   // крайние стержни отсутствуют (имеют большое сопротивление)
   *R[9] = *R[10] = MAX;
	// стержни с обмотками и ярма имеют минимальное сопротивление
   *R[6] = *R[7] = *R[8] = *R[11] = *R[12] = *R[13] = *R[14] = MIN;
   // индуктивность стержней
   *L[6] = *L[7] = *L[8] = 0.63; // Генри
   // Индуктивность обмоток выводов Х1
   *L[0] = *L[2] = *L[4] = 0.25;
   // индуктивность обмоток выводов Х2 (коэффициент трансформации определяется соотношением индуктивностей)
   *L[1] = *L[3] = *L[5] = 0.25;
   // сопротвление утечки обмоток
   *R[15] = *R[16] = *R[17] = *R[18] = *R[19] = *R[20] = MAX;
   // сопротивление в нетралях (раззаземлены)
   *R[21] = *R[22] = MAX; // это необходимо так как еще не известно связаны ли выводы со стороны нейтрали у обмоток
   // соединяем обмотки трансформатора
   Link_Windings();
   Name = "Тр";  // имя объекта
   Grounded_Node = 17;   // заземляемый узел
}
//---------------------------------------------------------------------------
void PTransformer_3 :: set_pins(){
   X1 [_N_] = set_pin(17);
   X1 [_A_] = set_pin(0);
   X1 [_B_] = set_pin(4);
   X1 [_C_] = set_pin(8);
   X1n[_N_] = set_pin(1);// да-да это не опечатка
   X1n[_A_] = set_pin(1);
   X1n[_B_] = set_pin(5);
   X1n[_C_] = set_pin(9);
   X2 [_N_] = set_pin(17);
   X2 [_A_] = set_pin(2);
   X2 [_B_] = set_pin(6);
   X2 [_C_] = set_pin(10);
   X2n[_N_] = set_pin(3); //да-да это не опечатка
   X2n[_A_] = set_pin(3);
   X2n[_B_] = set_pin(7);
   X2n[_C_] = set_pin(11);
   // имена
   X1.Name  = "X1";
   X2.Name  = "X2";
   X1n.Name = "X1n";
   X2n.Name = "X2n";
	// добавим в список групп выводов
	Out.add(&X1);
   Out.add(&X2);
   Out.add(&X1n);
   Out.add(&X2n);
}
//---------------------------------------------------------------------------
void PTransformer_3::Describe_Fields(){
   PFieldInfo fi;
   // тип магнитной системы
   fi.Name = "Магн.сист.";
   fi.Type = "MagnCircuit_Type";
   fi.ptr = (void*) &mType;
   Fields.push_back(fi);
   // Схема соединения первичной обмотки
   fi.Name = "Перв.обм.";
   fi.Type = "Winding_Connect";
   fi.ptr = (void*) &Winding_X1;
   Fields.push_back(fi);
   // Схема соединения вторичной обмотки
   fi.Name = "Втор.обм.";
   fi.Type = "Winding_Connect";
   fi.ptr = (void*) &Winding_X2;
   Fields.push_back(fi);
   // Кривая намагничивания
   fi.Name = "I(Ф)";
   fi.Type = "PCurve";
   fi.ptr = (void*) &mCurve;
   Fields.push_back(fi);
}
//---------------------------------------------------------------------------
void __fastcall PTransformer_3 :: R1(PType value){  // установка активных сопротивлений на фазу
   *R[0] = *R[2] = *R[4] = value;
}
void __fastcall PTransformer_3 :: L1(PType value){  // установка индуктивности фазы
   *L[0] = *L[2] = *L[4] = value;
}
void __fastcall PTransformer_3 :: C1(PType value){  // установка емкости фазы
   *C[0] = *C[2] = *C[4] = value;
}
void __fastcall PTransformer_3 :: Gz1(PType value){ // установка активных проводимостей на землю
   *R[15] = *R[17] = *R[19] = 1/value;
}
void __fastcall PTransformer_3 :: Cz1(PType value){ // установка емкостей на землю
   *C[15] = *C[17] = *C[19] = value;
}
PType PTransformer_3 :: R1(Phase ph){  // установка активных сопротивлений на фазу
   switch(ph){
      case _A_: return *R[0];
      case _B_: return *R[2];
      case _C_: return *R[4];
      case _N_: return *R[21];
      default:  err(0,"PTransformer::R1(): не верно указана фаза"); return null;
   }
}
PType PTransformer_3 :: L1(Phase ph){  // установка индуктивности фазы
   switch(ph){
      case _A_: return *L[0];
      case _B_: return *L[2];
      case _C_: return *L[4];
      case _N_: return *L[21];
      default:  err(0,"PTransformer::R1(): не верно указана фаза"); return null;
   }
}
PType PTransformer_3 :: C1(Phase ph){  // установка емкости фазы
   switch(ph){
      case _A_: return *C[0];
      case _B_: return *C[2];
      case _C_: return *C[4];
      case _N_: return *C[21];
      default:  err(0,"PTransformer::R1(): не верно указана фаза"); return null;
   }
}
PType PTransformer_3 :: Gz1(Phase ph){ // установка активных проводимостей на землю
   switch(ph){
      case _A_: return 1/(*R[15]);
      case _B_: return 1/(*R[17]);
      case _C_: return 1/(*R[19]);
      default:  err(0,"PTransformer::R1(): не верно указана фаза"); return null;
   }
}
PType PTransformer_3 :: Cz1(Phase ph){ // установка емкостей на землю
   switch(ph){
      case _A_: return *C[15];
      case _B_: return *C[17];
      case _C_: return *C[19];
      default:  err(0,"PTransformer::R1(): не верно указана фаза"); return null;
   }
}
//---------------------------------------------------------------------------
void __fastcall PTransformer_3 :: R2(PType value){  // установка активных сопротивлений на фазу
   *R[1] = *R[3] = *R[5] = value;
}
void __fastcall PTransformer_3 :: L2(PType value){  // установка индуктивности фазы
   *L[1] = *L[3] = *L[5] = value;
}
void __fastcall PTransformer_3 :: C2(PType value){  // установка емкости фазы
   *C[1] = *C[3] = *C[5] = value;
}
void __fastcall PTransformer_3 :: Gz2(PType value){ // установка активных проводимостей на землю
   *R[16] = *R[18] = *R[20] = 1/value;
}
void __fastcall PTransformer_3 :: Cz2(PType value){ // установка емкостей на землю
   *C[16] = *C[18] = *C[20] = value;
}
PType PTransformer_3 :: R2(Phase ph){  // установка активных сопротивлений на фазу
   switch(ph){
      case _A_: return *R[1];
      case _B_: return *R[3];
      case _C_: return *R[5];
      case _N_: return *R[22];
      default:  err(0,"PTransformer::R1(): не верно указана фаза"); return null;
   }
}
PType PTransformer_3 :: L2(Phase ph){  // установка индуктивности фазы
   switch(ph){
      case _A_: return *L[1];
      case _B_: return *L[3];
      case _C_: return *L[5];
      case _N_: return *L[22];
      default:  err(0,"PTransformer::R1(): не верно указана фаза"); return null;
   }
}
PType PTransformer_3 :: C2(Phase ph){  // установка емкости фазы
   switch(ph){
      case _A_: return *C[1];
      case _B_: return *C[3];
      case _C_: return *C[5];
      case _N_: return *C[22];
      default:  err(0,"PTransformer::R1(): не верно указана фаза"); return null;
   }
}
PType PTransformer_3 :: Gz2(Phase ph){ // установка активных проводимостей на землю
   switch(ph){
      case _A_: return 1/(*R[16]);
      case _B_: return 1/(*R[18]);
      case _C_: return 1/(*R[20]);
      default:  err(0,"PTransformer::R1(): не верно указана фаза"); return null;
   }
}
PType PTransformer_3 :: Cz2(Phase ph){ // установка емкостей на землю
   switch(ph){
      case _A_: return *C[16];
      case _B_: return *C[18];
      case _C_: return *C[20];
      default:  err(0,"PTransformer::R1(): не верно указана фаза"); return null;
   }
}
//---------------------------------------------------------------------------
/* Рассчитываем параметры схемы замещения */
bool PTransformer_3 :: accept(){
   if(Ix==0) {
   	err(0, "PTransformer_1: ток х.х. не может быть равным нулю");
      Ix=MIN;
      return false;
   	}
   // параметры пресчитанные на фазу
   PType U1_f,  U2_f, S_f;    // номинальные напряжения и мощность трансформатора
   PType dPk_f, Uk_f;  		   // данные КЗ
   PType dPx_f, Ix_f;			   // данные холостого хода
   // считаем обе обмотки соединенными в звезду
   // потом произведем преобразование треугольника в звезду (! не забыть пересчитать число витков)
   S_f = S/3;
   dPx_f = dPx/3;
   dPk_f = dPk/3;
   U1_f =  U1/sqrt(3);
   U2_f =  U2/sqrt(3);

   // коэффициенты
   PType k = U1_f / U2_f;        // действительный коэффициент трансформации
   PType w = 2*freq*M_PI;     // угловая частота

   // 1. Рассчет параметров на 1 фазу
   // активное сопротивление обмоток
   PType R_sum = dPk_f * pow(U1_f,2) / pow(S_f,2);
   PType R1 = 0.5 * R_sum;
   PType R2 = R1 / pow(k,2);
   // ток холостого хода (в Амперах)
   PType __Ix = Ix/100 * S_f/U1_f;   // в Амперах
   // реактивные потери холостого хода
   PType dQ2_f = pow(__Ix*U1_f,2) - pow(dPx_f,2);
   if(dQ2_f<0){
      err(0, "PTransformer_3::accept(): параметры трансформатора заданы не верно.Слишком маленький ток х.х. для заданных активн. потерь х.х.");
      return false;
      }
   PType dQx_f = sqrt( dQ2_f );
   PType q_xx;	// угол между током и напряжением  холостого хода
   if(dPx) q_xx = atan(dQx_f/dPx_f);
   else    q_xx = M_PI/2;
   // число витков обмоток
   n1 = U1_f / (w*F_nom);
   n2 = n1 / k;
   // сопротивление цепи намагничивания (учет потерь)
   PType Zu = __Ix * n1 / F_nom;
   PType Ru = Zu * sin(q_xx);
   PType Xu = Zu * cos(q_xx);
   // реактивное сопротивление обмоток
   PType Z_sum =  Uk/100 * pow(U1_f,2) / S_f;
   PType X_sum2 = pow(Z_sum,2) - pow(R_sum,2);
   if(X_sum2<0){
      err(0, "PTransformer_3::accept(): параметры трансформатора заданы не верно.Слишком маленький ток Uk для заданных активн. потерь к.з.");
      return false;
      }
   PType X_sum =  sqrt( X_sum2 );
   PType dX1 = X_sum / 2;           // сопротивление рассеяния первичной обмотки
   PType dX2 = dX1 / pow(k,2);      // сопротивление рассеяния вторичной обмотки */
   // 2. пересчет обмоток
   if( Winding_X1 == wTriangle ){ // если обмотка была соединены в треугольник, то пересчитываем параметры
      R1 = 3*R1;
      dX1 = 3*dX1;
      n1 = n1 * sqrt(3);
      }
   if( Winding_X2 == wTriangle ){ // если обмотка была соединены в треугольник, то пересчитываем параметры
      R2 = 3*R2;
      dX2 = 3*dX2;
      n2 = n2 * sqrt(3);
      }
   // 3. пересчет сопротивлений магнитной системы
   PType Ru_st, Ru_ya;                 // магнитные сопротивления стержня и ярма
   PType Xu_st, Xu_ya;
   if( mType == Three_Rod ){           // если трехстержневой
      Ru_st = 0.8 * Ru;
      Xu_st = 0.8 * Xu;
      Ru_ya = Ru_st / 8;
      Xu_ya = Xu_st / 8;
      *R[9] = *R[10] = MAX;            // сопротивление крайних стержней
      }
   else if( mType == Armored ){        // если бронированная магнитная система
      Ru_st = Ru / 1.223;
      Xu_st = Xu / 1.223;
      Ru_ya = Ru_st / 8;
      Xu_ya = Xu_st / 8;
      *R[9] = *R[10] = 8*Ru_st + Ru_ya;// сопротивление крайних стержней
      *L[9] = *L[10] = 8*Xu_st + Xu_ya;
      }
   else{
      err(0,"PTransformer_3::accept(): Тип магнитной системы не известен");
      return false;
      }
   // устанавливаем параметры
   // > первичная обмотка
   *R[0] = *R[2] = *R[4] = R1;
   *L[0] = *L[2] = *L[4] = dX1/w;
   // > вторичная обмотка
   *R[1] = *R[3] = *R[5] = R2;
   *L[1] = *L[3] = *L[5] = dX2/w;
   // > стержни
   *R[6] = *R[7] = *R[8] = Ru_st;
   *L[6] = *L[7] = *L[8] = Xu_st/w;
   // > ярма
   *R[11] = *R[12] = *R[13] = *R[14] = Ru_ya;
   *L[11] = *L[12] = *L[13] = *L[14] = Xu_ya/w;
   // > магнитная связь первичная обмотка
   magnetic_link(0,6,n1);
   magnetic_link(2,7,n1);
   magnetic_link(4,8,n1);
   // > магнитная связь вторичная обмотка
   magnetic_link(1,6,n2);
   magnetic_link(3,7,n2);
   magnetic_link(5,8,n2);
   // сборка обмоток
   if(!Link_Windings()) return false;
   update_differential_resistance();   // пересчитываем дифференциальные сопротивления
   return true;
}
//---------------------------------------------------------------------------
/* Пересчет сопротивления цепи намагничивания */
bool PTransformer_3 :: update_elements(bool force_update){
   // обновляем параметры схемы в соответствии с кривой намагничивания
   if( !mCurve.exist )  // задана ли кривая намагничивания
      return false;
   else{
      #define PT3_FST_Ru     6      // первая по номеру магнитная ветвь
      #define PT3_NUM_Ru     9      // число магнитных ветвей
      PType _F,Ru;      	// значение потока и соответствующего ему сопротивления
      for(unsigned i= 0; i< PT3_NUM_Ru; i++ ){
         _F = *I[ i + PT3_FST_Ru ];
         if(_F!=0){
         	Ru  = n1*mCurve.I(_F)/_F; 		// сопротивления ветвей магнитопроводов
         	if(Ru>=0) *R[ i + PT3_FST_Ru ] = Ru;   // установим сопротивление
         	}
         }
      return true;
      /*/PType _Ip[ PT3_NUM_Ru ];   	// какой должена быть мгнов. реактивная составляющая тока холостого хода
      PType _U[3];                  // напряжение на питающих обмотках
      _U[0] = *f[0] - *f[1];        // фаза А
      _U[1] = *f[4] - *f[5];        // фаза В
      _U[2] = *f[8] - *f[9];        // фаза С

      PType dRxx = pow(_U1,2)/_dPx;	// коэф. пропорциональности между напряж на обмотке и акт сост тока хх.  = pow(_U1/sqrt(3),2) / (_dPx/3)
      //PType _Ia[3];			      // мгновенное активно составл тока хх
      PType _I[PT3_NUM_Ru];	      // мгнов. значение тока намагничивания в дросселях, намотанных на магнитную ветвь
      for(unsigned i=0; i<3; i++ )
         _I[i] = _U[i]/dRxx + mCurve.I(_F[i]);    // акт. и реакт составл семмируем

      PType Ru[PT3_NUM_Ru];
      for(unsigned k=0;k< PT3_NUM_Ru;k++)
      	if(_F[k]!=0) Ru[k]  = _n1*mCurve.I(_F[k])/_F[k]; // сопротивления ветвей магнитопроводов
      // устанавливаем активное сопротивление магнитопровода, которое влияет на потери
      //int k; // номер обрабатываемой ветки
      // стержни
      //k=0; if(_F[k]!=0)   Ru[k]  = *R[k+PT3_FST_Ru]+(_n1*_I[k] - _n1*(*I[0]))/_F[k];  	// фаза A
      //k=0; if(_F[k]!=0)   Ru[k]  = (_n1*_I[k] - )/_F[k];
      //k=0; if(_F[k]!=0)   Ru[k]  = (_n1*_I[k] - *L[k+PT3_FST_Ru]*d_F[k])/_F[k];   // фаза A
      //k=1; if(_F[k]!=0)   Ru[k]  = (_n1*_I[k] - *L[k+PT3_FST_Ru]*d_F[k])/_F[k];   // фаза B
      //k=2; if(_F[k]!=0)   Ru[k]  = (_n1*_I[k] - *L[k+PT3_FST_Ru]*d_F[k])/_F[k];   // фаза B
      /*if(mType == Armored){   // если броневой магнитопровод
         k=3; if(_F[k]!=0)   Ru[k]  = (    0     - (*f[13]-*f[12]) - *L[k+PT3_FST_Ru]*d_F[k])/_F[k];   // дросселя нет
         k=4; if(_F[k]!=0)   Ru[k]  = (    0     - (*f[17]-*f[16]) - *L[k+PT3_FST_Ru]*d_F[k])/_F[k];
      } else
         Ru[3] = Ru[4] = 1e100;
      // ярма
      k=5; if(_F[k]!=0)   Ru[k]  = (    0     - (*f[14]-*f[12]) - *L[k+PT3_FST_Ru]*d_F[k])/_F[k];
      k=6; if(_F[k]!=0)   Ru[k]  = (    0     - (*f[15]-*f[13]) - *L[k+PT3_FST_Ru]*d_F[k])/_F[k];
      k=7; if(_F[k]!=0)   Ru[k]  = (    0     - (*f[16]-*f[14]) - *L[k+PT3_FST_Ru]*d_F[k])/_F[k];
      k=8; if(_F[k]!=0)   Ru[k]  = (    0     - (*f[17]-*f[15]) - *L[k+PT3_FST_Ru]*d_F[k])/_F[k];
      */
      }
}
//---------------------------------------------------------------------------
bool PTransformer_3 :: Link_Windings(){
   // есть вероятность собрать несколько раз
   Internal_Links.clear();
   // соединяем первичную обмотку
   if( wStar == Winding_X1  ){             // в звезду
      Internal_Links.add(X1n[_A_], X1n[_B_]);
      Internal_Links.add(X1n[_A_], X1n[_C_]);
   }else if( wTriangle == Winding_X1  ){   // в треугольник
      Internal_Links.add(X1n[_A_], X1[_B_]);
      Internal_Links.add(X1n[_B_], X1[_C_]);
      Internal_Links.add(X1n[_C_], X1[_A_]);
   }else if( wGroundedStar == Winding_X1  ){// в звезду c заземленным нулем
      Internal_Links.add(X1n[_A_], X1n[_B_]);
      Internal_Links.add(X1n[_A_], X1n[_C_]);
      *R[21] = *L[21] = *C[21] = 0;
   }else{
      err(0, "PTransformer_3::Winding_X2(): схема соединений не известна");
      return false;
      }
   // соединяем вторичную обмотку
   if( wStar == Winding_X2  ){             // в звезду
      Internal_Links.add(X2n[_A_], X2n[_B_]);
      Internal_Links.add(X2n[_A_], X2n[_C_]);
   }else if( wTriangle == Winding_X2  ){   // в треугольник
      Internal_Links.add(X2n[_A_], X2[_B_]);
      Internal_Links.add(X2n[_B_], X2[_C_]);
      Internal_Links.add(X2n[_C_], X2[_A_]);
   }else if( wGroundedStar == Winding_X2  ){// в звезду c заземленным нулем
      Internal_Links.add(X2n[_A_], X2n[_B_]);
      Internal_Links.add(X2n[_A_], X2n[_C_]);
      *R[22] = *L[22] = *C[22] = 0;
   }else{
      err(0, "PTransformer_3::Winding_X2(): схема соединений не известна");
      return false;
      }
   return true;
}
//---------------------------------------------------------------------------
void PTransformer_3::Save(ostream &stream){		// сохранение параметров объекта в поток
   // сначала сохранится базовый класс
   sire::Save(stream);
   // теперь сохраняем этот класс
   stream << mType << " ";
   stream << Winding_X1 << " ";
   stream << Winding_X2 << " ";
   // сохраняем кривую намагничивания
   mCurve.Save(stream);
}
//---------------------------------------------------------------------------
void PTransformer_3::Read(istream &stream){    // загрузка параметров объекта из потока
   // сначала читает данные базовый класс
   sire::Read(stream);
   // теперь читаем этот класс
   int tmp;
   stream >> tmp;          mType       = (MagnCircuit_Type)tmp ;
   stream >> tmp;          Winding_X1  = (Winding_Connect) tmp;
   stream >> tmp;          Winding_X2  = (Winding_Connect) tmp;
   // читаем кривую намагничивания
   mCurve.Read(stream);
}
//---------------------------------------------------------------------------
String PTransformer_3::ClassName(){return "PTransformer_3";};				// возвращает имя класса
//---------------------------------------------------------------------------






/* Базовый класс для трансформаторов тока */
PCurrent_Transformer::PCurrent_Transformer(unsigned NumBranches, unsigned NumNodes, unsigned NumCircuit) :
					PUnit(NumBranches, NumNodes, NumCircuit){
   Describe_Fields();   // опишем свойства класса
   S = 10;
   n1 = 0; n2 = 0;      // значение витков вычислится в accept()
   I1= 100; I2= 5;
   K = 0.5; d = 1;
   freq = 50;
   Name = "TA";
}
//---------------------------------------------------------------------------
void PCurrent_Transformer::Describe_Fields(){
   PFieldInfo fi;
   // ном. мощность
   fi.Name = "Мощность";
   fi.Type = "PType";
   fi.ptr = (void*) &S;
   Fields.push_back(fi);
   // первичный ток
   fi.Name = "I1";
   fi.Type = "PType";
   fi.ptr = (void*) &I1;
   Fields.push_back(fi);
   // вторичный ток
   fi.Name = "I2";
   fi.Type = "PType";
   fi.ptr = (void*) &I2;
   Fields.push_back(fi);
   // число витков в первичной обмотке
   fi.Name = "n1";
   fi.Type = "PType";
   fi.ptr = (void*) &n1;
   Fields.push_back(fi);
   // число витков во вторичной обмотке
   fi.Name = "n2";
   fi.Type = "PType";
   fi.ptr = (void*) &n2;
   Fields.push_back(fi);
   // Класс точности
   fi.Name = "Класс";
   fi.Type = "PType";
   fi.ptr = (void*) &K;
   Fields.push_back(fi);
   // Паспортная угловая погрешность
   fi.Name = "Угл.погр";
   fi.Type = "PType";
   fi.ptr = (void*) &d;
   Fields.push_back(fi);
   // Номинальная частота
   fi.Name = "Ном.частота";
   fi.Type = "PType";
   fi.ptr = (void*) &freq;
   Fields.push_back(fi);
   // Кривая намагничивания
   fi.Name = "I(Ф)";
   fi.Type = "PCurve";
   fi.ptr = (void*) &mCurve;
   Fields.push_back(fi);
}
//---------------------------------------------------------------------------
void PCurrent_Transformer::Save(ostream &stream){		// сохранение параметров объекта в поток
   // сначала сохранится базовый класс
   sire::Save(stream);
   // теперь сохраняем этот класс
   stream << n1 << " ";
   stream << n2 << " ";
   stream << I1 << " ";
   stream << I2 << " ";
   stream << S  << " ";
   stream << K  << " ";
   stream << d  << " ";
   stream <<freq<< " ";
   stream <<F_nom<< " ";
   // сохраняем кривую намагничивания
   mCurve.Save(stream);
}
//---------------------------------------------------------------------------
void PCurrent_Transformer::Read(istream &stream){    // загрузка параметров объекта из потока
   // сначала читает данные базовый класс
   sire::Read(stream);
   // теперь читаем этот класс
   stream >> n1 ;
   stream >> n2 ;
   stream >> I1 ;
   stream >> I2 ;
   stream >> S  ;
   stream >> K  ;
   stream >> d  ;
   stream >>freq;
   stream >>F_nom;
   // читаем кривую намагничивания
   mCurve.Read(stream);
}
//---------------------------------------------------------------------------
String PCurrent_Transformer::ClassName(){return "PCurrent_Transformer";};				// возвращает имя класса







//---------------------------------------------------------------------------
/* Модель трансформатора тока */
PCurrent_Transformer_1 :: PCurrent_Transformer_1() :
                           PCurrent_Transformer(5, 5, 1),
                           X1(2), X2(2){
   Describe_Fields(); // опишем поля трансформатора тока
   set_pins();        // опишем выводы
  	// заполняем матрицы
   char _M[5][5] =   {{-1, 0, 0,-1, 0},
                      { 1, 0, 0, 0, 0},
                      { 0,-1, 0, 0,-1},
                      { 0, 1, 0, 0, 0},
                      { 0, 0, 0, 1, 1}};
	*M = (char*)_M;
   char _N[1][5] =    {{0, 0, 1, 0, 0}};
   *N = (char*)_N;
   //mCurve.set(0,0);
   //mCurve.set(1e3,1e6);  // линейная кривая намагничивания с насыщением при грамандой индукции
   // рассчитаем параметры схемы на основе установленных данных
   R1 = 1e-3;
   L1 = 0   ;
   R2 = 1e-1;
   L2 = 1e-6 ;
   *R[2] = MIN;         // сопротивление цепи намагничивания
   *R[3] = *R[4] = MAX; // сопротвление утечки обмоток
   Grounded_Node = 4;   // заземляемый узел
}
//---------------------------------------------------------------------------
void PCurrent_Transformer_1 :: set_pins(){
   X1 [_N_] = set_pin(1);
   X1 [_A_] = set_pin(0);
   X2 [_N_] = set_pin(3);
   X2 [_A_] = set_pin(2);
   // имена
   X1.Name  = "X1";
   X2.Name  = "X2";
	// добавим в список групп выводов
	Out.add(&X1);
   Out.add(&X2);
}
//---------------------------------------------------------------------------
void PCurrent_Transformer_1::Describe_Fields(){
   PFieldInfo fi;
   // акт. сопротивление первичной обм
   fi.Name = "R1";
   fi.Type = "PType";
   fi.ptr = (void*) &R1;
   Fields.push_back(fi);
   // индуктивность первичной обм
   fi.Name = "L1";
   fi.Type = "PType";
   fi.ptr = (void*) &L1;
   Fields.push_back(fi);
   // акт. сопротивление вторичной обм
   fi.Name = "R2";
   fi.Type = "PType";
   fi.ptr = (void*) &R2;
   Fields.push_back(fi);
   // индуктивность вторичной обм
   fi.Name = "L2";
   fi.Type = "PType";
   fi.ptr = (void*) &L2;
   Fields.push_back(fi);
}
//---------------------------------------------------------------------------
bool PCurrent_Transformer_1 :: accept(){
   // коэффициенты
   PType k = I1 / I2;        // действительный коэффициент трансформации
   PType w = 2*freq*M_PI;     // угловая частота
   // если не заданы число обмоток
   if(n1==0) n1=1;
   if(n2==0)
      if(n1!=0) n2=k*n1;
      else {n1=1;n2=k*n1;}

   PType _d = d*M_PI/180;                // переводим угловую погрешность в радианы
   PType a = acos(0.8);                   // это косинус расчетной номинальной нагрузки
   PType psi = atan(K/100/_d)-a;             // угол между потоком и током намагничивания
   PType _I2 = I1*n1/n2*( cos(_d)-K/100 ); // дествительный ток с учетом погрешности при номинальной загрузке

   PType E2 = S / _I2;         // ЭДС во вторичном контуре при номинальной загрузке
   F_nom = E2 / (w*n2);        // поток, необходимый для обеспечения этого напряжения
   // МДС намагничивания
   PType F0 = K/100*(I1*n1)/sin(psi+a);
   // сопротивление цепи намагничивания (учет потерь)
   PType Zu = F0 / F_nom;
   PType Ru = Zu * sin(psi);
   PType Xu = Zu * cos(psi);

   // цепь намагничивания
   *R[2] = Ru;  *L[2] = Xu/w;
   // обмотки
   *R[0] = R1;  *L[0] = L1;
   *R[1] = R2;  *L[1] = L2;
   magnetic_link(0,2,n1);
   magnetic_link(1,2,n2);
   update_differential_resistance();   // пересчитываем дифференциальные сопротивления
   return true;
}
//---------------------------------------------------------------------------
bool PCurrent_Transformer_1:: update_elements(bool force_update){
   // обновляем параметры схемы в соответствии с кривой намагничивания
   if( !mCurve.exist )           // задана ли кривая намагничивания
      return false;
   else{
      PType _F,Ru;      	      // значение потока и соответствующего ему сопротивления
      _F = *I[2];
      Ru  = n1*mCurve.I(_F)/_F; 	// сопротивления ветвей магнитопроводов
      if(Ru>=0) *R[2] = Ru;      // установим сопротивление
      return true;
      }
}
//---------------------------------------------------------------------------
void PCurrent_Transformer_1::Save(ostream &stream){		// сохранение параметров объекта в поток
   // сначала сохранится базовый класс
   sire::Save(stream);
   // теперь сохраняем этот класс
   stream << R1 << " ";
   stream << R2 << " ";
   stream << L1 << " ";
   stream << L2 << " ";
}
//---------------------------------------------------------------------------
void PCurrent_Transformer_1::Read(istream &stream){    // загрузка параметров объекта из потока
   // сначала читает данные базовый класс
   sire::Read(stream);
   // теперь читаем этот класс
   stream >> R1 ;
   stream >> R2 ;
   stream >> L1 ;
   stream >> L2 ;
}
//---------------------------------------------------------------------------
String PCurrent_Transformer_1::ClassName(){return "PCurrent_Transformer_1";};				// возвращает имя класса







//---------------------------------------------------------------------------
/* Выключатель трехфазный */
PBreaker::PBreaker():
					      PUnit(9, 7, 3),
                     X1(4), X2(4){
   //установим выводы
   set_pins();
   // опишем свойства класса
   Describe_Fields();
  	// заполняем матрицы
   char _M[7][9] =   {{ 0, 0, 0, 1, 1, 1, 1, 1, 1},
                      {-1, 0, 0,-1, 0, 0, 0, 0, 0},
                      { 1, 0, 0, 0,-1, 0, 0, 0, 0},
                      { 0,-1, 0, 0, 0,-1, 0, 0, 0},
                      { 0, 1, 0, 0, 0, 0,-1, 0, 0},
                      { 0, 0,-1, 0, 0, 0, 0,-1, 0},
                      { 0, 0, 1, 0, 0, 0, 0, 0,-1}};
	*M = (char*)_M;
   char _N[3][9] =   {{ 1, 0, 0,-1, 1, 0, 0, 0, 0},
                      { 0, 1, 0, 0, 0,-1, 1, 0, 0},
                      { 0, 0, 1, 0, 0, 0, 0,-1, 1}};
   *N = (char*)_N;
   // максимальное сопротивление в выключенном состоянии
   Rmax = MAX;
   // сопротивление во включенном состоянии
   Rmin = MIN;
   // по умолчанию выключен
   IsOn = true;
   *R[0] = *R[1] = *R[2] = Rmin;
   // проводимости на землю максимальны
   *R[3] = *R[4] = *R[5] = *R[6] = *R[7] = *R[8] = MAX;
   // состояниие отключения (включения)
   for(int i=0; i<4;i++){
      T_on[i]   = 0;   // фаза не в состоянии включения
      T_off[i]  = 0;   // фаза не в состоянии выключения
      }
   t_on  = 0.2;             // собственное время включения
   t_off = 0.05;            // собственное время отключения
   t_full_off = 0.08;       // полное время отключения
   Name = "Q";
}
//---------------------------------------------------------------------------
void PBreaker :: set_pins(){
   X1 [_N_] = set_pin(0);
   X1 [_A_] = set_pin(1);
   X1 [_B_] = set_pin(3);
   X1 [_C_] = set_pin(5);
   X2 [_N_] = set_pin(0);
   X2 [_A_] = set_pin(2);
   X2 [_B_] = set_pin(4);
   X2 [_C_] = set_pin(6);
   // имена
   X1.Name  = "X1";
   X2.Name  = "X2";
	// добавим в список групп выводов
	Out.add(&X1);
   Out.add(&X2);
}
//---------------------------------------------------------------------------
void PBreaker::Describe_Fields(){
   PFieldInfo fi;
   // сопротивление во включ. состоянии
   fi.Name = "Сопрот. вкл";
   fi.Type = "PType";
   fi.ptr = (void*) &Rmin;
   Fields.push_back(fi);
   // сопротивление в отлюч. состоянии
   fi.Name = "Сопрот. откл";
   fi.Type = "PType";
   fi.ptr = (void*) &Rmax;
   Fields.push_back(fi);
   // собственное время включения
   fi.Name = "Время вкл";
   fi.Type = "PType";
   fi.ptr = (void*) &t_on;
   Fields.push_back(fi);
   // собственное время отключения
   fi.Name = "Собст.вр. откл";
   fi.Type = "PType";
   fi.ptr = (void*) &t_off;
   Fields.push_back(fi);
   // полное время отключения
   fi.Name = "Полн.вр. откл";
   fi.Type = "PType";
   fi.ptr = (void*) &t_full_off;
   Fields.push_back(fi);
   // текущее сопротивление выключателя
   fi.Name = "Включен";
   fi.Type = "bool";
   fi.ptr = (void*) &IsOn;
   Fields.push_back(fi);
}
//---------------------------------------------------------------------------
bool PBreaker::on(Phase phase){
   switch (phase){
   	case _A_: if(!T_off[_A_] && !T_on[_A_]) {T_on[_A_] = TIME; return true;}; break;  // если  фаза не в состоянии выключения, то включаем
      case _B_: if(!T_off[_B_] && !T_on[_B_]) {T_on[_B_] = TIME; return true;}; break;
      case _C_: if(!T_off[_C_] && !T_on[_C_]) {T_on[_C_] = TIME; return true;}; break;
      default:	 return false;
      }
   return false;
}
//---------------------------------------------------------------------------
bool PBreaker::off(Phase phase){
   switch (phase){
   	case _A_: if(!T_on[_A_] && !T_off[_A_]) {T_off[_A_] = TIME; return true;}; break;  // если  фаза не в состоянии включения, то выключаем
      case _B_: if(!T_on[_B_] && !T_off[_B_]) {T_off[_B_] = TIME; return true;}; break;
      case _C_: if(!T_on[_C_] && !T_off[_C_]) {T_off[_C_] = TIME; return true;}; break;
      default:	 return false;
      }
   return false;
}
//---------------------------------------------------------------------------
bool PBreaker::on(){
   bool a,b,c;
   a = on(_A_);
   b = on(_B_);
   c = on(_C_);
   return a && b && c;
}
//---------------------------------------------------------------------------
bool PBreaker::off(){
   bool a,b,c;
   a = off(_A_);
   b = off(_B_);
   c = off(_C_);
   return a && b && c;
}
//---------------------------------------------------------------------------
bool PBreaker::accept(){
	if(IsOn)
   	*R[0] = *R[1] = *R[2] = Rmin;
   else
      *R[0] = *R[1] = *R[2] = Rmax;
   update_differential_resistance();   // пересчитываем дифференциальные сопротивления
   return true;
}
//---------------------------------------------------------------------------
bool PBreaker::update_elements(bool force_update){
   bool ret = false;
   static PType I1[4]={0,0,0,0};
   for(int i=_A_; i<=_C_; i++){
      // если фаза в процессе включения
      if(T_on[i]){
         // считаем, что включение происходит без дуги
         if( (TIME-T_on[i])>t_on) {
            *R[i-_A_]=Rmin;
            T_on[i] = 0;
            ret = true;
            }
         }
      // если фаза в процессе выключения
      else if(T_off[i]){
         // момент полного гашения дуги
         if( (TIME-T_off[i])>t_full_off && I1[i-_A_]*(*I[i-_A_])<0 ) {
            *R[i-_A_]=Rmax;
            T_off[i] = 0;
            ret = true;
            }
         // определяем момент начала расхождения контактов
         else if( (TIME-T_off[i])>t_off) {
            /*/ считаем, что сопротивление изменяется по линейному закону
            PType dR = (Rmax-Rmin)/((t_full_off-t_off)/smp);
            *R[i-_A_] += dR;
            if(*R[i-_A_]>Rmax) *R[i-_A_]=Rmax; */
            // учитываем сопротивление дуги
            *R[i-_A_] = fabs(11/(*I[i-_A_]));
            ret = true;
            }
         I1[i-_A_] = *I[i-_A_]; // сохраняем текущее значение
         }
      }
   return ret;
}
//---------------------------------------------------------------------------
void PBreaker::Save(ostream &stream){		// сохранение параметров объекта в поток
   // сначала сохранится базовый класс
   sire::Save(stream);
   // теперь сохраняем этот класс
   stream << Rmax << " ";
   stream << Rmin << " ";
   stream << t_on << " ";
   stream << t_off << " ";
   stream << t_full_off << " ";
   // сохраняем текущие сопротивления в фазах - грязный хук для интерфейса
   stream << IsOn << " ";

}
//---------------------------------------------------------------------------
void PBreaker::Read(istream &stream){    // загрузка параметров объекта из потока
   // сначала читает данные базовый класс
   sire::Read(stream);
   // теперь читаем этот класс
   stream >> Rmax ;
   stream >> Rmin ;
   stream >> t_on ;
   stream >> t_off ;
   stream >> t_full_off ;
   // читаем сопротивления в фазах - грязный хук для интерфейса
   stream >> IsOn;
}
//---------------------------------------------------------------------------
String PBreaker::ClassName(){return "PBreaker";};				// возвращает имя класса













/* Описатель свойств классаов */
PFieldInfo::PFieldInfo(){
   ptr = NULL;
   Type = "PType";
   Mode = rw;     // режим чтения записи
}
//---------------------------------------------------------------------------
String PFieldInfo::Value(){
   if(!ptr) return "";
   if     (Type == "String")
      return *(String*)ptr;
   else if(Type == "PType")
      return FloatToStr(*(PType*)ptr);
   else if(Type == "int")
      return IntToStr(*(int*)ptr);
   else if(Type == "MagnCircuit_Type"){
      	if		 (*(MagnCircuit_Type*)ptr == Three_Rod)		return "Three_Rod";
         else if(*(MagnCircuit_Type*)ptr == Armored)			return "Armored";
         else																return "Unknown";
   		}
   else if(Type == "Winding_Connect"){
      	if		 (*(Winding_Connect*)ptr == wStar)				return "wStar";
         else if(*(Winding_Connect*)ptr == wTriangle)			return "wTriangle";
         else if(*(Winding_Connect*)ptr == wGroundedStar)	return "wGroundedStar";
         else																return "Unknown";
   		}
   else if(Type == "bool"){
      switch(*(bool*)ptr){
         case true: 			return "true";
         default:				return "false";
      	}
   	}
   else if(Type == "PCurve"){
      String val = "";
      PCurve *obj = (PCurve*)ptr;
      unsigned lineNum = obj->size();
      if(   lineNum>0   ){
         // сформируем строку
         for(int i=0, n=lineNum+1; i<n; i++){
            PType X=0, Y=0;
            obj->Point(i, X, Y);
            val += "(" + FloatToStr(X) + ";" + FloatToStr(Y) + ")";
            if( (i+1)<n ) val += " ";
            }
         return val;
         }
      else    // ломаная не задана
         return "";
      }
   else if(Type == "Pins_List"){
   	if(dynamic_cast<PGrid*>((PElement*)ptr))       			return ((PGrid*)ptr)->getOuts();
   	else if(dynamic_cast<PSubGrid*>((PElement*)ptr))  		return ((PSubGrid*)ptr)->getOuts();
      }
   return "";
}
//---------------------------------------------------------------------------
void PFieldInfo::Value(String newVal){
	if( Mode== ro) return;
   if(!ptr) return;
   if     (Type == "String")
      *(String*)ptr = newVal;
   else if(Type == "PType"){
      if(!TryStrToFloat(newVal, *(PType*)ptr))
         err(1, "PFieldInfo: Недопустимый символ при преобразовании строки в число");
      }
   else if(Type == "int"){
      if(!TryStrToInt(newVal, *(int*)ptr))
         err(1, "PFieldInfo: Недопустимый символ при преобразовании строки в число");
      }
   else if(Type == "MagnCircuit_Type"){
      if		 (  newVal == "Three_Rod")	*(MagnCircuit_Type*)ptr = Three_Rod;
      else if(  newVal == "Armored")	*(MagnCircuit_Type*)ptr = Armored;
      else                             *(MagnCircuit_Type*)ptr = Armored;
      }
   else if(Type == "Winding_Connect"){
   	if		 (  newVal == "wStar")		*(Winding_Connect*)ptr = wStar;
      else if(  newVal == "wTriangle")	*(Winding_Connect*)ptr = wTriangle;
      else if(  newVal == "wGroundedStar") *(Winding_Connect*)ptr = wGroundedStar;
      else                             *(Winding_Connect*)ptr = wStar;
   	}
   else if(Type == "bool"){
   	if		 (  newVal == "true")		*(bool*)ptr = true;
      else                             *(bool*)ptr = false;
   	}
   else if(Type == "PCurve"){
      PCurve *obj = (PCurve*)ptr;
      if(   newVal != ""   ){
         vector<String> Point;  // будет содержать строки X;Y
         // найдем все подстроки "X;Y", которые разделены пробелами
         unsigned pos;
         // возможно было более одного пробела в начале
         int numSpace, m;
         for(numSpace=1, m=newVal.Length(); numSpace<=m; numSpace++) if(newVal[numSpace]!=' ') break;
         newVal = newVal.SubString(numSpace, newVal.Length() - numSpace + 1);
         pos = newVal.Pos(" ");
         while( pos ){
            Point.push_back( newVal.SubString(2,pos-3) );
            newVal = newVal.SubString(pos+1, newVal.Length()-pos);
            // возможно было более одного пробела
            for(numSpace=1, m=newVal.Length(); numSpace<=m; numSpace++) if(newVal[numSpace]!=' ') break;
            newVal = newVal.SubString(numSpace, newVal.Length() - numSpace + 1);
            pos = newVal.Pos(" ");
            }
         Point.push_back( newVal.SubString(2,newVal.Length()-2) );
         // вычислим значения X, Y и сохраним в объекте точку
         obj->clear();
         PType X, Y;
         for(int i=0, n=Point.size(); i<n; i++){
            pos = Point[i].Pos(";");
            if( pos ){
               X=0; Y=0;
               TryStrToFloat( Point[i].SubString(1,     pos-1)                , X );
               TryStrToFloat( Point[i].SubString(pos+1, Point[i].Length()-pos), Y );
               //String f = Point[i].SubString(pos+1, Point[i].Length()-pos);
               obj->_set(X, Y);
               }
            }
         }
      else // строка пустая
         obj->clear();  // удаляем ломанную
      }
   else if(Type == "Pins_List"){
   	if(dynamic_cast<PGrid*>((PElement*)ptr))     		return ((PGrid*)ptr)->setOuts( newVal );
   	else if(dynamic_cast<PSubGrid*>((PElement*)ptr))  	return ((PSubGrid*)ptr)->setOuts( newVal );
   	}
}
//---------------------------------------------------------------------------
char *StringForSave(String *s){
   if(*s=="") *s = "_";
   for(int i=1,n=(*s).Length(); i<=n; i++ )
   	if((*s)[i]==' ')
      	(*s)[i]='_';
   return (*s).c_str();
}
//---------------------------------------------------------------------------
char *StringForSave(char *s){
   for(int i=0; s[i] && i<1024; i++ )
   	if(s[i]==' ')
      	s[i]='_';
   return s;
}
//---------------------------------------------------------------------------
void SaveObj(PElement *Obj, ostream &stream){
   String cname = Obj->ClassName();
   stream << StringForSave(&cname) << " ";	// имя класса
   if(Obj->ClassName() == "PLine") stream << ((PLine*)Obj)->Cells() << " ";
   // сохраняем данные объекта
   Obj->Save( stream );
}
//---------------------------------------------------------------------------
PElement* CreateObj(istream &stream){
      PElement *Obj;
      char cname[1024];
   	stream >> cname;
      String ClassName = cname; // имя класса
      if(ClassName == "PLine"){
         unsigned cells;
      	stream >> cells;
      	if(cells<1) cells = 1;
      	Obj = new PLine(cells);
      	}
  		else if ( ClassName == "PSource" )               Obj = new PSource() ;
   	else if ( ClassName == "PConsumer" )             Obj = new PConsumer() ;
   	else if ( ClassName == "PTransformer_1" )        Obj = new PTransformer_1() ;
   	else if ( ClassName == "PTransformer_3" )        Obj = new PTransformer_3() ;
   	else if ( ClassName == "PCurrent_Transformer_1" )Obj = new PCurrent_Transformer_1() ;
   	else if ( ClassName == "PBreaker" )              Obj = new PBreaker() ;
      else if ( ClassName == "PGrid" )                 Obj = new PGrid() ;
      else if ( ClassName == "PSubGrid" )              Obj = new PSubGrid() ;
   	else if ( ClassName == "NULL" )	Obj = NULL;
   	else									   Obj = NULL;
      // читаем данные объекта
      if(Obj) Obj->Read(stream);
      return Obj;
}
//---------------------------------------------------------------------------
// Params - параметры используемые при создании объекта
PElement* CreateObj(String ClassName, vector<void*> Params){
      PElement *Obj;
      if(ClassName == "PLine" && Params.size()>0){
         int cells = *(unsigned*)Params[0];
         if( cells < 1 ) cells = 1;
         Obj = new PLine(cells) ;
      	}
  		else if ( ClassName == "PSource" )               Obj = new PSource() ;
   	else if ( ClassName == "PConsumer" )             Obj = new PConsumer() ;
   	else if ( ClassName == "PTransformer_1" )        Obj = new PTransformer_1() ;
   	else if ( ClassName == "PTransformer_3" )        Obj = new PTransformer_3() ;
   	else if ( ClassName == "PCurrent_Transformer_1" )Obj = new PCurrent_Transformer_1() ;
   	else if ( ClassName == "PBreaker" )              Obj = new PBreaker() ;
      else if ( ClassName == "PGrid" )                 Obj = new PGrid() ;
      else if ( ClassName == "PSubGrid" )              Obj = new PSubGrid() ;
   	else if ( ClassName == "NULL" )	Obj = NULL;
   	else									   Obj = NULL;
      return Obj;
}

