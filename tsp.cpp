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
const int c_NAME	= 100;
const int c_COMMENT = 200;
const int c_TYPE  	= 10;
const int c_EWT     = 20;

// ------------------------------ DEFINICIONES -----------------------------
typedef pair<float, float> coordinates;								//DEFINICION DE PAR DE COORDENADAS
typedef vector<pair<long int ,coordinates> > list_nodes;			//DEFINICION DE LISTA DE NODOS (indice, (x,y))
typedef vector<pair<long int ,coordinates> >::iterator it_nodes;	//DEFINICION DE ITERADOR PARA LA LISTA DE NODOS
typedef vector<vector<pair<long int,double> > > list_adjacency;		//DEFINICION DE LA LISTA DE ADYACENCIA
typedef vector<vector<pair<long int,double> > >::iterator it_ad;	//DEFINICION DE ITERADOR PARA LA LISTA DE ADYACENCIA
typedef vector<pair<long int,double> >::iterator ith_la;			//DEFINICION DE ITERADOR PARA LA LISTA DE ADYACENCIA PESO
typedef vector<pair<long int, long int > > list_tabu;				//DEFINICION DE LA LISTA TABU
typedef vector<pair<long int, long int > >::iterator it_tabu;		//DEFINICION DE ITERADOR PARA LISTA TABU
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

long int tenure;													//TAMAÑO DE LA LISTA DE CANDIDATOS
long int tabu_tenure;												//TAMAÑO DE LA LISTA TABU
long int max_it_local;												//CANTIDA DE ITERACIONES MAXIMAS EN LA BUSQUEDA LOCAL

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
		scanf("%ld %f %f\n", &index, &(xy.first), &(xy.second));
		nodes.push_back(make_pair(index,xy));

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
	return sqrt((c2.first-c1.first)*(c2.first-c1.first) + (c2.second-c1.second)*(c2.second-c1.second));
}

//CARGAMOS LA LISTA DE ADYACENCIA
void create_list_adjacency()
{
	vector<pair<long int,double> > aux;											//VECTOR DE PARES(j, costo) AUXILIAR
	pair<long int,double> p;													//PAR(j, costo) AUXILIAR
	long int i,j;																//INDICES i,j

	for(it_nodes iti = nodes.begin(); iti!=nodes.end()-1; ++iti)				//PARA CADA INDICE HASTA dimension - 1
	{
		i = iti - nodes.begin();
		aux.clear();
		for(it_nodes itj = nodes.begin() + i + 1 ; itj != nodes.end(); ++itj)
		{
			j = itj - nodes.begin();
			p.first = j + 1;
			p.second = euc_2d(nodes[i].second, nodes[j].second);
			aux.push_back(p);
		}
		list_ad.push_back(aux);	
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

//CREA UNA SOLUCION ALEATORIA PARTIENDO DE UN VERTICE ALEATORIO
path initial_solution()
{
	srand (time(NULL));														//INICIALIZANDO UNA SEMILLA RANDOM
	vector<bool> marks(dimension,false); 									//VECTOR DE MARCAS
	path s_initial;															//path EN INIDICES
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

	s_initial.push_back(vertice_final);

	return s_initial;
}

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
		cost+=get_cost(i,j);							//OBTENGO EL COSTO DE IR DEL NODO i AL NODO j
	}

	return cost;										//RETORNO EL COSTO
}

//INICALIZAMOS LA INFORMACION PARA INTENSIFICACION Y DIVERSIFICACION
void initilizeHistoricalInformation(long int i_tenure, long int i_tabu_tenure, long int i_max_it_local)
{
	T.clear();											//INICIALIZAMOS LA LISTA TABU
	Nx.clear();											//INICIALIZAMOS LA LISTA DE CANDIDATOS
	tenure = i_tenure;									//TAMAÑO DE LA LISTA DE CANDIDATOS
	tabu_tenure = i_tabu_tenure;						//TAMAÑO DE LA LISTA TABU
	max_it_local = i_max_it_local;						//CANTIDAD DE ITERACIONES MAXIMAS LOCALES
}


//OBTIENE UN PAR DE INDICES (I,J) QUE NO SEAN IGUALES DE MANERA ALEATORIA
pair<long int,long int> get_pair_int()
{
	srand (time(NULL));									//INICIALIZANDO UNA SEMILLA RANDOM
	pair<long int, long int> pair(0,0);					//CREAMOS UN PAR (0,0)						

	while(pair.first==pair.second)						//MIENTRAS (i,j) SEAN IGUALES
	{													//CALCULO UNOS NUEVOS
		pair.first = (rand() % (dimension-1)) + 1;	
		pair.second = (rand() % (dimension-1)) + 1;
	}

	return pair;										//REGRESO EL PAR
}

//IMPRIME LA LISTA TABU
void print_list_tabu()
{
	cout << "Lista tabu: " << endl;
	for(it_tabu it = T.begin(); it!=T.end(); ++it)
		cout << (*it).first << " " << (*it).second << endl;
	cout << "end lista tabu" << endl;
}

//REALIZA EL PROCESO DE INTENSIFICACION, ES DECIR, LA BUSQUEDA LOCAL DENTRO DE UNA SOLUCION
path local_search(path s_initial)
{
	srand (time(NULL));															//INICIALIZANDO UNA SEMILLA RANDOM
	pair<long int, long int> pair_aux;											//PAR DE INDICES (i,j)
	float aux;																	//COSTO AUXILIAR
	long int cont_iter = 1;														//INCIALIZAMOS CONTADOR DE ITERACIONES
	float local_max = get_path_cost(s_initial);									//MAXIMO LOCAL INICIAL
	path s_prima = s_initial;													//CAMINO A RETORNAR, INICIALIZADO CON EL QUE SE RECIBE
	bool esta;

	cout << tabu_tenure << endl; 
	while(cont_iter <= max_it_local)											//MIENTRAS HAYA ITERACIONES 
	{
		cout << "Iteraciones: " << cont_iter << endl;
		pair_aux = get_pair_int();												//CALCULAMOS UN PAR
		esta = (find(T.begin(), T.end(), pair_aux)!=T.end());
		cout << esta << endl;
		if(!esta)																//VERIFICAMO QUE NO EXISTA EN LA LISTA TABU
		{
			cout << "entro " << T.size() << endl;
			if(T.size() >= tabu_tenure)											//SI EL TAMAÑO DE LA LISTA TABU >= tabu_tenure
				T.erase(T.begin()+0);											//BORRO EL PRIMER ELEMENTO DE LA LISTA
			T.push_back(pair_aux);												//AGREGO EL NUEVO ELEMENTO
			swap(s_initial[pair_aux.first],s_initial[pair_aux.second]);			//INTERCAMBIO LOS ELEMENTOS CONS LOS INDICES DE LOS PARES CREADOS
			float aux = get_path_cost(s_initial);								//CALCULO EL COSTO DEL NUEVO CAMINO
			if(aux < local_max)													//SI EL NUEVO CAMINO ES MAS BARATO QUE EL QUE TENIA 
			{
				local_max = aux;												//OBTENGO EL NUEVO OPTIMO LOCAL
				cont_iter = 1;													//REINICIO LAS ITERACIONEs
				s_prima = s_initial;											//GUARDO EL CAMINO
			}

		}
		cont_iter++;															//SUMO UNA ITERACION
	}

	print_list_tabu();
	return s_prima;																//RETORNO EL MEJOR CAMINO ENCONTRADO
}

//PROCEDIMIENTO TABU
void tsp_tabusearch(long int max_iterations, long int tenure, long int tabu_tenure, long int max_it_local)
{
	long int cont_global = 1;
	long int cont_iter = 1;
	path s_initial = initial_solution();
	initilizeHistoricalInformation(tenure,tabu_tenure,max_it_local);
	while( (cont_global <= tenure) && (cont_iter <= max_iterations) )
	{
		cont_iter++;
	}

}

//n^2-n/2
/*

//VERIFICA SI LAS CIUDADES FUERON VISITADAS
bool all_cities_visited(vector<bool> marcas)
{
	bool visited = true;
	for (vector<bool>::iterator itl = marcas.begin() ; itl!=marcas.end() && visited; ++itl)
		if(!(*itl))
			visited = false;
	return visited;
}

vector<long long int> vecino_mas_cercano()
{	
	srand (time(NULL));									//INICIALIZANDO UNA SEMILLA RANDOM

	vector<bool> marcas(dimension,false); 				//VECTOR DE MARCAS
	vector<long long int> indices_path;				//CAMINO EN INIDICES
	indices_camino.clear();								//LIMPIO EL VECTOR
	long long int vertice_bool;
	long long int vertice = rand() % dimension;			//ESCOGEMOS UN VERTICE DE INICIO
	long long int vertice_final = vertice;				//A DONDE DEBO REGRESAR
	marcas[vertice] = true;								//LO MARCAMOS COMO VISITADOS
	indices_camino.push_back(vertice);

	while(!all_cities_visited(marcas))					//SI TODAS LAS CIUDADES NO HAN SIDO VISITADAS
	{
		//vertice = get_vertice_min_distance(vertice,marcas);	//ESCOGEMOS EL CAMINO MAS CORTO DESDE EL VERTICE ACTUAL
		vertice = find(marcas.begin(), marcas.end(), false) - marcas.begin();
		marcas[vertice] = true;								//LO MARCAMOS COMO VISITADO
		indices_camino.push_back(vertice);
	}

	indices_camino.push_back(vertice_final);

	cout << "Solucion Inicial: [ "; 
	for(vector<long long int>::iterator it = indices_camino.begin(); it!=indices_camino.end();++it)
			cout << *it << " ";
	cout << "]" << endl;

	return indices_camino;
}

//VERIFICAMOS TODA LA VENCIDAD 
vector<long long int> local_search(vector<long long int> s0)
{
	srand (time(NULL));//INICIALIZANDO UNA SEMILLA RANDOM

	vector<pair<int,int> > T; 	//LISTA TABU
	int tabu_tenure = 10;		//TAMAÑO DE LA LISTA TABU
	int it = 1000;				//ITERACIONES 
	double optimo_local = get_cost_path(s0);
	pair<int,int> p;
	T.clear();

	while(it>0)
	{
		p = get_pair_int();

		bool esta = find(T.begin(), T.end(), p)!=T.end();
		if(!esta)
		{
			cout << T.size() << endl;
			if(T.size() >= 10)
				T.erase(T.begin()+0);

			T.push_back(p);
			swap(s0[p.first],s0[p.second]);
			double aux = get_cost_path(s0);

			if(aux < optimo_local)
			{
				optimo_local = aux;
				it = 100;
				cout << "OTRA VEZ EN 100" << endl;
			}

		}

		--it;
	}

	cout << "Lista tabu" << endl;
	for(vector<pair<int,int> >::iterator it = T.begin(); it!=T.end();++it)
		cout << (*it).first << " " << (*it).second << endl;
	
	cout << "Mejor tiempo: " << optimo_local << endl;
	return s0;

} 


void InicializarInformacionHistorica()
{
	Nx.clear();
	T.clear();
	max_iterations = 1000;	//MAXIMO NUMERO DE ITERACIONES EN LA DIVERSIFICACION
}

bool criterio_parada()
{
	return (max_iterations>0);
}

void actualizarSolucion(vector<long long int> solucion_prima)
{
	if(get_cost_path(solucion_prima) < get_cost_path(s_asterisco))
		s_asterisco = solucion_prima;
}


vector<long long int> tsp_tabusearch()
{
	vector<long long int> solucion_incial; //CONTIENE LA SOLUCION INICIAL
	vector<long long int> solucion_prima;	//CONTIENE LA SOLUCION PRIMA
	double initial_cost;		//COSTO INICIAL
	
	solucion_incial = vecino_mas_cercano();	//OBTENEMOS LA SOLUCION INICIAL
	s_asterisco = solucion_incial;
	initial_cost = get_cost_path(s_asterisco);

	InicializarInformacionHistorica();
	do
	{
		solucion_prima = local_search(solucion_incial);
		actualizarSolucion(solucion_prima);


	}while(criterio_parada());

	return s_asterisco;
}
*/


int main()
{
	input_nodes();
	create_list_adjacency();
	path s_initial = initial_solution();
	print_path(s_initial);
	cout << "Costo total:" << get_path_cost(s_initial) << endl;
	initilizeHistoricalInformation(100,10,10);
	path s_prima = local_search(s_initial);
	print_path(s_prima);
 	cout << "Costo total:" << get_path_cost(s_prima) << endl;

	return 0;
}