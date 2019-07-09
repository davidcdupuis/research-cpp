#include "InfScore.h"

using namespace std;

const int SLEEP = 2; // duration of sleep

InfScore::InfScore(Graph& g):graph(g){
  srand(time(NULL));
}

void InfScore::importSeedSet(string filePath){
  int user;
  seedSet.clear();
  // string file = "../../data/" + graph.dataset + "/" + file_path;
  cout << "Importing from: " << filePath << endl;
  if (!pathExists(filePath)){
    cerr << "Error path doesn't exist: " << filePath << " in " << __FILE__ << " at line " << __LINE__ << endl;
    exit(1);
  }
  ifstream infile(filePath.c_str());
  while(infile >> user){
    seedSet.push_back(user);
  }
  cout << "Seed set imported correctly!" << endl;
  sleep(SLEEP);
  clearLines(2);
}

int InfScore::run(string prevClass){
  return main(prevClass);
}

int InfScore::main(string prevClass){
  int result = 0;
  const int LINES = 9;
  while (result == 0){
    int choice = -1;
    string input;
    printTitle(60,"Compute Influence Score","cyan",'_');
    cout << "\t[1] with Monte Carlo" << endl;
    cout << "\t[2] with activation probabilities" << endl;
    cout << "\t[3] run infscore test" << endl;
    cout << "\t[4] run convergence test" << endl;
    cout << "\t[5] Return to " << prevClass << endl;
    cout << "\t[6] Exit Program" << endl;
    while(choice == -1){
      string val;
      cout << "> choice : ";
      getline(cin, val);
      choice = stoi(val);
      switch(choice){
        case 1:
          clearLines(LINES);
          result = computeSeedScoreMenu();
          break;
        case 2:
          clearLines(LINES);
          result = 0;
          break;
        case 3:
          clearLines(LINES);
          infScoreTest();
          result = 0;
          break;
        case 4:
          clearLines(LINES);
          convergenceTest();
          result = 0;
          break;
        case 5:
          clearLines(LINES);
          return -1;
          break;
        case 6:
          clearLines(LINES);
          return -2;
          break;
        default:
          cout << "Error: choice not recognized!" << endl;
          sleep(SLEEP);
          clearLines(2);
          choice = -1;
      }
    }
    break;
  }
  return result;
}

int InfScore::monteCarloMenu(string prevClass){
  int result = 0;
  while (result == 0){
    int choice;
    string input;
    cout << string(60,'_') << endl;
    cout << "Monte Carlo Arguments [# simulations, depth, seed set file]:" << endl;
    // asking for # simulations
    while(1){
      cout << "> simulations (" << simulations << "): ";
      getline(cin, input);
      if(input != ""){
        try{
          choice = stoi(input);
          if (choice > 0 && choice < 1000){
            simulations = choice;
            clearLines(1);
            cout << "> simulations (" << simulations << "): ";
            printInColor("yellow", to_string(simulations));
            break;
          }else{
            cout << "Error: expecting int value between 0 and 10000" << endl;
            sleep(SLEEP);
            clearLines(2);
          }
        }catch(invalid_argument& e){
          cout << "Error: invalid input!" << endl;
          sleep(SLEEP);
          clearLines(2);
        }
      }else{
        clearLines(1);
        cout << "> simulations (" << simulations << "): ";
        printInColor("yellow", to_string(simulations));
        break;
      }
    }
    // asking for depth
    while(1){
      cout << "> depth (" << depth << "): ";
      getline(cin, input);
      if(input != ""){
        try{
          choice = stoi(input);
          if (choice > 0 && choice <= 10000){
            depth = choice;
            clearLines(1);
            cout << "> simulations (" << depth << "): ";
            printInColor("yellow", to_string(depth));
            break;
          }else{
            cout << "Error: expecting int value between 0 and 10000" << endl;
            sleep(SLEEP);
            clearLines(2);
          }
        }catch(invalid_argument& e){
          cout << "Error: invalid input!" << endl;
          sleep(SLEEP);
          clearLines(2);
        }
      }else{
        clearLines(1);
        cout << "> depth (" << depth << "): ";
        printInColor("yellow", to_string(depth));
        break;
      }
    }
    // ask for seed set file
    // if necessary import graph
    if(!graph.loaded){
      graph.loadGraph("");
      graph.loaded = true;
    }
    // import seed set
    double score;
    printInColor("magenta", string(60, '-'));
    printLocalTime("magenta", "Computing Monte Carlo score", "starting");
    score = mcInfScore();
    printLocalTime("magenta", "Computing Monte Carlo score", "ending");
    printInColor("magenta", string(60, '-'));
    result = continueMenu(prevClass, "mcInfScore");
  }
  if (result < 0){
    return result;
  }else{
    return result - 1;
  }
}

int InfScore::computeSeedScoreMenu(){
  // seedSet.clear(); //in case it's a re-run
  int choice = -1;
  string file_path = "../../data/" + graph.dataset + "/";
  cout << string(60,'_') << endl;
  cout << "Choose a folder: " << endl;
  cout << "   [1] rtim/live/" << endl;
  cout << "   [2] imm/basic/" << endl;
  cout << "   [3] imm/" << endl;
  cout << "   [4] osim/" << endl;
  while(choice == -1){
    cout <<  "> choice: ";
    string val;
    getline(cin, val);
    choice = stoi(val);
    switch(choice){
      case 1:
        file_path += "rtim/live/";
        break;
      case 2:
        file_path += "imm/basic/";
        break;
      case 3:
        file_path += "imm/";
        break;
      case 4: 
	file_path += "osim/";
	break;
      default:
        cout << "Error: choice not recognized!" << endl;
        sleep(SLEEP);
        clearLines(2);
        choice = -1;
    }
  }
  // sleep(SLEEP);
  clearLines(6);

  string input;
  cout << "________________________________________" << endl;
  cout << "Input seed set file name" << endl;
  while (1){
    cout << "> seed file ["<< file_path <<"] : ";
    getline(cin, input);
    break;
  }
  file_path += input;
  importSeedSet(file_path);
  clearLines(3);
  // run monte carlo simulations
  printLocalTime("magenta","Computing seed score","starting");
  double score = mcInfScoreParallel();
  cout << "Influence score of " << input << " is " << toColor("yellow", to_string(score)) << endl;
  printLocalTime("magenta","Computing seed score","ending");
  return 0;
}

int InfScore::activationProbabilitiesMenu(string prevClass){
  int result = 0;
  while (result == 0){
    int choice;
    string input;
    printTitle(60,"Activation Probabilities Menu","cyan",'_');
    double score;
    printInColor("magenta", string(60, '-'));
    printLocalTime("magenta", "Computing Activation Probabilities score", "starting");
    score = apInfScore();
    printLocalTime("magenta", "Computing Activation Probabilities score", "ending");
    printInColor("magenta", string(60, '-'));
    result = continueMenu(prevClass, "apInfScore");
  }
  if (result < 0){
    return result;
  }else{
    return result - 1;
  }
}

int InfScore::continueMenu(string prevClass, string prevStage){
  int choice = -1;
  printTitle(60,"Continue","cyan",'_');
  // cout << "   [1] Repeat previous stage with same arguments (" << stage << ")" << endl;
  cout << "\t[1] Repeat previous method -" << prevStage << "-" << endl;
  cout << "\t[2] Choose influence score method" << endl;
  cout << "\t[3] Return to " << prevClass << endl;
  cout << "\t[4] End Program" << endl;
  while(choice == -1){
    cout << "> choice: ";
    string val;
    getline(cin, val);
    choice = stoi(val);
    switch(choice){
      case 1:
        // repeat previous method
        clearLines(7);
        return 0;
      case 2:
        // methods menu
        clearLines(7);
        return 1;
      case 3:
        // return to prevClass
        clearLines(7);
        return -1;
      case 4:
        // exit program
        clearLines(7);
        return -2;
      default:
        cout << "Error: choice not recognized!" << endl;
        choice = -1;
        clearLines(2);
    }
  }
}

// Monte Carlo

long double InfScore::mcInfScore(){
  // cout << "Computing influence score of: " << printSeed(seed_set) << endl;
  unsigned long long sum = 0;
  int values[simulations] = {};
  bool visitedOriginal[graph.nodes] = {};
  bool visited[graph.nodes];
  for (int i = 0; i < simulations; i++){
    // run influence simulation
memcpy(visited, visitedOriginal, graph.nodes);
    values[i] = infSimulation(visited);
  }
  for(int i = 0; i < simulations; i++){
    sum += values[i];
  }
  long double score = sum/(double)simulations;
  // cout << "Influence score is " << score << endl;
  return score;
}

long double InfScore::mcInfScoreParallel(){
  // cout << "Computing influence score of: " << printSeed(seed_set) << endl;
  unsigned long long sum = 0;
  int values[simulations] = {};
  bool visitedOriginal[graph.nodes] = {};
  //bool* visitedOriginal = (bool*) calloc (nodes,sizeof(bool));
  #pragma omp parallel shared(values) //depth, seedSet,
  {
    bool visited[graph.nodes];
    //bool* visited = (bool*) calloc(nodes,sizeof(bool));
    #pragma omp for
    for (int i = 0; i < simulations; i++){
      // run influence simulation
      memcpy(visited, visitedOriginal, graph.nodes);
      //bool* visited = (bool*) calloc (nodes, sizeof(bool));
      values[i] = infSimulation(visited);
    }
  }
  for(int i = 0; i < simulations; i++){
    sum += values[i];
    //cout << "sum: " << sum << endl;
  }
  long double score = sum/(double)simulations;
  /*cout << "sum: " << sum << endl;
  cout << "sim: " << simulations << endl;
  cout << "score: " << score << endl;
  cout << endl;
  cout << endl;*/
  // cout << "Influence score is " << score << endl;
  return score;
}

int InfScore::infSimulation(bool *visited){
  // cout << "depth: " << depth << endl;
  int activated = 0;
  // vector<int> activated_nodes;
  // bool visited[nodes] = {};
  double r;
  // seed nodes are already activated
  for(int node: seedSet){
    // activated_nodes.push_back(node);
    visited[node] = 1;
  }
  // simulate inf propagation from each seed node
  random_device rd;
  unsigned seed = rd();
  for (int node: seedSet){
    queue< pair<int, int> > queue;
    activated += 1;
    queue.push(make_pair(node, 0));
    while(!queue.empty()){
      pair<int, int> curr = queue.front();
      queue.pop();
      // cout << "(" << curr.first << ", " << curr.second << ") ";
      // test influence of all neigbhors
      // cout << "[ ";
      for(pair<int, double> neighbor: graph.graph[curr.first]){
        // if inf probability to neighbor node is greater than minimum threshold
        // attempt to influence
        if(neighbor.second > minEdgeWeight){
          r = rand_r(&seed)/(double)RAND_MAX;
          // cout << "(" << neighbor.first << " - " << r << ") ";
          // cout << neighbor.first << " - ";
          // check if neighbor is not in activated nodes
          // !(find(activated_nodes.begin(), activated_nodes.end(), neighbor.first)!=activated_nodes.end())
          if ( visited[neighbor.first] == 0 && r <= neighbor.second){
            // if influence increment activated,
            // add to queue, and activated_nodes and increase depth.
            activated += 1;
            // activated_nodes.push_back(neighbor.first);
            visited[neighbor.first] = 1;
            // check if max depth is reached
            if(curr.second + 1 <= depth){
              queue.push(make_pair(neighbor.first, curr.second + 1));
            }
          }
        }
      }
    }
  }
  // cout << "|-> " << activated << " ]" << endl;
  return activated;
}

// ACTIVATION PROBABILITIES

double InfScore::firstNeighborsScore(){
  // identify seed nodes in bool array
  double score = 0;
  bool seedNodes[graph.nodes] = {};
  for(int seed: seedSet){
    seedNodes[seed] = 1;
    score ++;
  }
  for(int seed: seedSet){
    for(pair<int, double> neighbor: graph.graph[seed]){
      if (seedNodes[neighbor.first] != 1){
        score += neighbor.second;
      }
    }
  }

  return score;
}

double InfScore::apInfScore(int depth){
  vector<double> tmpAPs;
  // identify seed nodes in bool array
  bool seedNodes[graph.nodes] = {};
  for(int seed: seedSet){
    seedNodes[seed] = 1;
  }

  // compute activation probability of every user of every node of seed set
  // up to a depth of 2
  for(int seed: seedSet){
    pathExploration(seedNodes, tmpAPs, seed, depth, 1);
  }

  // sum all activation probabilities to return the score
  double score = 0;
  for(double ap: tmpAPs){
    score += ap;
  }
  return score;
}

void InfScore::pathExploration(bool *seedNodes, vector<double> &tmpAPs, int currUser, int depth, double pathWeight){
  // add each new path in list
  tmpAPs.push_back(pathWeight);
  for(pair<int, double> neighbor: graph.graph[currUser]){
    if(depth - 1 >= 0 && seedNodes[neighbor.first] != 1){
      int newD = depth - 1;
      double newPathWeight = pathWeight * neighbor.second;
      pathExploration(seedNodes, tmpAPs, neighbor.first, newD, newPathWeight);
    }
  }
}

void InfScore::saveSeedScoreLog(string file, string startDate, string endDate, double& runtime, double& score){
  string path = "../../data/" + graph.dataset + "/logs/";
  if (!pathExists(path)){
    cerr << "Error path doesn't exist: " << path << " in " << __FILE__ << " at line " << __LINE__ << endl;
    exit(1);
  }
  path += "infscore.log";
  // printInColor("cyan", "Saving seed score log to: " + path);
  ofstream seedScoreLogFile;
  seedScoreLogFile.open(path, fstream::app);
  seedScoreLogFile << "File name  : " << file << endl;
  seedScoreLogFile << "Start date : " << startDate << endl;
  seedScoreLogFile << "End date   : " << endDate << endl;
  seedScoreLogFile << "Runtime    : " << runtime << endl;
  seedScoreLogFile << "Seed size  : " << seedSet.size() << endl;
  seedScoreLogFile << "Inf. score : " << score << endl;
  seedScoreLogFile << "-----------------------------------------------" << endl;
  seedScoreLogFile.close();
}

void InfScore::saveSeedScoreCSV(string file, string startDate, string endDate, double& runtime, double& score){
  string path = "../../data/" + graph.dataset + "/logs/";
  if (!pathExists(path)){
    cerr << "Error path doesn't exist: " << path << " in " << __FILE__ << " at line " << __LINE__ << endl;
    exit(1);
  }
  path += "infscore.csv";
  // printInColor("cyan", "Saving seed score csv to: " + path);

  // dataset,file_name, startDate, endDate, runtime, seed size, score
  ofstream seedScoreCSVFile;
  seedScoreCSVFile.open(path, fstream::app);
  seedScoreCSVFile << graph.dataset << ",";
  seedScoreCSVFile << file << ",";
  seedScoreCSVFile << startDate << ",";
  seedScoreCSVFile << endDate << ",";
  seedScoreCSVFile << runtime << ",";
  seedScoreCSVFile << seedSet.size() << ",";
  seedScoreCSVFile << score << endl;
  seedScoreCSVFile.close();
}

void InfScore::mcInfScoreParallelValues(std::vector<double>& values){
  values.resize(simulations, 0);
  bool visitedOriginal[graph.nodes] = {};
  #pragma omp parallel shared(values)
  {
    bool visited[graph.nodes];
    #pragma omp for
    for (int i = 0; i < simulations; i++){
      memcpy(visited, visitedOriginal, graph.nodes);
      values[i] = infSimulation(visited);
    }
  }
}

// double Graph::influenceScorePath(int node, int max_depth, string type, double edge_weight, double min_weight) const{
//   // if type == 'shortest' use shortest paths, if 'all' use all paths, else return error
//   double score = 1;
//   if (type == "shortest"){
//     // use function of shortest paths of length at most max_depth
//     // if node doesn't have neighbor return 1
//     if(graph[node].size() == 0){
//       score = 1;
//     }else{
//       map< int, double > minDistances;
//       shortestPathsWeights(minDistances, node, min_weight, max_depth, 1);
//       map< int, double >::iterator it = minDistances.begin();
//       while(it != minDistances.end()){
//         score += minDistances[it->first];
//         it++;
//       }
//     }
//   }else if (type == "all"){
//     score = 1;
//   }else{
//     cerr << "Error path type: " << type << " does not exist!" << endl;
//     exit(1);
//   }
//   return score;
// }
//
// double Graph::influenceScoreNeighbors(int node) const{
//   double score = 1;
//   int lim = graph[node].size();
//   for(int i=0; i < lim; i++){
//     score += graph[node][i].second;
//   }
//   return score;
// }
//
// void Graph::shortestPathsWeights(map<int, double> & distances, int node, double min_weight, int max_depth, double curr_dist) const{
//   if (max_depth == 0) {
//     return;
//   }
//
//   // iterate through neighbors of current node
//   for(pair<int, double> neighbor: graph[node]){
//     double new_dist;
//     // use the following condition to explore graph using set edge weights
//     new_dist = curr_dist * neighbor.second;
//
//     // if path weight is less than minimum, we ignore it
//     if (new_dist >= min_weight){
//       // check if new_path distance is greater than older one.
//       if(distances.find(neighbor.first) == distances.end()){
//         // if node not recorded in distances, initialize distance at 0
//         distances[neighbor.first] = 0;
//       }
//       distances[neighbor.first] = max(distances[neighbor.first], new_dist);
//       shortestPathsWeights(distances, neighbor.first, min_weight, max_depth - 1, new_dist);
//     }
//   }
// }
//
// void Graph::shortestPathsWeightsB(double* distances, int node, int max_depth, double curr_dist) const{
//   if (max_depth == 0) {
//     return;
//   }
//
//   for(pair<int, double> neighbor: graph[node]){
//     double new_dist = curr_dist * neighbor.second;
//     // check if new_path distance is greater than older one.
//     distances[neighbor.first] = max(distances[neighbor.first], new_dist);
//     shortestPathsWeightsB(distances, neighbor.first, max_depth - 1, new_dist);
//   }
// }
//
// double Graph::influenceLocalScore(int origine, vector<double>& scores, int node, vector<double>& activationProbs, int maxDepth) const{
//   for(pair<int, double> neighbor: graph[node]){
//     int newDepth;
//     scores[origine] = scores[origine] - activationProbs[neighbor.first];
//     if (maxDepth > 0){
//       newDepth = maxDepth - 1;
//       influenceLocalScore(origine, scores, neighbor.first, activationProbs, newDepth);
//     }
//   }
// }

void InfScore::infScoreTest(){
  vector<string> datasets = {"test"};
  graph.dataset = datasets[0];
  double a1Score, a2Score, a3Score, a4Score, a5Score;
  double runtime;
  for(string dataset: datasets){
    // import dataset
    graph.dataset = dataset;
    //initializeAlgoLog();
    graph.readAttributes();
    graph.loadGraph("");
    vector<int> sizes = {50};//{3000, 5000, 10000};
    for(int size: sizes){
      for(int i = 1; i < 4; i++){
        // generate random seet set
        seedSet = randomSeedGenerator(graph.nodes, size);

        // run APD1
        clock_t start= clock();
        a1Score = firstNeighborsScore();
        runtime = (double)(clock() - start)/CLOCKS_PER_SEC;
        recordAlgoLog(graph.dataset, size, i, "APD1", a1Score, runtime);

        // run APD2
        start = clock();
        a2Score = apInfScore();
        runtime = (double)(clock() - start)/CLOCKS_PER_SEC;
        recordAlgoLog(graph.dataset, size, i, "APD2", a2Score, runtime);

        // run MCD2
        depth = 2;
        start = clock();
        a3Score = mcInfScoreParallel();
        runtime = (double)(clock() - start)/CLOCKS_PER_SEC;
        recordAlgoLog(graph.dataset, size, i, "MCD2", a3Score, runtime);

        // run MCD3
        depth = 3;
        start = clock();
        a4Score = mcInfScoreParallel();
        runtime = (double)(clock() - start)/CLOCKS_PER_SEC;
        recordAlgoLog(graph.dataset, size, i, "MCD3", a4Score, runtime);

        // run MC

        depth = 10000;
        start = clock();
        a5Score = mcInfScoreParallel();
        runtime = (double)(clock() - start)/CLOCKS_PER_SEC;
        recordAlgoLog(graph.dataset, size, i, "MC", a5Score, runtime);

        cout << left << setw(9) << dataset;
        cout << left << setw(7) << seedSet.size();
        cout << left << setw(3) << i << " : ";
        cout << left << setw(5) << "APD1";
        cout << left << setw(11) << a1Score;
        cout << left << setw(7) << "| APD2";
        cout << left << setw(11) << a2Score;
        cout << left << setw(7) << "| MCD2";
        cout << left << setw(11) << a3Score;
        cout << left << setw(7) << "| MCD3";
        cout << left << setw(11) << a4Score;
        cout << left << setw(7) << "| MC";
        cout << left << setw(11) << a5Score;
        cout << endl;

      }
    }
  }
}

void InfScore::initializeAlgoLog(){
  string path = "../../data/" + graph.dataset + "/infscore/";
  if (!pathExists(path)){
    cerr << "Error path doesn't exist: " << path << " in " << __FILE__ << " at line " << __LINE__ << endl;
    exit(1);
  }
  path += "perfomance_log.txt";
  printInColor("cyan", "New infscore log      : " + path);
  ofstream infScoreLog;
  infScoreLog.open(path);
  infScoreLog << left << setw(8) << "Dataset";
  infScoreLog << left << setw(12) << "seedSetSize";
  infScoreLog << left << setw(15) << "seedSetVersion";
  infScoreLog << left << setw(10) << "algorithm";
  infScoreLog << left << setw(9) << "score";
  infScoreLog << left << setw(12) << "time (s)";
  infScoreLog << endl;
}

void InfScore::recordAlgoLog(std::string dataset, int size, int version, std::string algo, double score, double runtime){
  string path = "../../data/" + graph.dataset + "/infscore/perfomance_log.txt";
  if (!pathExists(path)){
    cerr << "Error path doesn't exist: " << path << " in " << __FILE__ << " at line " << __LINE__ << endl;
    exit(1);
  }
  ofstream infScoreLog;
  infScoreLog.open(path, fstream::app);
  infScoreLog << left << setw(8) << dataset;
  infScoreLog << left << setw(12) << size;
  infScoreLog << left << setw(15) << version;
  infScoreLog << left << setw(10) << algo;
  infScoreLog << left << setw(9) << score;
  infScoreLog << left << setw(12) << runtime;
  infScoreLog << endl;
}

void InfScore::convergenceTest(){
  // run
  vector<string> datasets = {"twitter"};
  for(string dataset: datasets){
    // import dataset
    graph.dataset = dataset;
    graph.readAttributes();
    graph.loadGraph("");
    vector<int> sizes = {50, 200, 500, 1000, 3000, 5000, 10000};
    for(int size: sizes){
      initializeConvergenceLog(graph.dataset, size);
      // initialize csv file
      for(int v = 1; v < 4; v++){ // go to 4
        seedSet = randomSeedGenerator(graph.nodes, size);
        vector<double> values;
        // run Monte Carlo with 50,000 sims and record values
        mcInfScoreParallelValues(values);
        // traverse all values, compute estimated score
        unsigned long long sum = 0;
        double sims = 0;
        double avg = 0;
        for(int i=0; i < values.size(); i++){
          sims++;
          sum += values[i];
          avg = sum / sims;
          recordConvergenceLog(graph.dataset, size, v, sims, avg);
        }
      }
    }
  }
}

void InfScore::initializeConvergenceLog(string dataset, int seedSize){
  string path = "../../data/" + dataset + "/infscore/convergence/";
  if (!pathExists(path)){
    cerr << "Error path doesn't exist: " << path << " in " << __FILE__ << " at line " << __LINE__ << endl;
    exit(1);
  }
  path += dataset + "_s" + to_string(seedSize) + "_convergence.csv";
  printInColor("cyan", "New convergence log : " + path);
  ofstream infScoreLog;
  infScoreLog.open(path);
  infScoreLog << "version,simulations,score" << endl;
}

void InfScore::recordConvergenceLog(string dataset, int seedSize, int version, int sim, double score){
  string path = "../../data/" + dataset + "/infscore/convergence/";
  if (!pathExists(path)){
    cerr << "Error path doesn't exist: " << path << " in " << __FILE__ << " at line " << __LINE__ << endl;
    exit(1);
  }
  path += dataset + "_s" + to_string(seedSize) + "_convergence.csv";
  ofstream infScoreLog;
  infScoreLog.open(path, fstream::app);
  infScoreLog << version << "," << sim << "," << score << endl;
}
