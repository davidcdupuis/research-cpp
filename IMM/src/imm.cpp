#define HEAD_INFO

#include "sfmt/SFMT.h"
#include "head.h"

class Argument{
public:
    int k;
    string dataset;
    double epsilon;
    string model;
    double T;
    bool subgraph;
    int subsize;
    string submodel;
};

#include "graph.h"
#include "infgraph.h"
#include "imm.h"




void run_with_parameter(InfGraph &g, Argument & arg) //const
{
  int seedSize = 0;
  cout << "--------------------------------------------------------------------------------" << endl;
  cout << arg.dataset << " k=" << arg.k << " epsilon=" << arg.epsilon <<   " " << arg.model << endl;

  vector<int> k_values{arg.k};
  vector<double> epsilon_values{0.1};

  for (size_t i = 0; i < k_values.size(); i++){
    arg.k = k_values[i];
    cout << "Arguments Seed Size: " << arg.k << endl;
    for (size_t j = 0; j < epsilon_values.size(); j++){
      g.seedSet.clear();
      arg.epsilon = epsilon_values[j];
      cout << "Arguments epsilon: " << arg.epsilon << endl;

      string startDatetime = getLocalDatetime();
      clock_t start = clock();
      Imm::InfluenceMaximize(g, arg);
      double duration = (clock() - start)/(double)CLOCKS_PER_SEC;
      string endDatetime = getLocalDatetime();
      Timer::show();
      // save g.seedSet here => NE_k100_e0,01.txt
      string fileName = datasets[arg.dataset] + "_k" + to_string(arg.k) + "_e" + properStringDouble(arg.epsilon) + "_ss.txt";
      string filePath = "../data/" + arg.dataset + "/imm/basic/" + fileName;
      seedSize = g.seedSet.size();
      cout << "Saving influence scores to " << filePath << endl;
      ofstream seedSetFile;
      seedSetFile.open(filePath);
      for(int node: g.seedSet){
        seedSetFile << node << endl;
      }
      seedSetFile.close();
      //INFO(g.seedSet);
      // use saved seed set to compute influence score using MC simulations
      double estScore = g.InfluenceHyperGraph();

      string pathCSV = "../data/" + arg.dataset + "/logs/imm.csv";
      string pathLog = "../data/" + arg.dataset + "/logs/imm.log";
      cout << "Saving IMM log to " << pathLog << endl;
      ofstream logFile;
      logFile.open(pathLog, fstream::app);
      logFile << "File name  : " << fileName << endl;
      logFile << "Start date : " << startDatetime << endl;
      logFile << "End date   : " << endDatetime << endl;
      logFile << "Duration   : " << duration << endl;
      logFile << "SeedSize   : " << seedSize << endl;
      logFile << "Est. score : " << estScore << endl;
      logFile << "-----------------------------------------------------" << endl;
      logFile.close();

      cout << "Saving IMM csv to " << pathCSV << endl;
      ofstream csvFile;
      csvFile.open(pathCSV, fstream::app);
      csvFile << arg.dataset << "," << arg.k << "," << arg.epsilon << "," << startDatetime << "," << endDatetime << "," << duration  << "," << seedSize << "," << estScore << endl;
      csvFile.close();

    }
  }
}

void Run(int argn, char **argv)
{
    Argument arg;


    for (int i = 0; i < argn; i++)
    {
        if (argv[i] == string("-help") || argv[i] == string("--help") || argn == 1)
        {
            cout << "./tim -dataset *** -epsilon *** -k ***  -model IC|LT|TR|CONT " << endl;
            return ;
        }
        if (argv[i] == string("-dataset"))
          arg.dataset = argv[i + 1];
        if (argv[i] == string("-epsilon"))
          arg.epsilon = atof(argv[i + 1]);
        if (argv[i] == string("-T"))
          arg.T = atof(argv[i + 1]);
        if (argv[i] == string("-k"))
          arg.k = atoi(argv[i + 1]);
        if (argv[i] == string("-model"))
          arg.model = argv[i + 1];
        if (argv[i] == string("-subgraph"))
          arg.subgraph = true;
        if (argv[i] == string("-subsize"))
          arg.subsize = atof(argv[i + 1]);
        if (argv[i] == string("-submodel"))
          arg.submodel = argv[i + 1];
    }
    ASSERT(arg.dataset != "");
    ASSERT(arg.model == "IC" || arg.model == "LT" || arg.model == "TR" || arg.model=="CONT");

    string graph_file;
    if (arg.subgraph){
      graph_file = "../data/" + arg.dataset + "/osim/" + arg.dataset + "_" + string(arg.subsize) + "_wc.inf";
    }else{
      if (arg.model == "IC"){
          //graph_file = arg.dataset + "graph_wc.inf";
  	      graph_file = "../data/" + arg.dataset + "/" + arg.dataset +"_wc.inf";
      }else if (arg.model == "LT")
          graph_file = arg.dataset + "graph_lt.inf";
      else if (arg.model == "TR")
          graph_file = arg.dataset + "graph_tr.inf";
      else if (arg.model == "CONT")
          graph_file = arg.dataset + "graph_cont.inf";
      else
          ASSERT(false);
    }

    InfGraph g(arg.dataset, graph_file);


    if (arg.model == "IC")
        g.setInfuModel(InfGraph::IC);
    else if (arg.model == "LT")
        g.setInfuModel(InfGraph::LT);
    else if (arg.model == "TR")
        g.setInfuModel(InfGraph::IC);
    else if (arg.model == "CONT")
        g.setInfuModel(InfGraph::CONT);
    else
        ASSERT(false);

    INFO(arg.T);

    run_with_parameter(g, arg);
}


int main(int argn, char **argv)
{
    __head_version = "v1";
    OutputInfo info(argn, argv);


    Run( argn, argv );
}
