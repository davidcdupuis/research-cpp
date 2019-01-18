#include "Main.h"

using namespace std;

const int SLEEP = 2;

Main::Main():graph(),rtim(graph), osim(graph), infScore(graph){
  // should we initialize object arguments here?
  loadDatasetsData();
  // cout << "Osim graph: " << osim.graph.loaded << endl;
  srand(time(NULL));
}


void Main::loadDatasetsData(){
  // check existence of file
  string path = "../../data/datasets.txt";
  string name, id, nodes, edges;
  ifstream infile(path.c_str());
  while(infile >> name >> id >> nodes >> edges){
    map<string, string> tmpMap;
    tmpMap.insert(pair<string, string>("id", id));
    tmpMap.insert(pair<string, string>("nodes", nodes));
    tmpMap.insert(pair<string, string>("edges", edges));
    datasets.insert(pair<string, map<string, string> >(name, tmpMap));
  }
  rtim.datasets = datasets;
}


void Main::mainMenu(){
  int result = 0;
  while(result == 0){
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
          clearLines(4);
          result = experimentsMenu();
          break;
        case 2:
          clearLines(4);
          result = datasetsMenu();
          break;
        default:
          cout << "Error: choice not recognized!" << endl;
          sleep(SLEEP);
          clearLines(2);
          choice = -1;
      }
    }
  }
  return;
  clearLines(4);
}


int Main::experimentsMenu(){
  string input;
  int result = 0;
  while(result == 0){
    cout << string(24, '_') + " Experiments " + string(23, '_') << endl;
    cout << "Input file path from: ../data/" << endl;
    // user makes choice
    while (1){
      cout << "> choose file [ experiments.txt ] : ";
      getline(cin, input);
      // run experiments.txt
      clearLines(3);
      result = runExperiments("../../data/experiments.txt");
      break;
    }
  }
  if(result < 0){
    return result;
  }else{
    return result - 1;
  }
}


int Main::experimentsContinueMenu(){
  int choice = -1;
  cout << string(25, '_') + " Continue " + string(25, '_') << endl;
  cout << "Choose option:" << endl;
  cout << " [1] Experiments Menu" << endl;
  cout << " [2] Main Menu" << endl;
  cout << " [3] Exit Program" << endl;
  while (choice == -1){
    cout <<  "> choice: ";
    string val;
    getline(cin, val);
    // test if int
    choice = stoi(val);
    switch(choice){
      case 1:
        clearLines(6);
        return 0;
      case 2:
        clearLines(6);
        return 1;
      case 3:
        clearLines(6);
        return -1;
      default:
        cout << "Error choice not recognized!" << endl;
        sleep(2);
        clearLines(2);
        choice = -1;
    }
  }
}


int Main::datasetsMenu(){
  int result = 0;
  while (result == 0){
    int lines = datasets.size() + 2;
    int choice = -1;
    string dataset;
    cout << string(25, '_') + " Datasets " + string(25, '_') << endl;
    // if(args.dataset != "" && graph.graph.size() != 0){
    //   cout << "Current imported dataset: " << args.dataset << endl;
    //   lines++;
    // }

    // for (int i = 0; i < datasetNames.size(); i++){
    //   cout << "\t" << left << setw(17) << "[" + to_string(i) + "] " + datasetNames[i];
    //   cout << "(";
    //   cout << right << setw(14) << cleanLongInt(datasetNodes[i]);
    //   cout << ",";
    //   cout << right << setw(14) << cleanLongInt(datasetEdges[i]);
    //   cout << ")" << endl;
    // }
    for(auto it = datasets.begin(); it != datasets.end(); ++it){
      cout << "\t" << left << setw(17) << it->first;
      cout << "(";
      cout << right << setw(14) << cleanLongInt(stoi(it->second["nodes"]));
      cout << ",";
      cout << right << setw(14) << cleanLongInt(stoi(it->second["edges"]));
      cout << ")" << endl;
    }
    while(1){
      cout <<  "> dataset name: ";
      string val;
      getline(cin, val);
      // choice = stoi(val);

      if(val != "test" && val != "nethept" && val != "dblp" && val != "orkut" && val != "youtube" && val != "twitter" && val != "livejournal"){
        cout << "Dataset not recognized: " << val << endl;
        sleep(SLEEP);
        clearLines(2);
      } else if (dataset == val){
        cout << "Dataset has already been imported!" << endl;
        sleep(SLEEP);
        // return 0;
      }else{
        graph.dataset = val;
        // graph.datasetDir = "../../data/" + graph.dataset; //necessary to readAttributes
        graph.loaded = false;
        graph.readAttributes();
        // graph.nodes = args.datasetNodes[choice];
        // graph.edges = args.datasetEdges[choice];
        // nodes = graph.nodes;
        // args.streamSize = nodes / 10;
        break;
      }
    }
    clearLines(lines);
    printDatasetArguments();
    result = algorithmsMenu();
  }
  if (result < 0){
    return result;
  }else{
    return result - 1;
  }
}


int Main::algorithmsMenu(){
  int result = 0;
  while(result == 0){
    int choice = -1;
    cout << string(24,'_') + " Algorithms " + string(24,'_') << endl;
    cout << "\t[1] RTIM" << endl;
    cout << "\t[2] IMM (not available)" << endl;
    cout << "\t[3] Compute Score"<< endl;
    cout << "\t[4] OSIM" << endl;
    cout << "\t[5] Test (not available)" << endl;
    cout << "\t[6] Return to Main Menu" << endl;
    cout << "\t[7] EXIT" << endl;
    while (choice == -1){
      cout <<  "> choice: ";
      string val;
      getline(cin, val);
      choice = stoi(val);
      switch(choice){
        case 1:
          // run rtim
          clearLines(8);
          result = rtim.run("Main");
          if (result == -2){
            result = -1;
          }else if (result == -1){
            result = 0;
          }
          break;
        case 2:
          // run imm
          // clearLines(8);
          // imm.run();
          // if (result == -2){
          //   result = -1;
          // }else if (result == -1){
          //   result = 0;
          // }
          cout << "Option not available!" << endl;
          sleep(2);
          clearLines(2);
          choice = -1;
          break;
        case 3:
          // run compute score
          clearLines(9);
          result = infScore.run("Main");
          if (result == -2){
            result = -1;
          }else if (result == -1){
            result = 0;
          }
          // cout << "Option not available!" << endl;
          // sleep(2);
          // clearLines(2);
          // choice = -1;
          break;
        case 4:
          clearLines(9);
          result = osim.run("Main");
          if (result == -2){
            result = -1;
          }else if (result == -1){
            result = 0;
          }
          break;
        case 5:
          // run test
          // clearLines(8);
          cout << "Option not available!" << endl;
          sleep(2);
          clearLines(2);
          choice = -1;
          break;
        case 6:
          // main menu
          clearLines(9);
          result = 2;
          break;
        case 7:
          //exit
          clearLines(9);
          result = -1;
          break;
        default:
          cout << "Error choice not recognized!" << endl;
          sleep(2);
          clearLines(2);
          choice = -1;
      }
    }
  }
  if(result < 0){
    return result;
  }else{
    return result - 1;
  }
}


int Main::runExperiments(string path){
  printInColor("magenta", string(60,'-'));
  printLocalTime("magenta", "Experiments", "starting");
  //
  readExperiments(path);
  //
  printLocalTime("magenta", "Experiments", "ending");
  printInColor("magenta", string(60,'-'));
  return experimentsContinueMenu();
}


void Main::readExperiments(string path){
  if (!pathExists(path)){
    cerr << "Error path doesn't exist: " << path << " in " << __FILE__ << " at line " << __LINE__ << endl;
    exit(1);
  }
  cout << "reading experiments" << endl;
  string line;
  ifstream infile(path.c_str());
  for(string line; getline(infile, line);){
    if (line[0] != '#'){
      getArguments(line);
      printDatasetArguments();
    }
  }
}


void Main::run(){
  cout << endl;
  printLocalTime("red", "Program", "starting");
  mainMenu();
  printLocalTime("red", "Program", "ending");
}


void Main::printDatasetArguments(){
  cout << string(26, '-') << toColor("red", " Dataset ") << string(25, '-') << endl;
  cout << "- name         : " << toColor("yellow", graph.dataset);
  cout << " [ v = " << toColor("yellow", to_string(graph.nodes)) << " | ";
  cout << "e = " << toColor("yellow", to_string(graph.edges))  << "]" << endl;
  cout << "- model        : " << toColor("yellow", graph.infModel) << endl;
  if(graph.edgeWeight == -1){
    cout << "- edge weights : " << toColor("yellow", "weighted cascade") << endl;
  }else{
    cout << "- edge weights : " << toColor("yellow", properStringDouble(graph.edgeWeight)) << endl;
  }
  cout << string(60, '-') << endl;
}


void Main::getArguments(int argn, char **argv){
//   for (int i = 0; i < argn; i++)
//   {
//     // print help
//     if (argv[i] == string("-help") || argv[i] == string("--help") || argn == 1){
//       printHelp();
//       exit(1);
//     }
//
//     // define stage to run
//     if (argv[i] == string("-stage")){
//       if ((argv[i + 1] != string("pre")) && (argv[i + 1] != string("live")) && (argv[i + 1] != string("newStream")) && (argv[i + 1] != string("special"))){
//         cerr << "Error: stage not recognized [ " << argv[i + 1] << " ]"<< endl;
//         exit(1);
//       }else{
//         stage = argv[i + 1];
//       }
//     }
//
//     // define dataset to use
//     if (argv[i] == string("-dataset")){
//       // verify if dataset among list of available datasets in directory
//       dataset = argv[i + 1];
//       if(dataset != "test" && dataset != "nethept" && dataset != "dblp" && dataset != "orkut" && dataset != "youtube" && dataset != "twitter" && dataset != "livejournal"){
//         cerr << "Dataset not recognized: " << dataset << endl;
//         exit(1);
//       }
//     }
//
//     // define model to use
//     if (argv[i] == string("-model")){
//       model = argv[i + 1];
//       if (model != "wc" && model != "ic"){
//         cerr << "Model not recognized: " << model << endl;
//         exit(1);
//       }
//     }
//
//     if (argv[i] == string("-depth")){
//       depth = atoi(argv[i + 1]);
//     }
//
//     // to help define inf. threshold, percentage of top influencers
//     if (argv[i] == string("-reach")){
//       reach = atoi(argv[i + 1]);
//     }
//
//     // activation probability [0, 1]
//     if (argv[i] == string("-actprob")){
//       theta_ap = atof(argv[i + 1]);
//     }
//
//     // max size of seed set to find
//     if (argv[i] == string("-k") || argv[i] == string("--size")){
//       k = atoi(argv[i + 1]);
//     }
//
//     if (argv[i] == string("-streamModel")){
//       streamModel = argv[i + 1];
//     }
//
//     if (argv[i] == string("-streamVersion")){
//       streamVersion = atoi(argv[i + 1]);
//     }
//
//     if (argv[i] == string("-streamSize")){
//       streamSize = atoi(argv[i + 1]);
//     }
//
//     if (argv[i] == string("-edge")){
//       edge_weight = atof(argv[i + 1]);
//     }
//
//     if (argv[i] == string("-minWeight")){
//       min_weight = atof(argv[i + 1]);
//     }
//
//   }
}


void Main::getArguments(string line){
  istringstream iss(line);
  vector<string> words{istream_iterator<string>{iss}, istream_iterator<string>{}};
  for(string s: words){
    cout << s << " ";
  }
  cout << endl;
  for (int i=0; i < words.size(); i++){
    // define stage to run
    if (words[i] == string("-stage")){
      if ((words[i + 1] != string("pre")) && (words[i + 1] != string("live")) && (words[i + 1] != string("newStream")) && (words[i + 1] != string("special"))){
        cerr << "Error: stage not recognized [ " << words[i + 1] << " ] in " << __FILE__ << " at line " << __LINE__ << endl;
        exit(1);
      }else{
        rtim.stage = words[i + 1];
      }
    }

    // define dataset to use
    if (words[i] == string("-dataset")){
      // verify if dataset among list of available datasets in directory
      graph.dataset = words[i + 1];
      if(graph.dataset != "test" && graph.dataset != "nethept" && graph.dataset != "dblp" && graph.dataset != "orkut" && graph.dataset != "youtube" && graph.dataset != "twitter" && graph.dataset != "livejournal"){
        cerr << "Dataset not recognized: " << graph.dataset << " in " << __FILE__ << " at line " << __LINE__ << endl;
        exit(1);
      }
    }

    // define model to use
    if (words[i] == string("-model")){
      graph.infModel = words[i + 1];
      if (graph.infModel != "wc" && graph.infModel != "ic"){
        cerr << "Model not recognized: " << graph.infModel << " in " << __FILE__ << " at line " << __LINE__ << endl;
        exit(1);
      }
    }

    if (words[i] == string("-depth")){
      rtim.maxDepth = stoi(words[i + 1]);
    }

    // to help define inf. threshold, percentage of top influencers
    if (words[i] == string("-reach")){
      rtim.reach = stoi(words[i + 1]);
    }

    // activation probability [0, 1]
    if (words[i] == string("-actprob")){
      rtim.theta_ap = stof(words[i + 1]);
    }

    // max size of seed set to find
    if (words[i] == string("-k") || words[i] == string("--size")){
      rtim.maxSeedSize = stoi(words[i + 1]);
    }

    if (words[i] == string("-streamModel")){
      rtim.streamModel = words[i + 1];
    }

    if (words[i] == string("-streamVersion")){
      rtim.streamVersion = stoi(words[i + 1]);
    }

    if (words[i] == string("-streamSize")){
      rtim.streamSize = stoi(words[i + 1]);
    }

    if (words[i] == string("-edge")){
      graph.edgeWeight = stof(words[i + 1]);
    }

    if (words[i] == string("-minWeight")){
      rtim.minEdgeWeight = stof(words[i + 1]);
    }

    if (words[i] == "-algo" || words[i] == "--algorithm"){
      algorithm = words[i+1];
    }
  }
}


int main(int argn, char **argv){
  Main main = Main();
  // if has arguments run with arguments else run program
  main.run();

}
