#include "Arguments.h"

using namespace std;

void Arguments::getArguments(int argn, char **argv){
  for (int i = 0; i < argn; i++)
  {
      // print help
      if (argv[i] == string("-help") || argv[i] == string("--help") || argn == 1)
      {
          cout << "./rtim -dataset *** -epsilon *** -k ***  -model IC|LT|TR|CONT " << endl;
          return ;
      }

      // get arguments
      if (argv[i] == string("-dataset"))
          dataset = argv[i + 1];
      if (argv[i] == string("-stage"))
          stage = argv[i + 1];
      if (argv[i] == string("-depth"))
           mc_depth = atoi(argv[i + 1]);
      if (argv[i] == string("-sim"))
          mc_sim = atoi(argv[i + 1]);
      //if (argv[i] == string("-epsilon"))
      //     epsilon = atof(argv[i + 1]);
      //if (argv[i] == string("-T"))
      //    T = atof(argv[i + 1]);
      if (argv[i] == string("-k"))
          k = atoi(argv[i + 1]);
      if (argv[i] == string("-model"))
           model = argv[i + 1];
  }
  if (dataset == ""){
    cerr << "ASSERT FAIL @ "<< __FILE__ << ":" << __LINE__ << endl;
    exit(1);
  }
  if (model != "IC" || model != "LT" || model != "TR" || model != "CONT"){
    cerr << "ASSERT FAIL @ "<< __FILE__ << ":" << __LINE__ << endl;
    exit(1);
  }
}

void Arguments::printArguments(){
  cout << "------------- Arguments --------------" << endl;
  cout << "Dataset: " <<  dataset << endl;
  cout << "Model: " << model << endl;
  cout << "Stage: " << stage << endl;
  if( stage == "pre"){
    cout << "Max Depth: " << mc_depth << endl;
    cout << "Simulations: " <<  mc_sim << endl;
  }else if (stage == "live"){
    cout << "Function: " <<  "independence" << endl;
    cout << "Max Depth: " << mc_depth << endl;
  }else if( stage == "evaluate"){
    cout << "Simulations: 10000" << endl;
  }else{
    cout << "Error stage not defined" << endl;
    exit(1);
  }
  cout << "Size of seed set: " << k << endl;
}
