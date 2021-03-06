#include "IMM.h"

using namespace std;

double logcnk(int n, int k){
  double ans = 0;
  for(int i = n - k + 1; i <= n; i++){
    ans += log(i);
  }
  for(int i = 1; i <= k; i++){
    ans -= log(i);
  }
  return ans;
}

void IMM::saveLog(const Arguments &arg, string fileName, string startDatetime, string endDatetime, double duration, int seedSize, double estScore){
  string pathLog = "../../data/" + arg.dataset + "/logs/imm.log";
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
}

void IMM::saveCSV(const Arguments &arg, string fileName, string startDatetime, string endDatetime, double duration, int seedSize, double estScore){
  string pathCSV = "../../data/" + arg.dataset + "/logs/imm.csv";
  cout << "Saving IMM csv to " << pathCSV << endl;
  ofstream csvFile;
  csvFile.open(pathCSV, fstream::app);
  csvFile << arg.dataset << "," << arg.k << "," << arg.epsilon << "," << startDatetime << "," << endDatetime << "," << duration  << "," << seedSize << "," << estScore << endl;
  csvFile.close();
}

double IMM::sampling(InfGraph &g, const Arguments & arg){
  double epsilon_prime = arg.epsilon * sqrt(2);
  Timer t(1, "step1");
  for (int x = 1; ; x++){
    //lambda = (2 + 2/3 * espilon') * (log(n choose k) + l*log(n) + log(log2(n))) * n / espilon'^2
    int64 ci = (2 + 2/3 * epsilon_prime) * ( logcnk(g.nodes, arg.k) + 1.0 * log(g.nodes) + log(log2(g.nodes))) * pow(2.0, x) / pow(epsilon_prime, 2.0);
    //cout << "ci: " << ci << endl;
    g.build_hyper_graph_r(ci, arg);
    g.build_seedset(arg.k);
    double ept = g.InfluenceHyperGraph()/g.nodes;
    //cout << "ept: " << ept << endl;
    double estimate_influence = ept * g.nodes;
    INFO(x, estimate_influence);
    if (ept > 1 / pow(2.0, x)){
      double OPT_prime = ept * g.nodes / (1+ epsilon_prime);
      //INFO("step1", OPT_prime);
      //INFO("step1", OPT_prime * (1+epsilon_prime));
      return OPT_prime;
    }
  }
  ASSERT(false);
  return -1;
}

double IMM::nodeSelection(InfGraph &g, const Arguments & arg, double OPT_prime){
  Timer t(2, "NodeSelection");
  ASSERT(OPT_prime > 0);
  double e = exp(1);
  double alpha = sqrt(log(g.nodes) + log(2));
  double beta = sqrt((1-1/e) * (logcnk(g.nodes, arg.k) + log(g.nodes) + log(2)));

  int64 R = 2.0 * g.nodes * pow((1-1/e) * alpha + beta, 2.0) /  OPT_prime / arg.epsilon / arg.epsilon ; //pow(arg.epsilon)
  //R/=100;
  g.build_hyper_graph_r(R, arg);
  g.build_seedset(arg.k);
  double opt = g.InfluenceHyperGraph();
  return opt;
}

void IMM::InfluenceMaximize(InfGraph &g, const Arguments &arg){
  Timer t(100, "InfluenceMaximize(Total Time)");

  INFO("########## Sampling ##########");
  // debugging mode lalala
  double OPT_prime;
  OPT_prime = sampling(g, arg ); //estimate OPT

  INFO("########## Node Selection ##########");
  //double opt_lower_bound = OPT_prime;
  //INFO(opt_lower_bound);
  nodeSelection(g, arg, OPT_prime);
  INFO("step2 finish");
}

void run_with_parameter(InfGraph &g, Arguments & arg){
  int seedSize = 0;

  vector<int> k_values{arg.k};
  vector<double> epsilon_values{0.1};

  g.seedSet.clear();
  g.setInfluModel(InfGraph::IC);
  cout << "Arguments epsilon: " << arg.epsilon << endl;

  string startDatetime = getLocalDatetime();
  clock_t start = clock();
  IMM::InfluenceMaximize(g, arg);
  double duration = (clock() - start)/(double)CLOCKS_PER_SEC;
  string endDatetime = getLocalDatetime();
  Timer::show();
  // save g.seedSet here => NE_k100_e0,01.txt
  string fileName = "";
  string filePath = "";
  if (arg.subgraph){
    fileName = datasets[arg.dataset] + "_" + arg.submodel + "_" + to_string(arg.subsize) + "_k" + to_string(arg.k) + "_e" + properStringDouble(arg.epsilon) + "_ss.txt";
    filePath = "../../data/" + arg.dataset + "/imm/osim/" + fileName;
  }else{
    fileName = datasets[arg.dataset] + "_k" + to_string(arg.k) + "_e" + properStringDouble(arg.epsilon) + "_ss.txt";
    filePath = "../../data/" + arg.dataset + "/imm/basic/" + fileName;
  }
  //
  //cout << "seed set: ";
  for(int node: g.seedSet){
    //cout << node << " ";
  }
  //cout << endl;
  //
  seedSize = g.seedSet.size();
  cout << "Saving seed set nodes to " << filePath << endl;
  ofstream seedSetFile;
  seedSetFile.open(filePath);
  for(int node: g.seedSet){
    seedSetFile << node << endl;
  }
  seedSetFile.close();
  //INFO(g.seedSet);
  // use saved seed set to compute influence score using MC simulations
  double estScore = g.InfluenceHyperGraph();

  // saveLog(arg, fileName, startDatetime, endDatetime, duration, seedSize, estScore);
  string pathLog = "../../data/" + arg.dataset + "/logs/imm.log";
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

  // saveCSV(arg, fileName, startDatetime, endDatetime, duration, seedSize, estScore);
  string pathCSV = "../../data/" + arg.dataset + "/logs/imm.csv";
  cout << "Saving IMM csv to " << pathCSV << endl;
  ofstream csvFile;
  csvFile.open(pathCSV, fstream::app);
  csvFile << arg.dataset << "," << arg.k << "," << arg.epsilon << "," << startDatetime << "," << endDatetime << "," << duration  << "," << seedSize << "," << estScore << endl;
  csvFile.close();
}

void run(int argn, char **argv){
  Arguments arg;
  arg.readArguments(argn, argv);
  arg.printArguments();
  // string graph_file;
  // if (arg.subgraph){
  //   graph_file = "../../data/" + arg.dataset + "/osim/" + arg.dataset + "_" + to_string(arg.subsize) + "_wc.inf";
  // }else{
  //   if (arg.model == "IC"){
  //     //graph_file = arg.dataset + "graph_wc.inf";
  //     graph_file = "../../data/" + arg.dataset + "/" + arg.dataset +"_wc.inf";
  //   }else if (arg.model == "LT")
  //     graph_file = arg.dataset + "graph_lt.inf";
  //   else if (arg.model == "TR")
  //     graph_file = arg.dataset + "graph_tr.inf";
  //   else if (arg.model == "CONT")
  //     graph_file = arg.dataset + "graph_cont.inf";
  //   else
  //     ASSERT(false);
  // }

  InfGraph graph(arg);

  // if (arg.model == "IC")
  //   g.setInfluModel(InfGraph::IC);
  // else if (arg.model == "LT")
  //   g.setInfluModel(InfGraph::LT);
  // else if (arg.model == "TR")
  //   g.setInfluModel(InfGraph::IC);
  // else if (arg.model == "CONT")
  //   g.setInfluModel(InfGraph::CONT);
  // else
  //   ASSERT(false);

  // INFO(arg.T);

  run_with_parameter(graph, arg);
}

int main(int argn, char **argv){
  cout << "\e\[0;32mProgram Start at: " << currentTimestampStr() << "\e[0m" << endl;
  run(argn, argv);
  cout << "\e\[0;31mProgram Terminate at: " << currentTimestampStr() << "\e[0m" << endl;
}
