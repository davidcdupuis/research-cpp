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
  unsigned long long sum = 0;

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
  cout << "Simulations: "  << sim << endl;
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
  keysFilePath = "../../data/" + graph.dataset + "/";
  return functionsMenu(prevClass);
}

int OSIM::functionsMenu(string prevClass){
  // ask what functions to run
  int result = 0;
  const int LINES = 10;
  while (result == 0){
    int choice = -1;
    printTitle(60,"Optimal Size Influence Maximization","cyan",'_');
    // cout << "Choose a function" << endl;
    cout << "\t[1] Find optimal size of seed set" << endl;
    cout << "\t[2] " << toColor("red","Select random seed set of opt. size") << endl;
    cout << "\t[3] " << toColor("red","Find best of all seed set") << endl;
    cout << "\t[4] " << toColor("red","Find best seed set with user frequency") << endl;
    cout << "\t[5] Import sub-graph" << endl;
    cout << "\t[6] Compute subgraph influence score" << endl;
    cout << "\t[7] Return to " << prevClass << endl;
    cout << "\t[8] EXIT PROGRAM" << endl;
    while(choice == -1){
      cout <<  "> choice: ";
      string val;
      getline(cin, val);
      choice = stoi(val);
      switch(choice){
        case 1:
          // find optimal size of seed set
          clearLines(LINES);
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
          clearLines(LINES);
          break;
        case 3:
          // find best of all seed sets
          clearLines(LINES);
          /*
          if(!graph.loaded){
            graph.loadGraph();
            graph.loaded = true;
          }
          findBestSeedSet(10000);
          */
          break;
        case 4:
          clearLines(LINES);
          /*
          if(!graph.loaded){
            graph.loadGraph();
            graph.loaded = true;
          }
          findFrequencySeedSet(10);
          */
          result = 0;
          break;
        case 5:
          clearLines(LINES);
          result = importSubGraphMenu(prevClass);
          break;
        case 6:
          clearLines(LINES);
          result = computeScoreSubGraphMenu(prevClass);
          break;
        case 7:
          // go to prevClass
          clearLines(LINES);
          return -1;
        case 8:
          // EXIT Program
          clearLines(LINES);
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
  const int LINES = 7;
  while (result == 0){
    printTitle(60,"Import Graph","cyan",'_');
    int choice = -1;
    // cout << "Choose a subgraph of dataset " << graph.dataset << endl;
    cout << "\t[1] Choose model : " << toColor("yellow",subgraphModel) << endl;
    cout << "\t[2] Choose size  : " << toColor("yellow",to_string(subgraphSize)) << endl;
    cout << "\t[3] Import       : " << toColor("yellow",((isSubgraph) ? ("true") : ("false"))) << endl;
    cout << "\t[4] Return to " << prevClass << endl;
    cout << "\t[5] EXIT PROGRAM" << endl;
    while(choice == -1){
      cout <<  "> choice: ";
      string val;
      getline(cin, val);
      choice = stoi(val);
      switch(choice){
        case 1:
          {
            clearLines(1);
            cout << "> model: ";
            getline(cin, val);
            if(val == "original"){
              clearLines(LINES);
              graph.datasetFile = "../../data/" + graph.dataset + "_wc.inf";
              graph.readAttributes();
              graph.loadGraph("");
              isSubgraph = false;
              subgraphModel = "";
              subgraphSize = 100;
              graph.printArguments();
              return 0;
            }
            subgraphModel = val;
            choice = 0;
            clearLines(LINES);
            break;
          }
        case 2:
          {
            clearLines(1);
            cout << "> size: ";
            getline(cin, val);
            subgraphSize = stoi(val);
            choice = 0;
            clearLines(LINES);
            break;
          }
        case 3:
          {
            // import 75%
            clearLines(LINES);
            isSubgraph = true;
            graph.datasetFile = "../../data/" + graph.dataset + "/osim";
            string att = "/attributes_" + subgraphModel + "_" + to_string(subgraphSize) + ".txt";
            string graphFile = "/" + graph.dataset + "_wc_" + subgraphModel + "_" + to_string(subgraphSize) + ".inf";
            readSubGraphAttr(graph.datasetFile + att);
            graph.readAttributes(graph.datasetFile + att);//graph.datasetFile + "/attributes_75.txt"
            graph.printArguments();
            graph.loadGraph(graph.datasetFile + graphFile);
            return 0;
          }
        case 4:
          clearLines(LINES);
          return 0;
        case 5:
          // EXIT Program
          clearLines(LINES);
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

int OSIM::computeScoreSubGraphMenu(string prevClass){
  // ask file name
  // confirm or cancel execution
  string file_path;
  bool convert = false;
  int result = 0;
  while (result == 0){
    file_path = "../../data/" + graph.dataset + "/";
    // keysFilePath = ;
    if(result == 0){ // ask for new arguments
      string input;
      printTitle(60,"Input arguments");
      // ask for file path
      while(1){
        cout << "> file path (" << file_path << "): ";
        getline(cin, input);
        if(input != ""){
          clearLines(1);
          file_path += input;
          cout << "> file path : ";
          if (!pathExists(file_path)){
            cerr << "Error path doesn't exist: " << file_path << endl;
            file_path = "../../data/" + graph.dataset + "/";
            sleep(2);
            clearLines(1);
          }else{
            printInColor("yellow", file_path);
            break;
          }
        }else{
          clearLines(1);
          file_path += "imm/osim/NE_top_25_k50_e0,1_ss.txt";
          cout << "> file path : ";
          printInColor("yellow", file_path);
          break;
        }
      }
      // ask if keys should be converted
      while(1){
        cout << "> convert keys ('no'): ";
        getline(cin, input);
        if(input != ""){
          clearLines(1);
          if(input == "yes"){
            convert = true;
            cout << "> convert keys ('no'): ";
            printInColor("yellow", "yes");
            break;
          }else if (input == "no"){
            convert = false;
            cout << "> convert keys ('no'): ";
            printInColor("yellow", "no");
            break;
          }
        }else{
          clearLines(1);
          cout << "> convert keys ('no'): ";
          printInColor("yellow", "no");
          break;
        }
      }
      if(convert){
        while(1){
          cout << "> file path (" << keysFilePath << "): ";
          getline(cin, input);
          if(input != ""){
            clearLines(1);
            keysFilePath += input;
            cout << "> file path : ";
            if (!pathExists(keysFilePath)){
              cerr << "Error path doesn't exist: " << keysFilePath << endl;
              file_path = "../../data/" + graph.dataset + "/";
              sleep(2);
              clearLines(1);
            }else{
              printInColor("yellow", keysFilePath);
              break;
            }
          }else{
            clearLines(1);
            keysFilePath += "osim/nethept_top_25_keys.txt";
            cout << "> file path : ";
            printInColor("yellow", keysFilePath);
            break;
          }
        }
      }
      // ask for confirmation
      while(1){
        cout << "> proceed ('yes'): ";
        input = "yes";
        getline(cin, input);
        if(input != ""){
          clearLines(1);
          if(input != "yes"){
            cout << "> proceed ('no'): ";
            printInColor("yellow","no");
            clearLines(5);
            return result - 1;
          }
          result = 1;
          break;
        }else{
          clearLines(1);
          cout << "> proceed ('yes'): ";
          printInColor("yellow", "yes");
          result = 1;
          break;
        }
      }
      sleep(SLEEP+1);
      clearLines(5);
    }
    InfScore infscore(graph);
    infscore.importSeedSet(file_path);

    double score;
    if(convert){
      // Import keys
      importKeys(keysFilePath);
      // Convert seed set
      convertKeys(infscore.seedSet, keys);
    }
    printInColor("magenta", string(60, '-'));
    printLocalTime("magenta", "Computing Monte Carlo score", "starting");
    score = infscore.mcInfScore();
    printLocalTime("magenta", "Computing Monte Carlo score", "ending");
    printInColor("magenta", string(60, '-'));
    clearLines(4);
    cout << "Seed file  : " << file_path << endl;
    cout << "MC score   : " << score << endl;
    saveSeedScoreResults(file_path, score);

    // Compute influence score
    // Save results
    // Return to previous menu
  }
  return result - 1;
}

void OSIM::importKeys(string filePath){
  int oldID, newID;
  cout << "Importing from: " << filePath << endl;
  if (!pathExists(filePath)){
    cerr << "Error path doesn't exist: " << filePath << " in " << __FILE__ << " at line " << __LINE__ << endl;
    exit(1);
  }
  ifstream infile(filePath.c_str());
  while(infile >> oldID >> newID){
    keys.insert(pair<int,int>(newID,oldID));
  }
  cout << "Keys imported correctly!" << endl;
  sleep(SLEEP);
  clearLines(2);
}

void OSIM::convertKeys(vector<int>& seedSet, map<int,int> & keys){
  for (int i = 0; i < seedSet.size(); i++){
    // cout << seedSet[i] << endl;
    seedSet[i] = keys[seedSet[i]];
  }
  cout << "Keys converted!" << endl;
}

void OSIM::saveSeedScoreResults(string folder, double score){
  string path = "../../data/" + graph.dataset + "/imm/osim/";
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
  resultsLog << "Seed set" << endl;
  resultsLog << "- file path    : " << folder << endl;
  resultsLog << "- inf. score   : " << score << endl;
  resultsLog << "----------------------------------------------------" << endl;
  resultsLog.close();
}
