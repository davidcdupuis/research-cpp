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
