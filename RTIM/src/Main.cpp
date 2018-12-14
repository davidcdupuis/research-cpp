#include "Main.h"

#include <unistd.h>
#include <iomanip>

using namespace std;

const int SLEEP = 2;

Main::Main(){
  // should we initialize object arguments here?
  args = Arguments();
  args.loadDatasetsData();
  // graph = Graph(args);
  // rtim(args);
  srand(time(NULL));
}


void Main::mainMenu(){
  int choice = -1;
  cout << string(25, '_') + " Main Menu " + string(24, '_') << endl;
  cout << " [1] pre-configured experiments" << endl;
  cout << " [2] personalized experiments" << endl;
  while (choice == -1){
    cout <<  "> choice: ";
    string val;
    getline(cin, val);
    // test if int
    choice = stoi(val);
    switch(choice){
      case 1:
        experimentsMenu();
        break;
      case 2:
        datasetsMenu();
        break;
      default:
        cout << "Error: choice not recognized!" << endl;
        sleep(SLEEP);
        clearLines(2);
        choice = -1;
    }
  }
  clearLines(4);
}


void Main::experimentsMenu(){
  string input;
  cout << string(24, '_') + " Experiments " + string(23, '_') << endl;
  cout << "Input file path from: ../data/" << endl;
  while (1){
    cout << "> choose file [ experiments.txt ] : ";
    getline(cin, input);
    // run experiments.txt
    runExperiments("../../data/experiments.txt");
    break;
  }
}


void Main::experimentsContinueMenu(){
  int choice = -1;
  cout << string(25, '_') + " Continue " + string(25, '_') << endl;
  cout << "Choose option:" << endl;
  cout << " [1] Experiments Menu" << endl;
  cout << " [2] Main menu" << endl;
  while (choice == -1){
    string val;
    getline(cin, val);
    // test if int
    choice = stoi(val);
    switch(choice){
      case 1:
        experimentsMenu();
        break;
      case 2:
        mainMenu();
        break;
      default:
        cout << "Error choice not recognized!" << endl;
        sleep(2);
        clearLines(2);
        choice = -1;
    }
  }
}


void Main::datasetsMenu(){
  int lines = args.datasetNames.size() + 2;
  int choice = -1;
  string dataset;
  cout << string(25, '_') + " Datasets " + string(25, '_') << endl;
  // if(args.dataset != "" && graph.graph.size() != 0){
  //   cout << "Current imported dataset: " << args.dataset << endl;
  //   lines++;
  // }

  for (int i = 0; i < args.datasetNames.size(); i++){
    cout << "\t" << left << setw(17) << "[" + to_string(i) + "] " + args.datasetNames[i];
    cout << "(";
    cout << right << setw(14) << cleanLongInt(args.datasetNodes[i]);
    cout << ",";
    cout << right << setw(14) << cleanLongInt(args.datasetEdges[i]);
    cout << ")" << endl;
  }
  while(1){
    cout <<  "> choice: ";
    string val;
    getline(cin, val);
    choice = stoi(val);
    dataset = args.datasetNames[choice];
    if(dataset != "test" && dataset != "nethept" && dataset != "dblp" && dataset != "orkut" && dataset != "youtube" && dataset != "twitter" && dataset != "livejournal"){
      cout << "Dataset not recognized: " << dataset << endl;
      sleep(SLEEP);
      clearLines(2);
    } else if (args.dataset == dataset){
      cout << "Dataset has already been imported!" << endl;
      sleep(SLEEP);
      // return 0;
    }else{
      args.dataset = dataset;
      //graph.directory = "../../data/" + args.dataset; //necessary to readAttributes
      //graph.readAttributes();
      // graph.nodes = args.datasetNodes[choice];
      // graph.edges = args.datasetEdges[choice];
      // nodes = graph.nodes;
      // args.streamSize = nodes / 10;
      break;
    }
  }
  clearLines(lines);
  algorithmMenu();
  // return 1;
}


void Main::algorithmMenu(){
  int choice = -1;
  cout << string(24,'_') + " Algorithms " + string(24,'_') << endl;
  cout << "\t[1] RTIM" << endl;
  cout << "\t[2] IMM" << endl;
  cout << "\t[3] Compute Score"<< endl;
  cout << "\t[4] Test" << endl;
  cout << "\t[5] EXIT" << endl;
  while (choice == -1){
    cout <<  "> choice: ";
    string val;
    getline(cin, val);
    choice = stoi(val);
    switch(choice){
      case 1:
        // run rtim
        rtim.run();
        break;
      case 2:
        // run imm
        // imm.run();
        break;
      case 3:
        // run compute score
        break;
      case 4:
        // run test
        break;
      case 5:
        // exit
      default:
        cout << "Error choice not recognized!" << endl;
        sleep(2);
        clearLines(2);
        choice = -1;
    }
  }
}


void Main::runExperiments(string path){
  printInColor("magenta", string(60,'-'));
  printLocalTime("magenta", "Experiments", "starting");
  //
  readExperiments(path);
  //
  printInColor("magenta", string(60,'-'));
  printLocalTime("magenta", "Experiments", "ending");
  experimentsContinueMenu();
}


void Main::readExperiments(string path){

  if (!pathExists(path)){
    cerr << "Error in readExperiments, path doesn't exist: " << path << endl;
    exit(1);
  }
  cout << "reading experiments" << endl;
  string line;
  ifstream infile(path.c_str());
  for(string line; getline(infile, line);){
    if (line[0] != '#'){
      args.getArguments(line);
      args.printArguments();
    }
  }
}


void Main::run(){
  cout << endl;
  cout << ">>> TEST RUN <<<" << endl;
  printLocalTime("red", "Program", "starting");
  mainMenu();
  printLocalTime("red", "Program", "ending");
}


int main(){
  Main main = Main();
  main.run();
  // main.datasetsMenu();
}
