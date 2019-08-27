//---------------------------------------------------------------------------
#pragma hdrstop

#include "model.h"
//---------------------------------------------------------------------------
PObjectList ObjectList;    // �������� ������ ���� ��������� ��������
//---------------------------------------------------------------------------
PObjectList::~PObjectList(){
   // ������� ��� ������� �� ������, � ������� ��� ���������
   vector<PElement*> toDel;
   vector<PElement*>::iterator iter = Obj.begin();
    while( iter != Obj.end() ){
      if((*iter)->Owner == NULL) // ������� ������� ��� ���������, ��������� ������ ���� ������� �����������
         toDel.push_back(*iter);
      iter++;
      }
   // ������� ��� ������� �� ������, � ������� ��� ���������
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
// ��� ����� ���� ������� ����� � ��������� �����, � ������� ������ ������
// �������� ���_������.���������_���.�1
// ��� ������� (���� ��� ������������) � ����� ������� ������ ���� ���������
PElement* PObjectList::Find (String Name){
   String obj, grid;   // ������ � ������ ������� � ���� ������, �������� ���.����2.T2
   // ��������� ��� � ������ ��� ����
   int point = 0;
   for(int i=Name.Length(); i>0; i--) if(Name[i]=='.') {point=i;break;}
   if(point!=0){
      grid = Name.SubString(1,point-1);
      obj = Name.SubString(point+1, Name.Length()-point);
   }else{
      grid = "";
      obj = Name;
      }
   // ������ ���� ������
   for(int i=0,n=Obj.size(); i<n; i++)
   	if( Obj[i]->Name == obj && Obj[i]->GridName() == grid)
      	return Obj[i];
   return NULL;
   /*/ ��������� ��� �� ������ �����
   int point=Name.Pos(".");
   if(point!=0){
      grid = Name.SubString(1,point-1);
      obj = Name.SubString(point+1, Name.Length()-point);
   }else{
      grid = "";
      obj = Name;
      }
   // ������ ������
   if(grid!=""){
   	// ���� ������ ��������� ����
      // ��� ���� ������� ������ ������ ���� ���������
      PSubGrid *gr = dynamic_cast<PSubGrid*>(  Find( grid ) );
      if ( gr ) return gr->obj_from_name( obj );
      else      return NULL;
      }
	else
		{
      // ������� ��� ��� ��������
      PElement *ret = NULL;
      int cnt=0;
   	for(int i=0,n=Obj.size(); i<n; i++)
      	if( Obj[i]->Name == Name && Obj[i]->Grid == NULL){  // ���� � ���� �������� ������
         	ret = Obj[i];
            cnt++;
            }
      if( cnt>1 ){
      	err(0, "PObjectList: ������ "+ IntToStr(cnt) +" ������� � ������: "+Name);
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
   Grounded_Node = 0;   // ����������� ����
   // ��������� ������ � ������
   ObjectList.Add( this );
   // ������ ���� ������
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
   D = new Throttle*[v];  for(unsigned i=0;i<v;i++) D[i] = NULL; // �������� ��������� �� ���� �������������
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
   Grounded_Node = 0;   // ����������� ����
   // ��������� ������ � ������
   ObjectList.Add( this );
   // ������ ���� ������
   Describe_Fields();
}
//---------------------------------------------------------------------------
PElement::PElement(PElement *obj):null(0),MIN(1e-3),MAX(1e8){
   if(obj->M->Cols != obj->N->Cols){err(0,"�������� ������� Element ����� ����� ������ ���������� �� ����� ����� �����");}
   v = obj->M->Cols;
   u = obj->M->Rows;
   k = obj->N->Rows;
   _Lm = obj->_Lm;
   E = new PType*[v];
   U_C= new PType*[v];
   U_L= new PType*[v];
   U_Lm= new PType*[v];
   D = new Throttle*[v];   for(unsigned i=0;i<v;i++) D[i] = obj->D[i];  // �������� ��������� �� ��������
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
   Grounded_Node = obj->Grounded_Node;   // ����������� ����
   // ��������� ������ � ������
   ObjectList.Add( this );
   // ������ ���� ������
   Describe_Fields();
}
//---------------------------------------------------------------------------
void PElement::Describe_Fields(){
   // ��������� ���������� � ��������� ������
   PFieldInfo fi;
   fi.Name = "���";
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
   // ������� ������ �� �������
   ObjectList.Delete( this );
}
//---------------------------------------------------------------------------
unsigned __fastcall PElement::Branches(){ return v; }
unsigned __fastcall PElement::Nodes(){ return u; }
unsigned __fastcall PElement::Circuit(){ return k; }
//---------------------------------------------------------------------------
void PElement::mutual_inductance(unsigned branch_1, unsigned branch_2, const PType &value){
   if(value==0) return;
   // ������������������� �� ����� ���� ������ ����� ������������ ��������������
   if((fabs(value) - sqrt((*L[branch_1])*(*L[branch_2])))>1e-300){
   	err(0, "mutual_inductance: ������������� ������������������� ������ ����� ������������ ��������������. ��������� ��� �� ��������");
      }
   PType *p = &_Lm.push_back(value);
   //Lm[ branch_1 ][ branch_2 ] = _Lm.end()-1;
   //Lm[ branch_2 ][ branch_1 ] = _Lm.end()-1;
   Lm[ branch_1 ][ branch_2 ] = p;
   Lm[ branch_2 ][ branch_1 ] = p;
}
//---------------------------------------------------------------------------
void PElement::magnetic_link(unsigned Electric_Branch, unsigned Magnetic_Branch, unsigned Windings){
   // ������� ����� ��������
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
/* �������� ����������������� ������������� ��� ������ ��������������� ��� ���������� ��������� /
void PElement::update_differential_resistance_rect(){
   for(unsigned i=0; i<v; i++)
   	if(*C[i])
      	*Z[i] = *R[i] + *L[i]/smp + smp/(*C[i]);
      else
      	*Z[i] = *R[i] + *L[i]/smp;
} */
//---------------------------------------------------------------------------
/* �������� ����������������� ������������� ��� ������ ��������*/
void PElement::update_differential_resistance(){
   for(unsigned i=0; i<v; i++)
   	if(*C[i])
      	*Z[i] = *R[i] + *L[i]/smp + smp/(*C[i])/2;
      else
      	*Z[i] = *R[i] + *L[i]/smp;
}
//---------------------------------------------------------------------------
/* ������� ���������� ����� �� ���������� ��������� ������ */
bool PElement::accept(){
   update_differential_resistance();
   return true;
};
//---------------------------------------------------------------------------
void PElement::Save(ostream &stream){		// ���������� ���������� ������� � �����
   // ���
   stream << StringForSave( &Name ) << " ";
	// ����, � ������� ������
   String grName = GridName(); if( grName == "" ) grName = "N";
   stream << StringForSave( &grName ) << " ";
}
//---------------------------------------------------------------------------
void PElement::Read(istream &stream){    // �������� ���������� ������� �� ������
   char name[1024];
   // ���
	stream >> name;    Name = name;
   // ����, � ������� ������
   stream >> name;    String grName = name;
   if(grName == "N")		Grid = NULL;
   else						Grid = ObjectList.Find( grName );
}
//---------------------------------------------------------------------------
String PElement::ClassName(){ return "PElement";}
//---------------------------------------------------------------------------
// ������ ��� ����, � ������� ������ ������
String PElement::GridName(){
	if( Grid == NULL ) return "";
   else					 return Grid->FullName();
}
//---------------------------------------------------------------------------
// ��� ������� � ������ ���� ��������, � ������� �� ������
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
	// �������� ��� ���� ������, ���������� � ����
   PType I=0;
   unsigned n=obj->Branches();
   for(unsigned j=0; j<n;j++){
   	I += (*obj->M)[node][j] * (*obj->I[j]);
      }
   return I;
}
//---------------------------------------------------------------------------
void PPin::Save(ostream &stream){			// ��������� ���������� � ������
   stream << StringForSave(&obj->FullName()) << " ";
   stream << node << " ";
}
//---------------------------------------------------------------------------
void PPin::Read(istream &stream){			// ��������� ���������� � ������
   char n[1024];
   stream >> n;
   obj = ObjectList.Find( (String)n );
   if( !obj ){
      err(0, "PPin: ������ �� ������. ��� ������� ("+ (String)n +") �� ������� � ������ ��������");
      }
   stream >> node;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
/* ������ ������� */
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
      err(0,"Pins: ������ � ��������������� ��������. ������� 0 �������");
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
      err(0, "Pins::operator=: ������ ��� ������������. ����� ������� � �������� ������� �� �����");
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
   if(First.Count != Second.Count) {err(0, "Pins_Link::add: ����� ������� � ������� ������� �� ���������. �� ��� ������ ������ ��������� ��������� �� ������");}
   for(unsigned i=0, n=First.Count; i<n; i++){
   	pair.First = &First[i];
   	pair.Second = &Second[i];
		Link.push_back(pair);
      }
}
//---------------------------------------------------------------------------
// �������� ����� �� ������� obj � ���� ������
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
void Pins_List::add(Pins* X){ _Pins.push_back(X); }   /* ��������� ������ ������� */
Pins& Pins_List::operator[](unsigned int i){          // ������� ������ �������
	if(i<_Pins.size()) return *_Pins[i];
   else{
      err(0, "Pins_List::operator[]: ���������� � �������������� ������ �������.");
   	return *_Pins[0];
   	}
}
unsigned Pins_List::Count(){ return _Pins.size();} // ����� ����� �������
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
      _R[i]=MIN;     // ����� �� ���� ���������������� ������, ��� ������� ���� ������ ������������ � �����
      _L[i]=0;
      _C[i]=0;
      _Z[i]=0;
      _I[i]=0;
      }
   for(unsigned i=0;i<u;i++){
      _J[i]=0;
      _f[i]=0;
      }
   // ��������� ��������� �� ��� ��������
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
   // ������ ���� ������
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
   // ��������� ��������� �� ��� ��������
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
   Fields = obj->Fields;   // ���� �������� �����
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
   X1(4),               // ������ ������ ������
   X2(4){               // ������ ������ ������
   z=Num_Of_Cells;	   // ����� �������
   set_pins();          // ����������� ������� �� ����������
   Describe_Fields();   // ��������� ���������� � ��������� ������
	/*/ ��������� �������
   u= 1 + 3*z + 3;
   v=15*z + 12;
   k=4*z + 2;	// ����� �����, ������, �����. ��������*/
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
   Name = "���";        // ��� �������
   Grounded_Node = 0;   // ����������� ����
   // ��������  ��������� �����
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
   /*/ ������� ����������� �������� �������������
   for(unsigned i=0; i<(z+1); i++)
      *R[i*15]   = *R[i*15+1] = *R[i*15+2] =
      *R[i*15+3] = *R[i*15+4] = *R[i*15+5] =
      *R[i*15+6] = *R[i*15+7] = *R[i*15+8] =
      *R[i*15+9] = *R[i*15+10]= *R[i*15+11]= MAX;
   // ����������� ���������� ������������� G (���������� ��� ����������� ������ 2 ������ ��������)
   for(unsigned i=0; i<z; i++)
      *R[i*15+12] = *R[i*15+13] = *R[i*15+14] = MIN; */
   length = 1;  // ������ �����
}
//---------------------------------------------------------------------------
PLine::PLine(PLine &ob):
   PUnit(&ob),
   X1(4),
   X2(4){
   z=ob.z;	      // ����� �������
   set_pins();    // ����������� ������� �� ����������
}
//---------------------------------------------------------------------------
PLine::~PLine(){ ;}
//---------------------------------------------------------------------------
/* ��������� ��������-�������� ������� �� ���������� */
void PLine::set_pins(){
   Out.add(&X1);  // ��������� �� ������ ���������   Pins_Group[0] = &X1
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
   // ������
   fi.Name = "������";
   fi.Type = "PType";
   fi.ptr = (void*) &length;
   Fields.push_back(fi);
   // ����� �������
   fi.Name = "�������";
   fi.Type = "int";
   fi.ptr = (void*) &z;
   fi.Mode = ro;        // ������ ������
   Fields.push_back(fi);
   // ���������� ����� ������ ��� ����������� �����
   fi.Mode = rw;
   // �������� �������������
   for(int i=0; i<3; i++){
      fi.Name = "R0[_" + (String)((char)('A'+i)) + "_]";
      fi.Type = "PType";
      fi.ptr = (void*) &R0[_A_+i];
      Fields.push_back(fi);
      }
   // �������������
   for(int i=0; i<3; i++){
      fi.Name = "L0[_" + (String)((char)('A'+i)) + "_]";
      fi.Type = "PType";
      fi.ptr = (void*) &L0[_A_+i];
      Fields.push_back(fi);
      }
   /*/ ������� � �����
   for(int i=0; i<3; i++){
      fi.Name = "C0[_" + (String)((char)('A'+i)) + "_]";
      fi.Type = "PType";
      fi.ptr = (void*) &C0[_A_+i];
      Fields.push_back(fi);
      } */
   // �������� �������������
   for(int i=0; i<3; i++){
      switch(i){
         case _AB_: name = "AB"; break;
         case _BC_: name = "��"; break;
         case _CA_: name = "��"; break;
         default:   name = "";
      }
      fi.Name = "M_[_" + name + "_]";
      fi.Type = "PType";
      fi.ptr = (void*) &M0[_AB_+i];
      Fields.push_back(fi);
      }
   // �������� ������������ �� �����
   for(int i=0; i<3; i++){
      fi.Name = "Gz0[_" + (String)((char)('A'+i)) + "_]";
      fi.Type = "PType";
      fi.ptr = (void*) &Gz0[_A_+i];
      Fields.push_back(fi);
      }
   // ��������� ������������ �� �����
   for(int i=0; i<3; i++){
      fi.Name = "Cz0[_" + (String)((char)('A'+i)) + "_]";
      fi.Type = "PType";
      fi.ptr = (void*) &Cz0[_A_+i];
      Fields.push_back(fi);
      }
   // �������� ����������� ������������
   for(int i=0; i<3; i++){
      switch(i){
         case _AB_: name = "AB"; break;
         case _BC_: name = "��"; break;
         case _CA_: name = "��"; break;
         default:   name = "";
      }
      fi.Name = "Gm0[_"+ name +"_]";
      fi.Type = "PType";
      fi.ptr = (void*) &Gm0[_AB_+i];
      Fields.push_back(fi);
      }
   // ��������� ����������� ������������
   for(int i=0; i<3; i++){
      switch(i){
         case _AB_: name = "AB"; break;
         case _BC_: name = "��"; break;
         case _CA_: name = "��"; break;
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
      k != ob.k) {err(0,"������������ ������� �����: ����� ������� ������ (TPL::operator=).");return *this; }
   length = ob.length;  // ������ �����
   *M = *ob.M;
   *N = *ob.N;
   // �������� ����������
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
/* ��������� ����� 
void PLine::setR_(PType value){  // ��������� �������� ������������� �� ����
   PType R_zvena = value / z;
   for(unsigned i=0;i<z;i++){
      *R[i*15+12] = R_zvena;
      *R[i*15+13] = R_zvena;
      *R[i*15+14] = R_zvena;
      }
}
void PLine::setL_(PType value){  // ��������� ������������� ����
   PType L_zvena = value / z;
   for(unsigned i=0;i<z;i++){
      *L[i*15+12] = L_zvena;
      *L[i*15+13] = L_zvena;
      *L[i*15+14] = L_zvena;
      }
}
void PLine::setC_(PType value){  // ��������� ������� ����
   PType C_zvena = value * z;   // ������ ��������! ��������
   for(unsigned i=0;i<z;i++){
      *C[i*15+12] = C_zvena;
      *C[i*15+13] = C_zvena;
      *C[i*15+14] = C_zvena;
      }
}
void PLine::setGz_(PType value){ // ��������� �������� ������������� �� �����
   PType Rz_zvena = (z+1) / value;   // ����� ������������� �� 1 ������, ��� ����� �������
   for(unsigned i=0, cnt=z+1; i<cnt; i++){
      *R[i*15+6] = Rz_zvena;
      *R[i*15+7] = Rz_zvena;
      *R[i*15+8] = Rz_zvena;
      }
}
void PLine::setCz_(PType value){ // ��������� �������� �� �����
   PType Cz_zvena = value / (z+1);   // ��-��! ������ ������
   for(unsigned i=0, cnt=z+1; i<cnt; i++){
      *C[i*15+9 ] = Cz_zvena;
      *C[i*15+10] = Cz_zvena;
      *C[i*15+11] = Cz_zvena;
      }
   // �������� �������� ������������� � ���� ������
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
void PLine::setGm_(PType value){ // ��������� �������� ����������� �������������
   PType Rm_zvena = (z+1) / value;   // ����� ������������� �� 1 ������, ��� ����� �������
   for(unsigned i=0, cnt=z+1; i<cnt; i++){
      *R[i*15+0] = Rm_zvena;
      *R[i*15+1] = Rm_zvena;
      *R[i*15+2] = Rm_zvena;
      }
}
void PLine::setCm_(PType value){ // ��������� ����������� ��������
   PType Cm_zvena = value / (z+1);   // ��-��! ������ ������
   for(unsigned i=0, cnt=z+1; i<cnt; i++){
      *C[i*15+3] = Cm_zvena;
      *C[i*15+4] = Cm_zvena;
      *C[i*15+5] = Cm_zvena;
      }
   // �������� �������� ������������� � ���� ������
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
void PLine::setLm_(PType value){ // ��������� ����������� ��������������������
   PType Lm_zvena = value / z;
   for(unsigned i=0;i<z;i++){
   	mutual_inductance(i*15+12, i*15+13, Lm_zvena);
      mutual_inductance(i*15+12, i*15+14, Lm_zvena);
      mutual_inductance(i*15+13, i*15+14, Lm_zvena);
   	}
}
//---------------------------------------------------------------------------
// �������� ���������
void PLine::setR0(PType value){  // ��������� �������� ������������� �� ����
   PType R_zvena = value * length / z;
   for(unsigned i=0;i<z;i++){
      *R[i*15+12] = R_zvena;
      *R[i*15+13] = R_zvena;
      *R[i*15+14] = R_zvena;
      }
}
void PLine::setL0(PType value){  // ��������� ������������� ����
   PType L_zvena = value * length / z;
   for(unsigned i=0;i<z;i++){
      *L[i*15+12] = L_zvena;
      *L[i*15+13] = L_zvena;
      *L[i*15+14] = L_zvena;
      }
}
void PLine::setC0(PType value){  // ��������� ������� ����
   PType C_zvena = (value / length) * z;   // ������ ��������! ��������
   for(unsigned i=0;i<z;i++){
      *C[i*15+12] = C_zvena;
      *C[i*15+13] = C_zvena;
      *C[i*15+14] = C_zvena;
      }
}
void PLine::setGz0(PType value){ // ��������� �������� ������������� �� �����
   PType Rz_zvena = (z+1) / (value * length);   // ����� ������������� �� 1 ������, ��� ����� �������
   for(unsigned i=0, cnt=z+1; i<cnt; i++){
      *R[i*15+6] = Rz_zvena;
      *R[i*15+7] = Rz_zvena;
      *R[i*15+8] = Rz_zvena;
      }
}
void PLine::setCz0(PType value){ // ��������� �������� �� �����
   PType Cz_zvena = (value * length ) / (z+1);   // ��-��! ������ ������
   for(unsigned i=0, cnt=z+1; i<cnt; i++){
      *C[i*15+9 ] = Cz_zvena;
      *C[i*15+10] = Cz_zvena;
      *C[i*15+11] = Cz_zvena;
      }
   // �������� �������� ������������� � ���� ������
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
void PLine::setGm0(PType value){ // ��������� �������� ����������� �������������
   PType Rm_zvena = (z+1) / (value * length);   // ����� ������������� �� 1 ������, ��� ����� �������
   for(unsigned i=0, cnt=z+1; i<cnt; i++){
      *R[i*15+0] = Rm_zvena;
      *R[i*15+1] = Rm_zvena;
      *R[i*15+2] = Rm_zvena;
      }
}
void PLine::setCm0(PType value){ // ��������� ����������� ��������
   PType Cm_zvena = (value * length) / (z+1);   // ��-��! ������ ������
   for(unsigned i=0, cnt=z+1; i<cnt; i++){
      *C[i*15+3] = Cm_zvena;
      *C[i*15+4] = Cm_zvena;
      *C[i*15+5] = Cm_zvena;
      }
   // �������� �������� ������������� � ���� ������
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
void PLine::setLm0(PType value){ // ��������� ����������� ��������������������
   PType Lm_zvena = (value * length) / z;
   for(unsigned i=0;i<z;i++){
   	mutual_inductance(i*15+12, i*15+13, Lm_zvena);
      mutual_inductance(i*15+12, i*15+14, Lm_zvena);
      mutual_inductance(i*15+13, i*15+14, Lm_zvena);
   	}
}*/
//---------------------------------------------------------------------------
/* ����������� ���������� ����� ��������� ����� �� ������ ��������� ���� */
bool PLine::set_params(const PType _L0, const PType _R0, const PType _M0, const PType dP_f, const PType dQ_f, const PType U_n_f, const PType U_k_f, const PType dP, const PType dQ, const PType U_n, const PType U_k, const PType freq ){
	// ����� ������������� ���������� ������
   PType Xz = 2*M_PI*freq*(_L0*length/z);
   //PType Lz = L0*length/z;
   PType Rz = 2*M_PI*freq*(_R0*length/z);
	// ����� ������������� ���������� ������ �� �����
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
   // ����� ����������� �������������
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
   // ��������� ��������
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
/* ����������� ���������� ����� ��������� ���� ��� ���*/
bool PLine::Phase_Params(Phase phase, PType __R, PType __L, PType __C, PType Gz, PType Cz){
   PType R2;	   // ������������� �� �����
   if(Gz) R2 = (z+1)/Gz;
   else R2 = MAX;
   PType C2 = Cz/(z+1);
   PType L_zvena  = __L/z;
   PType R_zvena  = __R/z;
   PType C_zvena  = __C/z;
   // ���������� ����
   unsigned p;
   switch (phase){
   	case _A_: p=0; break;
      case _B_: p=1; break;
      case _C_: p=2; break;
      default:	 return false;
      }
   // ��������� ��������
   for(unsigned i=0;i<(z+1);i++){
      // ����� �������� ������������
      *R[i*15+6+p] = R2;
      *L[i*15+6+p] = 0;
      *C[i*15+6+p] = 0;
      // ����� ��������� ������������
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
/* ����������� ���������� ����� ��������� ���� ��� ���*/
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
/* ����������� ���������� ����� ��������� ����� - ������������������� */
bool PLine::Mutual_Inductance_0(PType M0_ab, PType M0_bc, PType M0_ca){
   return Mutual_Inductance(M0_ab*length, M0_bc*length, M0_ca*length);
}
//---------------------------------------------------------------------------
/* ����������� ���������� ����� ��������� ��� ��� - ����������� ������������ */
bool PLine::Mutual_Conductance(unsigned Phase1, unsigned Phase2, PType Gm, PType Cm){
   PType R1 = (z+1)/Gm;	// ����������� �������������
   PType C1 = Cm/(z+1);
   // ���������� ����
   unsigned p;
   switch (Phase1 + Phase2){
   	case (_A_ + _B_): p=0; break;
      case (_B_ + _C_): p=1; break;
      case (_C_ + _A_): p=2; break;
      default:	 return false;
      }
   // ��������� ��������
   for(unsigned i=0;i<(z+1);i++){
      // �������� ������������
      *R[i*15+p] = R1;
      *L[i*15+p] = 0;
      *C[i*15+p] = 0;
      // ��������� ������������
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
/* ����������� ���������� ����� ��������� ��� ��� - ����������� ������������ */
bool PLine::Mutual_Conductance_0(Phase Phase1, Phase Phase2, PType Gm0, PType Cm0){
   return Mutual_Conductance(Phase1, Phase2, Gm0*length, Cm0*length);
}
//---------------------------------------------------------------------------
bool PLine::accept(){
   bool ret = true;
   // ������ ���������
   for(int i=_A_; i<=_C_; i++)
      ret = ret & Phase_Params_0((Phase) i, R0[i], L0[i], Gz0[i], Cz0[i]);
   // ����������� �������������
   ret = ret & Mutual_Inductance_0(M0[_AB_], M0[_BC_], M0[_CA_]);
   // ����������� ������������
   ret = ret & Mutual_Conductance_0(_A_, _B_, Gm0[_AB_], Cm0[_AB_]);
   ret = ret & Mutual_Conductance_0(_B_, _C_, Gm0[_BC_], Cm0[_BC_]);
   ret = ret & Mutual_Conductance_0(_C_, _A_, Gm0[_CA_], Cm0[_CA_]);
   update_differential_resistance();
   return ret;
}
//---------------------------------------------------------------------------
void PLine::Save(ostream &stream){		// ���������� ���������� ������� � �����
   // ������� ���������� ������� �����
   sire::Save(stream);
   // ������ ��������� ���� �����
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
void PLine::Read(istream &stream){    // �������� ���������� ������� �� ������
   // ������� ������ ������ ������� �����
   sire::Read(stream);
   // ������ ������ ���� �����
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
	// ������������� ������� ���������
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
   // ������ ���
   Name = "����";
   Describe_Fields();	// ������ �������� �����
   BuildOK = false;      // ���� ������ ������������ ��� �������
}
//---------------------------------------------------------------------------
/* ������� ���� �� ������ ������� */
PGrid::PGrid(PUnit *obj){
   PElement **Block = new  PElement*[1];
   Block[0] = obj;
   if(obj->Internal_Links.Number() == 0)   // ����� ������ ��� ���������
      Link_Elements(1, Block, 0, NULL);
   else{     // ����� ������ ���� �����
      Pins_Link Link;
      Link.add(obj->Internal_Links);
	   unsigned *link = new unsigned [Link.Number() * 4];
      for(unsigned i=0,m=Link.Number();i<m;i++) {
         // ������ ����
         link[4*i] = 0;
         link[4*i + 1] = Link[i].First->Node();
         // ������ ������
         link[4*i + 2] = 0;
         link[4*i + 3] = Link[i].Second->Node();
         }
	   BuildOK = Link_Elements(1, Block, Link.Number(), (unsigned (*)[2][2])link);
      delete [] link;
      }
   delete [] Block;
   // ������ ���
   Name = "����";
   Describe_Fields();	// ������ �������� �����
}
//---------------------------------------------------------------------------
/* ��������� ���������� ��������� */
PGrid::PGrid(Pins_Link Link){
   if(Link.Number() <=0 ){
      err(0, "PGrid: �� �������� ������� ����, �.�. �� ������� �� ����� ����� ����� ����������");
   	return;
   	}
   /*/ � ���� ����� ���� ��������� ������ ���� ���������
   // �������� �����
   for(unsigned i=0, n=Link.Number(); i<n; i++){
      String nameA = Block[i]->Name;
      for(unsigned j=i+1; j<num; j++)
         if( name == Block[j]->Name ){
            err(0, "PGrid: ������ ���� ����� ���� �������� ������ ���� ���������. ���� �� �������");
            return false;
         	} */
   // ���������� ������ ��� ������ ����
   vector<PElement*> Block;
   vector<int> link;
   for(unsigned i=0;i<Link.Number();i++) {
      // ������ ������
      unsigned j, n=Block.size(); // ��� �������� ?
      PElement *e = Link[i].First->Obj();
         for(j=0;j<n;j++)
      	   if(e == Block[j]) break;
         if(j==n){
            Block.push_back(Link[i].First->Obj());
            Link.add(((PUnit*)Link[i].First->Obj())->Internal_Links);   // ����� ���������� ���������� ����� ������ ������ �������
            }
         link.push_back( j );
         link.push_back( Link[i].First->Node() );
      // ������ ������
      n=Block.size(); // ��� �������� ?
      e = Link[i].Second->Obj();
         for(j=0;j<n;j++)
      	   if(e == Block[j]) break;
         if(j==n) {
            Block.push_back(Link[i].Second->Obj());
            Link.add(((PUnit*)Link[i].Second->Obj())->Internal_Links);   // ����� ���������� ���������� ����� ������ ������ �������
            }
         link.push_back( j );
         link.push_back( Link[i].Second->Node() );
      }
	BuildOK = Link_Elements(Block.size(), Block.begin(), Link.Number(), (unsigned (*)[2][2])link.begin());
   // ������ ���
   Name = "����";
   Describe_Fields();	// ������ �������� �����
}
//---------------------------------------------------------------------------
void PGrid::Describe_Fields(){
   /*PFieldInfo fi;
   // ������������� �� �����. ���������
   fi.Name = "������ �������";
   fi.Type = "Pins_List";
   fi.ptr = (void*)this;
   Fields.push_back(fi);*/
}
//---------------------------------------------------------------------------
/* num - ����� ������ ��� ���������� ����
   Block - ������ �� ���� ���������
   Link  - ������, ����������� ��� ������� ����� {  { {�_�����_1, �������_�����_1},{�_�����_2, �������_�����_2} }, ...}
*/
bool PGrid::Link_Elements(unsigned num, PElement *Block[], unsigned numLink,unsigned Link[][2][2]){
	// ������������� ������� ���������
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
   if(num==0){err(0,"PGrid::Link_Elements(): ��� �������� ���� ������������ 0 ���������. ���� �� �������."); return false;}
   // ���������� ���������� ��������
   numElement = num;
   Element = new PElement*[numElement];
   for(unsigned i=0;i<numElement;i++)
      Element[i] = Block[i];
   // ���������� �����
   numLinks = numLink;
   Links = new unsigned [numLink * 4];
   for(unsigned i=0; i<numLink; i++){
      Links[0+4*i] = Link[i][0][0];
      Links[1+4*i] = Link[i][0][1];
      Links[2+4*i] = Link[i][1][0];
      Links[3+4*i] = Link[i][1][1];
   	}
   // �������� ��������� �� ����� ��������� ��������
   for(unsigned i=0; i<numElement; i++){
      String name = Element[i]->FullName();
      for(unsigned j=i+1; j<numElement; j++)
         if( name == Element[j]->FullName() ){
            err(0, "PSubGrid: ������ ���� ����� ���� �������� ������ ���� ���������, ���� �� �������");
            return false;
         	}
      }
   // ������� � ��������� ������ ������� ������ ������ � �����
   u = 0;
   v = 0;
   k = 0;
   for(unsigned i=0; i<num;i++){
      PElement *p = Block[i];
      u += p->M->Rows;     // ����� ����� �����
      v += p->M->Cols;    // ����� ����� ������
      k += p->N->Rows;    // ����� ����� ��������
      }
   //numNode = u;
   //numBranch = v;
   if(u<numLink) {err(0, "PGrid::Link_Elements(): ������������� �������� ������"); return false;};
   if(v==0) {err(0, "PGrid::Link_Elements(): ����� ������ ����� 0"); return false;};
   //u = u - numLink;  // c ������ ������������ �����

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
   // ��������� ������������ ������
   unsigned i_branch=0; // ������� ����� ������ � �������������� �������
   for(unsigned i=0; i<num;i++){
      PElement *p = Block[i];
      unsigned tmp_rows = p->M->Cols;
      for(unsigned j=0; j<tmp_rows; j++){
         Branch[i][j] = i_branch;
         i_branch++;
         }
      }
   // ��������� ������������ ����� (� ������ ������)
   unsigned i_node=0;         // ������� ����� ����� � �������������� �������
   Grounded_Node = i_node;    // ��������� ���� ���� ��� �����������
   bool *link_use = new bool[numLink];
   for(unsigned i=0; i<numLink;i++) link_use[i]=false;
   // �) ���������� ����������� ����
   for(unsigned b=0; b<num;b++){
   	PElement *p = Block[b];
      Node[  b  ][  p->Grounded_Node  ] = Grounded_Node;
      // ������ ��������� ����
      for(unsigned i=0; i<numLink; i++)
   	   if( ! link_use[i])
            if( Link[i][0][0]==b &&   Link[i][0][1]==p->Grounded_Node )
			      linked_nodes(numLink, Link, link_use, Grounded_Node, Link[i][0][0], Link[i][0][1]);
            else if( Link[i][1][0]==b &&  Link[i][1][1]==p->Grounded_Node )
			      linked_nodes(numLink, Link, link_use, Grounded_Node, Link[i][1][0], Link[i][1][1]);
      }
   i_node++;
   // b) ������� ���� ����� ���� ��� ���������� ������������
   for(unsigned i=0; i<numLink; i++)
   	if( ! link_use[i] ){
      	// ��� ����
         Node[  Link[i][0][0]  ][  Link[i][0][1]  ] = i_node;
         // c ��� ���������
			linked_nodes(numLink, Link, link_use, i_node, Link[i][0][0], Link[i][0][1]);//, i_node);//tmp);
      	// ����� � �������������� ������� ����� �� 1 ������
      	i_node++;
      	}
   delete [] link_use;
   // c) ������� ���� ����� ���� �� ���� �� ��������� ����
   for(unsigned i=0; i<num;i++){
      unsigned tmp_rows = Block[i]->M->Rows;
      for(unsigned j=0; j<tmp_rows; j++){
      	// �� �� ���������� ���� ���� ��� ������������?
         if( -1 == Node_From_LocalNode(i,j) ){
         	// ���� ��� �� ��� �����
            Node[i][j] = i_node;
            i_node++;
         	}
      	}
      }
   u = i_node; // ����� ������� ����� ���������
   // ��������� ������� M ������ ������ � ������
   M = new Matrix<char>(u, v);
   unsigned *Short_Branch = new unsigned[v];  // �������-��������� �����, ������ � ����� ������� � ����� ����
   unsigned num_Short_Branch = 0; // ����� �������-��������� ������
   for(unsigned b=0; b<num;b++){
   	PElement *p = Block[b];
      unsigned tmp_rows = p->M->Rows;
      unsigned tmp_cols = p->M->Cols;
      for(unsigned i=0; i<tmp_rows; i++)
      	for(unsigned j=0; j<tmp_cols; j++){
         unsigned br = Branch_From_LocalBranch(b,j);
         unsigned nd = Node_From_LocalNode(b,i);
         if(br>=v || nd>=u)
            {err(0,"PGrid::Link_Elements(): ����� ����(�����) ������ ����� �����(�������)");return false;}
         (*M)[nd][br] += (*p->M)[i][j];	// ���� ����� ���������� ��� �� ��� �������������, �� ������� 0 � �����
         if(fabs((*M)[nd][br]) < 0.01 && fabs((*p->M)[i][j])>0.01){  // ���� ���, �� ��� �������-��������� �����
            Short_Branch[num_Short_Branch]=br;
            num_Short_Branch++;}
      	}
      }

   // ������� 2 ������� ���������� N (2 ����� ��������)
   k = v-u+1;
   N = new Matrix<char>(k,v);
   //if(k != N->Rows){err(0, "�������� ����: ����� �������� �� ������������� ����������� ���������� ��� ������� ��������� ( PGrid() )");}
   unsigned nd = 0;  // ����� ��� ���������� ���������
   for(unsigned b=0; b<num;b++){
   	PElement *p = Block[b];
      unsigned tmp_rows = p->N->Rows;
      unsigned tmp_cols = p->N->Cols;
      for(unsigned i=0; i<tmp_rows; i++){
      	for(unsigned j=0; j<tmp_cols; j++){
            unsigned br = Branch_From_LocalBranch(b,j);
            (*N)[nd][br] = (*p->N)[i][j];
      	   }
         nd++;   // ������� ��������� ���������
         }
      }
   // 2 ����� �������� �� ��������� ����������� ������������ �����
   if( nd < N->Rows ){
   	/* ��� ����������� n ����� (2 �������) ����� ��������� n-1 ����������� ��������� ��������� �������:
      ������������ ���������� 2� ������������ ����� ������������ 2� ������ ������������ �����.
      ��������� ����� ���������� ��� �����, ������� ������������ ��� ���������� ���� ������
      ��������.
      	��� ����������� 2� ����� ������ �������, �� ������ ������������ ���� ����� ���������
      ����� ��������.
      */
      // ���� ���� ���������� ��� ����������� 2x ��������
      bool *Used_Link = new bool[numLink]; for(unsigned i=0;i<numLink;i++) Used_Link[i] = false;
      bool Loop = true; // ���������� ����
      while(Loop){
      	unsigned i;
      	for(i=0; i<numLink;i++) if( !Used_Link[i] )break; // ���� �� �������������� �����
         if (i!=numLink){ // �����
         	unsigned Obj1 = Link[i][0][0];	// ������ ������
         	unsigned Obj2 = Link[i][1][0];   // ������ ������
      		vector<unsigned>  Node_For_Obj1; // ��������� �������������� ���� 1 �������
      		vector<unsigned>  Node_For_Obj2; // ��������� �������������� ���� 2 �������
            Node_For_Obj1.push_back(Link[i][0][1]);
            Node_For_Obj2.push_back(Link[i][1][1]);
            Used_Link[i] = true;
            if( Obj1 == Obj2 ){ //���������� ���� ������ �������
               Matrix<char> Way(1,v);
               Matrix<char> Way1 = Element[Obj1]->M->Way(Node_For_Obj1[0], Node_For_Obj2[0]);
               for(unsigned s=0, n=Way1.Cols; s<n; s++)
                  Way[0][Branch_From_LocalBranch(Obj1, s)] += Way1[0][s];    // Way = Way1
               /*/ ���� ����� ������ 2
               if( Element[Obj1]->Nodes > 2){
                  // �������� ������ �����
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
               else if( Element[Obj1]->Nodes == 2){ // ����� ������ 2
                  Matrix<char> Way1 = Element[Obj1]->M->Way(Node_For_Obj1[0], Node_For_Obj2[0]);
               	for(unsigned s=0, n=Way1.Cols; s<n; s++)
            			Way[0][ Branch_From_LocalBranch(Obj1, s) ] += Way1[0][s];    // Way = Way2
               	}*/
               // ������� ����� ������ � ������� (������ ���� ����� 0)
               unsigned branches=0; for(unsigned s=0;s<v;s++) if(Way[0][s]!=0) branches++;
               // ��������� �����
   //PType t0[2000];
               if(branches > 0){    // >1
            		for(unsigned s=0;s<v;s++) {(*N)[nd][s] = Way[0][s];}//t0[s] = Way[0][s];}
         			nd++;	// ������� ��� ���� ��������� �� 2 ������ ��������
                  if(nd>=N->Rows) Loop = false; // ��������� ����������
         			}
            	}
            else{  // ���������� 2 ������ �������
             // ���� ������ ������������ ���� ��� ����������� ���� �������� ���� � ������
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
            // � Node_For_Obj1 � Node_For_Obj2 ��� ����������� ��������� ����� ������������ ����
            // �������� 2 ����� �������� (���������� ���������� ���� �����)
            // ���� ����� ���������� �� ������ ����� (�=0) �� ���������
            unsigned node_A = Block[Obj1]->Grounded_Node; //Node_For_Obj1[0];
            unsigned node_B = Block[Obj2]->Grounded_Node; //Node_For_Obj2[0];
            Matrix<char> Way_AB = M->Way( Node_From_LocalNode(Obj1, node_A), Node_From_LocalNode(Obj2, node_B));  // node_A � node_� ���� � ��� �� ���� - ���� �.�. �������
            for(unsigned y=0, n=Node_For_Obj1.size(); y<n; y++){  // y=1
               if( Node_For_Obj1[y] != node_A || Node_For_Obj2[y] != node_B ){
            	   // ���������� ����
      			   Matrix<char> Way1 = Element[Obj1]->M->Way(node_A, Node_For_Obj1[y]);
         		   Matrix<char> Way2 = Element[Obj2]->M->Way(node_B, Node_For_Obj2[y]);
                  // ���������� �����
                  Matrix<char> Way(Way_AB);  // Way = way_AB
                  for(unsigned s=0, n=Way2.Cols; s<n; s++){
            		   unsigned br = Branch_From_LocalBranch(Obj2, s);
            		   Way[0][br] += Way2[0][s];    // Way = ay_AB + way2
               	   }
            	   for(unsigned s=0, n=Way1.Cols; s<n; s++){
               	   unsigned br = Branch_From_LocalBranch(Obj1, s);
            		   Way[0][br] -=  Way1[0][s];                      // Way = Way_AB + way2 - way1
                     }
                  // ������� ����� ������ � ������� (������ ���� ����� 0)
                  unsigned branches=0;
                  for(unsigned s=0;s<v;s++) {if(Way[0][s]!=0) branches++;}
                  // ���� ����� ������ ������ 0 ��������� �����
                  //PType t0[3200];
         		   if(branches > 0){    // >1
            		   for(unsigned s=0;s<v;s++) {(*N)[nd][s] = Way[0][s];}//t0[s] = Way[0][s];}
         			   nd++;	// ������� ��� ���� ��������� �� 2 ������ ��������
         			   }
                  if(nd>=N->Rows) {Loop = false; break; } // ��������� ����������
                  } // ������� if
            	}  // ����� ����� ������ ���
             } // ����� ��� 2� ������������ �����
            }
         else
         	Loop = false; // ����������� ��� �����
         }
      delete [] Used_Link;
      /* ����� ������������ ���� � ������� �� ��������� 1 ������� ���������� M
      ��� ������ ���� �� ������������ �����. � ��� ��� ��� ���� ���������� ������� ������.
      ����� ������� ���������� ���� ����� ������:
      1) ����� ���� A 1 ������� � ������� ���� �� ������������� ���� U1 �� ������� �1
      2) ������� ���� �� ���� A �� ������ ���� � 2 ������� �� ������� �
      3) ������� ���� �� � �� ������������� ���� U2 �� ������� �2
      3.1) ���������� ���� {A,U1} {B,U2} � ������ �� �� ���������� ��� �������� ��������
      4) ��������� 2 ����� ��������
      5) ���� ����� ����� ���� ������� ��� ���������������� �����, ����� ��� �� ���������,
      	� ���� ������ ���� � � �������� �.3-5
      6) ���� ����� ����� ����� 0 ������, ����� ��� �� ���������,
      	� ���� ������ ���� � � �������� �.3-5
      7) ���� ����� ����� ��������� ����� ������ ������ ������� - �� ���������, �������� �.3-5
      //
      vector< vector<unsigned> >  First_Node_For_Obj(numElement); // ��������� �������������� ���� ����� - Pairs_Node_For_Obj[Obj][�][0/1]
      vector< vector<unsigned> >  Second_Node_For_Obj(numElement);// ��������� �������������� ����
      for(unsigned i=0; i<numLink;i++){
      	unsigned Obj1 = Link[i][0][0];
         unsigned Obj2 = Link[i][1][0];
      	// A = 0, B = 0, U1 = Link[i][0][1], U2 = Link[i][1][1]
         unsigned node_U1 = Link[i][0][1];
         unsigned node_U2 = Link[i][1][1];
         unsigned node_A = 0;
         unsigned node_B = 0;
         //*Pairs_Node_For_Obj = new vector<unsigned[2]>[numElement];
         Matrix<char> Way(1, v); // ��������� ������
         unsigned branches = 0;	// ����� ������ � �������������� ������
         bool Loop = false;		// ��������� ������ ��� ������ ����� A, B
//PType t0[200];
         do{
            branches=0;
            // �� ������������ �� �� ������ ���� ���� �����
            bool use_1, use_2;
            do{
         	   if(node_A == node_U1)  node_A = node_A++;
               if(node_B == node_U2)  node_B = node_B++;
                  // �� 1 �������
               use_1=false;
               for(unsigned i=0, n=First_Node_For_Obj[ Obj1 ].size(); i<n; i++)
                  if( First_Node_For_Obj[ Obj1 ][i] == node_A  && Second_Node_For_Obj[ Obj1 ][i] == node_U1) {use_1 = true; break;}
                  else if( First_Node_For_Obj[ Obj1 ][i] == node_U1  && Second_Node_For_Obj[ Obj1 ][i] == node_A  ) {use_1 = true; break;}
                  // �� 2 �������
               use_2=false;
               for(unsigned i=0, n=First_Node_For_Obj[ Obj2 ].size(); i<n; i++)
                  if( First_Node_For_Obj[ Obj2 ][i] == node_B  && Second_Node_For_Obj[ Obj2 ][i] == node_U2) {use_2 = true; break;}
                  else if( First_Node_For_Obj[ Obj2 ][i] == node_U2  && Second_Node_For_Obj[ Obj2 ][i] == node_B  ) {use_2 = true; break;}
               if(use_1) node_A = node_A++;
               if(use_2) node_B = node_B++;
               if(   node_A >= Element[Obj1]->Nodes  ||
                     node_B >= Element[Obj2]->Nodes     ){
                     use_1 = false; // � ����� ������� ������� ������
                     use_2 = false;
                     }
               }while( use_1 || use_2);
            if(   node_A >= Element[Obj1]->Nodes  ||
                  node_B >= Element[Obj2]->Nodes     ){ break;}
            // ���������� ��� ����
            First_Node_For_Obj[ Obj1 ].push_back(node_U1);
            Second_Node_For_Obj[ Obj1 ].push_back(node_A);
            First_Node_For_Obj[ Obj2 ].push_back(node_U2);
            Second_Node_For_Obj[ Obj2 ].push_back(node_B);
            // ���������� ����
      		Matrix<char> Way1 = Element[Obj1]->M->Way(node_A, node_U1);
         	Matrix<char> Way2 = Element[Obj2]->M->Way(node_B, node_U2);
            Matrix<char> Way_AB = M->Way( Node_From_LocalNode(Obj1, node_A), Node_From_LocalNode(Obj2, node_B));
            // ��������: � �� ������������� ����� ��������� ������ ������ �������
            unsigned branches_1 = 0;// ����� ������ �� ������� �������
            unsigned branches_2 = 0;// ����� ������ �� ������� �������
            for(unsigned i=0, n=Way1.Cols; i<n; i++){
            	unsigned br = Branch_From_LocalBranch(Obj1, i);
               if( (Way1[0][i] + Way_AB[0][ br ])!=0 ) branches_1++;
               }
            if( branches_1>0 ){ // ���� ����� �� ������� �������
               for(unsigned i=0; i<v; i++)Way[0][i]=Way_AB[0][i]; // Way = way_AB
            	for(unsigned i=0, n=Way2.Cols; i<n; i++){
            		unsigned br = Branch_From_LocalBranch(Obj2, i);
            		Way[0][ br ] = Way2[0][i] + Way_AB[0][ br ];    // Way = ay_AB + way2
               	if( Way[0][ br ]!=0 ) branches_2++;
               	}
               if( branches_2>0){ // ���� ����� �� ������� �������
            		// ���������� ���������
         			for(unsigned i=0, n=Way1.Cols; i<n; i++){
                  	unsigned br = Branch_From_LocalBranch(Obj1, i);
            			Way[0][br] -=  Way1[0][i];                      // Way = ay_AB + way2 - way1
                     }
         			// ������� ����� ������ � ������� (������ ���� ����� 2�)
            		branches=0;
         			for(unsigned i=0;i<v;i++) if(Way[0][i]!=0) branches++;
               	}
               }
            if(branches <= 1 ||	// ����� ����� ��� ���������������� �����,����� ����� �� 0 ������
            	branches_1==0 || branches_2==0){// ��� ����� ���������� ����� ������ ������ ������
               Loop = true; // �������� ������ ��� ���� ������������ ������ ������� ���� � �/��� �
            	node_B++;
               if(node_B >= Element[Obj2]->Nodes){
               	node_B = 0;
               	node_A++;
               	}
               }
            else
            	Loop = false;	// ��������� ����� ������������� ���� ��������, ���� � ������ ���� ������������ �����
         	}while( Loop && node_A<Element[Obj1]->Nodes);  // <=1
         //PType t1[8000];
         if(branches > 1){    // >1
            for(unsigned i=0;i<v;i++) {(*N)[nd][i] = Way[0][i];}//  t1[i]= Way[0][i];}
         	nd++;	// ������� ��� ���� ��������� �� 2 ������ ��������
         	}
         if (nd == N->Rows) break; // ��������� ����������
         } */
   	}
   // 2 ����� �������� ��� ���������������� ������ (���� �� ������� ���������) - � ���������� ������ ������ �������
   if( nd < N->Rows ){
   	unsigned num_use_SB = N->Rows - nd;
      if( num_use_SB>num_Short_Branch ) num_use_SB = num_Short_Branch;
   	for(unsigned i=0; i<num_use_SB;i++){
      	(*N)[nd][ Short_Branch[i] ] = 1;
      	nd++; // ������� ��������� ���������
      	}
   	}
   delete [] Short_Branch;
   if(nd < N->Rows) {err(0, "PGrid::Link_Elements: �� ������� ��������� ����������� ���������� ��������� �� 2 ������ ��������");}
   // ������� ������� ����� � ������� �����������
   _J = new PType[u];
   J = new PType*[u];
   for(unsigned j=0;j<u;j++) {_J[j]=0; J[j]=&_J[j];}
   // �������� ������� �����������
   f = new PType*[u];
   for(unsigned j=0;j<u;j++) {f[j]=NULL;}
   for(unsigned b=0; b<numElement; b++){
      PElement *p = Element[b];
      unsigned tmp_rows = p->Nodes();
      for(unsigned i=0; i<tmp_rows; i++){
         unsigned nd = Node_From_LocalNode(b,i);
         (*J[nd]) += *(p->J[i]);	// ���� ���� ���� ����������, �� �� �������� ���� ������������
         if( f[nd]==NULL )
            f[nd] = p->f[i];
         else  // i-�� ���� �������� p ��� ��������� �� ���� ������� ��������_2 (��� �������)
         	p->f[i] = f[nd];	// ����� ��������� i-��� ����� �������� p, ����� ����� ���������� ���� ��������_2
      	}
      }

   // ������� ���������������� �������������, ��������� �������
   // ������� ��� � ������
   E = new PType*[v];
   // ������� ��������������� �������������
   Z = new PType*[v];
   // ���������� �� �������� � ������ (��������� �������)
   U_C= new PType*[v];
   // ���������� �� �������������� � ������ (��������� �������)
   U_L= new PType*[v];
   // ���������� �� ������������� ��������������������� � ������ (��������� �������)
   U_Lm= new PType*[v];
   // �������� � ������
   D = new Throttle*[v];
   // ���������� �� ��������� � ������
   U_D= new PType*[v];
   // ������� �����������
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
            Lm[i][j] = (PType*)first;  // �� ��������� ��������� �� ��������� ������� ��������������������
   for(unsigned b=0; b<numElement;b++){
   	PElement *p = Element[b];
      //p->accept(); // <-- ��� ������ ���������� �� ������ ����, �.�. ��� �����
      					// �������� ���������� �����, ����������� ��� ������ ����
                     // (���� ���������� �����. PTransformer_3). ����� �������� ��� �� ������
      ((PUnit*)p)->update_differential_resistance(); // �������� ������� Z - ��� �� ��� ��� ������
      unsigned tmp_cols = p->Branches();
      for(unsigned j=0; j<tmp_cols; j++){
         unsigned br = Branch_From_LocalBranch(b,j);
         E[br] = p->E[j];
         Z[br] = p->Z[j];      // ������� ���������������� �������������
         U_C[br] = p->U_C[j];  // ���������� �� �������� � ������ (��������� �������)
         U_L[br] = p->U_L[j];  // ���������� �� �������������� � ������ (��������� �������)
         U_Lm[br] = p->U_Lm[j];// ���������� �� ������������� ��������������������� � ������ (��������� �������)
         D[br] = p->D[j];      // �������� � �����
         U_D[br] = p->U_D[j];  // ���������� �� ��������� � ������
         I[br] = p->I[j];      // ���������� ������� �����
         R[br] = p->R[j];      // �������� ������������� �����
         L[br] = p->L[j];      // ������������� �����
         C[br] = p->C[j];      // ������� �����
         for(unsigned k=0;k<tmp_cols;k++){
            unsigned br2 = Branch_From_LocalBranch(b,k);
            Lm[br][br2] = p->Lm[j][k];  // ������������������� ����� ������� (��������� �� _Lm ��������������� �������, �� ��� ������������� mutual_inductence ����� ��������� �������� ����� ������� _Lm � ��������� �� ����)
            }
      	}
      }
   // �������� ��������������� ����
   _I2= new PType[v];
   for(unsigned i=0;i<v;i++)
        _I2[i] = *I[i];

   update_sources(true); // ������������� ��������� ��������
   update_elements(true);  // ������������� ��������� ��������
   // �������� ������ �������
   for(unsigned b=0; b<numElement;b++){
   	PElement *p = Element[b];
      for(unsigned j=0, n=p->Out.Count(); j<n; j++){
         Pins *unit_pins = &p->Out[j];                	// ��� ������ ������� ��������� �������
         Pins *grid_pins = new Pins(*unit_pins);     		// ������� ������ ������� ��� ���� ��� �����
         grid_pins->Name = p->FullName() + "." + unit_pins->Name; // ������ ���
         Out.add(grid_pins); 										// ��������� ������ ������� � ������
      	}
      }
   return true;
}
//---------------------------------------------------------------------------
PGrid::~PGrid(){
   // ������ ������, ���������� ��� ����������
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
   // ������� ����������� ��������� ������ �������
   for(unsigned i=0, n=Out.Count(); i<n; i++)
   	delete &Out[i];
}
//---------------------------------------------------------------------------
/* ������� ��� ���� ������������ � ��������
   numLink  - ����� ������
   Link[][2][2] - �������� ������
   link_use[] - ����� ���������� ��� ���?
   i_node - ����� ���� ����, ������� ������������� ���� ������������ ����� (������� ����� ������ � �������������� �������)
   obj, node - ����� � ����� �������� ����
*/
void PGrid::linked_nodes(const unsigned numLink, const unsigned Link[][2][2], bool link_use[],const unsigned i_node,const unsigned obj,const unsigned node){//, unsigned &tmp){
   for(unsigned j=0; j<numLink; j++){
   	if( ! link_use[j] ){
      	if( Link[j][0][0] == obj && Link[j][0][1] == node ){ // ���� ������� ���� ���������� � ���
         	// � ��� ���������
            Node[  Link[j][1][0]  ][  Link[j][1][1]  ] = i_node;
            link_use[j]=true;  // ����� ����������
            // ����� � ���� ��������?
            linked_nodes(numLink, Link, link_use, i_node, Link[j][1][0], Link[j][1][1]);
            }
         else if( Link[j][1][0] == obj && Link[j][1][1] == node ){ // ���� ������� ���� ���������� � ���
         	// � ��� ���������
            Node[  Link[j][0][0]  ][  Link[j][0][1]  ] = i_node;
            link_use[j]=true;  // ����� ����������
            // ����� � ���� ��������?
            linked_nodes(numLink, Link, link_use, i_node, Link[j][0][0], Link[j][0][1]);
            }
         }
      }
}
//---------------------------------------------------------------------------
/* ���������� ����� ���������� ������� �� ������ ������� � ���� � ������ ��� ��������� �������   */
unsigned __fastcall PGrid::Node_From_LocalNode(unsigned Obj, unsigned LocalNode) { return Node[Obj][LocalNode]; }
//---------------------------------------------------------------------------
/*  ���������� ����� ���������� ����� �� ������ ������� � ���� � ������ ��� ��������� �����  */
unsigned __fastcall PGrid::Branch_From_LocalBranch(unsigned Obj, unsigned LocalBranch){ return Branch[Obj][LocalBranch]; }
//---------------------------------------------------------------------------
unsigned PGrid::ObjNum_From_Object(PElement *obj){
   for(unsigned i=0; i<numElement; i++)
      if( Element[i] == obj )
         return i;
   return -1;  // ��� ������� � ������
}
//---------------------------------------------------------------------------
/* ���������� ����� ���������� ������� �� ������ ������� � ���� � ������ ��� ��������� �������   */
unsigned __fastcall PGrid::Node_From_LocalNode(PElement* Obj, unsigned LocalNode) {
   unsigned obj_num = ObjNum_From_Object(Obj);
   if(obj_num == -1) return -1;
   return Node[obj_num][LocalNode];
}
//---------------------------------------------------------------------------
/*  ���������� ����� ���������� ����� �� ������ ������� � ���� � ������ ��� ��������� �����  */
unsigned __fastcall PGrid::Branch_From_LocalBranch(PElement* Obj, unsigned LocalBranch){
   unsigned obj_num = ObjNum_From_Object(Obj);
   if(obj_num == -1) return -1;
   return Branch[obj_num][LocalBranch];
}
//---------------------------------------------------------------------------
bool PGrid::Make_Equation(){
   if( !BuildOK ){
      err(0, "PGrid: ��������� ��� ���� �� ����������, �.�. ���� ���� ������� � ��������");
      return false;
      }
   // �������������� ������� �� ���������� ��������
   unsigned n = u-1+k;
   if( n != v) {err(0, "PGrid::Make_Equation: ����� ��������� �� ������� �������� �� ����� ����� ������. ������ � ����.");}
   A = new Matrix<PType>(n, v);
   B = new PType[v];  	// �� ����� ��������� == ����� ������
   U = new PType[v];	// ������� ���������� � ������
   F = new Matrix<PType>( M->Rows-1, M->Cols );
   _U_Lm2 = new PType[v];  		// ������� ���������� �� ������������������� �� �������� ����
   U_Lm2 = new PType*[v];  		// ������� ���������� �� ������������������� �� �������� ����
   for(unsigned i=0;i<v;i++) _U_Lm2[i]=0;
   for(unsigned i=0;i<v;i++) U_Lm2[i]=&_U_Lm2[i];

   // ��������� ������� A �� ���������� ���������
   Update_Equation();

   // �������� ������� ��� ���������� �����������
   Matrix<PType> *tmp = new Matrix<PType>( M->Rows, M->Rows-1 );
   bool *deleted_branch = new bool[M->Cols];
   Copy_Matrix( & M->tree(deleted_branch), tmp);
   //bool as[8];for(int i=0;i<M->Cols;i++) as[i]=deleted_branch[i];
   tmp->Delete_Row((unsigned)(tmp->Rows-1));
   *tmp = -1.0*(tmp->transpose().inverse());

   for(unsigned j=0, k=0; j<F->Cols; j++)
   	if( deleted_branch[j] ){
         // ����� ���� ������� ��� ����������� ������� tmp
         for(unsigned i=0; i<F->Rows; i++) (*F)[i][j] = 0;
      	}
      else{ // ����� ���� ��������� ��� ���������� ������� tmp
   		for(unsigned i=0; i<F->Rows; i++) (*F)[i][j] = (*tmp)[i][k];
         k++; // ����� ��������� ����� � ������� tmp
         }
   delete tmp;
   delete [] deleted_branch;
   return true;
}
//---------------------------------------------------------------------------
void PGrid::Update_Equation(){
   // �������������� ������� �� ���������� ��������
   unsigned n = u-1+k;
   if( n != v) {err(0, "PGrid::Update_Equation: ����� ��������� �� ������� �������� �� ����� ����� ������. ������ � ����.");}
   // ��������� ������� �� ���������� ��������
   // 1 ����� ��������
   for(unsigned i=0; i<u-1; i++)
   	for(unsigned j=0; j<v; j++)
      	(*A)[i][j]= (*M)[i][j];
   // ���� ������������������� ��� 2 ������ ��������
   Matrix<PType> *Nm = new Matrix<PType>(k,v);
   *Nm = (1/smp) * Multiply_Matrix(N, Lm, v, v);
/*PType t0[3][3];
for(int i=0;i<2;i++)
	for(int j=0;j<3;j++)
   	t0[i][j] = (*N)[i][j];*/

   // 2 ����� ��������
   for(unsigned i=0; i<k; i++)
   	for(unsigned j=0; j<v; j++)
     	(*A)[i+u-1][j]= (*N)[i][j]*(*Z[j]) + (*Nm)[i][j];
   delete Nm;

   // ���� ���������
   for(unsigned j=0; j<v; j++)
      if( D[j]!=NULL ){ // ����� � ������� ���������� ��������
      // ��������� �����
      unsigned br = Branch_From_LocalBranch(D[j]->Obj, D[j]->MBranch);
      // ����� ��������� � ������� ������ ����� � ���������
      for(unsigned i=0; i<k; i++){
         if( (*N)[i][j] != 0 ){  // ��������� � ������� ������ ����� � ���������
            int sgn_for_branch = (*N)[i][j];               // ����� ������������ ��� ����������������� � ��������
            (*A)[i+u-1][br] += sgn_for_branch * D[j]->Windings / smp;  // ���� ������� ������ � �������������� �� ��� �����
            }
      	// ����� ���������, � ������� ������ ��������� �����
         if( (*N)[i][br] != 0 ){  // ��������� � ������� ������ ����� � ���������
            int sgn_for_branch = (*N)[i][br];      // ����� ������������ ��� ����������������� � ��������
            (*A)[i+u-1][j] += - sgn_for_branch * D[j]->Windings;	// ���� ���
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
      err(0,"PGrid::Update_Equation(): �������� ������� ������� � ������������ ����� 1%.");
   */
}
//---------------------------------------------------------------------------
// ��������� ���������� �� ��������� ���� � ��� � ������
bool PGrid::update_sources(bool force_update){
   bool ret=false;
   //PType i = *Element[0]->I[1];
   for(unsigned i=0; i<numElement; i++){
      bool ret2 = Element[i]->update_sources(force_update);
      ret = ret | ret2;
      }
   // ���� ����������
   if(ret || force_update){
      // ������� ������� �����
      for(unsigned j=0;j<u;j++) *J[j]=0;
      for(unsigned b=0; b<numElement; b++){
         PElement *p = Element[b];
         unsigned tmp_rows = p->Nodes();
         for(unsigned i=0; i<tmp_rows; i++){
            unsigned nd = Node_From_LocalNode(b,i);
            (*J[nd]) += *(p->J[i]);	// ���� ���� ���� ����������, �� �� �������� ���� ������������
      	   }
         }
      }
   return ret;
}
//---------------------------------------------------------------------------
// ��������� ���������� � ������� ���������� � ������ (���� ���������� ���������)
bool PGrid::update_elements(bool force_update){
   bool ret=false;
   for(unsigned i=0; i<numElement; i++){
      bool  ret_i = Element[i]->update_elements(force_update);
      if(ret_i) Element[i]->update_differential_resistance();  // ���� ���������� �������������, ������� �������� ���������������� �������������
                                                               // � ��������. � ��� ��� ���� ���������� ��������� �� �������� ������������� �
                                                               // ��������, �� ���������� ���������������� ������������� � ������ ����
      ret = ret | ret_i;
      }
   return ret;
}
//---------------------------------------------------------------------------
PElement* PGrid::obj_from_name(String Name){
   String obj, Child;   // ������ � ������ ������� � ���� ������, �������� ���.����2.T2
   // ��������� ��� �� ������ �����
   int point=Name.Pos(".");
   if(point!=0){
      obj = Name.SubString(1,point-1);
      Child = Name.SubString(point+1, Name.Length()-point);
   }else{
      obj = Name;
      Child = "";
      }
   for(unsigned i=0; i<numElement;i++)
      if(Element[i]->Name == obj){   // ����� ������
         if(Child!="")  // ���� ������ ������� � ���� ������
            return ((PGrid*)Element[i])->obj_from_name(Child);
         else
            return Element[i];
         }
   return NULL;   // �� ����� ������ � ���� ������
}
/*/---------------------------------------------------------------------------
void PGrid::calc(){
   // ��������� ���������� �� ��������� ���� � ��� � ������
   update_sources();
   // ��������� ���������� � ������������� ������
   bool elm = update_elements();
   // ���� ���������� ������������� ���������, �� �������� ��������� ���������
   if( elm ) Update_Equation();
	// �� ��������� � ����� ��������������� ������ ���������� ��� �� �������� (��������� �������)
   for(unsigned i=0; i<v; i++)
 		if(*C[i])*U_C[i] = *U_C[i] + *I[i]*smp/(*C[i]);
		else *U_C[i]=0;
   // �� ��������� ��������������� ������ ���������� ��� � �������������� (��������� �������)
   for(unsigned i=0; i<v; i++)
		*U_L[i] = *L[i]*(*I[i])/smp;
   // �� ��������� ��������������� ������ ���������� �������������� ��� �� ����������������� ���������(��������� �������)
   for(unsigned int j=0; j<v; j++)
   	(*U_Lm[j]) = *U_Lm2[j];
   // ������� ������� B �� ��������� A*I=B
   for(unsigned i=0; i<u-1; i++)
      B[i] = *J[i];	// ������ ����� ��������� �� 1 ������ ��������
	for(unsigned i=u-1; i<v; i++){
   	B[i] = 0;
   	for(unsigned j=0; j<v; j++)
			B[i] += (*N)[i-u+1][j]*((*E[j])+(*U_L[j])-(*U_C[j])+(*U_Lm[j])); // ������ ����� �� 2 ������ ��������
      }

   // ������ ����� �������� ����� I=A*B
   Multiply_Matrix( A, B, I);

   // �������� ����������� � ������
   for(unsigned i=0; i<v; i++){
   	U[i] = (*Z[i])*(*I[i]) - (*E[i]) - (*U_L[i]) + (*U_C[i]) - (*U_Lm[i]); // Z �������� ������� ���������� �� ��� � ���, �� �� �������� ������� ���������� �� ��������� (����� ����)
      // ������� ���������� �� �������������������� �� �������� ����
      *U_Lm2[i]=0;
   	for(unsigned int j=0; j<v; j++)
			(*U_Lm2[i]) += (*Lm[i][j])/smp * (*I[j]);
      // ���� ����� ������� ����������
      U[i] += *U_Lm2[i];
      }
	// ������ ����������� �����
   Multiply_Matrix( F, U, f);
   *f[u-1]=0; // ��������� ������ ���������� ���� == 0
}*/
//---------------------------------------------------------------------------
void PGrid::calc(){
   // ��������� ���������� �� ��������� ���� � ��� � ������
   update_sources();
   // ��������� ���������� � ������������� ������
   bool elm = update_elements();
   // ���� ���������� ������������� ���������, �� �������� ��������� ���������
   if( elm ) Update_Equation();
	// �� ��������� � ����� ��������������� ������ ���������� ��� �� �������� (��������� �������)
   for(unsigned i=0; i<v; i++)
 		if(*C[i])*U_C[i] = *U_C[i] + smp/(*C[i])*(_I2[i] + *I[i])/2;
		else *U_C[i]=0;
   // �� ��������� ��������������� ������ ���������� ��� � �������������� (��������� �������)
   for(unsigned i=0; i<v; i++)
		*U_L[i] = *L[i]*(*I[i])/smp;
   // �� ��������� ��������������� ������ ���������� �������������� ��� �� ����������������� ���������(��������� �������)
   for(unsigned int j=0; j<v; j++)
   	(*U_Lm[j]) = *U_Lm2[j];
   // �� ��������� ��������������� ������ ���������� ��� �� ���������
   for(unsigned i=0; i<v; i++)
      if( D[i]!=NULL ){ // �����, � ������� ���������� ��������
         // ��������� �����, �� ������� ������� ��������
         unsigned br = Branch_From_LocalBranch(D[i]->Obj, D[i]->MBranch);
         if(br!=-1) *U_D[i] = *I[br] * D[i]->Windings / smp;
         }
   // ������� ������� B �� ��������� A*I=B
   for(unsigned i=0; i<u-1; i++)
      B[i] = *J[i];	// ������ ����� ��������� �� 1 ������ ��������
	for(unsigned i=u-1; i<v; i++){
   	B[i] = 0;
   	for(unsigned j=0; j<v; j++)
         B[i] += (*N)[i-u+1][j]*( (*E[j])+(*U_L[j])-(*U_C[j])+(*U_Lm[j])+(*U_D[j]) ); // ������ ����� �� 2 ������ ��������
         if(*C[i])    B[i] = B[i] - smp/(*C[i])/2*(*I[i]);
      }

   // ��������� �������������� �������� ����
   for(unsigned i=0; i<v;i++)
        _I2[i] = *I[i];

   // ������ ����� �������� ����� I=A*B
   Multiply_Matrix( A, B, I);

   // �������� ����������� � ������
   for(unsigned i=0; i<v; i++){
   	U[i] = (*Z[i])*(*I[i]) - (*E[i]) - (*U_L[i]) + (*U_C[i]) - (*U_Lm[i]) - (*U_D[i]); // Z �������� ������� ���������� �� ���, ��� � ��������, �� �� �������� ������� ���������� �� ��������� (����� ����)
      if(*C[i])    U[i] = U[i] + smp/(*C[i])/2*_I2[i];
      // ������� ���������� �� �������������������� �� �������� ����
      *U_Lm2[i]=0;
   	for(unsigned int j=0; j<v; j++)
			(*U_Lm2[i]) += (*Lm[i][j])/smp * (*I[j]);
      // ���� ����� ������� ����������
      U[i] += *U_Lm2[i];
      // ������� ���������� �� �������� �� �������� ����
      if(D[i]!=NULL){
         unsigned br = Branch_From_LocalBranch(D[i]->Obj, D[i]->MBranch);
         U[i] += D[i]->Windings * (*I[br]) / smp;
         }
      }
	// ������ ����������� �����
   Multiply_Matrix( F, U, f);
   *f[u-1]=0; // ��������� ������ ���������� ���� == 0

   /*/ ����� ������������ ����
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
   // ��������� ������ �������� ������
	sire::Save(stream);
   // ����� ��������
   stream << numElement << " ";
   // ��������� �������
   for(unsigned i=0; i<numElement; i++)
      SaveObj(Element[i], stream);
   // ����� ������
   stream << numLinks << " ";
   // ��������� ���������� � ������
   for(unsigned i=0; i<numLinks; i++){
      stream << Links[0+4*i] << " ";
      stream << Links[1+4*i] << " ";
      stream << Links[2+4*i] << " ";
      stream << Links[3+4*i] << " ";
   	}
   // ��������� ���������� � ������� �������
   String outs = getOuts();
   stream << StringForSave( &outs )  << " ";
}
//---------------------------------------------------------------------------
void PGrid::Read(istream &stream){
   // ������ ������ �������� ������
	sire::Read(stream);
   PElement **el;
   unsigned *lnk;
   unsigned numEl;
   unsigned numLnk;
   // ����� ��������
   stream >> numEl;
   el = new PElement*[numEl];
   // ������� �������
   char cname[1024]; String ClassName;
   for(unsigned i=0; i<numEl; i++)
      el[i] = CreateObj( stream );
   // ����� ������
   stream >> numLnk;
   lnk = new unsigned[4*numLnk];
   // ��������� ���������� � ������
   for(unsigned i=0; i<numLnk; i++){
      stream >> lnk[0+4*i];
      stream >> lnk[1+4*i];
      stream >> lnk[2+4*i];
      stream >> lnk[3+4*i];
   	}
   // ����� ��������� ���� �������� ���������
   for(unsigned i=0; i< numEl; i++ ) el[i]->accept();
   // ������� ��� ����
   Link_Elements(numEl, el, numLnk, (unsigned (*)[2][2])lnk );
   // ������������� ������ �������
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
// ������� ���������� ������ ������� ����������� � ������ �������
void PGrid::setOuts(String strOut){
   // �������
   Out.Clear();    // ������ ����� �������
   // ������� ������ �������
   //for(int i=0, n=X.size(); i<n; i++) delete X[i];
   //X.clear();
   // ������� ������ �������
	vector<String> Point;
	// �������� ���� ����� ������ ������� � ������
   int numSpace, m;
   for(numSpace=1, m=strOut.Length(); numSpace<=m; numSpace++) if(strOut[numSpace]!=' ') break;
   strOut = strOut.SubString(numSpace, strOut.Length() - numSpace + 1);
   int pos = strOut.Pos(";");
   while( pos ){
   	Point.push_back( strOut.SubString(1,pos-1) );
      strOut = strOut.SubString(pos+1, strOut.Length()-pos);
      // �������� ���� ����� ������ �������
      for(numSpace=1, m=strOut.Length(); numSpace<=m; numSpace++) if(strOut[numSpace]!=' ') break;
      strOut = strOut.SubString(numSpace, strOut.Length() - numSpace + 1);
      pos = strOut.Pos(";");
      }
   Point.push_back( strOut.SubString(1,strOut.Length()) );
   // ���������� ������ ������ �������
   String obj, outName;   // ������ � ������ ������� � ���� ������, �������� ���.����2.T2
   for(unsigned i=0, n=Point.size(); i<n; i++){
   	// ��������� ��� �� ������ �����
   	int point=Point[i].Pos(".");
   	if(point!=0){
      	obj =Point[i].SubString(1,point-1);
      	outName = Point[i].SubString(point+1, Point[i].Length()-point);
         PElement *Obj = obj_from_name(obj);
         if(Obj){
         	// ��������� ������ ������� ����� �������
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
   // �������� ������ �������
   Pins *x = new Pins( pins->Count );
   x->Name = Obj->Name + "." + pins->Name;
   // �������� ������ ����������� �� ����
   for(int i=0, n=pins->Count; i<n; i++){
      PPin tmp_pin;
      unsigned node = Node_From_LocalNode( (*pins)[i].Obj(), (*pins)[i].Node() );
      // ���� ����� ���� ������
      if(node != -1)
         tmp_pin = set_pin( node );
      else{
         err(0, "PGrid: ���� �� ������ � ����");
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
	Describe_Fields(); // ������ ����
   Name = "����";
}
//---------------------------------------------------------------------------
void PSubGrid::Describe_Fields(){
   PFieldInfo fi;
   // ������������� �� �����. ���������
   fi.Name = "������ �������";
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
   // ��������� ������ �������� ������
	sire::Save(stream);
}
//---------------------------------------------------------------------------
void PSubGrid::Read(istream &stream){
   // ������ ������ �������� ������
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
// ������� ���������� ������ ������� ����������� � ������ �������
void PSubGrid::setOuts(String strOut){
   // �������
   Out.Clear();    // ������ ����� �������
   // ������� ������ �������
	vector<String> Point;
	// �������� ���� ����� ������ ������� � ������
   int numSpace, m;
   for(numSpace=1, m=strOut.Length(); numSpace<=m; numSpace++) if(strOut[numSpace]!=' ') break;
   strOut = strOut.SubString(numSpace, strOut.Length() - numSpace + 1);
   int pos = strOut.Pos(";");
   while( pos ){
   	Point.push_back( strOut.SubString(1,pos-1) );
      strOut = strOut.SubString(pos+1, strOut.Length()-pos);
      // �������� ���� ����� ������ �������
      for(numSpace=1, m=strOut.Length(); numSpace<=m; numSpace++) if(strOut[numSpace]!=' ') break;
      strOut = strOut.SubString(numSpace, strOut.Length() - numSpace + 1);
      pos = strOut.Pos(";");
      }
   Point.push_back( strOut.SubString(1,strOut.Length()) );
   // ���������� ������ ������ �������
   String obj, outName;   // ������ � ������ ������� � ���� ������, �������� ���.����2.T2
   for(unsigned i=0, n=Point.size(); i<n; i++){
   	// ��������� ��� �� ��������� �����
      int point=Point[i].Pos(".");
      //for(int i=Point[i].Pos("."); i!=0; i=Point[i].Pos(".")) point = i;
   	if(point!=0){
      	obj =Point[i].SubString(1,point-1);
      	outName = Point[i].SubString(point+1, Point[i].Length()-point);
         PElement *Obj = obj_from_name(obj);
         if(Obj){
         	// ��������� ������ ������� ����� �������
            for(unsigned j=0, m=Obj->Out.Count(); j<m; j++)
               if(Obj->Out[j].Name == outName)
                  Out.add( &Obj->Out[j] );
            }
			}
      }
}
//---------------------------------------------------------------------------
PElement* PSubGrid::obj_from_name(String Name){
   String obj, Child;   // ������ � ������ ������� � ���� ������, �������� ���.����2.T2
   // ��������� ��� �� ������ �����
   int point=Name.Pos(".");
   if(point!=0){
      obj = Name.SubString(1,point-1);
      Child = Name.SubString(point+1, Name.Length()-point);
   }else{
      obj = Name;
      Child = "";
      }
   for(unsigned i=0, n=Element.size(); i<n; i++)
      if(Element[i]->Name == obj){   // ����� ������
         if(Child!="")  // ���� ������ ������� � ���� ������
            return ((PSubGrid*)Element[i])->obj_from_name(Child);
         else
            return Element[i];
         }
   return NULL;   // �� ����� ������ � ���� ������
}
//---------------------------------------------------------------------------
// ���������� �������� ����
bool PSubGrid::Change( Pins_Link &InLink, vector<PSubGrid*> SubGrids ){
   // �� �� �������� ����� ��� ���������, ������� �� "��������"
   for(unsigned i=0, n=Element.size(); i<n; i++)
   	if(Element[i]->Grid==this)
   		Element[i]->Grid = NULL;
   // "�������" �������������� ��������
   Element.clear();
   // �������� �����
   Internal_Links = InLink;
   // �������� �������
   for(int i=0, n=SubGrids.size(); i<n; i++){
      PSubGrid *sg = SubGrids[i];
      // �������, ���� ������� ��� �� ��������
      int i;
      int n=Element.size();
      for(i=0; i<n; i++) if( Element[i]==sg ) break;
      if(i==n){
         if(sg->Grid == NULL )
         	sg->Grid = this;			// ����������� ������� ����� ���������� � ���� ����
      	Element.push_back( sg );}
   	}
   // �������� ������� �������
   for(int i=0, n=InLink.Number(); i<n; i++){
      PElement * A = InLink[i].First->Obj();
      PElement * B = InLink[i].Second->Obj();
      // �������� ������� �, ���� �� �� �������������
      int i;
      int n=Element.size();
      for(i=0; i<n; i++) if( Element[i]==A ) break;
      if(i==n){
      	Element.push_back( A );
         // ���� ����� ��� ����������, �������� �� ��� ��� �������
         // �������, ���� �����-�� ������� �������, ��� ��� ������������ ���,
         // �� �� ������������ �������.
         if(A->Grid == NULL )
         	A->Grid = this;			// ����������� ������� ����� ���������� � ���� ����
         }
      // �������� ������� B, ���� �� �� �������������
      n=Element.size();
      for(i=0; i<n; i++) if( Element[i]==B ) break;
      if(i==n){
      	Element.push_back( B );
         // ���� ����� ��� ����������, �������� �� ��� ��� �������
         // �������, ���� �����-�� ������� �������, ��� ��� ������������ ���,
         // �� �� ������������ �������.
         if(B->Grid == NULL )
         	B->Grid = this;			// ����������� ������� ����� ���������� � ���� ����
         }
      }
   // �������� ��������� �� ����� ���������
   for(unsigned i=0, n=Element.size(); i<n; i++){
      String name = Element[i]->Name;
      for(unsigned j=i+1; j<n; j++)
         if( name == Element[j]->Name ){
            err(0, "PSubGrid: ������ ���� ����� ���� �������� ������ ���� ���������, ���� �� �������");
            return false;
         	}
      }
   // ������ �������������� ������ �� ����
   for(unsigned i=0, n=Out.Count(); i<n; i++){
      PElement *Obj = Out[i][0].Obj();
      int j, m=Element.size();
      for(j=0; j<m; j++) if(Element[j] == Obj) break;
      if(j==m) // ������, �� ������� ��������� ������ �� ������
      	Out.Delete( &Out[i] );	// ������ ������ �������	
   	}
   return true;
}
//---------------------------------------------------------------------------
// ���������� ��� ����� ������ ����. ������� ����� ��������
Pins_Link PSubGrid::Links(){
   Pins_Link   Link;
   LinksForSubGrid( this, Link );
   return Link;
}
//---------------------------------------------------------------------------
// ���������� ������� �������
void PSubGrid::LinksForSubGrid(PSubGrid *grid, Pins_Link &Link){
   // ����� �������� ������ ���� ���� ������� �� �����
   Link.add( grid->Internal_Links );
   // ������� ����� ����� �������� ������ ��������, ������� ����������� ������ ���� ����
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
   /*/ ������ ���� ������ �� ���
   _U_L[0]=0;_U_L[1]=0; _U_L[2]=0;
   _U_C[0]=0;_U_C[1]=0; _U_C[2]=0;
   _R[0]=1; _R[1]=1; _R[2]=3;
   _L[0]=0.001; _L[1]=0.001; _L[2]=0;
   _C[0]=0; _C[1]=0; _C[2]=0;
   _E[0]=0; _E[1]=0; _E[2]=-10;
   _J[0]=0; _J[1]=0; _J[2]=0;*/

   // ������ ������� Z
	for(unsigned i=0; i<v; i++)
   	if(*C[i])
      	*Z[i] = *R[i] + *L[i]/smp + smp/(*C[i]);
      else
      	*Z[i] = *R[i] + *L[i]/smp;

   // ��������� ��������������������
   mutual_inductance(0,1,-1*sqrt(_L[0]*_L[1]));
   // ��������� �������
  Out.add(&X);  // ��������� �� ������ ���������
   X[0] = set_pin(0);
   X[1] = set_pin(1);

   /*/ ���������� �������������
   _Magn = new MCircuit(2,2,1);
   char magn_M[2][2] ={{-1,-1},
                       { 1, 1}};
   char magn_N[1][2] = { 1,-1};
   *_Magn->M = (char*)magn_M;
   *_Magn->N = (char*)magn_N;
   *_Magn->R[0] = *_Magn->R[1] = 1e3/2;
   Magn = new PGrid(_Magn);
   Magn->Make_Equation();

   // ����������� ������������� � ��������
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
// ��������� ���������� �� ��������� ���� � ��� � ������
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
   // ������ ���� ������ �� ���
   _U_L[0]=0;_U_L[1]=0; _U_L[2]=0; _U_L[3]=0; _U_L[4]=0;
   _U_C[0]=0;_U_C[1]=0; _U_C[2]=0; _U_C[4]=0; _U_C[5]=0;
   _R[0]=2; _R[1]=1; _R[2]=2; _R[3]=0; _R[4]=2;
   _L[0]=0; _L[1]=0; _L[2]=0.002; _L[3]=0; _L[4]=0;
   _C[0]=0; _C[1]=0; _C[2]=0; _C[3]=0.0001; _C[4]=0;
   _E[0]=0; _E[1]=1; _E[2]=0; _E[3]=1; _E[4]=0;
   _J[0]=0; _J[1]=0; _J[2]=0; _J[3]=0;

   // ������ ������� Z
	for(unsigned i=0; i<v; i++)
   	if(*C[i])
      	*Z[i] = *R[i] + *L[i]/smp + smp/(*C[i]);
      else
      	*Z[i] = *R[i] + *L[i]/smp;
   // ��������� �������
   Out.add(&X);  // ��������� �� ������ ���������
   X[0] = set_pin(0);
   X[1] = set_pin(3);
}
//---------------------------------------------------------------------------
void __fastcall tstB::SetR(PType value){  // ��������� �������� ������������� �� ����
}
void __fastcall tstB::SetL(PType value){  // ��������� ������������� ����
}
void __fastcall tstB::SetC(PType value){  // ��������� ������� ����
}
void __fastcall tstB::SetGz(PType value){ // ��������� �������� ������������� �� �����
}
void __fastcall tstB::SetCz(PType value){ // ��������� �������� �� �����
}
void __fastcall tstB::SetGm(PType value){ // ��������� �������� ����������� �������������
}
void __fastcall tstB::SetCm(PType value){ // ��������� ����������� ��������
}
void __fastcall tstB::SetLm(PType value){ // ��������� ����������� ��������������������
}
bool tstB::Mutual_Inductance(PType M_ab, PType M_bc, PType M_ca){// �������� ������������� ����� ������������� ������
   return true;
}
bool tstB::Mutual_Conductance(unsigned Phase1, unsigned Phase2, PType Gm, PType Cm){ // ������������ ����� ������������� ������
   return true;
}
bool tstB::Phase_Params(Phase phase, PType __R, PType __L, PType __C, PType Gz, PType Cz){return true;};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------











/*  ������ ���������� ��� �������� ��� ��������� �������� */
//---------------------------------------------------------------------------
void Source_Params::Change(String  description){	// ������� ��������
   Field.clear();
	// ��������� ������ �� :
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
void Source_Params::Save(ostream &stream){			// ���������  ������
   if(Type != ""){
      String param = Get();
	   stream << StringForSave( &param ) << " ";
      }
   else
      stream << "no" << " ";
}
//---------------------------------------------------------------------------
void Source_Params::Read(istream &stream){			// ���������  ������
   char tmp[2048];
   String param;
   stream >> tmp;							// ���������
   param = tmp;
   if( param!="no" ) Change(param);
}
//---------------------------------------------------------------------------
// ������� �������� ��� �� �������� ���������� �� sourceParams
PType _Ea(PElement* obj, double &time){
	// ������ ��������� �������� ���
	Source_Params *sp = &((PSource*)obj)->sourceParams;
   if(sp->Type == "���������")
   	return sp->Field[0];
	else if(sp->Type == "sin"){
   	PType w = 2*M_PI*sp->Field[0];
      return sqrt(2)/sqrt(3)*sp->Field[1]*sin( w*time );
      }
   else if (sp->Type == "�����������"){
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
	// ������ ��������� �������� ���
	Source_Params *sp = &((PSource*)obj)->sourceParams;
   if(sp->Type == "���������")
   	return sp->Field[0];
	else if(sp->Type == "sin"){
   	PType w = 2*M_PI*sp->Field[0];
      return sqrt(2)/sqrt(3)*sp->Field[1]*sin( w*time - 2*M_PI/3);
      }
   else if (sp->Type == "�����������"){
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
	// ������ ��������� �������� ���
	Source_Params *sp = &((PSource*)obj)->sourceParams;
   if(sp->Type == "���������")
   	return sp->Field[0];
	else if(sp->Type == "sin"){
   	PType w = 2*M_PI*sp->Field[0];
      return sqrt(2)/sqrt(3)*sp->Field[1]*sin( w*time + 2*M_PI/3);
      }
   else if (sp->Type == "�����������"){
      PType w = 2*M_PI*sp->Field[0];
      return sqrt(2)/sqrt(3)*( 	sp->Field[1]*sin( w*time + 2*M_PI/3) +
      						         sp->Field[2]*sin( w*time - 2*M_PI/3) +
                                 sp->Field[3]*sin( w*time ) );
      }
   else
   	return 0;
}








// ��������� �������� ����������� �������� (����������������)
PSource::PSource():
   PUnit(16,5,12),
   X(4){
   Ea=_Ea;  // ������� �� ��������� ��� �������� ���
   Eb=_Eb;
   Ec=_Ec;
   set_pins();          // ����������� ������� �� ���������
   Describe_Fields();   // ��������� �������� ������
	// ��������� �������
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
   Name = "G";  // ��� �������
   Grounded_Node = 0;   // ����������� ����
   // ��������  ���������
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
   *R[0] = MIN;   // �������� ���������
   /*/ ������� ����������� �������������
   *R[4] = *R[5] = *R[6] = *R[7] = *R[8] = *R[9] = *R[10] = *R[11] = *R[12] = *R[13] = *R[14] = *R[15] = MAX;
   // ����������� ������������� G (���������� ��� ����������� ������ 2 ������ ��������)
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
/* ��������� ��������-�������� ������� �� ���������� */
void PSource::set_pins(){
   Out.add(&X);  // ��������� �� ������ ���������
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
   // �������� �������������
   for(int i=_N_; i<=_C_; i++){
      if(i==_N_) name = "N";
      else       name = (String)((char)('A'+i-_A_));
      fi.Name = "R_[_" + name + "_]";
      fi.Type = "PType";
      fi.ptr = (void*) &R_[i];
      Fields.push_back(fi);
      }
   // �������������
   for(int i=_N_; i<=_C_; i++){
      if(i==_N_) name = "N";
      else       name = (String)((char)('A'+i-_A_));
      fi.Name = "L_[_" + name + "_]";
      fi.Type = "PType";
      fi.ptr = (void*) &L_[i];
      Fields.push_back(fi);
      }
   // �������
   for(int i=_N_; i<=_C_; i++){
      if(i==_N_) name = "N";
      else       name = (String)((char)('A'+i-_A_));
      fi.Name = "C_[_" + name + "_]";
      fi.Type = "PType";
      fi.ptr = (void*) &C_[i];
      Fields.push_back(fi);
      }
   // �������� �������������
   for(int i=0; i<3; i++){
      switch(i){
         case _AB_: name = "AB"; break;
         case _BC_: name = "��"; break;
         case _CA_: name = "��"; break;
         default:   name = "";
      }
      fi.Name = "M_[_" + name + "_]";
      fi.Type = "PType";
      fi.ptr = (void*) &M_[i];
      Fields.push_back(fi);
      }
   // �������� ������������ �� �����
   for(int i=_A_; i<=_C_; i++){
      fi.Name = "Gz_[_" + (String)((char)('A'+i-_A_)) + "_]";
      fi.Type = "PType";
      fi.ptr = (void*) &Gz_[i];
      Fields.push_back(fi);
      }
   // ��������� ������������ �� �����
   for(int i=_A_; i<=_C_; i++){
      fi.Name = "Cz_[_" + (String)((char)('A'+i-_A_)) + "_]";
      fi.Type = "PType";
      fi.ptr = (void*) &Cz_[i];
      Fields.push_back(fi);
      }
   // �������� ����������� ������������
   for(int i=0; i<3; i++){
      switch(i){
         case _AB_: name = "AB"; break;
         case _BC_: name = "��"; break;
         case _CA_: name = "��"; break;
         default:   name = "";
      }
      fi.Name = "Gm_[_"+ name +"_]";
      fi.Type = "PType";
      fi.ptr = (void*) &Gm_[i];
      Fields.push_back(fi);
      }
   // ��������� ����������� ������������
   for(int i=0; i<3; i++){
      switch(i){
         case _AB_: name = "AB"; break;
         case _BC_: name = "��"; break;
         case _CA_: name = "��"; break;
         default:   name = "";
      }
      fi.Name = "Cm_[_"+ name +"_]";
      fi.Type = "PType";
      fi.ptr = (void*) &Cm_[i];
      Fields.push_back(fi);
      }
}
/*/---------------------------------------------------------------------------
void PSource::R_(PType value){  // ��������� �������� ������������� �� ����
   *R[1] = value;
   *R[2] = value;
   *R[3] = value;
}
void PSource::L_(PType value){  // ��������� ������������� ����
   *L[1] = value;
   *L[2] = value;
   *L[3] = value;
}
void PSource::C_(PType value){  // ��������� ������� ����
   *C[1] = value;
   *C[2] = value;
   *C[3] = value;
}
void PSource::Gz_(PType value){ // ��������� �������� ������������� �� �����
   *R[10] = 1/value;
   *R[11] = 1/value;
   *R[12] = 1/value;
}
void PSource::Cz_(PType value){ // ��������� �������� �� �����
   *C[13] = value;
   *C[14] = value;
   *C[15] = value;
   // �������� �������� ������������� � ���� ������
   if(value!=0)
      *R[13] = *R[14] = *R[15] = 0;
   else
      *R[13] = *R[14] = *R[15] = MAX;
}
void PSource::Gm_(PType value){ // ��������� �������� ����������� �������������
   *R[4] = 1/value;
   *R[5] = 1/value;
   *R[6] = 1/value;
}
void PSource::Cm_(PType value){ // ��������� ����������� ��������
   *C[7] = value;
   *C[8] = value;
   *C[9] = value;
   // �������� �������� ������������� � ���� ������
   if(value!=0)
      *R[7] = *R[8] = *R[9] = 0;
   else
      *R[7] = *R[8] = *R[9] = MAX;
}
void PSource::Lm_(PType value){ // ��������� ����������� ��������������������
   mutual_inductance(1, 2, value);
   mutual_inductance(1, 3, value);
   mutual_inductance(2, 3, value);
}*/
bool PSource::Mutual_Inductance(PType M_ab, PType M_bc, PType M_ca){// �������� ������������� ����� ������������� ������
   mutual_inductance(1, 2, M_ab);
   mutual_inductance(2, 3, M_bc);
   mutual_inductance(1, 3, M_ca);
   return true;
}
bool PSource::Mutual_Conductance(unsigned Phase1, unsigned Phase2, PType Gm, PType Cm){ // ������������ ����� ������������� ������
   PType Rm;	// ����������� �������������
   if(Gm) Rm = 1/Gm;
   else   Rm = MAX;
   // ���������� ����
   unsigned p;
   switch (Phase1 + Phase2){
   	case (_A_ + _B_): p=0; break;
      case (_B_ + _C_): p=1; break;
      case (_C_ + _A_): p=2; break;
      default:	 return false;
      }
   // ��������� ��������
   *R[4+p] = Rm;  *L[4+p] = 0;   *C[4+p] = 0;
   if(Cm) {*C[7+p] = Cm; *R[7+p] = 0;  *L[7+p] = 0;}
   else   {*C[7+p] = Cm; *R[7+p] =MAX; *L[7+p] = 0;}
   return true;
}
bool PSource::Phase_Params(Phase phase, PType __R, PType __L, PType __C, PType Gz, PType Cz){ // ��������� �� 1 ����
   // ���������� ����
   unsigned p;
   switch (phase){
   	case _A_: p=0; break;
      case _B_: p=1; break;
      case _C_: p=2; break;
      default:	 return false;
      }
   // ��������� ��������
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
   // ������ ���������
   for(int i=_A_; i<=_C_; i++)
      ret = ret & Phase_Params((Phase) i, R_[i], L_[i], C_[i], Gz_[i], Cz_[i]);
   // ��������� ��������
   *R[0] = R_[_N_];
   *L[0] = L_[_N_];
   *C[0] = C_[_N_];
   // ����������� �������������
   ret = ret & Mutual_Inductance(M_[_AB_], M_[_BC_], M_[_CA_]);
   // ����������� ������������
   ret = ret & Mutual_Conductance(_A_, _B_, Gm_[_AB_], Cm_[_AB_]);
   ret = ret & Mutual_Conductance(_B_, _C_, Gm_[_BC_], Cm_[_BC_]);
   ret = ret & Mutual_Conductance(_C_, _A_, Gm_[_CA_], Cm_[_CA_]);
   update_differential_resistance();
   return ret;
}
//---------------------------------------------------------------------------
void PSource::Save(ostream &stream){		// ���������� ���������� ������� � �����
   // ������� ���������� ������� �����
   sire::Save(stream);
   // ������ ��������� ���� �����
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
   // �������� ��������� ����������
   sourceParams.Save( stream );
}
//---------------------------------------------------------------------------
void PSource::Read(istream &stream){    // �������� ���������� ������� �� ������
   // ������� ������ ������ ������� �����
   sire::Read(stream);
   // ������ ������ ���� �����
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
   // ������ ��������� ����������
   sourceParams.Read( stream );
}
//---------------------------------------------------------------------------
String PSource::ClassName(){return "PSource";};				// ���������� ��� ������
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------













// ����������� (���������������� �� ���������)
PConsumer::PConsumer(){
   Name = "����";  // ��� �������
   Grounded_Node = 0;   // ����������� ����
   // ��������  ���������
   for(int i=_N_;i<=_C_;i++){
      R_[i] = MAX;            // �� ��������� �������� �� ����������
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
   *R[0] = MIN;   // �������� ���������
   /*/ ������� ����������� �������������
   *R[4] = *R[5] = *R[6] = *R[7] = *R[8] = *R[9] = *R[10] = *R[11] = *R[12] = *R[13] = *R[14] = *R[15] = MAX;
   // �������� ��������� (!=0 ��� ������ 2 ������ �������� ��� ������ �����)
   *R[0] = MIN;
   // �� ��������� �������� ������ �� ����������
   *R[1] = *R[2] = *R[3] = MAX;*/
   // ��������� ��������������
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
/* ��������� ��������-�������� ������� �� ���������� /
void PConsumer::set_pins(){
   Out.add(&X);  // ��������� �� ������ ���������
   X.Name = "X";
   X[ _N_ ] = set_pin(0); //, 0);
   X[ _A_ ] = set_pin(1); //, 1);
   X[ _B_ ] = set_pin(2); //, 2);
   X[ _C_ ] = set_pin(3); //, 3);
}
//---------------------------------------------------------------------------
void __fastcall PConsumer::SetR(PType value){  // ��������� �������� ������������� �� ����
   *R[1] = value;
   *R[2] = value;
   *R[3] = value;
}
void __fastcall PConsumer::SetL(PType value){  // ��������� ������������� ����
   *L[1] = value;
   *L[2] = value;
   *L[3] = value;
}
void __fastcall PConsumer::SetC(PType value){  // ��������� ������� ����
   *C[1] = value;
   *C[2] = value;
   *C[3] = value;
}
void __fastcall PConsumer::SetGz(PType value){ // ��������� �������� ������������� �� �����
   *R[10] = 1/value;
   *R[11] = 1/value;
   *R[12] = 1/value;
}
void __fastcall PConsumer::SetCz(PType value){ // ��������� �������� �� �����
   *C[13] = value;
   *C[14] = value;
   *C[15] = value;
   // �������� �������� ������������� � ���� ������
   *R[13] = *R[14] = *R[15] = 0;
}
void __fastcall PConsumer::SetGm(PType value){ // ��������� �������� ����������� �������������
   *R[4] = 1/value;
   *R[5] = 1/value;
   *R[6] = 1/value;
}
void __fastcall PConsumer::SetCm(PType value){ // ��������� ����������� ��������
   *C[7] = value;
   *C[8] = value;
   *C[9] = value;
   // �������� �������� ������������� � ���� ������
   *R[7] = *R[8] = *R[9] = 0;
}
void __fastcall PConsumer::SetLm(PType value){ // ��������� ����������� ��������������������
   mutual_inductance(1, 2, value);
   mutual_inductance(1, 3, value);
   mutual_inductance(2, 3, value);
}
bool PConsumer::Mutual_Inductance(PType M_ab, PType M_bc, PType M_ca){// �������� ������������� ����� ������������� ������
   mutual_inductance(1, 2, M_ab);
   mutual_inductance(2, 3, M_bc);
   mutual_inductance(3, 1, M_ca);
   return true;
}
bool PConsumer::Mutual_Conductance(unsigned Phase1, unsigned Phase2, PType Gm, PType Cm){ // ������������ ����� ������������� ������
   PType Rm = 1/Gm;	// ����������� �������������
   // ���������� ����
   unsigned p=0;
   switch (Phase1 + Phase2){
   	case (_A_ + _B_): p=0; break;
      case (_B_ + _C_): p=1; break;
      case (_C_ + _A_): p=2; break;
      default:	 return false;
      }
   // ��������� ��������
   *R[4+p] = Rm;
   *C[7+p] = Cm;
   return true;
}
bool PConsumer::Phase_Params(Phase phase, PType __R, PType __L, PType __C, PType Gz, PType Cz){ // ��������� �� 1 ����
   // ���������� ����
   unsigned p=0;
   switch (phase){
   	case _A_: p=0; break;
      case _B_: p=1; break;
      case _C_: p=2; break;
      default:	 return false;
      }
   // ��������� ��������
   *R[1+p] = __R;
   *L[1+p] = __L;
   *C[1+p] = __C;
   *R[10+p] = 1/Gz;
   *C[13+p] = Cz;
   return true;
}*/
//---------------------------------------------------------------------------
/* ����������� ���������� ����� ��������� ����� �� ������ �������� */
bool PConsumer::Load(const PType P,const PType Q, const PType Unom, bool Grounding, const PType freq ){
   PType P_1 = P/3;
   PType r = Unom*Unom/P_1;
   PType Q_1 = Q/3;
   PType l=0, c=0;
   if(Q_1>0) l = Unom*Unom/(Q_1*2*M_PI*freq);
   else c = Q_1/(Unom*Unom*2*M_PI*freq);
   PType m=0;
   if(!Grounding) // ������������� � ������������� ���������
      *R[0] = MAX;
   else
      *R[0] = MIN;
   // ��������� ��������
   for(int i=_A_;i<=_C_;i++){
      R_[i] = r;            // �� ��������� �������� �� ����������
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
String PConsumer::ClassName(){return "PConsumer";};				// ���������� ��� ������
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------















// ������������� ���������� ��� ����� ���� ��������������
PSingleTransformer ::PSingleTransformer():
					PUnit(6, 6, 1),
               X1(2), X2(2){	// ������� 4, �� ������������ ������ � ��������� 1-3, �1[0] == X1[_N_] �� ������������
   set_pins();
  	// ��������� �������
   char _M[6][6] =   {{-1, 0, 0,-1, 0, 0},
                      { 1, 0, 0, 0, 0, 0},
                      { 0,-1, 0, 0,-1, 0},
                      { 0, 1, 0, 0, 0, 0},
                      { 0, 0, 1, 1, 1, 1},
                      { 0, 0,-1, 0, 0,-1}};
	*M = (char*)_M;
   char _N[1][6] =    {{0, 0,-1, 0, 0, 1}};
   *N = (char*)_N;
   /* ������� ������ � ���� ������������� 1 */
   _U1 = 1;
   _U2 = 1;
   _S  = 1;
   _dPk= 0;
   _Uk = 10.5;
   _dPx= 0;
   _Ix = 0.01;
   _freq = 50;
   // ������������ ������ �������
   *R[3] = *R[4] = MAX;
   Name = "���";  // ��� �������
   Grounded_Node = 4;   // ����������� ����
}
//---------------------------------------------------------------------------
void PSingleTransformer :: set_pins(){
   X1 [_N_] = set_pin(1);
   X1 [_A_] = set_pin(0);
   X2 [_N_] = set_pin(3);
   X2 [_A_] = set_pin(2);
   // �����
   X1.Name  = "X1";
   X2.Name  = "X2";
	// ������� � ������ ����� �������
	Out.add(&X1);
   Out.add(&X2);
}
//---------------------------------------------------------------------------
void PSingleTransformer :: Params(PType U1,  PType U2, PType S,  // ��������� ����������
      	                         PType dPk, PType Uk,
                                  PType dPx, PType Ix,
                                  PType freq){
   // ������
   PType Km = 1;				  // ����������� ��������� �����
   _Iu = 1e3;               // , � �������� ��� � ���� �������������� (��������������)
                             // �������: ��� ������ Iu ��� ������ ����������� 2 ���. �������� ��� ���� ��������������
   // ��������
	_U1 = U1;
   _U2 = U2;
   _S  = S;
   _dPk= dPk;
   _Uk = Uk;
   _dPx= dPx;
   _Ix = Ix;
   _freq = freq;

   // ������������
   PType k = U1/U2;        // ����������� �������������
   PType w = 2*freq*M_PI;  // ������� �������

   // ����� ������������
   // �������� ������������� �������
   PType R_sum = dPk * pow(U1,2) / pow(S,2);
   PType R1 = 0.5 * R_sum;
   PType R2 = R1 / pow(k,2);
   // �������� ��������� ����� (��� ����� ������������� ��������� ���xx = Uxx)
   PType Xm1 = U1 / _Iu ;
   PType Xm2 = U2 / _Iu ;
   // ��� ��������� ����
   PType __Ix = Ix/100 * S/U1;   // � �������
   //PType Ru = (dPx - R1 * pow(__Ix,2)) / pow(_Iu,2);
   // ���������� ������ ��������� ����
   PType dQ2 = pow(__Ix*U1,2) - pow(dPx,2);
   if(dQ2<0){
      err(0, "PSingleTransformer::Params(): ��������� �������������� ������ �� �����.������� ��������� ��� �.�. ��� �������� ������. ������ �.�. ����������� ����� �������� ���� �.�.");
      __Ix = dPx/U1;
      _Ix = __Ix / (S/U1) * 100;
      //Ix = _Ix;
      dQ2 = 0;
      }
   PType dQx = sqrt( dQ2 );
   PType q_xx;	// ���� ����� ����� � �����������  ��������� ����
   if(dPx)
      q_xx = atan(dQx/dPx);
   else
      q_xx = M_PI/2;
   // �������� ������������� ���� �������������� (���� ������)
   PType Ru = __Ix * Xm1 * cos(q_xx)/_Iu;
   PType X_pot = __Ix * Xm1 * sin(q_xx)/_Iu;
   // ���������� ������������� ���� �������������� (���� ������)
   //PType X_pot  = (dQx - X1 * pow(__Ix,2) - 2*Xm1*__Ix*_Iu*sin(atan(dQx/dPx))) / pow(_Iu,2);	// ���������� ���������� ������ � ��������������
   // ���������� ������������� �������
   PType Z_sum =  Uk/100 * pow(U1,2) / S;
   PType X_sum2 = pow(Z_sum,2) - pow(R_sum,2);
   if(X_sum2<0){
      err(0, "PSingleTransformer::Params(): ��������� �������������� ������ �� �����.������� ��������� ��� Uk ��� �������� ������. ������ �.�. ����������� ����� �������� Uk");
      _Uk = R_sum * S / pow(U1,2) * 100;
      //Uk = _Uk;
      //Z_sum =  _Uk/100 * pow(U1,2) / S;
      X_sum2 = 0;
      }
   PType X_sum =  sqrt( X_sum2 );
   PType X1 = 0.5*X_sum ;
   //PType X1 = 0.5 * X_sum + Xm1*(1+k)/2;  // ����� �� �������� ���
   PType X2 = X1 / pow(k,2);
   // ������������� � ���� ��������������
   PType Xu1 = pow(Xm1/Km,2) / X1;    		// ��� ��������������� ��������
   PType Xu2 = pow(Xm2/Km,2) / X2;
   PType Xu  = Xu1 + Xu2;
   PType X_Cu = Xu + X_pot;  // ����������� �������
   /*
   PType Z_sum =  Uk/100 * pow(U1,2) / S;
   PType X_sum2 = pow(Z_sum,2) - pow(R_sum,2);
   if(X_sum2<0){
      err(0, "PSingleTransformer::Params(): ��������� �������������� ������ �� �����.������� ��������� ��� Uk ��� �������� ������. ������ �.�. ����������� ����� �������� Uk");
      _Uk = R_sum * S / pow(U1,2) * 100;
      Uk = _Uk;
      Z_sum =  _Uk/100 * pow(U1,2) / S;
      X_sum2 = 0;
      }
   PType X_sum =  sqrt( X_sum2 );
   PType Zxx = U1 / __Ix;

   Km = sqrt (  (2*Zxx - X_sum)/(2*Zxx - 0.5*X_sum)  );

   PType X1 = Zxx / (1 - 0.5*pow(Km,2));
   Xm1 = (k+1)*(X1 - X_sum/2); // �� ������� �������� X1 �� ������� ��� ��� Iu ����� ������

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
   // ������������� ���������
   *R[0] = R1; *L[0] = X1/w;
   *R[1] = R2; *L[1] = X2/w;
   *R[2] = Ru; *L[2] = Xu/w;  *C[2] = 1/(X_Cu*w);
   *R[5] = 0;  *L[5] = 0;
   //_Ru_nom = *C[2];  // ���������� ����������� �������������

   mutual_inductance(0, 2, Xm1/w);
   mutual_inductance(1, 2, Xm2/w);
   update_differential_resistance();
}
//---------------------------------------------------------------------------
String PSingleTransformer::ClassName(){return "PSingleTransformer";}				// ���������� ��� ������








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
   if(Y_X == NULL) exist = false;  // �������� ������ �� ������, � ������� ������� = ������ �� �����
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
   else if( size() < 1 ) // � ������� �������� � ������� ������ ���
      exist = false;
}
//---------------------------------------------------------------------------
PType __fastcall PCurve :: X(PType _Y_){
 if (Y_X==NULL){  // ���� �� ������ ������
   int sgn = 1;
   if( Symmetry ){ // ������ ����������� ������������ ������ ���������
      sgn = (_Y_>0)?1:-1;
      _Y_ = sgn*_Y_;
      }
   PType dY, dX;
   if(_Y.size() == 0) {
      err(0, "PCurve::X(): ������ �� ������. ������� �������� -1.");
      return -1;}
   int n=_Y.size(); // ����� ��������
   int i;
   for(i=0;i<n;i++)
      if( _Y_ < _Y[i]) break;
   if (i==n || i==0){ // �� ������ �� ��������� �������
      err(0, "PCurve::X(): ������ �� ������ ��� �������� Y="+ FloatToStr(_Y_)+". ������� �������� -1.");
      return -1;
      }
   else{
      i--;  // ������� ������ ��������������, �� ������� ��������� ������� �����
      dY = _Y[i+1] - _Y[i]; // ���������� �� �������
      dX = _X[i+1] - _X[i]; // ���������� �� �������
      }
   // ������ ������ ������������ ������������ ������ ���������
   return sgn*(_X[i] + dX/dY * (_Y_-_Y[i]));
 }
 else{
 	// ������� �� ������������� ������
   // �� ������� H(B) = 0
   PType _Ymin=0, _Ymax = 1e10;
   PType e=1e-3; 			// ���������� ����������� ��������
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
         err(0,"PCurve :: X(): �� ������� �������� X �� Y="+FloatToStr(_Y_)+". �� ����� ������������� �������.");
         return c;
      	}
   }while(fabs(a - b) > e || fabs(Y_c - _Y_) > e);
   return c;	// ���������� �������� ��������
 }
}
//---------------------------------------------------------------------------
PType __fastcall PCurve :: Y(PType _X_){
 if(Y_X==NULL){    // ���� �� ������ ������
   int sgn = 1;
   if( Symmetry ){ // ������ ����������� ������������ ������ ���������
      sgn = (_X_>0)?1:-1;
      _X_ = sgn*_X_;
      }
   PType dY, dX;
   if(_Y.size() == 0) {
      err(0, "PCurve::Y(): ������ �� ������. ������� �������� -1.");
      return -1;}
   int n=_X.size(); // ����� ��������
   int i;
   for(i=0;i<n;i++)
      if( _X_ < _X[i]) break;
   if (i==n || i==0){ // �� ������ �� ��������� �������
      err(0, "PCurve::Y(): ������ �� ������ ��� �������� X="+FloatToStr(_X_)+". ������� �������� -1.");
      return -1;
      }
   else{
      i--;  // ������� ������ ��������������, �� ������� ��������� ������� �����
      dY = _Y[i+1] - _Y[i]; // ���������� �� �������
      dX = _X[i+1] - _X[i]; // ���������� �� �������
      }
   // ������ ������ ������������ ������������ ������ ���������
   return sgn*(_Y[i] + dY/dX * (_X_-_X[i]));
 	}
 else{
 	// ������� �� ������������� ������
   return (*Y_X)(_X_);
 }
}
//---------------------------------------------------------------------------
void PCurve::Save(ostream &stream){		// ���������� ���������� ������� � �����
   /* ������ ������� �������� ����� ��������� ��� ����� �����
   � ��� �������� ������� �������� ������, �� ���� ���������� �� ������� */
   // ��������� ������ �������������� �������� ��������
   stream << _X.size() << " ";
   for(int i=0, n=_X.size(); i<n; i++){
      stream << _X[i] << " ";
      stream << _Y[i] << " ";
      }
}
//---------------------------------------------------------------------------
void PCurve::Read(istream &stream){    // �������� ���������� ������� �� ������
   // ������ ������ �������������� �������� ��������
   int n=0;
   stream >> n;
   PType X, Y;
   for(int i=0; i<n; i++){
      stream >> X;   _X.push_back(X);
      stream >> Y;   _Y.push_back(Y);
      }
   //  ���� ������� ������ ������
   if( size() > 0 ) exist = true;
   //else             exist = false;  // �� ����, �.�. ����� ���� ������ ������������� ������� - ��������� ��� ���� 
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
/* ������� ����� ��� ������� ��������������� */
PTransformer::PTransformer(unsigned NumBranches, unsigned NumNodes, unsigned NumCircuit) :
					PUnit(NumBranches, NumNodes, NumCircuit){
   Describe_Fields();   // ������ �������� ������
   /* ������� ������ � ���� ������������� 1 */
   U1 = 1;
   U2 = 1;
   S  = 1;
   dPk= 0;
   Uk = 10.5;
   dPx= 0;
   Ix = MIN;
   freq = 50;
   F_nom = 0.01;
   Name = "��";  // ��� �������
}
//---------------------------------------------------------------------------
void PTransformer::Describe_Fields(){
   PFieldInfo fi;
   // ��������
   fi.Name = "��������";
   fi.Type = "PType";
   fi.ptr = (void*) &S;
   Fields.push_back(fi);
   // ��������� ����������
   fi.Name = "U1";
   fi.Type = "PType";
   fi.ptr = (void*) &U1;
   Fields.push_back(fi);
   // ��������� ����������
   fi.Name = "U2";
   fi.Type = "PType";
   fi.ptr = (void*) &U2;
   Fields.push_back(fi);
   // ���������� ��
   fi.Name = "����. �.�.";
   fi.Type = "PType";
   fi.ptr = (void*) &Uk;
   Fields.push_back(fi);
   // ������ ��
   fi.Name = "���.������ �.�.";
   fi.Type = "PType";
   fi.ptr = (void*) &dPk;
   Fields.push_back(fi);
   // ��� xx
   fi.Name = "��� x.x.";
   fi.Type = "PType";
   fi.ptr = (void*) &Ix;
   Fields.push_back(fi);
   // ������ xx
   fi.Name = "���.������ x.x.";
   fi.Type = "PType";
   fi.ptr = (void*) &dPx;
   Fields.push_back(fi);
   // ����������� �������
   fi.Name = "���.�������";
   fi.Type = "PType";
   fi.ptr = (void*) &freq;
   Fields.push_back(fi);
   // ����������� �����
   fi.Name = "���.�����";
   fi.Type = "PType";
   fi.ptr = (void*) &F_nom;
   Fields.push_back(fi);
}
/*/---------------------------------------------------------------------------
PType PTransformer :: U1()  {return _U1;}                // ������ �������� ���������� ��������� �������
PType PTransformer :: U2()  {return _U2;}
PType PTransformer :: S()   {return _S;}
PType PTransformer :: dPk() {return _dPk;}
PType PTransformer :: Uk()  {return _Uk;}
PType PTransformer :: dPx() {return _dPx;}
PType PTransformer :: Ix()  {return _Ix;}
PType PTransformer :: freq(){return _freq;}
PType PTransformer :: F_nom(){return _F_nom;}
//---------------------------------------------------------------------------
void PTransformer  :: U1(PType value)  {_U1   = value;}      // ��������� ����������
void PTransformer  :: U2(PType value)  {_U2   = value;}
void PTransformer  :: S(PType value)   {_S    = value;}
void PTransformer  :: dPk(PType value) {_dPk  = value;}
void PTransformer  :: Uk(PType value)  {_Uk   = value;}
void PTransformer  :: dPx(PType value) {_dPx  = value;}
void PTransformer  :: Ix(PType value)  {_Ix   = value;}
void PTransformer  :: freq(PType value){_freq = value;}
void PTransformer  :: F_nom(PType value){_F_nom= value;}  */
//---------------------------------------------------------------------------
void PTransformer::Save(ostream &stream){		// ���������� ���������� ������� � �����
   // ������� ���������� ������� �����
   sire::Save(stream);
   // ������ ��������� ���� �����
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
void PTransformer::Read(istream &stream){    // �������� ���������� ������� �� ������
   // ������� ������ ������ ������� �����
   sire::Read(stream);
   // ������ ������ ���� �����
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
String PTransformer::ClassName(){return "PTransformer";};				// ���������� ��� ������









PTransformer_1::PTransformer_1() :
					PTransformer(5, 5, 1),
               X1(2), X2(2){	// ������� 4, �� ������������ ������ � ��������� 1-3, �1[0] == X1[_N_] �� ������������
   set_pins();          // ��������� ������
   Describe_Fields();   // ������ ����
  	// ��������� �������
   char _M[5][5] =   {{-1, 0, 0,-1, 0},
                      { 1, 0, 0, 0, 0},
                      { 0,-1, 0, 0,-1},
                      { 0, 1, 0, 0, 0},
                      { 0, 0, 0, 1, 1}};
	*M = (char*)_M;
   char _N[1][5] =    {{0, 0, 1, 0, 0}};
   *N = (char*)_N;
   //mCurve.set(0,0);
   //mCurve.set(1e3,1e6);  // �������� ������ �������������� � ���������� ��� ��������� ��������
   //_Bnom = 1.2; 		// ��������� ���������� �������������� ��� ������ ����������� ��������
   //Params(_U1,_U2,_S,_dPk,_Uk,_dPx,_Ix,_freq);
   // ������������ ������ �������
   *R[3] = *R[4] = MAX;
   // ���������� ��������� ����� �� ������ ������������� ������
   Name = "���";  // ��� �������
   Grounded_Node = 4;   // ����������� ����
}
//---------------------------------------------------------------------------
void PTransformer_1 :: set_pins(){
   X1 [_N_] = set_pin(1);
   X1 [_A_] = set_pin(0);
   X2 [_N_] = set_pin(3);
   X2 [_A_] = set_pin(2);
   // �����
   X1.Name  = "X1";
   X2.Name  = "X2";
	// ������� � ������ ����� �������
	Out.add(&X1);
   Out.add(&X2);
}
//---------------------------------------------------------------------------
void PTransformer_1::Describe_Fields(){
   PFieldInfo fi;
   // ������ ��������������
   fi.Name = "I(�)";
   fi.Type = "PCurve";
   fi.ptr = (void*) &mCurve;
   Fields.push_back(fi);
}
//---------------------------------------------------------------------------
bool PTransformer_1 :: accept(){
   if(Ix==0) {
   	err(0, "PTransformer_1: ��� �.�. �� ����� ���� ������ ����");
      Ix=MIN;
      return false;
   	}
   // ������������
   PType k = U1 / U2;        // �������������� ����������� �������������
   PType w = 2*freq*M_PI;     // ������� �������

   // �������� ������������� �������
   PType R_sum = dPk * pow(U1,2) / pow(S,2);
   PType R1 = 0.5 * R_sum;
   PType R2 = R1 / pow(k,2);
   // ��� ��������� ���� (� �������)
   PType __Ix = Ix/100 * S/U1;   // � �������
   // ���������� ������ ��������� ����
   PType dQ2 = pow(__Ix*U1,2) - pow(dPx,2);
   if(dQ2<0){
      err(0, "PTransformer_1::accept(): ��������� �������������� ������ �� �����.������� ��������� ��� �.�. ��� �������� ������. ������ �.�.");
      return false;
      }
   PType dQx = sqrt( dQ2 );
   PType q_xx;	// ���� ����� ����� � �����������  ��������� ����
   if(dPx) q_xx = atan(dQx/dPx);
   else    q_xx = M_PI/2;
   // ����� ������ �������
   n1 = U1 / (w*F_nom);
   n2 = n1 / k;
   // ������������� ���� �������������� (���� ������)
   PType Zu = __Ix * n1 / F_nom;
   PType Ru = Zu * sin(q_xx);
   PType Xu = Zu * cos(q_xx);
   // ���������� ������������� �������
   PType Z_sum =  Uk/100 * pow(U1,2) / S;
   PType X_sum2 = pow(Z_sum,2) - pow(R_sum,2);
   if(X_sum2<0){
      err(0, "PTransformer_1::accept(): ��������� �������������� ������ �� �����.������� ��������� ��� Uk ��� �������� ������. ������ �.�.");
      return false;
      }
   PType X_sum =  sqrt( X_sum2 );
   PType dX1 = X_sum / 2;   // ������������� ��������� ��������� �������
   PType dX2 = dX1 / pow(k,2);      // ������������� ��������� ��������� ������� */


   // ������������� ���������
   *R[0] = R1; *L[0] = dX1/w;
   *R[1] = R2; *L[1] = dX2/w;
   *R[2] = Ru; *L[2] = Xu/w;
   magnetic_link(0,2,n1);
   magnetic_link(1,2,n2);
   update_differential_resistance();   // ������������� ���������������� �������������
   return true;
}
//---------------------------------------------------------------------------
bool PTransformer_1 :: update_elements(bool force_update){
   // ��������� ��������� ����� � ������������ � ������ ��������������
   if( !mCurve.exist )  // ������ �� ������ ��������������
      return false;
   else{
   	PType Ru;
      PType _F = *I[2];          	// �������� ������
      if(!_F) return false;
      PType _Ip = mCurve.I(_F);   	// ����� ������� ���� �����. ���������� ������������ ���� ��������� ����
      PType _U = *f[0] - *f[1];		// ���������� �� �������
      PType dRxx = pow(U1,2)/dPx;	// ����. ������������������ ����� ������ �� ������� � ��� ���� ���� ��.
      PType _Ia = _U / dRxx;			// ���������� ������� ������� ���� ��
      PType _I = _Ia + _Ip; 			// �����. �������� ���� ��

      static PType _F_1 = 0;
      PType d_F = (_F - _F_1)/smp;
      Ru = (n1*_I - *L[2]*d_F)/_F; // ������������� �������� ������������� ��������������, ������� ������ �� ������
      if(Ru<0)	Ru=0;						// ���������. �� �.�. �����. - ��� ����������� �������
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
void PTransformer_1::Save(ostream &stream){		// ���������� ���������� ������� � �����
   // ������� ���������� ������� �����
   sire::Save(stream);
   // ��������� ������ ��������������
   mCurve.Save(stream);
}
//---------------------------------------------------------------------------
void PTransformer_1::Read(istream &stream){    // �������� ���������� ������� �� ������
   // ������� ������ ������ ������� �����
   sire::Read(stream);
   // ������ ������ ���� �����
   // ������ ������ ��������������
   mCurve.Read(stream);
}
//---------------------------------------------------------------------------
String PTransformer_1::ClassName(){return "PTransformer_1";};				// ���������� ��� ������
















// ������������� (�� ��������� ���������������)
PTransformer_3 ::PTransformer_3():
					PTransformer(23, 18, 6),
               X1(4), X2(4), X1n(4), X2n(4){	// ������� 4, �� ������������ ������ � ��������� 1-3, �1[0] == X1[_N_] �� ������������
	set_pins();	         // ��������� �������
   Describe_Fields();   // ��������� ����
   	// ��������� �������
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
                      { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-1, 0, 0, 0, 0, 0, 1, 0},   // ������������� � �������� ������ �������
                      { 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-1, 0, 0, 0, 0, 0, 1}};  // ������������� � �������� ������ �������
   *N = (char*)_N;
   mType = Armored;           // ��� �������������� - ��������
   Winding_X1 = Winding_X2 = wStar;   // ����� ���������� ������� - ������
   // ������� ������� ����������� (����� ������� �������������)
   *R[9] = *R[10] = MAX;
	// ������� � ��������� � ���� ����� ����������� �������������
   *R[6] = *R[7] = *R[8] = *R[11] = *R[12] = *R[13] = *R[14] = MIN;
   // ������������� ��������
   *L[6] = *L[7] = *L[8] = 0.63; // �����
   // ������������� ������� ������� �1
   *L[0] = *L[2] = *L[4] = 0.25;
   // ������������� ������� ������� �2 (����������� ������������� ������������ ������������ ��������������)
   *L[1] = *L[3] = *L[5] = 0.25;
   // ������������ ������ �������
   *R[15] = *R[16] = *R[17] = *R[18] = *R[19] = *R[20] = MAX;
   // ������������� � �������� (������������)
   *R[21] = *R[22] = MAX; // ��� ���������� ��� ��� ��� �� �������� ������� �� ������ �� ������� �������� � �������
   // ��������� ������� ��������������
   Link_Windings();
   Name = "��";  // ��� �������
   Grounded_Node = 17;   // ����������� ����
}
//---------------------------------------------------------------------------
void PTransformer_3 :: set_pins(){
   X1 [_N_] = set_pin(17);
   X1 [_A_] = set_pin(0);
   X1 [_B_] = set_pin(4);
   X1 [_C_] = set_pin(8);
   X1n[_N_] = set_pin(1);// ��-�� ��� �� ��������
   X1n[_A_] = set_pin(1);
   X1n[_B_] = set_pin(5);
   X1n[_C_] = set_pin(9);
   X2 [_N_] = set_pin(17);
   X2 [_A_] = set_pin(2);
   X2 [_B_] = set_pin(6);
   X2 [_C_] = set_pin(10);
   X2n[_N_] = set_pin(3); //��-�� ��� �� ��������
   X2n[_A_] = set_pin(3);
   X2n[_B_] = set_pin(7);
   X2n[_C_] = set_pin(11);
   // �����
   X1.Name  = "X1";
   X2.Name  = "X2";
   X1n.Name = "X1n";
   X2n.Name = "X2n";
	// ������� � ������ ����� �������
	Out.add(&X1);
   Out.add(&X2);
   Out.add(&X1n);
   Out.add(&X2n);
}
//---------------------------------------------------------------------------
void PTransformer_3::Describe_Fields(){
   PFieldInfo fi;
   // ��� ��������� �������
   fi.Name = "����.����.";
   fi.Type = "MagnCircuit_Type";
   fi.ptr = (void*) &mType;
   Fields.push_back(fi);
   // ����� ���������� ��������� �������
   fi.Name = "����.���.";
   fi.Type = "Winding_Connect";
   fi.ptr = (void*) &Winding_X1;
   Fields.push_back(fi);
   // ����� ���������� ��������� �������
   fi.Name = "����.���.";
   fi.Type = "Winding_Connect";
   fi.ptr = (void*) &Winding_X2;
   Fields.push_back(fi);
   // ������ ��������������
   fi.Name = "I(�)";
   fi.Type = "PCurve";
   fi.ptr = (void*) &mCurve;
   Fields.push_back(fi);
}
//---------------------------------------------------------------------------
void __fastcall PTransformer_3 :: R1(PType value){  // ��������� �������� ������������� �� ����
   *R[0] = *R[2] = *R[4] = value;
}
void __fastcall PTransformer_3 :: L1(PType value){  // ��������� ������������� ����
   *L[0] = *L[2] = *L[4] = value;
}
void __fastcall PTransformer_3 :: C1(PType value){  // ��������� ������� ����
   *C[0] = *C[2] = *C[4] = value;
}
void __fastcall PTransformer_3 :: Gz1(PType value){ // ��������� �������� ������������� �� �����
   *R[15] = *R[17] = *R[19] = 1/value;
}
void __fastcall PTransformer_3 :: Cz1(PType value){ // ��������� �������� �� �����
   *C[15] = *C[17] = *C[19] = value;
}
PType PTransformer_3 :: R1(Phase ph){  // ��������� �������� ������������� �� ����
   switch(ph){
      case _A_: return *R[0];
      case _B_: return *R[2];
      case _C_: return *R[4];
      case _N_: return *R[21];
      default:  err(0,"PTransformer::R1(): �� ����� ������� ����"); return null;
   }
}
PType PTransformer_3 :: L1(Phase ph){  // ��������� ������������� ����
   switch(ph){
      case _A_: return *L[0];
      case _B_: return *L[2];
      case _C_: return *L[4];
      case _N_: return *L[21];
      default:  err(0,"PTransformer::R1(): �� ����� ������� ����"); return null;
   }
}
PType PTransformer_3 :: C1(Phase ph){  // ��������� ������� ����
   switch(ph){
      case _A_: return *C[0];
      case _B_: return *C[2];
      case _C_: return *C[4];
      case _N_: return *C[21];
      default:  err(0,"PTransformer::R1(): �� ����� ������� ����"); return null;
   }
}
PType PTransformer_3 :: Gz1(Phase ph){ // ��������� �������� ������������� �� �����
   switch(ph){
      case _A_: return 1/(*R[15]);
      case _B_: return 1/(*R[17]);
      case _C_: return 1/(*R[19]);
      default:  err(0,"PTransformer::R1(): �� ����� ������� ����"); return null;
   }
}
PType PTransformer_3 :: Cz1(Phase ph){ // ��������� �������� �� �����
   switch(ph){
      case _A_: return *C[15];
      case _B_: return *C[17];
      case _C_: return *C[19];
      default:  err(0,"PTransformer::R1(): �� ����� ������� ����"); return null;
   }
}
//---------------------------------------------------------------------------
void __fastcall PTransformer_3 :: R2(PType value){  // ��������� �������� ������������� �� ����
   *R[1] = *R[3] = *R[5] = value;
}
void __fastcall PTransformer_3 :: L2(PType value){  // ��������� ������������� ����
   *L[1] = *L[3] = *L[5] = value;
}
void __fastcall PTransformer_3 :: C2(PType value){  // ��������� ������� ����
   *C[1] = *C[3] = *C[5] = value;
}
void __fastcall PTransformer_3 :: Gz2(PType value){ // ��������� �������� ������������� �� �����
   *R[16] = *R[18] = *R[20] = 1/value;
}
void __fastcall PTransformer_3 :: Cz2(PType value){ // ��������� �������� �� �����
   *C[16] = *C[18] = *C[20] = value;
}
PType PTransformer_3 :: R2(Phase ph){  // ��������� �������� ������������� �� ����
   switch(ph){
      case _A_: return *R[1];
      case _B_: return *R[3];
      case _C_: return *R[5];
      case _N_: return *R[22];
      default:  err(0,"PTransformer::R1(): �� ����� ������� ����"); return null;
   }
}
PType PTransformer_3 :: L2(Phase ph){  // ��������� ������������� ����
   switch(ph){
      case _A_: return *L[1];
      case _B_: return *L[3];
      case _C_: return *L[5];
      case _N_: return *L[22];
      default:  err(0,"PTransformer::R1(): �� ����� ������� ����"); return null;
   }
}
PType PTransformer_3 :: C2(Phase ph){  // ��������� ������� ����
   switch(ph){
      case _A_: return *C[1];
      case _B_: return *C[3];
      case _C_: return *C[5];
      case _N_: return *C[22];
      default:  err(0,"PTransformer::R1(): �� ����� ������� ����"); return null;
   }
}
PType PTransformer_3 :: Gz2(Phase ph){ // ��������� �������� ������������� �� �����
   switch(ph){
      case _A_: return 1/(*R[16]);
      case _B_: return 1/(*R[18]);
      case _C_: return 1/(*R[20]);
      default:  err(0,"PTransformer::R1(): �� ����� ������� ����"); return null;
   }
}
PType PTransformer_3 :: Cz2(Phase ph){ // ��������� �������� �� �����
   switch(ph){
      case _A_: return *C[16];
      case _B_: return *C[18];
      case _C_: return *C[20];
      default:  err(0,"PTransformer::R1(): �� ����� ������� ����"); return null;
   }
}
//---------------------------------------------------------------------------
/* ������������ ��������� ����� ��������� */
bool PTransformer_3 :: accept(){
   if(Ix==0) {
   	err(0, "PTransformer_1: ��� �.�. �� ����� ���� ������ ����");
      Ix=MIN;
      return false;
   	}
   // ��������� ������������ �� ����
   PType U1_f,  U2_f, S_f;    // ����������� ���������� � �������� ��������������
   PType dPk_f, Uk_f;  		   // ������ ��
   PType dPx_f, Ix_f;			   // ������ ��������� ����
   // ������� ��� ������� ������������ � ������
   // ����� ���������� �������������� ������������ � ������ (! �� ������ ����������� ����� ������)
   S_f = S/3;
   dPx_f = dPx/3;
   dPk_f = dPk/3;
   U1_f =  U1/sqrt(3);
   U2_f =  U2/sqrt(3);

   // ������������
   PType k = U1_f / U2_f;        // �������������� ����������� �������������
   PType w = 2*freq*M_PI;     // ������� �������

   // 1. ������� ���������� �� 1 ����
   // �������� ������������� �������
   PType R_sum = dPk_f * pow(U1_f,2) / pow(S_f,2);
   PType R1 = 0.5 * R_sum;
   PType R2 = R1 / pow(k,2);
   // ��� ��������� ���� (� �������)
   PType __Ix = Ix/100 * S_f/U1_f;   // � �������
   // ���������� ������ ��������� ����
   PType dQ2_f = pow(__Ix*U1_f,2) - pow(dPx_f,2);
   if(dQ2_f<0){
      err(0, "PTransformer_3::accept(): ��������� �������������� ������ �� �����.������� ��������� ��� �.�. ��� �������� ������. ������ �.�.");
      return false;
      }
   PType dQx_f = sqrt( dQ2_f );
   PType q_xx;	// ���� ����� ����� � �����������  ��������� ����
   if(dPx) q_xx = atan(dQx_f/dPx_f);
   else    q_xx = M_PI/2;
   // ����� ������ �������
   n1 = U1_f / (w*F_nom);
   n2 = n1 / k;
   // ������������� ���� �������������� (���� ������)
   PType Zu = __Ix * n1 / F_nom;
   PType Ru = Zu * sin(q_xx);
   PType Xu = Zu * cos(q_xx);
   // ���������� ������������� �������
   PType Z_sum =  Uk/100 * pow(U1_f,2) / S_f;
   PType X_sum2 = pow(Z_sum,2) - pow(R_sum,2);
   if(X_sum2<0){
      err(0, "PTransformer_3::accept(): ��������� �������������� ������ �� �����.������� ��������� ��� Uk ��� �������� ������. ������ �.�.");
      return false;
      }
   PType X_sum =  sqrt( X_sum2 );
   PType dX1 = X_sum / 2;           // ������������� ��������� ��������� �������
   PType dX2 = dX1 / pow(k,2);      // ������������� ��������� ��������� ������� */
   // 2. �������� �������
   if( Winding_X1 == wTriangle ){ // ���� ������� ���� ��������� � �����������, �� ������������� ���������
      R1 = 3*R1;
      dX1 = 3*dX1;
      n1 = n1 * sqrt(3);
      }
   if( Winding_X2 == wTriangle ){ // ���� ������� ���� ��������� � �����������, �� ������������� ���������
      R2 = 3*R2;
      dX2 = 3*dX2;
      n2 = n2 * sqrt(3);
      }
   // 3. �������� ������������� ��������� �������
   PType Ru_st, Ru_ya;                 // ��������� ������������� ������� � ����
   PType Xu_st, Xu_ya;
   if( mType == Three_Rod ){           // ���� ��������������
      Ru_st = 0.8 * Ru;
      Xu_st = 0.8 * Xu;
      Ru_ya = Ru_st / 8;
      Xu_ya = Xu_st / 8;
      *R[9] = *R[10] = MAX;            // ������������� ������� ��������
      }
   else if( mType == Armored ){        // ���� ������������� ��������� �������
      Ru_st = Ru / 1.223;
      Xu_st = Xu / 1.223;
      Ru_ya = Ru_st / 8;
      Xu_ya = Xu_st / 8;
      *R[9] = *R[10] = 8*Ru_st + Ru_ya;// ������������� ������� ��������
      *L[9] = *L[10] = 8*Xu_st + Xu_ya;
      }
   else{
      err(0,"PTransformer_3::accept(): ��� ��������� ������� �� ��������");
      return false;
      }
   // ������������� ���������
   // > ��������� �������
   *R[0] = *R[2] = *R[4] = R1;
   *L[0] = *L[2] = *L[4] = dX1/w;
   // > ��������� �������
   *R[1] = *R[3] = *R[5] = R2;
   *L[1] = *L[3] = *L[5] = dX2/w;
   // > �������
   *R[6] = *R[7] = *R[8] = Ru_st;
   *L[6] = *L[7] = *L[8] = Xu_st/w;
   // > ����
   *R[11] = *R[12] = *R[13] = *R[14] = Ru_ya;
   *L[11] = *L[12] = *L[13] = *L[14] = Xu_ya/w;
   // > ��������� ����� ��������� �������
   magnetic_link(0,6,n1);
   magnetic_link(2,7,n1);
   magnetic_link(4,8,n1);
   // > ��������� ����� ��������� �������
   magnetic_link(1,6,n2);
   magnetic_link(3,7,n2);
   magnetic_link(5,8,n2);
   // ������ �������
   if(!Link_Windings()) return false;
   update_differential_resistance();   // ������������� ���������������� �������������
   return true;
}
//---------------------------------------------------------------------------
/* �������� ������������� ���� �������������� */
bool PTransformer_3 :: update_elements(bool force_update){
   // ��������� ��������� ����� � ������������ � ������ ��������������
   if( !mCurve.exist )  // ������ �� ������ ��������������
      return false;
   else{
      #define PT3_FST_Ru     6      // ������ �� ������ ��������� �����
      #define PT3_NUM_Ru     9      // ����� ��������� ������
      PType _F,Ru;      	// �������� ������ � ���������������� ��� �������������
      for(unsigned i= 0; i< PT3_NUM_Ru; i++ ){
         _F = *I[ i + PT3_FST_Ru ];
         if(_F!=0){
         	Ru  = n1*mCurve.I(_F)/_F; 		// ������������� ������ ���������������
         	if(Ru>=0) *R[ i + PT3_FST_Ru ] = Ru;   // ��������� �������������
         	}
         }
      return true;
      /*/PType _Ip[ PT3_NUM_Ru ];   	// ����� ������� ���� �����. ���������� ������������ ���� ��������� ����
      PType _U[3];                  // ���������� �� �������� ��������
      _U[0] = *f[0] - *f[1];        // ���� �
      _U[1] = *f[4] - *f[5];        // ���� �
      _U[2] = *f[8] - *f[9];        // ���� �

      PType dRxx = pow(_U1,2)/_dPx;	// ����. ������������������ ����� ������ �� ������� � ��� ���� ���� ��.  = pow(_U1/sqrt(3),2) / (_dPx/3)
      //PType _Ia[3];			      // ���������� ������� ������� ���� ��
      PType _I[PT3_NUM_Ru];	      // �����. �������� ���� �������������� � ���������, ���������� �� ��������� �����
      for(unsigned i=0; i<3; i++ )
         _I[i] = _U[i]/dRxx + mCurve.I(_F[i]);    // ���. � ����� ������� ���������

      PType Ru[PT3_NUM_Ru];
      for(unsigned k=0;k< PT3_NUM_Ru;k++)
      	if(_F[k]!=0) Ru[k]  = _n1*mCurve.I(_F[k])/_F[k]; // ������������� ������ ���������������
      // ������������� �������� ������������� ��������������, ������� ������ �� ������
      //int k; // ����� �������������� �����
      // �������
      //k=0; if(_F[k]!=0)   Ru[k]  = *R[k+PT3_FST_Ru]+(_n1*_I[k] - _n1*(*I[0]))/_F[k];  	// ���� A
      //k=0; if(_F[k]!=0)   Ru[k]  = (_n1*_I[k] - )/_F[k];
      //k=0; if(_F[k]!=0)   Ru[k]  = (_n1*_I[k] - *L[k+PT3_FST_Ru]*d_F[k])/_F[k];   // ���� A
      //k=1; if(_F[k]!=0)   Ru[k]  = (_n1*_I[k] - *L[k+PT3_FST_Ru]*d_F[k])/_F[k];   // ���� B
      //k=2; if(_F[k]!=0)   Ru[k]  = (_n1*_I[k] - *L[k+PT3_FST_Ru]*d_F[k])/_F[k];   // ���� B
      /*if(mType == Armored){   // ���� �������� �������������
         k=3; if(_F[k]!=0)   Ru[k]  = (    0     - (*f[13]-*f[12]) - *L[k+PT3_FST_Ru]*d_F[k])/_F[k];   // �������� ���
         k=4; if(_F[k]!=0)   Ru[k]  = (    0     - (*f[17]-*f[16]) - *L[k+PT3_FST_Ru]*d_F[k])/_F[k];
      } else
         Ru[3] = Ru[4] = 1e100;
      // ����
      k=5; if(_F[k]!=0)   Ru[k]  = (    0     - (*f[14]-*f[12]) - *L[k+PT3_FST_Ru]*d_F[k])/_F[k];
      k=6; if(_F[k]!=0)   Ru[k]  = (    0     - (*f[15]-*f[13]) - *L[k+PT3_FST_Ru]*d_F[k])/_F[k];
      k=7; if(_F[k]!=0)   Ru[k]  = (    0     - (*f[16]-*f[14]) - *L[k+PT3_FST_Ru]*d_F[k])/_F[k];
      k=8; if(_F[k]!=0)   Ru[k]  = (    0     - (*f[17]-*f[15]) - *L[k+PT3_FST_Ru]*d_F[k])/_F[k];
      */
      }
}
//---------------------------------------------------------------------------
bool PTransformer_3 :: Link_Windings(){
   // ���� ����������� ������� ��������� ���
   Internal_Links.clear();
   // ��������� ��������� �������
   if( wStar == Winding_X1  ){             // � ������
      Internal_Links.add(X1n[_A_], X1n[_B_]);
      Internal_Links.add(X1n[_A_], X1n[_C_]);
   }else if( wTriangle == Winding_X1  ){   // � �����������
      Internal_Links.add(X1n[_A_], X1[_B_]);
      Internal_Links.add(X1n[_B_], X1[_C_]);
      Internal_Links.add(X1n[_C_], X1[_A_]);
   }else if( wGroundedStar == Winding_X1  ){// � ������ c ����������� �����
      Internal_Links.add(X1n[_A_], X1n[_B_]);
      Internal_Links.add(X1n[_A_], X1n[_C_]);
      *R[21] = *L[21] = *C[21] = 0;
   }else{
      err(0, "PTransformer_3::Winding_X2(): ����� ���������� �� ��������");
      return false;
      }
   // ��������� ��������� �������
   if( wStar == Winding_X2  ){             // � ������
      Internal_Links.add(X2n[_A_], X2n[_B_]);
      Internal_Links.add(X2n[_A_], X2n[_C_]);
   }else if( wTriangle == Winding_X2  ){   // � �����������
      Internal_Links.add(X2n[_A_], X2[_B_]);
      Internal_Links.add(X2n[_B_], X2[_C_]);
      Internal_Links.add(X2n[_C_], X2[_A_]);
   }else if( wGroundedStar == Winding_X2  ){// � ������ c ����������� �����
      Internal_Links.add(X2n[_A_], X2n[_B_]);
      Internal_Links.add(X2n[_A_], X2n[_C_]);
      *R[22] = *L[22] = *C[22] = 0;
   }else{
      err(0, "PTransformer_3::Winding_X2(): ����� ���������� �� ��������");
      return false;
      }
   return true;
}
//---------------------------------------------------------------------------
void PTransformer_3::Save(ostream &stream){		// ���������� ���������� ������� � �����
   // ������� ���������� ������� �����
   sire::Save(stream);
   // ������ ��������� ���� �����
   stream << mType << " ";
   stream << Winding_X1 << " ";
   stream << Winding_X2 << " ";
   // ��������� ������ ��������������
   mCurve.Save(stream);
}
//---------------------------------------------------------------------------
void PTransformer_3::Read(istream &stream){    // �������� ���������� ������� �� ������
   // ������� ������ ������ ������� �����
   sire::Read(stream);
   // ������ ������ ���� �����
   int tmp;
   stream >> tmp;          mType       = (MagnCircuit_Type)tmp ;
   stream >> tmp;          Winding_X1  = (Winding_Connect) tmp;
   stream >> tmp;          Winding_X2  = (Winding_Connect) tmp;
   // ������ ������ ��������������
   mCurve.Read(stream);
}
//---------------------------------------------------------------------------
String PTransformer_3::ClassName(){return "PTransformer_3";};				// ���������� ��� ������
//---------------------------------------------------------------------------






/* ������� ����� ��� ��������������� ���� */
PCurrent_Transformer::PCurrent_Transformer(unsigned NumBranches, unsigned NumNodes, unsigned NumCircuit) :
					PUnit(NumBranches, NumNodes, NumCircuit){
   Describe_Fields();   // ������ �������� ������
   S = 10;
   n1 = 0; n2 = 0;      // �������� ������ ���������� � accept()
   I1= 100; I2= 5;
   K = 0.5; d = 1;
   freq = 50;
   Name = "TA";
}
//---------------------------------------------------------------------------
void PCurrent_Transformer::Describe_Fields(){
   PFieldInfo fi;
   // ���. ��������
   fi.Name = "��������";
   fi.Type = "PType";
   fi.ptr = (void*) &S;
   Fields.push_back(fi);
   // ��������� ���
   fi.Name = "I1";
   fi.Type = "PType";
   fi.ptr = (void*) &I1;
   Fields.push_back(fi);
   // ��������� ���
   fi.Name = "I2";
   fi.Type = "PType";
   fi.ptr = (void*) &I2;
   Fields.push_back(fi);
   // ����� ������ � ��������� �������
   fi.Name = "n1";
   fi.Type = "PType";
   fi.ptr = (void*) &n1;
   Fields.push_back(fi);
   // ����� ������ �� ��������� �������
   fi.Name = "n2";
   fi.Type = "PType";
   fi.ptr = (void*) &n2;
   Fields.push_back(fi);
   // ����� ��������
   fi.Name = "�����";
   fi.Type = "PType";
   fi.ptr = (void*) &K;
   Fields.push_back(fi);
   // ���������� ������� �����������
   fi.Name = "���.����";
   fi.Type = "PType";
   fi.ptr = (void*) &d;
   Fields.push_back(fi);
   // ����������� �������
   fi.Name = "���.�������";
   fi.Type = "PType";
   fi.ptr = (void*) &freq;
   Fields.push_back(fi);
   // ������ ��������������
   fi.Name = "I(�)";
   fi.Type = "PCurve";
   fi.ptr = (void*) &mCurve;
   Fields.push_back(fi);
}
//---------------------------------------------------------------------------
void PCurrent_Transformer::Save(ostream &stream){		// ���������� ���������� ������� � �����
   // ������� ���������� ������� �����
   sire::Save(stream);
   // ������ ��������� ���� �����
   stream << n1 << " ";
   stream << n2 << " ";
   stream << I1 << " ";
   stream << I2 << " ";
   stream << S  << " ";
   stream << K  << " ";
   stream << d  << " ";
   stream <<freq<< " ";
   stream <<F_nom<< " ";
   // ��������� ������ ��������������
   mCurve.Save(stream);
}
//---------------------------------------------------------------------------
void PCurrent_Transformer::Read(istream &stream){    // �������� ���������� ������� �� ������
   // ������� ������ ������ ������� �����
   sire::Read(stream);
   // ������ ������ ���� �����
   stream >> n1 ;
   stream >> n2 ;
   stream >> I1 ;
   stream >> I2 ;
   stream >> S  ;
   stream >> K  ;
   stream >> d  ;
   stream >>freq;
   stream >>F_nom;
   // ������ ������ ��������������
   mCurve.Read(stream);
}
//---------------------------------------------------------------------------
String PCurrent_Transformer::ClassName(){return "PCurrent_Transformer";};				// ���������� ��� ������







//---------------------------------------------------------------------------
/* ������ �������������� ���� */
PCurrent_Transformer_1 :: PCurrent_Transformer_1() :
                           PCurrent_Transformer(5, 5, 1),
                           X1(2), X2(2){
   Describe_Fields(); // ������ ���� �������������� ����
   set_pins();        // ������ ������
  	// ��������� �������
   char _M[5][5] =   {{-1, 0, 0,-1, 0},
                      { 1, 0, 0, 0, 0},
                      { 0,-1, 0, 0,-1},
                      { 0, 1, 0, 0, 0},
                      { 0, 0, 0, 1, 1}};
	*M = (char*)_M;
   char _N[1][5] =    {{0, 0, 1, 0, 0}};
   *N = (char*)_N;
   //mCurve.set(0,0);
   //mCurve.set(1e3,1e6);  // �������� ������ �������������� � ���������� ��� ��������� ��������
   // ���������� ��������� ����� �� ������ ������������� ������
   R1 = 1e-3;
   L1 = 0   ;
   R2 = 1e-1;
   L2 = 1e-6 ;
   *R[2] = MIN;         // ������������� ���� ��������������
   *R[3] = *R[4] = MAX; // ������������ ������ �������
   Grounded_Node = 4;   // ����������� ����
}
//---------------------------------------------------------------------------
void PCurrent_Transformer_1 :: set_pins(){
   X1 [_N_] = set_pin(1);
   X1 [_A_] = set_pin(0);
   X2 [_N_] = set_pin(3);
   X2 [_A_] = set_pin(2);
   // �����
   X1.Name  = "X1";
   X2.Name  = "X2";
	// ������� � ������ ����� �������
	Out.add(&X1);
   Out.add(&X2);
}
//---------------------------------------------------------------------------
void PCurrent_Transformer_1::Describe_Fields(){
   PFieldInfo fi;
   // ���. ������������� ��������� ���
   fi.Name = "R1";
   fi.Type = "PType";
   fi.ptr = (void*) &R1;
   Fields.push_back(fi);
   // ������������� ��������� ���
   fi.Name = "L1";
   fi.Type = "PType";
   fi.ptr = (void*) &L1;
   Fields.push_back(fi);
   // ���. ������������� ��������� ���
   fi.Name = "R2";
   fi.Type = "PType";
   fi.ptr = (void*) &R2;
   Fields.push_back(fi);
   // ������������� ��������� ���
   fi.Name = "L2";
   fi.Type = "PType";
   fi.ptr = (void*) &L2;
   Fields.push_back(fi);
}
//---------------------------------------------------------------------------
bool PCurrent_Transformer_1 :: accept(){
   // ������������
   PType k = I1 / I2;        // �������������� ����������� �������������
   PType w = 2*freq*M_PI;     // ������� �������
   // ���� �� ������ ����� �������
   if(n1==0) n1=1;
   if(n2==0)
      if(n1!=0) n2=k*n1;
      else {n1=1;n2=k*n1;}

   PType _d = d*M_PI/180;                // ��������� ������� ����������� � �������
   PType a = acos(0.8);                   // ��� ������� ��������� ����������� ��������
   PType psi = atan(K/100/_d)-a;             // ���� ����� ������� � ����� ��������������
   PType _I2 = I1*n1/n2*( cos(_d)-K/100 ); // ������������� ��� � ������ ����������� ��� ����������� ��������

   PType E2 = S / _I2;         // ��� �� ��������� ������� ��� ����������� ��������
   F_nom = E2 / (w*n2);        // �����, ����������� ��� ����������� ����� ����������
   // ��� ��������������
   PType F0 = K/100*(I1*n1)/sin(psi+a);
   // ������������� ���� �������������� (���� ������)
   PType Zu = F0 / F_nom;
   PType Ru = Zu * sin(psi);
   PType Xu = Zu * cos(psi);

   // ���� ��������������
   *R[2] = Ru;  *L[2] = Xu/w;
   // �������
   *R[0] = R1;  *L[0] = L1;
   *R[1] = R2;  *L[1] = L2;
   magnetic_link(0,2,n1);
   magnetic_link(1,2,n2);
   update_differential_resistance();   // ������������� ���������������� �������������
   return true;
}
//---------------------------------------------------------------------------
bool PCurrent_Transformer_1:: update_elements(bool force_update){
   // ��������� ��������� ����� � ������������ � ������ ��������������
   if( !mCurve.exist )           // ������ �� ������ ��������������
      return false;
   else{
      PType _F,Ru;      	      // �������� ������ � ���������������� ��� �������������
      _F = *I[2];
      Ru  = n1*mCurve.I(_F)/_F; 	// ������������� ������ ���������������
      if(Ru>=0) *R[2] = Ru;      // ��������� �������������
      return true;
      }
}
//---------------------------------------------------------------------------
void PCurrent_Transformer_1::Save(ostream &stream){		// ���������� ���������� ������� � �����
   // ������� ���������� ������� �����
   sire::Save(stream);
   // ������ ��������� ���� �����
   stream << R1 << " ";
   stream << R2 << " ";
   stream << L1 << " ";
   stream << L2 << " ";
}
//---------------------------------------------------------------------------
void PCurrent_Transformer_1::Read(istream &stream){    // �������� ���������� ������� �� ������
   // ������� ������ ������ ������� �����
   sire::Read(stream);
   // ������ ������ ���� �����
   stream >> R1 ;
   stream >> R2 ;
   stream >> L1 ;
   stream >> L2 ;
}
//---------------------------------------------------------------------------
String PCurrent_Transformer_1::ClassName(){return "PCurrent_Transformer_1";};				// ���������� ��� ������







//---------------------------------------------------------------------------
/* ����������� ���������� */
PBreaker::PBreaker():
					      PUnit(9, 7, 3),
                     X1(4), X2(4){
   //��������� ������
   set_pins();
   // ������ �������� ������
   Describe_Fields();
  	// ��������� �������
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
   // ������������ ������������� � ����������� ���������
   Rmax = MAX;
   // ������������� �� ���������� ���������
   Rmin = MIN;
   // �� ��������� ��������
   IsOn = true;
   *R[0] = *R[1] = *R[2] = Rmin;
   // ������������ �� ����� �����������
   *R[3] = *R[4] = *R[5] = *R[6] = *R[7] = *R[8] = MAX;
   // ���������� ���������� (���������)
   for(int i=0; i<4;i++){
      T_on[i]   = 0;   // ���� �� � ��������� ���������
      T_off[i]  = 0;   // ���� �� � ��������� ����������
      }
   t_on  = 0.2;             // ����������� ����� ���������
   t_off = 0.05;            // ����������� ����� ����������
   t_full_off = 0.08;       // ������ ����� ����������
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
   // �����
   X1.Name  = "X1";
   X2.Name  = "X2";
	// ������� � ������ ����� �������
	Out.add(&X1);
   Out.add(&X2);
}
//---------------------------------------------------------------------------
void PBreaker::Describe_Fields(){
   PFieldInfo fi;
   // ������������� �� �����. ���������
   fi.Name = "������. ���";
   fi.Type = "PType";
   fi.ptr = (void*) &Rmin;
   Fields.push_back(fi);
   // ������������� � �����. ���������
   fi.Name = "������. ����";
   fi.Type = "PType";
   fi.ptr = (void*) &Rmax;
   Fields.push_back(fi);
   // ����������� ����� ���������
   fi.Name = "����� ���";
   fi.Type = "PType";
   fi.ptr = (void*) &t_on;
   Fields.push_back(fi);
   // ����������� ����� ����������
   fi.Name = "�����.��. ����";
   fi.Type = "PType";
   fi.ptr = (void*) &t_off;
   Fields.push_back(fi);
   // ������ ����� ����������
   fi.Name = "����.��. ����";
   fi.Type = "PType";
   fi.ptr = (void*) &t_full_off;
   Fields.push_back(fi);
   // ������� ������������� �����������
   fi.Name = "�������";
   fi.Type = "bool";
   fi.ptr = (void*) &IsOn;
   Fields.push_back(fi);
}
//---------------------------------------------------------------------------
bool PBreaker::on(Phase phase){
   switch (phase){
   	case _A_: if(!T_off[_A_] && !T_on[_A_]) {T_on[_A_] = TIME; return true;}; break;  // ����  ���� �� � ��������� ����������, �� ��������
      case _B_: if(!T_off[_B_] && !T_on[_B_]) {T_on[_B_] = TIME; return true;}; break;
      case _C_: if(!T_off[_C_] && !T_on[_C_]) {T_on[_C_] = TIME; return true;}; break;
      default:	 return false;
      }
   return false;
}
//---------------------------------------------------------------------------
bool PBreaker::off(Phase phase){
   switch (phase){
   	case _A_: if(!T_on[_A_] && !T_off[_A_]) {T_off[_A_] = TIME; return true;}; break;  // ����  ���� �� � ��������� ���������, �� ���������
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
   update_differential_resistance();   // ������������� ���������������� �������������
   return true;
}
//---------------------------------------------------------------------------
bool PBreaker::update_elements(bool force_update){
   bool ret = false;
   static PType I1[4]={0,0,0,0};
   for(int i=_A_; i<=_C_; i++){
      // ���� ���� � �������� ���������
      if(T_on[i]){
         // �������, ��� ��������� ���������� ��� ����
         if( (TIME-T_on[i])>t_on) {
            *R[i-_A_]=Rmin;
            T_on[i] = 0;
            ret = true;
            }
         }
      // ���� ���� � �������� ����������
      else if(T_off[i]){
         // ������ ������� ������� ����
         if( (TIME-T_off[i])>t_full_off && I1[i-_A_]*(*I[i-_A_])<0 ) {
            *R[i-_A_]=Rmax;
            T_off[i] = 0;
            ret = true;
            }
         // ���������� ������ ������ ����������� ���������
         else if( (TIME-T_off[i])>t_off) {
            /*/ �������, ��� ������������� ���������� �� ��������� ������
            PType dR = (Rmax-Rmin)/((t_full_off-t_off)/smp);
            *R[i-_A_] += dR;
            if(*R[i-_A_]>Rmax) *R[i-_A_]=Rmax; */
            // ��������� ������������� ����
            *R[i-_A_] = fabs(11/(*I[i-_A_]));
            ret = true;
            }
         I1[i-_A_] = *I[i-_A_]; // ��������� ������� ��������
         }
      }
   return ret;
}
//---------------------------------------------------------------------------
void PBreaker::Save(ostream &stream){		// ���������� ���������� ������� � �����
   // ������� ���������� ������� �����
   sire::Save(stream);
   // ������ ��������� ���� �����
   stream << Rmax << " ";
   stream << Rmin << " ";
   stream << t_on << " ";
   stream << t_off << " ";
   stream << t_full_off << " ";
   // ��������� ������� ������������� � ����� - ������� ��� ��� ����������
   stream << IsOn << " ";

}
//---------------------------------------------------------------------------
void PBreaker::Read(istream &stream){    // �������� ���������� ������� �� ������
   // ������� ������ ������ ������� �����
   sire::Read(stream);
   // ������ ������ ���� �����
   stream >> Rmax ;
   stream >> Rmin ;
   stream >> t_on ;
   stream >> t_off ;
   stream >> t_full_off ;
   // ������ ������������� � ����� - ������� ��� ��� ����������
   stream >> IsOn;
}
//---------------------------------------------------------------------------
String PBreaker::ClassName(){return "PBreaker";};				// ���������� ��� ������













/* ��������� ������� �������� */
PFieldInfo::PFieldInfo(){
   ptr = NULL;
   Type = "PType";
   Mode = rw;     // ����� ������ ������
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
         // ���������� ������
         for(int i=0, n=lineNum+1; i<n; i++){
            PType X=0, Y=0;
            obj->Point(i, X, Y);
            val += "(" + FloatToStr(X) + ";" + FloatToStr(Y) + ")";
            if( (i+1)<n ) val += " ";
            }
         return val;
         }
      else    // ������� �� ������
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
         err(1, "PFieldInfo: ������������ ������ ��� �������������� ������ � �����");
      }
   else if(Type == "int"){
      if(!TryStrToInt(newVal, *(int*)ptr))
         err(1, "PFieldInfo: ������������ ������ ��� �������������� ������ � �����");
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
         vector<String> Point;  // ����� ��������� ������ X;Y
         // ������ ��� ��������� "X;Y", ������� ��������� ���������
         unsigned pos;
         // �������� ���� ����� ������ ������� � ������
         int numSpace, m;
         for(numSpace=1, m=newVal.Length(); numSpace<=m; numSpace++) if(newVal[numSpace]!=' ') break;
         newVal = newVal.SubString(numSpace, newVal.Length() - numSpace + 1);
         pos = newVal.Pos(" ");
         while( pos ){
            Point.push_back( newVal.SubString(2,pos-3) );
            newVal = newVal.SubString(pos+1, newVal.Length()-pos);
            // �������� ���� ����� ������ �������
            for(numSpace=1, m=newVal.Length(); numSpace<=m; numSpace++) if(newVal[numSpace]!=' ') break;
            newVal = newVal.SubString(numSpace, newVal.Length() - numSpace + 1);
            pos = newVal.Pos(" ");
            }
         Point.push_back( newVal.SubString(2,newVal.Length()-2) );
         // �������� �������� X, Y � �������� � ������� �����
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
      else // ������ ������
         obj->clear();  // ������� ��������
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
   stream << StringForSave(&cname) << " ";	// ��� ������
   if(Obj->ClassName() == "PLine") stream << ((PLine*)Obj)->Cells() << " ";
   // ��������� ������ �������
   Obj->Save( stream );
}
//---------------------------------------------------------------------------
PElement* CreateObj(istream &stream){
      PElement *Obj;
      char cname[1024];
   	stream >> cname;
      String ClassName = cname; // ��� ������
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
      // ������ ������ �������
      if(Obj) Obj->Read(stream);
      return Obj;
}
//---------------------------------------------------------------------------
// Params - ��������� ������������ ��� �������� �������
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

