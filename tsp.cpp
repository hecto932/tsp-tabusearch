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
typedef pair<double, double> coordenadas; 					//PAR DE DOBLES (double, double)
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
			cout << m[i][j] << "|\t";
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
vector<int> vecino_mas_cercano()
{
	srand (time(NULL));//INICIALIZANDO UNA SEMILLA RANDOM

	vector<bool> marcas(dimension,false); 	//VECTOR DE MARCAS
	vector<string> camino; 					//STRING DEL CAMINO
	vector<int> indices_camino;				//CAMINO EN INIDICES
	camino.clear();							//LIMPIAMOS EL VECTOR
	indices_camino.clear();

	int vertice = rand() % dimension;			//ESCOGEMOS UN VERTICE DE INICIO
	int vertice_final = vertice;				//A DONDE DEBO REGRESAR
	marcas[vertice] = true;						//LO MARCAMOS COMO VISITADOS
	camino.push_back(get_name_node(vertice));	//LO AGREGAMOS AL CAMINO
	indices_camino.push_back(vertice);
	
	while(!all_cities_visited(marcas))						//SI TODAS LAS CIUDADES NO HAN SIDO VISITADAS
	{
		//cout << "Antes " << vertice << endl;
		vertice = get_vertice_min_distance(vertice,marcas);	//ESCOGEMOS EL CAMINO MAS CORTO DESDE EL VERTICE ACTUAL
		//cout << "Luego " << vertice << endl;
		marcas[vertice] = true;								//LO MARCAMOS COMO VISITADO
		camino.push_back(get_name_node(vertice));			//LO AGREGAMOS AL CAMIN
		indices_camino.push_back(vertice);
	}
	camino.push_back(get_name_node(vertice_final));
	indices_camino.push_back(vertice_final);

	//cout << "Solucion Inicial - string: [";
	/*for(vector<string>::iterator it = camino.begin(); it!=camino.end();++it)
		cout << *it <<endl;*/
	//cout << "]" << endl;
	cout << "Solucion Inicial: [ "; 
	//cout << "Solucion Inicial - int: [";
	for(vector<int>::iterator it = indices_camino.begin(); it!=indices_camino.end();++it)
			cout << *it << " ";
	cout << "]" << endl;

	return indices_camino;
}

double get_cost_path(vector<int> s0)
{
	double acum = 0; //C

	cout.precision(4);//PRECISION A 2 DECIMALES
	cout.setf(ios::fixed);

	for(vector<int>::iterator it=s0.begin();it!=s0.end();++it){
		//cout << *it << " " << *it+1 << " " << m[*it][*it+1] << endl;
		acum+=m[*it][*it+1];
 	}
	return acum;
}

vector<int> local_search(vector<int> s0)
{
	srand (time(NULL));//INICIALIZANDO UNA SEMILLA RANDOM

	vector<pair<int,int> > T; 	//LISTA TABU
	int tabu_tenure = 10;		//TAMAÑO DE LA LISTA TABU
	int it = 10;				//ITERACIONES 
	double optimo_local = get_cost_path(s0);
	pair<int,int> p;
	T.clear();

	while(it>0)
	{
		p.first = rand() % (dimension-1) + 1;
		p.second = rand() % (dimension-1) + 1;
		//cout << "Par: " << p.first << " " << p.second << endl;
		//cout << "Iteraciones: " << it << endl;
		//cout << "Resultado: " << !binary_search(T.begin(), T.end(), p) << endl;
		if(!binary_search(T.begin(), T.end(), p))
		{
			//cout << "No esta " << p.first << " " << p.second << endl;
			if(T.size() >= 10)
				T.erase(T.begin()+0);

			T.push_back(p);
			swap(s0[p.first],s0[p.second]);
			double aux = get_cost_path(s0);

			if(aux < optimo_local)
			{
				optimo_local = aux;
				it = 10;
			}
			else{
				--it;
			}

		}	
	}

	cout << "Lista tabu" << endl;
	for(vector<pair<int,int> >::iterator it = T.begin(); it!=T.end();++it)
		cout << (*it).first << " " << (*it).second << endl;
	
	return s0;

} 

int main()
{
  	srand (time(NULL));//INICIALIZANDO UNA SEMILLA RANDOM

	input_nodes();
	create_matrix_distance();

	vector<int> s0 = vecino_mas_cercano();
	
	cout << get_cost_path(s0) << endl;

	vector<int> s_prima = local_search(s0);

	cout << "Solucion prima : [ ";
	for(vector<int>::iterator it = s_prima.begin(); it!=s_prima.end();++it)
		cout << *it << " ";
	cout << "]" << endl;
	
	cout << get_cost_path(s_prima) << endl;
	
	return 0;
}