#include "head.h"

class Arguments{
public:
  int k; // size of seed set if defined
  string dataset; //name of dataset
  string model;
  string stage; // pre|live|evaluate
  int mc_depth; // max depth for monte carlo simulations
  int mc_sim; // number of simulations for monte carlo simulations

  /* Function to get arguments from command line
  */
  void getArguments(int argn, char **argv){
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
             mc_depth = argv[i + 1]
        if (argv[i] == string("-sim"))
            mc_sim = argv[i + 1]
        if (argv[i] == string("-epsilon"))
             epsilon = atof(argv[i + 1]);
        if (argv[i] == string("-T"))
            T = atof(argv[i + 1]);
        if (argv[i] == string("-k"))
            k = atoi(argv[i + 1]);
        if (argv[i] == string("-model"))
             model = argv[i + 1];
    }
    ASSERT(dataset != "");
    ASSERT( model == "IC" || model == "LT" ||  model == "TR" || model=="CONT");
  }

  /* Function to print arguments out to command line
  */
  void printArguments(){
    cout << "------------- Arguments --------------" << endl;
    cout << "Dataset: " <<  dataset << endl;
    cout << "Model: " << model << endl;
    cout << "Stage: " << "" << endl;
    if( stage == "pre"){
      cout << "Max Depth: " << mc_depth << endl;
      cout << "Simulations: " <<  mc_sim << endl;
    }else if (stage == "live"){
      cout << "Function: " <<  live_function << endl;
      cout << "Max Depth: " << mc_depth << endl;
    }else if( stage == "evaluate"){
      cout << "Simulations: 10000" << endl;
    }else{
      cout << "Error stage not defined" << endl;
      exit(1);
    }
    cout << "Size of seed set: " << k << endl;
  }
};
