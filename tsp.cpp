/*
	Problema: Agente viajero
	Metaheuristica: Tabu search
	
	Integrantes:
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

using namespace std;

//CONSTANTES 
const int _NAME_ 	= 100;
const int _COMMENT_ = 200;
const int _TYPE_  	= 10;
const int _EWT_     = 20;
const int _N_ 		= 1000;

//DEFINICIONES
typedef pair<double, double> coordenadas; 					//PAR DE DOBLES
typedef vector<pair<string,coordenadas> >::iterator itl; 	//ITERADOR VECTOR DE PAR(STRING, PAR DE DOBLES)
typedef vector<bool>::iterator itlb;						//ITERADOR DE VECTOR DE MARCAS

//VARIABLES GLOBALES
char name[_NAME_];							//NOMBRE DE LA ENTRADA
char comment[_COMMENT_];					//COMMENT
char type[_TYPE_];							//TYPE
long long int dimension;					//DIMENSION
char edge_weight_type[_EWT_];				//2D o 3D
vector<pair<string,coordenadas> > nodos; 	//VECTOR DE NODOS
vector<vector<double> > m; 					//VECTOR DE DISTANCIAS EUCLIDIANAS 2D 
vector<pair<int,int> > T;					//LISTA TABU


//CARGA DE DATOS
void input_nodes()
{
	coordenadas xy;		//PAR DE COORDENADAS
	string name_nodo;	//NOMBRE DE LA COORDENADA
	double x,y;			//AUXILIARES

	scanf("NAME : %[^\n]\n", name);
	scanf("COMMENT : %[^\n]\n", comment);
	scanf("TYPE : %s\n", type);
	scanf("DIMENSION: %lld\n", &dimension);
	scanf("EDGE_WEIGHT_TYPE : %s\n", edge_weight_type);
	scanf("NODE_COORD_SECTION\n");

	nodos.clear();

	for(int i = 0; i < dimension; ++i)
	{
		cin >> name_nodo >> x >> y;
		xy.first = x;
		xy.second = y;
		nodos.push_back(make_pair(name_nodo,xy));
	}

}

//RETORNA EL NOMBRE DEL NODO
string get_name_node(int i)
{
	return nodos[i].first;
}

//RETORNO LAS COORDENADAS DEL NODO
coordenadas get_coordenadas(int i)
{
	return nodos[i].second;
}

//RETORNA LA DISTANCIA DE UNA CIUDAD A OTRA
double get_distancia(int i,int j)
{
	return m[i][j];
}

//REGRESO EL VERTICE CON LA DISTANCIA MINIMA DESDE EL ACTUAL
int get_vertice_min_distance(int i, vector<bool> marcas)
{
	int vertice_min=-1;
	double min = 100000;

	for(int j= 0;j<dimension;++j)
	{
		if(min > m[i][j] && !marcas[j] && i!=j)
		{
			min = m[i][j];
			vertice_min = j;
		}
	}
	return vertice_min;
}

//DISTANCIA EUCLIDIANA DE DOS CORDENADAS
double EUC_DISTANCE_2D(coordenadas c1, coordenadas c2)
{
	return sqrt((c2.first-c1.first)*(c2.first-c1.first) + (c2.second-c1.second)*(c2.second-c1.second));
}

//CARGA LA MATRIZ DE DISTANCIA 
void create_matrix_distance()
{
	m.clear();
	for (int i = 0; i < dimension; ++i)
	{
		vector<double> aux;
		aux.clear();
		for (int j = 0; j < dimension; ++j)
			(i==j) ? aux.push_back(0) : aux.push_back(EUC_DISTANCE_2D(get_coordenadas(i), get_coordenadas(j)));		
		m.push_back(aux);
	}
}

//MUESTRA LA MATRIZ DISTANCIA
void show_matrix_distance()
{
	for(int i=0;i<dimension;++i)
	{
		for(int j=0;j<dimension;++j)
		{
			cout << m[i][j] << " ";
		}
		cout << endl;
	}
}

//IMPRIME LAS COORDENADAS DE CADA NODO
void show_nodes()
{
	for( itl it = nodos.begin(); it != nodos.end() ; ++it )
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

//DEVUELVE UNA LISTA DEL CAMINO MAS CORTO
vector<string> vecino_mas_cercano()
{
	vector<bool> marcas(dimension,false); 	//VECTOR DE MARCAS
	vector<string> camino; 					//STRING DEL CAMINO
	camino.clear();							//LIMPIAMOS EL VECTOR

	int vertice = rand() % dimension;			//ESCOGEMOS UN VERTICE DE INICIO
	marcas[vertice] = true;						//LO MARCAMOS COMO VISITADOS
	camino.push_back(get_name_node(vertice));	//LO AGREGAMOS AL CAMINO
	
	while(!all_cities_visited(marcas))						//SI TODAS LAS CIUDADES NO HAN SIDO VISITADAS
	{
		vertice = get_vertice_min_distance(vertice,marcas);	//ESCOGEMOS EL CAMINO MAS CORTO DESDE EL VERTICE ACTUAL
		marcas[vertice] = true;								//LO MARCAMOS COMO VISITADO
		camino.push_back(get_name_node(vertice));			//LO AGREGAMOS AL CAMINO
	}

	cout << "Solucion Inicial: [";
	for(vector<string>::iterator it = camino.begin(); it!=camino.end();++it)
		cout << *it << " ";
	cout << "]" << endl;

	return camino;
}

double tsp_tabu_search()
{
	//s0 = vecino_mas_cercano(); //SOLUCION INICIAL
}

void move(vector<pair<int,int> > T)
{

}

int main()
{
  	srand (time(NULL));//INICIALIZANDO UNA SEMILLA RANDOM

	input_nodes();
	create_matrix_distance();
	vector<string> s0 = vecino_mas_cercano();

	return 0;
}