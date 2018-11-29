#include "Main.h"
#include "Tools.h"
#include "Arguments.h"
#include "RTIM.h"
#include "Graph.h"

#include <string>

using namespace std;

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
    // check existence of file
    // run experiments.txt
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
    switch(choice){
      case 1:
        break;
      case 2:
        break;
      default:
        cout << "Error choice not recognized!" << endl;
        sleep(2);
        clearLines(2);
        choice = -1;
    }
  }
}


void Main::datasetMenu(){
  int lines = 10;
  int choice = -1;
  string dataset;
  cout << string(25, '_') + " Datasets " + string(25, '_') << endl;
  if(args.dataset != "" && graph.graph.size() != 0){
    cout << "Current imported dataset: " << args.dataset << endl;
    lines++;
  }

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
      return 0;
    }else{
      args.dataset = dataset;
      //graph.directory = "../../data/" + args.dataset; //necessary to readAttributes
      //graph.readAttributes();
      graph.nodes = args.datasetNodes[choice];
      graph.edges = args.datasetEdges[choice];
      nodes = graph.nodes;
      args.streamSize = nodes / 10;
      break;
    }
  }
  clearLines(lines);
  return 1;
}


void Main::algorithmMenu(){
  int choice = -1;
  cout << string(24,'_') + " Algorithms " + string(24,'_') << endl;
  cout << "\t[1] RTIM" << endl;
  cout << "\t[2] IMM" << endl;
  cout << "\t[3] Compute Score"<< endl;
  cout << "\t[4] Test" << endl;
  cout << "\t[5] EXIT" << endl;
}


void Main::runExperiments(string path){
  printInColor("magenta", string(60,'-'));
  printLocalTime("magenta", "Experiments", "starting");
  /*
  Read each line in file.
  If new dataset, import
  Run algorithm with stage and parameters
  */
  printInColor("magenta", string(60,'-'));
  printLocalTime("magenta", "Experiments", "ending");
}


void Main::run(){
  cout << endl;
  cout << ">>> TEST RUN <<<" << endl;
  printLocalTime("red", "Program", "starting");
  int choice = 0;
  int loadDataset;
  int loadScores;
  while (choice != -1){
    loadScores = 0;
    loadDataset = 0;
    switch(choice){
      case 1:
        // PREVIOUS STAGE SAME ARGUMENTS
        break;
      case 2:
        // PREVIOUS STAGE MENU
        break;
      case 3:
        // ALGORITHM MENU
        break;
      case 4:
        // DATASET MENU
        break;
      case 5:
        // EXPERIMENTS MENU
        break;
      default:
        // MAIN MENU
        mainMenu();
        // datasetMenu();
        algorithmMenu();
        // varies with algorithm
        break;
    }
    if(loadDataset){
      graph.args = args;
      graph.graph.resize(graph.nodes);
      graph.loadGraph();
      clearLines(3);
    }
    if (args.stage == "pre"){
      printInColor("magenta", string(60,'-'));
      printLocalTime("magenta", "Pre_processing", "starting");
      //
      cout << "Pre-process is running..." << endl;
      //
      printLocalTime("magenta", "Pre_processing", "ending");
      printInColor("magenta", string(60,'-'));
    }else if (args.stage == "live"){
      cout << "Initialize influence scores is running..." << endl;
      printInColor("magenta", string(60,'-'));
      printLocalTime("magenta", "Live", "starting");
      //
      cout << "Live is running..." << endl;
      //
      printLocalTime("magenta", "Live", "ending");
      printInColor("magenta", string(60,'-'));
    }else if (args.stage == "compute_seed_score"){
      printInColor("magenta", string(60,'-'));
      printLocalTime("magenta", "Compute seed score", "starting");
      //
      cout << "Computing seed score is running..." << endl;
      //
      printLocalTime("magenta", "Compute seed score", "ending");
      printInColor("magenta", string(60,'-'));
    }else if (args.stage == "test"){
      printInColor("magenta", string(60,'-'));
      printLocalTime("magenta", "Test", "starting");
      //
      cout << "Test is running..." << endl;
      //
      printLocalTime("magenta", "Test", "ending");
      printInColor("magenta", string(60,'-'));
    } else{
      cout << "Error! stage not recognized: " << args.stage << endl;
      exit(1);
    }
    choice = continueMenu();
  }

  printLocalTime("red", "Program", "ending");
  cout << endl;
}

int main(){
  Main main = Main();
  Arguments args = Arguments();
  args.loadDatasetsData();
  main.run();
}
