#include <string>
#include <iostream>

using namespace std;

class Graph{
public:
  string dataset;
  Graph();
};

class Algorithm{
public:
  Graph& graph;
  Algorithm(Graph& g);
};

class Main{
public:
  Graph& graph;
  Algorithm& algo;
  Main(Graph& g, Algorithm& a);
};

Graph::Graph(){}

Algorithm::Algorithm(Graph& g):graph(g){
  // graph = g;
}

Main::Main(Graph& g, Algorithm& a):graph(g),algo(a){
  graph = g;
}

int main(){
  Graph graph = Graph();
  graph.dataset = "nethept";
  Algorithm algo = Algorithm(graph);
  Main main = Main(graph, algo);
  cout << "graph: " << graph.dataset << endl;
  cout << "algo: " << algo.graph.dataset << endl;
  cout << "main: " << main.graph.dataset << endl;
  cout << "--------------" << endl;
  main.graph.dataset = "new dataset";
  cout << "graph: " << graph.dataset << endl;
  cout << "algo: " << algo.graph.dataset << endl;
  cout << "main: " << main.graph.dataset << endl;
}
