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
    if(i > 1){
      clearLines(1);
    }
    cout << "Simulation " << i << " done!" << endl;
    values[i] = selectedUsers.size();
    // once over record size of selectedUsers in array of values
  }
  // compute average of values
  double sum = 0;
  for(int value: values){
    sum += value;
  }
  optimalSize = sum/sim;
  cout << "Optimal size found is: " << optimalSize << endl;
}

void OSIM::findRandomSeedSet(){
  //
}

void OSIM::findBestSeedSet(){
  //
}

void OSIM::findFrequencySeedSet(){
  //
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
    cout << "\t[3] Find best score seed set of opt. size (not implemented)" << endl;
    cout << "\t[4] Generate seed set of opt. size with most freq. users (not implemented)" << endl;
    cout << "\t[5] Return to " << prevClass << endl;
    cout << "\t[6] EXIT PROGRAM" << endl;
    while(choice == -1){
      cout <<  "> choice: ";
      string val;
      getline(cin, val);
      choice = stoi(val);
      switch(choice){
        case 1:
          // find optimal size of seed set
          clearLines(6);
          // find optimal seed size
          if(!graph.loaded){
            graph.loadGraph();
            graph.loaded = true;
          }
          findOptimalSize(100);
          result = 0;
          break;
        case 2:
          // select random seed set of opt. size
          clearLines(6);
          break;
        case 3:
          // find best score seed set of opt. size
          clearLines(6);
          break;
        case 4:
          // test menu
          clearLines(9);
          break;
        case 5:
          // go to prevClass
          clearLines(9);
          return -1;
        case 6:
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

int OSIM::optSizeMenu(string prevClass){
  // ask number of simulations to run
}

int OSIM::seedSizeMenu(string prevClass){
  // ask size of seed
}

int OSIM::continueMenu(string prevClass){
  // ask if what to do next
}
