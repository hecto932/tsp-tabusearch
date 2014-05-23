/*
	Problema: Agente viajero
	Metaheuristica: Tabu search
	
	Integrantes:
	Acosta, Ricardo C.I 20.082.974
	Flores, Hector 	C.I 20.162.504

*/

#include <iostream>
#include <algorithm>
#include <utility>
#include <vector>
#include <deque>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <map>

using namespace std;

//CONSTANTES 
const int c_NAME	= 100;											//TAMAÑO DEL NOMBRE
const int c_COMMENT = 200;											//TAMAÑO DEL COMENTARIO
const int c_TYPE  	= 10;											//TAMAÑO DEL TIPO
const int c_EWT     = 20;											//TAMAÑO DEL EDGE

// ------------------------------ DEFINICIONES -----------------------------
typedef pair<float, float> coordinates;								//DEFINICION DE PAR DE COORDENADAS
typedef vector<pair<long int ,coordinates> > list_nodes;			//DEFINICION DE LISTA DE NODOS (indice, (x,y))
typedef vector<pair<long int ,coordinates> >::iterator it_nodes;	//DEFINICION DE ITERADOR PARA LA LISTA DE NODOS
typedef vector<vector<pair<long int,double> > > list_adjacency;		//DEFINICION DE LA LISTA DE ADYACENCIA
typedef vector<vector<pair<long int,double> > >::iterator it_ad;	//DEFINICION DE ITERADOR PARA LA LISTA DE ADYACENCIA
typedef vector<pair<long int,double> >::iterator ith_la;			//DEFINICION DE ITERADOR PARA LA LISTA DE ADYACENCIA PESO
typedef map<pair<long int, long int > ,bool> list_tabu;				//DEFINICION DE LA LISTA TABU
typedef map<pair<long int, long int > ,bool>::iterator it_tabu;		//DEFINICION DE ITERADOR PARA LISTA TABU
typedef vector<vector<long int> > list_candidate;					//DEFINICION DE LA LISTA DE CANDIDATOS 
typedef vector<vector<long int> >::iterator it_candidate;			//DEFINICION DE ITERADOR PARA LISTA DE CANDIDATOS
typedef vector<long int> path;										//DEFINICION DE UN VECTOR DE NODOS
typedef vector<long int>::iterator it_path;							//DEFINICION DE ITERADOR PARA UN path

// ------------------------------ VARIABLES GLOBALES -----------------------
char name[c_NAME];													//NOMBRE DE LA ENTRADA
char comment[c_COMMENT];											//COMMENT
char type[c_TYPE];													//TYPE
long int dimension;													//DIMENSION
char edge_weight_type[c_EWT];										//2D o 3D

list_nodes nodes; 													//LISTA DE NODOS (indice, (x,y))
list_adjacency list_ad;												//LISTA DE ADYACENCIA
list_tabu T;														//LISTA TABU
list_candidate Nx;													//LISTA DE CANDIDATOS

long int tenure 		= 20;										//TAMAÑO DE LA LISTA DE CANDIDATOS
long int tabu_tenure 	= 10;										//TAMAÑO DE LA LISTA TABU
long int max_it_local 	= 100;										//CANTIDAD DE ITERACIONES MAXIMAS EN LA BUSQUEDA LOCAL
long int max_it_global 	= 1000;										//CANTIDAD DE ITERACIONES MAXIMAS EN LA DIVERSIFICACION

// ---------------------------- FUNCIONES & PROCEDIMIENTOS--------------------------

//LECTURA Y CARGA DE LOS NODOS EN LA LISTA DE NODOS
void input_nodes()
{
	coordinates xy;													//PAR DE COORDENADAS (X,Y)
	long int index;													//INDICE DE LA COORDENADA

	scanf("NAME : %[^\n]\n", name);									//NAME
	scanf("COMMENT : %[^\n]\n", comment);							//COMMENT
	scanf("TYPE : %s\n", type);										//TYPE
	scanf("DIMENSION : %ld\n", &dimension);							//DIMENSION
	scanf("EDGE_WEIGHT_TYPE : %s\n", edge_weight_type);				//EDGE_WEIGHT_TYPE
	scanf("NODE_COORD_SECTION\n");									//NODE_COORD_SECTION

	for(long int i=dimension; i>0;--i)								//AGREGAMOS LOS NODOS A LISTA DE NODOS
	{
		scanf("%ld %f %f\n", &index, &(xy.first), &(xy.second));	//LEEMOS INDICE X Y 
		nodes.push_back(make_pair(index,xy));						//AGREGAMOS EL NODO A LA LISTA

	}
}

//IMPRIME POR PANTALLA LA LISTA DE NODOS
void print_nodes()
{
	for(it_nodes it = nodes.begin(); it!=nodes.end(); ++it)										
		cout << (*it).first << " " << (*it).second.first << " " << (*it).second.second << endl;  
}

//DISTANCIA EUCLIDIANA DE PUNTOS CORDENADAS
float euc_2d(coordinates c1, coordinates c2)
{
	return sqrt((c2.first-c1.first)*(c2.first-c1.first) + (c2.second-c1.second)*(c2.second-c1.second));	//RETORNAMOS LA DISTANCIA ENTRE DOS PUNTOS
}

//CARGAMOS LA LISTA DE ADYACENCIA
void create_list_adjacency()
{
	vector<pair<long int,double> > aux;											//VECTOR DE PARES(j, costo) AUXILIAR
	pair<long int,double> p;													//PAR(j, costo) AUXILIAR
	long int i,j;																//INDICES i,j

	for(it_nodes iti = nodes.begin(); iti!=nodes.end()-1; ++iti)				//PARA CADA INDICE HASTA dimension - 1
	{
		i = iti - nodes.begin();												//CALCULAMOS EL VERTICE i
		aux.clear();															//LIMPIAMOS EL VECTOR AUXILIAR
		for(it_nodes itj = nodes.begin() + i + 1 ; itj != nodes.end(); ++itj)	//PARA CADA POSICION DEL VECTOR i+1
		{
			j = itj - nodes.begin();
			p.first = j + 1;													
			p.second = euc_2d(nodes[i].second, nodes[j].second);				//CALCULAMOS LA DISTNACIA
			aux.push_back(p);													//LA GUARDAMOS
		}
		list_ad.push_back(aux);													//AGREGAMOS EL VECTOR AL VECTOR EN LA POSICION i
	}
}

//IMPRIME POR PANTALLA LA LISTA DE ADYACENCIA
void print_list_adjacency()
{
	long int i;
	for(it_ad iti=list_ad.begin(); iti!=list_ad.end(); ++iti)
	{
		i = iti-list_ad.begin();
		cout << "Posicion: " << i << endl;
		for(vector<pair<long int,double> >::iterator itj=list_ad[i].begin();itj!=list_ad[i].end();++itj)
		{
			cout << i << " " << (*itj).first - 1 << " " << (*itj).second << endl;
		}
	}
}

//OBTENGO EL COSTO TOTAL DE UN CAMINO
float get_path_cost(path p)
{
	float cost = 0;										//COSTO INCIAL EN 0
	long int i,j;										//INDICES I,J A CALCULAR EL CAMINO ENTRE ELLOS
	for(it_path itp = p.begin(); itp!=p.end()-1;++itp)	//PARA CADA PARA [i,j) DEL CAMINO
	{
		i = *itp;										//OBTENGO EL INDICE i
		j = *(itp+1);									//OBTENGO EL INDICE j
		if(i > j) swap(i,j);							//SI i > j ENTONCES SWAP
		//cost+=get_cost(i,j);							//OBTENGO EL COSTO DE IR DEL NODO i AL NODO j
		//cout << i << " " << j << " " << euc_2d(nodes[i].second,nodes[j].second) << endl;
		cost+=euc_2d(nodes[i].second,nodes[j].second);	//OBTENGO EL COSTO DE IR DEL NODO i AL NODO j
	}

	/*
	float cost = 0;
	long int i,j,k;
	for(long int i=0;i<p.size()-1;++i)
	{
		j = p[i];
		k = p[i+1];
		if(j > k) swap(j,k);							//SI i > j ENTONCES SWAP
		cost+=euc_2d(nodes[j].second,nodes[k].second);	//OBTENGO EL COSTO DE IR DEL NODO i AL NODO j
	}
	*/
	return cost;										//RETORNO EL COSTO
}

//CREA UNA SOLUCION ALEATORIA PARTIENDO DE UN VERTICE ALEATORIO
path initial_solution()
{
	
	vector<bool> marks(dimension,false); 									//VECTOR DE MARCAS
	path s_initial;															//path EN INIDICES
	s_initial.clear();
	long int vertice = rand() % dimension;									//ESCOGEMOS UN VERTICE DE INICIO
	long int vertice_final = vertice;										//A DONDE DEBO REGRESAR
	marks[vertice] = true;													//LO MARCAMOS COMO VISITADOS
	s_initial.push_back(vertice);

	
	while(find(marks.begin(), marks.end(), false)!=marks.end())
	{
		vertice = find(marks.begin(), marks.end(), false) - marks.begin();
		marks[vertice] = true;				
		s_initial.push_back(vertice);
	}
	

	/*
	path s_initial;
	vector<bool> marks(dimension,false);
	long int vertice = rand() % dimension;
	long int vertice_final = vertice;
	marks[vertice] = true;
	s_initial.push_back(vertice);

	while(find(marks.begin(), marks.end(), false)!=marks.end())
	{
		vertice = rand() % dimension;
		if(!marks[vertice])
		{
			marks[vertice] = true;				
			s_initial.push_back(vertice);
		}	
	}
	*/
	s_initial.push_back(vertice_final);
	
	return s_initial;
}

//IMPRIME UN CAMINO
void print_path(path p)
{
	cout << "[";
	for(it_path it = p.begin(); it!=p.end();++it)
		cout << " " << *it;
	cout << " ]" << endl;
}


//OBTENGO EL COSTO DE IR A UNA CIUDAD A OTRA
float get_cost(long int i, long int j)											
{
	float cost = 0;																//COSTO EN 0
	for(ith_la it = list_ad[i].begin(); it!=list_ad[i].end() && cost==0; ++it)	//RECORRO LA LISTA DEL INDICE i DE LA LISTA DE ADYACENCIA
		if( (*it).first == j+1 )												//SI EL INDICE DE LA POSICION *it DE LA LISTA ES IGUAL A j+1
			cost = (*it).second;												//GUARDO EL COSTO

	return cost;																//DEVUELVO EL COSTO
}


//INICALIZAMOS LA INFORMACION PARA INTENSIFICACION Y DIVERSIFICACION


//OBTIENE UN PAR DE INDICES (I,J) QUE NO SEAN IGUALES DE MANERA ALEATORIA
pair<long int,long int> get_pair_int()
{
	pair<long int, long int> a((rand() % (dimension-1)) + 1,(rand() % (dimension-1)) + 1);	//CREAMOS UN PAR ALEATORIO					

	while(a.first==a.second)																//MIENTRAS (i,j) SEAN IGUALES
	{																						//CALCULO UNOS NUEVOS
		a.first = (rand() % (dimension-1)) + 1;	
		a.second = (rand() % (dimension-1)) + 1;
	}

	return a;																				//RETORNO EL PAR
}


//IMPRIME LA LISTA TABU
void print_list_tabu()
{
	cout << "Lista tabu: " << endl;
	for(it_tabu it = T.begin(); it!=T.end(); ++it)
		cout << (*it).first.first << " " << (*it).first.second << " " << (*it).second << endl;
	cout << "end lista tabu" << endl;
}

//INICIALIZAMOS LA INFORMACION HISTORICA
void initilizeHistoricalInformation()
{
	T.clear();		//INICIALIZAMOS LA LISTA TABU
	Nx.clear();		//INICIALIZAMOS LA LISTA DE CANDIDATOS										
}

//REALIZA EL PROCESO DE INTENSIFICACION, ES DECIR, LA BUSQUEDA LOCAL DENTRO DE UNA SOLUCION
path local_search(path s_initial)
{
	T.clear();																	//LIMPIAMOS LA LISTA TABU
	pair<long int, long int> a;													//PAR DE INDICES (i,j)
	pair<long int, long int> b;													//PAR DE INDICES (i,j)
	float aux;																	//COSTO AUXILIAR
	long int cont_iter = 1;														//INCIALIZAMOS CONTADOR DE ITERACIONES
	float local_max = get_path_cost(s_initial);									//MAXIMO LOCAL INICIAL
	path s_prima = s_initial;													//CAMINO A RETORNAR, INICIALIZADO CON EL QUE SE RECIBE

	while(cont_iter <= max_it_local)											//MIENTRAS HAYA ITERACIONES 
	{
		a = get_pair_int();														//CALCULAMOS UN PAR
		b.first = a.second;
		b.second = a.first;		
		if(T.find(a)==T.find(b))												//VERIFICAMO QUE NO EXISTA EN LA LISTA TABU
		{
			if(T.size() >= tabu_tenure)											//SI EL TAMAÑO DE LA LISTA TABU >= tabu_tenure
				T.erase(T.begin());												//BORRO EL PRIMER ELEMENTO DE LA LISTA
			T[a]=true;															//AGREGO EL NUEVO ELEMENTO
			swap(s_initial[a.first],s_initial[a.second]);						//INTERCAMBIO LOS ELEMENTOS CONS LOS INDICES DE LOS PARES CREADOS
			aux = get_path_cost(s_initial);										//CALCULO EL COSTO DEL NUEVO CAMINO
			if(aux < local_max)													//SI EL NUEVO CAMINO ES MAS BARATO QUE EL QUE TENIA 
			{
				local_max = aux;												//OBTENGO EL NUEVO OPTIMO LOCAL
				cont_iter = 1;													//REINICIO LAS ITERACIONEs
				s_prima = s_initial;											//GUARDO EL CAMINO
			}

		}
		cont_iter++;															//SUMO UNA ITERACION
	}
	//print_list_tabu();
	return s_prima;																//RETORNO EL MEJOR CAMINO ENCONTRADO
}

void update_solution(path& s, path s_initial, path s_prima, long int& cont_global)
{
	float aux1 = get_path_cost(s_initial);
	float aux2 = get_path_cost(s_prima);
	float aux3 = get_path_cost(s);

	if(aux2 < aux1 && aux2 < aux3){
		s = s_prima;
		cont_global = 1;
		cout << "1" << endl;
	}
	else if(aux1 < aux2 && aux1 < aux3){
		s = s_initial;
		cont_global = 1;
		cout << "2" << endl;
	}
	else
	{
		cout << "3" << endl;
		cont_global++;
	}
}

//ACTUALIZAMOS LA INFORMACION HISTORICA
void updateHistoricalInformation(path s_initial)
{
	if(Nx.size()>=tenure)			//SI LA LISTA DE CANDIDATOS ESTA LLENA
		Nx.erase(Nx.begin()+0);		//ELIMINAMOS LA PRIMERA POSICION
	Nx.push_back(s_initial);		//AGREGAMOS LA SOLUCION USADA

}

//BUSCAMOS UNA NUEVA SOLUCION INICIAL QUE NO HAYA SIDO TOMADA
path findNewSolution()
{
	path new_solution = initial_solution();						//CALCULAMOS UNA SOLUCION ALEATORIA
	while(find(Nx.begin(), Nx.end(), new_solution)!=Nx.end())	//VERIFICAMOS SI NO LA HEMOS TOMADO
		new_solution = initial_solution();						//SI LA TOMAMOS CALCULAMOS OTRA
	return new_solution;										//RETORNAMOS UNA NUEVA SOLUCION INICIAL
}

//PROCEDIMIENTO TABU
path tsp_tabusearch()
{
	path s;
	s.clear();
	path s_initial;
	path s_prima;
	long int cont_global;
	long int cont_iter; 

	s_initial = initial_solution();
	s = s_initial;
	print_path(s_initial);
	cout << get_path_cost(s_initial);
	initilizeHistoricalInformation();
	cont_global = 1; cont_iter = 1;
	while( (cont_global <= tenure) && (cont_iter <= max_it_global) )
	{
		s_prima = local_search(s_initial);									//INTENSIFICACION
		update_solution(s, s_initial, s_prima, cont_global);				//ACTUALIZO LA MEJOR SOLUCION
		updateHistoricalInformation(s_initial);								//ACTUALIZO LA LISTA DE CANDIDATOS
		s_initial = findNewSolution();										//NUEVA SOLUCION INICIAL
		cont_iter++;														//UNA ITERACION MAS
	}

	return s;
}

int main()
{

	
	srand (time(NULL));											//INICIALIZAMOS LA SEMILLA
	clock_t t_begin;											//TIEMPO DE INICIO
	clock_t t_end;												//TIEMPO FINAL
	double secs;							

	input_nodes();												//LEEMOS LOS NODOS
	create_list_adjacency();									//CREAMOS LA LISTA DE ADYACENCIA
	t_begin = clock();											//TOMAMOS EL TIEMPO DE INICIO DE LA FUNCION TABU
	path path_optimum = tsp_tabusearch();						//INICIAMOS LA FUNCION TABU
	print_path(path_optimum);									//MOSTRAMOS EL CAMINO
	cout << "COSTO: " << get_path_cost(path_optimum) << endl;	//IMPRIMOS EL COSTO
	t_end = clock();											//TOMAMOS EL TIEMPO FINAL DEL PROBLEMA
	secs = (double)(t_end - t_begin)/CLOCKS_PER_SEC;			//OBTENEMOS LA DIFERENCIA ENTRE EL TIEMPO FINAL Y DE INICIO
	cout << "TIEMPO: " << secs << " segundos." <<endl;			//IMPRIMOS LA CANTIDAD DE SEGUNDOS
	

	
	//srand (time(NULL));											//INICIALIZAMOS LA SEMILLA
	//input_nodes();											//LEEMOS LOS NODOS
	//create_list_adjacency();									//CREAMOS LA LISTA DE ADYACENCIA
	//print_list_adjacency();
	//path s_initial = initial_solution();
	//print_path(camino);
	//cout << "Costo: " << get_path_cost(camino) << endl;
	//print_list_adjacency();
	
	return 0;
}