#include "Arguments.h"

using namespace std;


void Arguments::printHelp(){
  cout << "Help: " << endl;
  cout << "-stage: What stage to run." << endl;
  cout << "\t\t [pre, live]" << endl;
  cout << "-dataset: What dataset to use." << endl;
  cout << "\t\t [test, nethept, dblp, youtube, livejournal, twitter]" << endl;
  cout << "-model: What model to use." << endl;
  cout << "\t\t [wc, ic]" << endl;
  cout << "-depth: What depth to reach when computing influence score." << endl;
  cout << "\t\tDefault is 'infinity'" << endl;
  cout << "-reach: Percentage of top influencers to use as influence threshold." << endl;
  cout << "-actProb: Activation probability to use." << endl;
  cout << "-k --size: Max size of seed set to find." << endl;
  cout << "-streamModel" << endl;
  cout << "-streamVersion" << endl;
  cout << "-streamSize: default is number of nodes in graph" << endl;
  cout << "-edge: defines edge weight to use, default is weighted cascade model" << endl;
}


void Arguments::getArguments(int argn, char **argv){
  for (int i = 0; i < argn; i++)
  {
    // print help
    if (argv[i] == string("-help") || argv[i] == string("--help") || argn == 1){
      printHelp();
      exit(1);
    }

    // define stage to run
    if (argv[i] == string("-stage")){
      if ((argv[i + 1] != string("pre")) && (argv[i + 1] != string("live")) && (argv[i + 1] != string("newStream")) && (argv[i + 1] != string("special"))){
        cerr << "Error: stage not recognized [ " << argv[i + 1] << " ]"<< endl;
        exit(1);
      }else{
        stage = argv[i + 1];
      }
    }

    // define dataset to use
    if (argv[i] == string("-dataset")){
      // verify if dataset among list of available datasets in directory
      dataset = argv[i + 1];
      if(dataset != "test" && dataset != "nethept" && dataset != "dblp" && dataset != "orkut" && dataset != "youtube" && dataset != "twitter" && dataset != "livejournal"){
        cerr << "Dataset not recognized: " << dataset << endl;
        exit(1);
      }
    }

    // define model to use
    if (argv[i] == string("-model")){
      model = argv[i + 1];
      if (model != "wc" && model != "ic"){
        cerr << "Model not recognized: " << model << endl;
        exit(1);
      }
    }

    if (argv[i] == string("-depth")){
      depth = atoi(argv[i + 1]);
    }

    // to help define inf. threshold, percentage of top influencers
    if (argv[i] == string("-reach")){
      reach = atoi(argv[i + 1]);
    }

    // activation probability [0, 1]
    if (argv[i] == string("-actprob")){
      theta_ap = atof(argv[i + 1]);
    }

    // max size of seed set to find
    if (argv[i] == string("-k") || argv[i] == string("--size")){
      k = atoi(argv[i + 1]);
    }

    if (argv[i] == string("-streamModel")){
      streamModel = argv[i + 1];
    }

    if (argv[i] == string("-streamVersion")){
      streamVersion = atoi(argv[i + 1]);
    }

    if (argv[i] == string("-streamSize")){
      streamSize = atoi(argv[i + 1]);
    }

    if (argv[i] == string("-edge")){
      edge_weight = atof(argv[i + 1]);
    }

    if (argv[i] == string("-minWeight")){
      min_weight = atof(argv[i + 1]);
    }

  }
}


void Arguments::printArguments(){
  cout << "------------- Arguments --------------" << endl;
  cout << "Dataset: ";
  printInColor("yellow", dataset);
  cout << "Model: ";
  printInColor("yellow", model);
  if(edge_weight == -1){
    cout << "Edge weights: ";
    printInColor("yellow", "weighted cascade");
  }else{
    cout << "Edge weights: ";
    printInColor("yellow", to_string(edge_weight));
  }
  cout << "Stage: ";
  printInColor("yellow", stage);
  if( stage == "pre"){
    cout << "Max Depth: ";
    printInColor("yellow", to_string(depth));
    cout << "Min Weight: ";
    printInColor("yellow", to_string(min_weight));
    cout << "Model: ";
    printInColor("yellow", model);
    // cout << "Simulations:\t\t" <<  mc_sim << endl;
  }else if (stage == "live"){
    cout << "Stream" << endl;
    cout << "> Model: ";
    printInColor("yellow", streamModel);
    cout << "> Version: ";
    printInColor("yellow", to_string(streamVersion));
    if (streamSize == -1){
      cout << "> Size: # nodes in graph" << endl;
    }else{
      cout << "> Size: ";
      printInColor("yellow", to_string(streamSize));
    }
    if (k == -1){
      cout << "Max seed size: # nodes in graph" << endl;
    }else{
      cout << "Max seed size: ";
      printInColor("yellow", to_string(k));
    }
    cout << "Reach: ";
    printInColor("yellow", properStringDouble(reach));
    cout << "Activation threshold: ";
    printInColor("yellow", properStringDouble(theta_ap));
  }else if(stage == "newStream"){
    cout << "Stream" << endl;
    cout << "> Model: ";
    printInColor("yellow", streamModel);
    cout << "> Version: ";
    printInColor("yellow", to_string(streamVersion));
    if (streamSize == -1){
      cout << "> Size: # nodes in graph" << endl;
    }else{
      cout << "> Size: ";
      printInColor("yellow", to_string(streamSize));
    }
  }
  cout << "--------------------------------------" << endl;
}

string Arguments::generateFileName(string type){
  string file_name = "";
  map<string, string> datasets = {
    { "test", "TS" },
    { "nethept", "NE" },
    { "dblp", "DB" },
    { "youtube", "YO" },
    { "livejournal", "LJ" },
    { "orkut", "OR" },
    { "twitter", "TW"}
  };
  map<string, string> keyword = {
    { "infScores", "infS"},
    { "seedSet", "ss"},
    { "stream", "st"},
    { "rand_repeat", "urr"},
    { "rand_no_repeat", "urnr"},
    { "progress", "prg"},
  };
  // file_name = <dataset>_params_<type>
  if (type == "infScores"){
    // file_name = <dataset>_d<value>_m<value>_infS.txt
    // example: "NE_d2_m0,01_infS.txt"
  }else if (type == "seedSet"){
    // imm
    // file_name = <dataset>_k<value>_e<value>_ss.txt
    // example: "NE_k200_e0,01_ss.txt"
    // rtim
    // file_name = <dataset>_<stream type>_v<version>_s<stream size>_r<value>_ap<value>_ss.txt
    // example: NE_urr_v1_s15229_r1_ap0,01_ss.txt
    file_name = datasets[dataset] + "_" + keyword[streamModel] + "_v" + to_string(streamVersion) + "_s" + to_string(streamSize) + "_r" + properStringDouble(reach) + "_ap" + properStringDouble(theta_ap) + "_ss.txt";
    // intersection
    // file_name =
  }else if (type == "stream"){
    // file_name = <dataset>_<type>_v<version>_s<stream size>_st.txt
    // example: "NE_urr_v1_s15229_st.txt"
    file_name = datasets[dataset] + "_v" + to_string(streamVersion) + "_s" + to_string(streamSize) + "_st.txt";
  }else if (type == "log"){
    // file_name = ?
  }

  return file_name;
}
