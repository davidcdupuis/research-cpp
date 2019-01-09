#include "OSIM.h"

OSIM::OSIM(Grap& g):graph(g){
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

void OSIM::extractNonActivated(bool *activated, set<int> nonActivated){
  // iterate through 'activated' add values equal to 0 to 'nonActivated'
}

void OSIM::findOptimalSize(int sim){
  // 1. randomly choose a node from the body of non-activated users
  // 2. simulate influence from that node, recording activated users
  // 3. repeat 1. and 2. until all nodes have been selected
  // repeat 1. 2. and 3. n number of times

  // we can parallelize this for loop
  int values[sim] = {};
  for(int i=0; i < sim; i++){

    // these are all private variables in a parallel setting
    int totActivated = 0;
    bool activated[graph.nodes] = {};
    set<int> nonActivated; //initialize with all nodes
    vector<int> selectedUsers;
    int randUser;

    while(totActivated < graph.nodes){
      extractNonActivated(activated, nonActivated);
      randUser = selectRandomUser(nonActivated);
      selectedUsers.push_back(randUser);
      // simulate influence propagation from randUser
    }
    values[i] = selectedUsers.size();
    // once over record size of selectedUsers in array of values
  }



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

int OSIM::run(string prevClass){
  return paramsMenu(prevClass);
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
          result = optSizeMenu(prevClass);
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
