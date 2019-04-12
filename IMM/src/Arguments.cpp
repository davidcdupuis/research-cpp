#include "Arguments.h"

using namespace std;

void Arguments::readArguments(int argn, char ** argv){
  for (int i = 0; i < argn; i++){
    if (argv[i] == string("-help") || argv[i] == string("--help") || argn == 1){
      cout << "./main -dataset *** -epsilon *** -k ***  -model IC|LT|TR|CONT " << endl;
      return ;
    }
    if (argv[i] == string("-dataset"))
      dataset = argv[i + 1];
    if (argv[i] == string("-epsilon"))
      epsilon = atof(argv[i + 1]);
    if (argv[i] == string("-T"))
      T = atof(argv[i + 1]);
    if (argv[i] == string("-k"))
      k = atoi(argv[i + 1]);
    if (argv[i] == string("-model"))
      model = argv[i + 1];
    if (argv[i] == string("-subgraph"))
      subgraph = true;
    if (argv[i] == string("-subsize"))
      subsize = atof(argv[i + 1]);
    if (argv[i] == string("-submodel"))
      submodel = argv[i + 1];
  }
  ASSERT(dataset != "");
  ASSERT(model == "IC" || model == "LT" || model == "TR" || model=="CONT");
}

void Arguments::printArguments(){
  cout << string(19, '-') << toColor("red", " Arguments ") << string(20, '-') << endl;
  cout << "- dataset    : " << toColor("yellow", dataset) << endl;
  cout << "- model      : " << toColor("yellow", model) << endl;
  cout << "- seed size  : " << toColor("yellow", to_string(k)) << endl;
  cout << "- epsilon    : " << toColor("yellow", properStringDouble(epsilon)) << endl;
  if(subgraph){
    cout << "- subgraph   : " << toColor("yellow", ((subgraph) ? ("true") : ("false"))) << endl;
    cout << "- subsize    : " << toColor("yellow", to_string(subsize)) << endl;
    cout << "- submodel   : " << toColor("yellow", submodel) << endl;
  }
  cout << string(50, '-') << endl;
}
