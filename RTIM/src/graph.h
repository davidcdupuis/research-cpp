#define HEAD_INFO
//#define HEAD_TRACE
#include "sfmt/SFMT.h"
#include "head.h"
using namespace std;
typedef double (*pf)(int, int);


/*
  Influence Graph class
*/
class Graph
{
  public:
    int nodes; // nodes
    int edges; // edges
    int k; //
    vector<int> inDeg; //inDegree
    vector<vector<int>> gT; //inverse graph matrix of relationships
    vector<vector<double>> probT; // matrix containing graph edge weights

    /* Function to read attributes.txt */
    void readAttributes();

    /* Function to import graph from file */
    void loadGraph();

    //Graph(string folder, string graph_file): folder(folder), graph_file(graph_file)
    //{
        //readNM();

        //init vector
        /*FOR(i, n)
        {
            gT.push_back(vector<int>());
            hasnode.push_back(false);
            probT.push_back(vector<double>());
            //hyperGT.push_back(vector<int>());
            inDeg.push_back(0);
        }*/
        //readGraph();
        //system("sleep 10000");
    //}

};
