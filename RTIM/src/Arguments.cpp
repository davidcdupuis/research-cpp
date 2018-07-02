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
  cout << "-availability: What availability model to use." << endl;
  cout << "-reach: Percentage of top influencers to use as influence threshold." << endl;
  cout << "-actProb: Activation probability to use." << endl;
  cout << "-k --size: Max size of seed set to find." << endl;
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
      if ((argv[i + 1] != string("pre")) && (argv[i + 1] != string("live"))){
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

    // name of availability model to use
    if (argv[i] == string("-availability")){
      // check if file exists
      availability = argv[i + 1];
    }

    // to help define inf. threshold, percentage of top influencers
    if (argv[i] == string("-reach")){
      reach = atoi(argv[i + 1]);
    }

    // activation probability [0, 1]
    if (argv[i] == string("-actprob")){
      activation_threshold = atof(argv[i + 1]);
    }

    // max size of seed set to find
    if (argv[i] == string("-k") || argv[i] == string("--size")){
      k = atoi(argv[i + 1]);
    }
  }
}

void Arguments::printArguments(){
  cout << "------------- Arguments --------------" << endl;
  cout << "Dataset: " <<  dataset << endl;
  cout << "Model: " << model << endl;
  cout << "Stage: " << stage << endl;
  if( stage == "pre"){
    cout << "Max Depth: " << depth << endl;
    // cout << "Simulations:\t\t" <<  mc_sim << endl;
  }else if (stage == "live"){
    cout << "Availability model: " << availability << endl;
    cout << "Max seed size: " << k << endl;
    cout << "Reach: " << reach << endl;
    cout << "Activation threshold: " << activation_threshold << endl;
    cout << "--------------------------------------" << endl;
  }
}
