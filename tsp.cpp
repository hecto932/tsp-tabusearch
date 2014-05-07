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
typedef map<coordinates,bool> list_tabu;							//DEFINICION DE LA LISTA TABU
typedef map<coordinates,bool>::iterator it_tabu;					//DEFINICION DE ITERADOR PARA LISTA TABU
typedef vector<vector<long int> > list_candidate;					//DEFINICION DE LA LISTA DE CANDIDATOS 
typedef vector<vector<long int> >::iterator it_candidate;			//DEFINICION DE ITERADOR PARA LISTA DE CANDIDATOS

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

// ---------------------------- FUNCIONES & PROCEDIMIENTOS--------------------------

//LECTURA Y CARGA DE LOS NODOS EN LA LISTA DE NODOS
void input_nodes()
{
	coordinates xy;										//PAR DE COORDENADAS (X,Y)
	long int index;										//INDICE DE LA COORDENADA

	scanf("NAME : %[^\n]\n", name);						//NAME
	scanf("COMMENT : %[^\n]\n", comment);				//COMMENT
	scanf("TYPE : %s\n", type);							//TYPE
	scanf("DIMENSION : %ld\n", &dimension);			//DIMENSION
	scanf("EDGE_WEIGHT_TYPE : %s\n", edge_weight_type);	//EDGE_WEIGHT_TYPE
	scanf("NODE_COORD_SECTION\n");						//NODE_COORD_SECTION

	for(long int i=dimension; i>0;--i)
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
	vector<pair<long int,double> > aux;
	pair<long int,double> p;
	long int i,j;

	for(it_nodes iti = nodes.begin(); iti!=nodes.end(); ++iti)
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
	long long int i;
	for(it_ad iti=list_ad.begin(); iti!=list_ad.end(); ++iti)
	{
		i = iti-list_ad.begin();
		cout << "Posicion: " << i+1 << endl;
		for(vector<pair<long int,double> >::iterator itj=list_ad[i].begin();itj!=list_ad[i].end();++itj)
		{
			cout << i+1 << " " << (*itj).first << " " << (*itj).second << endl;
		}
	}
}

//n^2-n/2
/*

//LLENAMOS LA LISTA DE ADYACENCIA
void create_lista_adyacencia()
{
	vector<pair<long,double> > aux;							//AUXILIAR PARA LLENAR CADA FILA
	pair<long long int,double> p_aux;						//PAR AUXILIAR
	long long int i,j;										//INDICES i,j

	la.clear();												//LIMPIAMOS LA LISTA DE ADYACENCIA

	for(itl iti=nodes.begin();iti!=nodes.end();++iti)		//PARA CADA NODO
	{
		aux.clear();										//LIMPIAMOS LA LISTA AUXILIAR
		for(itl itj=nodes.begin();itj!=nodes.end();++itj)	//LE SACAMOS LA DISTANCIA CON TODOS LOS DEMAS NODOS
		{ 
			if(iti-nodes.begin()!=itj-nodes.begin())		//SACAMOS LAS DISTANCIAS ENTRE NODO A EXCEPCION A NOSOTROS MISMOS
			{
				p_aux.first = itj-nodes.begin()+1;			//GUARDAMOS EL INDICE
				p_aux.second = EUC_DISTANCE_2D((*iti).second, (*itj).second);	//GUARDAMOS LA DISTANCIA
				aux.push_back(p_aux);						//LLENAMOS LA LISTA PARA ESE I
			}	
		}
		la.push_back(aux);									//INSERTAMOS LA LISTA EN ESE I
	}
}

void show_lista_adyacencia()
{
	long long int i;
	for(vector<vector<pair<long,double> > >::iterator iti=la.begin();iti!=la.end();++iti)
	{
		i = iti-la.begin();
		cout << "Posicion: " << i << endl;
		for(vector<pair<long,double> >::iterator itj=la[i].begin();itj!=la[i].end();++itj)
		{
			cout << i << " " << (*itj).first-1 << " " << (*itj).second << endl;
		}
	}
}

//RETORNO LAS COORDENADAS DEL NODO
coordenadas get_coordenadas(long i)
{
	return nodes[i].second;
}

//RETORNA LA DISTANCIA DE UNA CIUDAD A OTRA
double get_distancia(long long int i, long long int j)
{
	return EUC_DISTANCE_2D(nodes[i-1].second, nodes[j-1].second);
}

//REGRESO EL VERTICE CON LA DISTANCIA MINIMA DESDE EL ACTUAL
long long int get_vertice_min_distance(long long int i, vector<bool> marcas)
{
	long long int vertice;
	double min = 100000;

	for(vector<pair<long, double> >::iterator it=la[i].begin();it!=la[i].end();++it)
	{
		if((*it).second < min && !marcas[(*it).first-1])
		{
			min = (*it).second;
			vertice = (*it).first-1;
		}
	}

	//cout << i << " " << vertice << " " << min << endl;
	return vertice;
}

//IMPRIME LAS COORDENADAS DE CADA NODO
void show_nodes()
{
	for( itl it = nodes.begin(); it != nodes.end() ; ++it )
		cout << (*it).first << " " << (*it).second.first << " " << (*it).second.second << endl;
}

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
	vector<long long int> indices_camino;				//CAMINO EN INIDICES
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

double get_cost_path(vector<long long int> s0)
{
	double acum = 0;

	cout.precision(4);//PRECISION A 4 DECIMALES
	cout.setf(ios::fixed);

	for(vector<long long int>::iterator it=s0.begin();it!=(s0.end()-1);++it){
		//cout << *it << " " << *(it+1) << " " << get_distancia(*it+1, *(it+1)+1) << endl;
		acum+=get_distancia(*it+1, *(it+1)+1);
	}

	return acum;
}


pair<int,int> get_pair_int()
{
	pair<int,int> p(0,0);

	while(p.first==p.second)
	{
		p.first = rand() % (dimension-1) + 1;
		p.second = rand() % (dimension-1) + 1;
		//cout << "(" << p.first << ", " << p.second << ")" << endl;
	}
	return p;
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
	print_nodes();
	create_list_adjacency();
	print_list_adjacency();
 
	return 0;
}