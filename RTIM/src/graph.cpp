#include "graph.h"

class Graph{
  Graph::Graph(string d){
    dataset =  "../data/" + dataset;
    readAttributes();
    graph.resize(nodes);
    loadGraph();
  }

  /* Function to add an edge */
  Graph::addEdge(int a, int b, double w){
    graph[a].push_back(make_pair(b, w));
  }

  /* Function to read attributes.txt */
  Graph::readAttributes(){
    ifstream cin((folder + "attribute.txt").c_str());
    ASSERT(!cin == false);
    string s;
    while (cin >> s)
    {
        if (s.substr(0, 2) == "n=")
        {
            nodes = atoi(s.substr(2).c_str());
            continue;
        }
        if (s.substr(0, 2) == "m=")
        {
            edges = atoi(s.substr(2).c_str());
            continue;
        }
        ASSERT(false);
    }
    TRACE(n, m);
    cin.close();
  }

  /* Function to import graph from file */
  Graph::loadGraph(){
    FILE *fin = fopen((graph_file).c_str(), "r");
    ASSERT(fin != false);
    int readCnt = 0;
    for (int i = 0; i < m; i++)
    {
        readCnt ++;
        int user1, user2;
        double weight;
        int c = fscanf(fin, "%d%d%lf", &user1, &user2, &p);
        ASSERTT(c == 3, user1, user2, weight, c);

        //TRACE_LINE(a, b);
        // check if node ids within graph array range
        ASSERT( user1 < nodes );
        ASSERT( user2 < nodes );
        hasnode[user1] = true;
        hasnode[user2] = true;
        this->add_edge(user1, user2, weight);
    }
    TRACE_LINE_END();
    int s = 0;
    for (int i = 0; i < n; i++)
        if (hasnode[i])
            s++;
    INFO(s);
    ASSERT(readCnt == m);
    fclose(fin);
  }

  /* Function to calculate influence score of seed set */
  Graph::influenceScore(int sim){
    double score;
    for (int i = 0; i < sim; i++){
      // run random walk
    }
  }

  /* Function to perform influence coverage from seed set */
  Graph::influenceCoverage(){
    int score = 0;

    return score;
  }

  Graph::print(){
    cout << dataset << " graph:" << endl;
    for(int i = 0; i < nodes; i++){
      for (int j = 0; j < graph[i].size(); j++){
        cout << "(" << i << ")" <<"-["<< graph[i][j].second <<"]->"
             << "(" << graph[i][j].first << ")" << endl;
      }
    }
  }

};
