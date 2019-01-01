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
    cerr << "Error path doesn't exist: " << filePath << endl;
    exit(1);
  }
  ifstream infile(filePath.c_str());
  while(infile >> user){
    seedSet.push_back(user);
  }
  cout << "Seed set imported correctly!" << endl;
}

int InfScore::run(string prevClass){
  return main(prevClass);
}

int InfScore::main(string prevClass){
  int result = 0;
  while (result == 0){
    int choice = -1;
    string input;
    cout << string(60,'_') << endl;
    cout << "Compute Influence Scores:" << endl;
    cout << "\t[1] with Monte Carlo" << endl;
    cout << "\t[2] with activation probabilities" << endl;
    cout << "\t[3] Return to " << prevClass << endl;
    cout << "\t[4] Exit Program" << endl;
    while(choice == -1){
      string val;
      cout << "> choice : ";
      getline(cin, val);
      choice = stoi(val);
      switch(choice){
        case 1:
          clearLines(7);
          break;
        case 2:
          clearLines(7);
          break;
        case 3:
          clearLines(7);
          break;
        case 4:
          clearLines(7);
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
    // if necessary import graph
    // if necessary import seedSet
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

// void RTIM::computeSeedScoreMenu(){
//   // seedSet.clear(); //in case it's a re-run
//   int choice = -1;
//   string file_path = "../../data/" + graph.dataset + "/";
//   cout << string(60,'_') << endl;
//   cout << "Choose a folder: " << endl;
//   cout << "   [1] rtim/live/" << endl;
//   cout << "   [2] imm/basic/" << endl;
//   cout << "   [3] imm/" << endl;
//   while(choice == -1){
//     cout <<  "> choice: ";
//     string val;
//     getline(cin, val);
//     choice = stoi(val);
//     switch(choice){
//       case 1:
//         file_path += "rtim/live/";
//         break;
//       case 2:
//         file_path += "imm/basic/";
//         break;
//       case 3:
//         file_path += "imm/";
//         break;
//       default:
//         cout << "Error: choice not recognized!" << endl;
//         sleep(SLEEP);
//         clearLines(2);
//         choice = -1;
//     }
//   }
//   sleep(SLEEP);
//   clearLines(7);
//
//   string input;
//   cout << "________________________________________" << endl;
//   cout << "Input seed set file name" << endl;
//   while (1){
//     cout << "> seed file ["<< file_path <<"] : ";
//     getline(cin, input);
//     break;
//   }
//   file_path += input;
//   seedSetPath = file_path;
//   importSeedSet(file_path);
//   sleep(SLEEP);
//   clearLines(5);
//   seedFile = input;
// }

int InfScore::activationProbabilitiesMenu(string prevClass){
  int result = 0;
  while (result == 0){
    int choice;
    string input;
    cout << string(60,'_') << endl;
    cout << "Activation Probabilities Menu:" << endl;
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
  cout << string(60, '_') << endl;
  cout << "Continue: " << endl;
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

double InfScore::mcInfScore(){
  // cout << "Computing influence score of: " << printSeed(seed_set) << endl;
  int sum = 0;
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
  double score = sum/(double)simulations;
  // cout << "Influence score is " << score << endl;
  return score;
}

double InfScore::mcInfScoreParallel(){
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

double InfScore::apInfScore(){
}

void InfScore::saveSeedScoreLog(string file, string startDate, string endDate, double& runtime, double& score){
  string path = "../../data/" + graph.dataset + "/logs/";
  if (!pathExists(path)){
    cerr << "Error path doesn't exist: " << path << endl;
    exit(1);
  }
  path += "seed_set_score.log";
  printInColor("cyan", "Saving seed score log to: " + path);
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
    cerr << "Error path doesn't exist: " << path << endl;
    exit(1);
  }
  path += "seed_set_score.csv";
  printInColor("cyan", "Saving seed score csv to: " + path);

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

// void RTIM::seedComputationTest(int seedSize, int depth, double minEdgeWeight){
//   // select random nodes from graph to generate seed
//   cout << "------------------------------------------------------------------------------------------" << endl;
//   cout << "Starting seed computation test on " << graph.dataset << " > seed size: " << seedSize << " | depth: " << depth << "| minEdgeWeight: " << minEdgeWeight  <<  endl;
//   if (seedSize > graph.nodes){
//     cerr << "Error: Seed set size larger than graph size!" << endl;
//     exit(1);
//   }
//   cout << "Sampling " << seedSize << " nodes for seed." << endl;
//   vector<int> seedSet;
//   double nodesNeeded = seedSize;
//   double nodesLeft = graph.nodes;
//   double selectProb;// = nodesNeeded / nodesLeft;
//   double r;
//   int i = 0;
//   // we select sample with reservoir sampling
//   while (seedSet.size() != seedSize && i < graph.nodes){
//     if(graph.graph[i].size() >= 1){ // ignore nodes that have no neighbors
//       selectProb = nodesNeeded / nodesLeft;
//       r = rand()/(double)RAND_MAX;
//       if (r < selectProb){
//         // cout << i << " - " << r << " <= " << selectProb << " = " << nodesNeeded << " / " << nodesLeft << endl;
//         seedSet.push_back(i);
//         nodesNeeded --;
//       }
//     }
//     nodesLeft --;
//     i ++;
//   }
//   // test computation using monte carlo simulations
//   cout << "Computing seed set score." << endl;
//   double start = omp_get_wtime();
//   double score;
//   score = graph.influenceScoreParallel(seedSet, maxDepth, minEdgeWeight);
//   double delta = omp_get_wtime() - start;
//   cout << "Seed set score is: " << score << " / " << graph.nodes << " computed in: " << cleanTime(delta, "s") << endl;
//   cout << "------------------------------------------------------------------------------------------" << endl;
// }

// void Graph::influenceScoreValues(std::vector<double>& values, const std::vector<int>& seed_set, int depth, int sim) const{
//   values.resize(sim, 0);
//   bool visitedOriginal[nodes] = {};
//   bool visited[nodes];
//   memcpy(visited, visitedOriginal, nodes);
//   #pragma omp parallel shared(depth, seed_set, values)
//   #pragma omp for
//   for (int i = 0; i < sim; i++){
//     // values[i] = influenceSimulation(seed_set, visited, depth);
//   }
// }
//
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
