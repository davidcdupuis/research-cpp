#include "OSIM.h"

using namespace std;

const int SLEEP = 2; // duration of sleep

OSIM::OSIM(Graph& g):graph(g){
  srand(time(NULL));
}

void OSIM::reduceGraph(int top){
  // rank nodes based on outDegrees, remove 100 - top % = bottom %
  // re-generate graph without those nodes
}

int OSIM::selectRandomUser(set<int>& s){
  double r = rand() % s.size();
  set<int>::const_iterator it(s.begin());
  // return value at position r
  advance(it, r);
  return *it;
}

void OSIM::extractNonActivated(bool *activated, set<int>& nonActivated){
  // iterate through 'activated' add values equal to 0 to 'nonActivated'
  nonActivated.clear();
  for(int i=1;i < graph.nodes; i++){
    if (activated[i] == 0){
      nonActivated.insert(i);
    }
  }
}

void OSIM::findOptimalSize(int sim){
  // 1. randomly choose a node from the body of non-activated users
  // 2. simulate influence from that node, recording activated users
  // 3. repeat 1. and 2. until all nodes have been selected
  // repeat 1. 2. and 3. n number of times

  // we can parallelize this for loop
  printLocalTime("magenta", "Finding optimal size", "starting");
  int values[sim] = {};

  // this for loop should be parallelized, to run multiple simulations at once
  #pragma omp parallel shared(values)
  #pragma omp for
  for(int i=0; i < sim; i++){

    // these are all private variables in a parallel setting
    int totActivated = 0;
    bool activated[graph.nodes] = {};
    set<int> nonActivated; //initialize with all nodes
    vector<int> selectedUsers;
    int randUser;

    while(totActivated < graph.nodes - 1){
      extractNonActivated(activated, nonActivated);
      randUser = selectRandomUser(nonActivated);
      selectedUsers.push_back(randUser);
      // simulate influence propagation from randUser
      // cout << "selected user: " << randUser << endl;
      totActivated += influenceSimulation(randUser, activated);
      // print selected users
      // make them activated
      // repeat
    }
    //if(i > 1){
    //  clearLines(1);
    //}
    // cout << "Simulation " << i << " done!" << endl;
    values[i] = selectedUsers.size();
    // once over record size of selectedUsers in array of values
  }
  // compute average of values
  double sum = 0;

  string path = "../../data/" + graph.dataset + "/osim/convergence/";
  if (!pathExists(path)){
    cerr << "Error path doesn't exist: " << path << " in " << __FILE__ << " at line " << __LINE__ << endl;
    exit(1);
  }
  path += graph.dataset + "_s" + to_string(subgraphSize) + "_sim" + to_string(simulations) + ".txt";
  ofstream simSizesFile;
  simSizesFile.open(path, fstream::app);

  for(int value: values){
    sum += value;
    simSizesFile << value << endl;
  }
  simSizesFile.close();
  optimalSize = sum/sim;
  cout << "Optimal size found is: " << optimalSize << endl;
}

void OSIM::findRandomSeedSet(){
  //
}

void OSIM::findBestSeedSet(int sim){
  printLocalTime("magenta", "Finding best seed set", "starting");
  int values[sim] = {};
  set<vector<int> > setSeedSets;
  #pragma omp parallel shared(values)
  #pragma omp for
  for(int i=0; i < sim; i++){

    // these are all private variables in a parallel setting
    int totActivated = 0;
    bool activated[graph.nodes] = {};
    set<int> nonActivated; //initialize with all nodes
    vector<int> selectedUsers;
    int randUser;

    while(totActivated < graph.nodes - 1){
      extractNonActivated(activated, nonActivated);
      randUser = selectRandomUser(nonActivated);
      selectedUsers.push_back(randUser);
      // simulate influence propagation from randUser
      // cout << "selected user: " << randUser << endl;
      totActivated += influenceSimulation(randUser, activated);
      // print selected users
      // make them activated
      // repeat
    }
    //if(i > 0){
    //  clearLines(1);
    //}
    // cout << "Simulation " << i << " done!" << endl;
    values[i] = selectedUsers.size();
    setSeedSets.insert(selectedUsers);
    // once over record size of selectedUsers in array of values
  }
  double sum = 0;
  for(int value: values){
    sum += value;
  }
  optimalSize = sum/sim;
  cout << "Optimal size found is: " << optimalSize << endl;

  // 1. compute inf. score of all sets
  // vector<int> seedSet;
  printLocalTime("magenta", "Computing influence scores", "starting");
  double bestScore;
  InfScore infscore = InfScore(graph);
  for(vector<int> set: setSeedSets){
    infscore.seedSet = set;
    double score = infscore.mcInfScoreParallel();
    if(score > bestScore){
      bestScore = score;
      seedSet = set;
    }
  }
  printLocalTime("magenta", "Computing influence scores", "ending");

  // 2. save best seed set
  cout << "Best seed set size: " << seedSet.size() << endl;
  cout << "Score of best seed set is: " << bestScore << endl;
  saveSeedSet("bestSeedSet");
  printLocalTime("magenta", "Finding best seed set", "ending");
}

void OSIM::findFrequencySeedSet(int sim){
  /*
  printLocalTime("magenta", "Finding best frequency seed set", "starting");
  int values[sim] = {};
  set<set<int> > setSeedSets;
  // #pragma omp parallel shared(values)
  {
    // #pragma omp for
    for(int i=0; i < sim; i++){

      // these are all private variables in a parallel setting
      int totActivated = 0;
      bool activated[graph.nodes] = {};
      set<int> nonActivated; //initialize with all nodes
      set<int> selectedUsers;
      int randUser;

      while(totActivated < graph.nodes - 1){
        extractNonActivated(activated, nonActivated);
        randUser = selectRandomUser(nonActivated);
        selectedUsers.insert(randUser);
        // simulate influence propagation from randUser
        // cout << "selected user: " << randUser << endl;
        totActivated += influenceSimulation(randUser, activated);
        // print selected users
        // make them activated
        // repeat
      }
      if(i > 1){
        clearLines(1);
      }
      cout << "Simulation " << i << " done!" << endl;
      values[i] = selectedUsers.size();
      setSeedSets.insert(selectedUsers);
      // once over record size of selectedUsers in array of values
    }
  }
  double sum = 0;
  for(int value: values){
    sum += value;
  }
  optimalSize = sum/sim;
  cout << "Optimal size found is: " << optimalSize << endl;

  // once we have computed the optimal size we can extract the best seed set
  while(setSeedSets.size() > 1){
    // 1. compute frequency of nodes
    int frequency[graph.nodes] = {};
    for(vector<int> set: setSeedSets){
      for(int val: set){
        frequency[val] ++;
      }
    }

    // 2. select best and add to seed set
    int best;
    int bestVal = -1;
    for(int i=1; i < graph.nodes; i++){
      if(frequency[i] > bestVal){
        best = i;
        bestVal = frequency[i];
      }
    }
    seedSet.insert(best);

    // 3. Keep sets in which that node belongs
    for(vector<int> set: setSeedSets){
      // if current best is not in set
      if(!set.find(best)){
        setSeedSets.erase(set);
      }
      // remove from setSeedSets
    }
  }
  cout << "Best frequency seed set size: " << seedSet.size() << endl;
  saveSeedSet("bestFrequencySeedSet");
  printLocalTime("magenta", "Finding best frequency seed set", "ending");

  // 5. Compute inf. score of seed set
  // InfScore infscore;
  // infscore.seedSet = seedSet;
  // double score = infscore.mcInfScoreParallel();
  // cout << "Best seed set score is: " << score << endl;
  */
}

int OSIM::influenceSimulation(int user, bool* activated){
  int totActivated = 0;
  bool visited[graph.nodes] = {};
  double r;
  visited[user] = 1;
  activated[user] = 1;
  totActivated ++;
  // simulate inf propagation from each seed node
  random_device rd;
  unsigned seed = rd();
  queue<int> queue;
  queue.push(user);
  while(!queue.empty()){
    int curr = queue.front();
    queue.pop();
    for(pair<int, double> neighbor: graph.graph[curr]){
      // if inf probability to neighbor node is greater than minimum threshold
      // attempt to influence
      r = rand_r(&seed)/(double)RAND_MAX;
      if ( visited[neighbor.first] == 0 && r <= neighbor.second){
        // if influence increment activated,
        // add to queue, and activated_nodes and increase depth.
        activated[neighbor.first] = 1;
        totActivated++;
        // activated_nodes.push_back(neighbor.first);
        visited[neighbor.first] = 1;
        // check if max depth is reached
        queue.push(neighbor.first);
      }
    }
  }
  return totActivated;
}

void OSIM::saveSeedSet(string type){
  string file = "../../data/" + graph.dataset + "/osim/";
  if (!pathExists(file)){
    cerr << "Error path doesn't exist: " << file << " in " << __FILE__ << " at line " << __LINE__ << endl;
    exit(1);
  }
  file += graph.dataset + "_" + type + ".txt";
  ofstream seedSetFile;
  seedSetFile.open(file);
  for (int i = 0; i < seedSet.size() ; i++){
     seedSetFile << seedSet[i] << endl;
  }
  seedSetFile.close();
}

void OSIM::testConvergence(){

}

int OSIM::run(string prevClass){
  return functionsMenu(prevClass);
}

int OSIM::functionsMenu(string prevClass){
  // ask what functions to run
  int result = 0;
  while (result == 0){
    int choice = -1;
    cout << string(26,'_') + " Optimal Size Influence Maximization " + string(26,'_') << endl;
    // cout << " Optimal Size Influence Maximization" << endl;
    cout << "Choose a function" << endl;
    cout << "\t[1] Find optimal size of seed set" << endl;
    cout << "\t[2] Select random seed set of opt. size (not implemented)" << endl;
    cout << "\t[3] Find best of all seed set" << endl;
    cout << "\t[4] Find best seed set with user frequency" << endl;
    cout << "\t[5] Import sub-graph" << endl;
    cout << "\t[6] Return to " << prevClass << endl;
    cout << "\t[7] EXIT PROGRAM" << endl;
    while(choice == -1){
      cout <<  "> choice: ";
      string val;
      getline(cin, val);
      choice = stoi(val);
      switch(choice){
        case 1:
          // find optimal size of seed set
          clearLines(9);
          // find optimal seed size
          if(!graph.loaded){
            graph.loadGraph();
            graph.loaded = true;
          }
          findOptimalSize(simulations);
          saveResults();
          result = 0;
          break;
        case 2:
          // select random seed set of opt. size
          clearLines(9);
          break;
        case 3:
          // find best of all seed sets
          clearLines(9);
          if(!graph.loaded){
            graph.loadGraph();
            graph.loaded = true;
          }
          findBestSeedSet(10000);
          break;
        case 4:
          clearLines(9);
          if(!graph.loaded){
            graph.loadGraph();
            graph.loaded = true;
          }
          findFrequencySeedSet(10);
          result = 0;
          break;
        case 5:
          clearLines(9);
          result = importSubGraphMenu(prevClass);
          break;
        case 6:
          // go to prevClass
          clearLines(9);
          return -1;
        case 7:
          // EXIT Program
          clearLines(9);
          return -2;
        default:
          cout << "Error: choice not recognized!" << endl;
          sleep(SLEEP);
          clearLines(2);
          choice = -1;
      }
    }
  }
  if (result < 0){
    return result;
  }else{
    return result - 1;
  }
}

int OSIM::optSizeMenu(string prevClass){
  // ask number of simulations to run
}

int OSIM::seedSizeMenu(string prevClass){
  // ask size of seed
}

int OSIM::continueMenu(string prevClass){
  // ask if what to do next
}

int OSIM::importSubGraphMenu(string prevClass){
  int result = 0;
  while (result == 0){
    int choice = -1;
    cout << string(26,'_') + " Optimal Size Influence Maximization " + string(26,'_') << endl;
    // cout << " Optimal Size Influence Maximization" << endl;
    cout << "Choose a subgraph of dataset " << graph.dataset << endl;
    cout << "\t[1] 25%" << endl;
    cout << "\t[2] 50%" << endl;
    cout << "\t[3] 75%" << endl;
    cout << "\t[4] Return to " << prevClass << endl;
    cout << "\t[5] EXIT PROGRAM" << endl;
    while(choice == -1){
      cout <<  "> choice: ";
      string val;
      getline(cin, val);
      choice = stoi(val);
      switch(choice){
        case 1:
          // import 25%
          clearLines(9);
          isSubgraph = true;
          subgraphSize = 25;
          graph.datasetFile = "../../data/" + graph.dataset + "/osim";
          readSubGraphAttr(graph.datasetFile + "/attributes_25.txt");
          graph.readAttributes();//graph.datasetFile + "/attributes_25.txt"
          graph.printArguments();
          graph.loadGraph(graph.datasetFile + "/" + graph.dataset + "_wc_25.inf");
          return 0;
        case 2:
          // import 50%
          clearLines(9);
          isSubgraph = true;
          subgraphSize = 50;
          graph.datasetFile = "../../data/" + graph.dataset + "/osim";
          readSubGraphAttr(graph.datasetFile + "/attributes_50.txt");
          graph.readAttributes();//graph.datasetFile + "/attributes_50.txt"
          graph.printArguments();
          graph.loadGraph(graph.datasetFile + "/" + graph.dataset + "_wc_50.inf");
          return 0;
        case 3:
          // import 75%
          clearLines(9);
          isSubgraph = true;
          subgraphSize = 75;
          graph.datasetFile = "../../data/" + graph.dataset + "/osim";
          readSubGraphAttr(graph.datasetFile + "/attributes_75.txt");
          graph.readAttributes();//graph.datasetFile + "/attributes_75.txt"
          graph.printArguments();
          graph.loadGraph(graph.datasetFile + "/" + graph.dataset + "_wc_75.inf");
          return 0;
        case 4:
          // go to prevClass
          clearLines(9);
          return -1;
        case 5:
          // EXIT Program
          clearLines(9);
          return -2;
        default:
          cout << "Error: choice not recognized!" << endl;
          sleep(SLEEP);
          clearLines(2);
          choice = -1;
      }
    }
  }
}

void OSIM::readSubGraphAttr(string folder){
  // cout << "Attributes file: " << folder << endl;
  ifstream infile(folder.c_str());
  //cout << "Loading attributes from: " << folder << endl;
  string s;
  while(infile >> s){
    if(s.substr(0, 2)=="n="){
      subgraphNodes = atoi(s.substr(2).c_str());
      // cout << "Number of nodes: " << nodes << endl;
      continue;
    }
    if (s.substr(0, 2) == "m="){
      subgraphEdges = atoi(s.substr(2).c_str());
      // cout << "Number of edges: " << edges << endl;
      continue;
    }
    cerr << "Error: bad attributes in " << __FILE__ << " at line: " << __LINE__ << endl;
    exit(1);
  }
  infile.close();
}

void OSIM::saveResults(){
  string path = "../../data/" + graph.dataset + "/osim/";
  if (!pathExists(path)){
    cerr << "Error path doesn't exist: " << path << " in " << __FILE__ << " at line " << __LINE__ << endl;
    exit(1);
  }
  path += "results.log";

  ofstream resultsLog;
  resultsLog.open(path, fstream::app);
  resultsLog << "Date           : " << getLocalDatetime() << endl;
  resultsLog << "Graph" << endl;
  resultsLog << "- dataset      : " << graph.dataset << endl;
  resultsLog << "- subgraph     : " << ((isSubgraph) ? ("true") : ("false")) << endl;
  resultsLog << "- size         : " << subgraphSize << "%" << endl;
  resultsLog << "- nodes        : " << subgraphNodes << endl;
  resultsLog << "- edges        : " << subgraphEdges << endl;
  resultsLog << "OSIM" << endl;
  resultsLog << "- simulations  : " << simulations << endl;
  resultsLog << "- time         : " << "not defined" << endl;
  resultsLog << "- optimal size : " << optimalSize << endl;
  resultsLog << "----------------------------------------------------" << endl;
  resultsLog.close();
}
