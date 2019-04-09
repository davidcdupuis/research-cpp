#include "InfGraph.h"

using namespace std;

InfGraph::InfGraph(string folder, string graph_file): Graph(folder, graph_file){
  sfmt_init_gen_rand(&sfmtSeed , 95082);
  init_hyper_graph();
  visit = vector<bool> (n);
  visit_mark = vector<int> (n);
}

void InfGraph::init_hyper_graph(){
  hyperG.clear();
  for (int i = 0; i < n; i++){
    hyperG.push_back(vector<int>());
  }
  hyperGT.clear();
}

void infGraph::build_hyper_graph_r(int64 R, const Arguments & arg){
  if( R > INT_MAX ){
    cout<<"Error:R too large"<<endl;
    exit(1);
  }
  //INFO("build_hyper_graph_r", R);
  int prevSize = hyperGT.size();
  while ((int)hyperGT.size() <= R){
    hyperGT.push_back( vector<int>() );
  }

  vector<int> random_number;
  for (int i = 0; i < R; i++){
    random_number.push_back(  sfmt_genrand_uint32(&sfmtSeed) % n);
  }
  //trying BFS start from same node
  for (int i = prevSize; i < R; i++){
    BuildHypergraphNode(random_number[i], i );
  }

  int totAddedElement = 0;
  for (int i = prevSize; i < R; i++){
    for (int t : hyperGT[i]){
      hyperG[t].push_back(i);
      //hyperG.addElement(t, i);
      totAddedElement++;
    }
  }
}

/*
 * BFS starting from one node
 */

int InfGraph::BuildHypergraphNode(int uStart, int hyperiiid){
  int n_visit_edge = 1;
  ASSERT((int)hyperGT.size() > hyperiiid);
  hyperGT[hyperiiid].push_back(uStart);

  int n_visit_mark = 0;

  q.clear();
  q.push_back(uStart);
  ASSERT(n_visit_mark < n);
  visit_mark[n_visit_mark++] = uStart;
  visit[uStart] = true;
  while (!q.empty()){
    int expand = q.front();
    q.pop_front();
    if (influModel == IC){
      int i = expand;
      for (int j = 0; j < (int)gT[i].size(); j++){
        //int u=expand;
        int v = gT[i][j];
        n_visit_edge++;
        double randDouble = sfmt_genrand_real1(&sfmtSeed);
        if (randDouble > probT[i][j])
          continue;
        if (visit[v])
          continue;
        if (!visit[v]){
          ASSERT(n_visit_mark < n);
          visit_mark[n_visit_mark++] = v;
          visit[v] = true;
        }
        q.push_back(v);
        ASSERT((int)hyperGT.size() > hyperiiid);
        hyperGT[hyperiiid].push_back(v);
      }
    }
    else if (influModel == LT){
      if (gT[expand].size() == 0)
        continue;
      ASSERT(gT[expand].size() > 0);
      n_visit_edge += gT[expand].size();
      double randDouble = sfmt_genrand_real1(&sfmtSeed);
      for(int i = 0; i < (int)gT[expand].size(); i++){
        ASSERT( i < (int)probT[expand].size());
        randDouble -= probT[expand][i];
        if (randDouble > 0)
          continue;
        //int u=expand;
        int v = gT[expand][i];
        if (visit[v])
          break;
        if (!visit[v]){
          visit_mark[n_visit_mark++] = v;
          visit[v] = true;
        }
        q.push_back(v);
        ASSERT((int)hyperGT.size() > hyperiiid);
        hyperGT[hyperiiid].push_back(v);
        break;
      }
    }
    else
      ASSERT(false);
  }
  for (int i = 0; i < n_visit_mark; i++)
    visit[visit_mark[i]] = false;
  return n_visit_edge;
}
