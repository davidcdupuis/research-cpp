#include <vector>
#include <cstdio>
#include <ctime>
#include <time.h>
#include <omp.h>
#include <string>
#include <algorithm>
#include <sys/stat.h>

#include "RTIM.h"
#include "Arguments.h"
#include "Graph.h"

#include <iostream>
#include <fstream>

using namespace std;

inline string cleanTime(double t, string type="ms"){
  if (type == "ms"){
    if (t >= 1000){
      t = t / 1000;
      type = "s";
    }else{
      return to_string(t) + " milliseconds";
    }
  }
  if (type == "s"){
    if (t >= 60){
      t = t / 60;
      type = "m";
    }else{
      return to_string(t) + " seconds";
    }
  }
  if (type == "m"){
    if (t >= 60){
      t = t / 60;
      type = "h";
    }else{
      return to_string(t) + " minutes";
    }
  }
  if (type == "h"){
    if (t >= 24){
      t = t / 24;
      return to_string(t) + " days";
    }else{
      return to_string(t) + " hours";
    }
  }
}


RTIM::RTIM(Arguments& arguments, bool loadGraph):graph(arguments, loadGraph){
  args = arguments;

  if (args.streamSize == -1){
    args.streamSize = graph.nodes;
  }
  if (args.k == -1){
    args.k = graph.nodes;
  }
  nodes = graph.nodes;
  srand(time(NULL));
}


void RTIM::saveToCSV(string fileName){
  ofstream myfile;
  myfile.open(fileName);
  myfile << "i,score\n";
  for (int j = 0; j < nodes; j++){
      myfile << j << "," << infScores[j] << "\n";
  }
  //myfile << "semicolon"; // ?
  myfile.close();
}


int RTIM::print_progress(int nb_threads, int finishedProcess, int numNodes, time_t startTime, int* nb_nodes, int save){
  int j, sum = 0;
  for (j = 0; j < nb_threads; j++){
    sum += nb_nodes[j*8];
  }

  float progress = (float)sum/numNodes;

  time_t currentTime;
  time ( &currentTime );
  double duration = difftime(currentTime,startTime);
  double durationPerPercent = duration / progress;
  double timeLeft = (1-progress)*durationPerPercent;
  int barWidth = 40;

  std::cout << "\r[";
  int pos = barWidth * progress;
  for (int i = 0; i < barWidth; ++i) {
      if (i < pos) std::cout << "=";
      else if (i == pos) std::cout << ">";
      else std::cout << " ";
  }
  std::cout << "] " << int(progress * 100.0) << " % -- (" << (nb_threads-finishedProcess) << "/" << nb_threads << " threads) -- (" << sum << "/" << numNodes << ") -- (" << duration << "s / " << timeLeft << "s)";
  //std::cout.flush();

  int testPourcent = (int)(progress*100.0);
  if (testPourcent%10 == 0 && save == 0){
    save = 1;
    string fileName = "../../data/algoritmhs/rtim/infScores_"+ std::to_string(testPourcent) + ".csv";
    saveToCSV(fileName);
  } else if (testPourcent%10 != 0){
    save = 0;
  }

  return save;
}


void RTIM::pre_process(){
  // for each node in graph compute influence score
  cout << "Running pre_process on " << args.dataset << endl;
  double score;
  // nodes = graph.graph.size();

  // infScores.reserve(nodes);
  infScores.resize(nodes, 0);
  nodeTime.resize(nodes, 0);
  // for(int i = 0; i < nodes; i++){
  //   infScores.push_back(0);
  // }
  double start = omp_get_wtime();
  int* nb_nodes = 0;
  int nb_threads = 0;
  int num_thread = 0;
  time_t startTime;
  int save = 0;
  int finishedProcess = 0;
  #pragma omp parallel private(score, num_thread, startTime, save) shared(nb_nodes, nb_threads, finishedProcess) //shared(graph, infScores, numNodes)
  {
    nb_threads = omp_get_num_threads();
    num_thread = omp_get_thread_num();
    if (num_thread == 0){
      if (nb_nodes == 0){
        nb_nodes = (int*)calloc (nb_threads*8,sizeof(int));
      }
      time ( &startTime );
    }
    #pragma omp barrier
    #pragma omp for schedule(dynamic) nowait
    for(int i = 0; i < nodes; i++){
      if (num_thread == 0){
        save = print_progress(nb_threads, finishedProcess, nodes, startTime, nb_nodes, save);
      }
      // Compute the influence score of a node in G
      // score = graph.influenceScore({i}, 1);
      clock_t nodeStart = clock();
      score = graph.influenceScorePath(i, args.depth,"shortest", args.edge_weight, args.min_weight);
      double duration = (clock() - nodeStart)/(double)CLOCKS_PER_SEC;
      // score = graph.influenceScoreNeighbors(i);
      infScores[i] = score;
      nodeTime[i] = duration;
      nb_nodes[num_thread*8]++;
    }

    #pragma omp critical
    {
      finishedProcess ++;
      //printf("\r[%d/%d] Finished !\n",num_thread, nb_threads);
    }
    if (num_thread == 0){
      while (finishedProcess != nb_threads) {
        save = print_progress(nb_threads, finishedProcess, nodes, startTime, nb_nodes, save);
      }
      save = print_progress(nb_threads, finishedProcess, nodes, startTime, nb_nodes, save);
    }

  }
  cout << endl;
  double time = omp_get_wtime() - start;
  /*
  if (nodes <= 20){
    printScores();
  }
  */
  saveScores();
};


void RTIM::live(){
  clock_t startLive = clock();
  nodes = graph.graph.size();
  cout << "-------------------------------" << endl;
  cout << "Running live on " << args.dataset << endl;
  activationProbabilities.resize(nodes, 0);
  importScores();
  sortedScores = infScores;
  sort(sortedScores.begin(), sortedScores.end());
  getInfIndex(sortedScores);
  cout << "Starting influence score threshold: " << sortedScores[infIndex] << endl;

  // read availability stream
  string file = args.streamModel + "_" + to_string(args.streamSize) + "_m" + to_string(args.streamVersion);
  string folder = "../../data/" + args.dataset + "/availability_models/" + args.streamModel + "/" + args.streamModel + "_m" + to_string(args.streamVersion) + "/" + args.streamModel + "_" + to_string(args.streamSize) + "_m" + to_string(args.streamVersion) + ".txt";
  int user;
  cout << "Reading availability stream: " << folder << endl;
  ifstream infile(folder.c_str());
  double max_time = 0;
  clock_t start;
  clock_t startStream = clock();
  time_t startTime;
  time ( &startTime );
  double duration;
  int sum = 0;
  while (infile >> user){
    sum ++;
    if (args.dataset == "test"){
      cout << "User: " << user << " is online." << endl;
    }
    if (activationProbabilities[user] < args.theta_ap && infScores[user] >= sortedScores[infIndex]){
      activationProbabilities[user] = 1.0;
      // measure update time
      start = clock();
      //graph.updateNeighborsAP(user, activationProbabilities, {}, 1.0, 1);
      graph.updateNeighborsAPShort(user, activationProbabilities);
      duration = (clock() - start)/(double)CLOCKS_PER_SEC;
      if (duration > max_time){
        max_time = duration;
      }
      seedSet.push_back(user);
      infIndex --;
      if (args.dataset == "test" || args.k < 20){
        cout << "Targeted user: " << user << endl;
      }
    }
    if (seedSet.size() >= args.k){
      break;
    }
    // print progress

    float progress = (float)sum/args.streamSize;

    time_t currentTime;
    time ( &currentTime );
    double duration = difftime(currentTime,startTime);
    double durationPerPercent = duration / progress;
    double timeLeft = (1 - progress) * durationPerPercent;
    int barWidth = 50;

    cout << "\r[";
    int pos = barWidth * progress;
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) {
          cout << "=";
        } else if (i == pos){
          cout << ">";
        } else {
          cout << " ";
        }
    }
    cout << "] " << int(progress * 100.0) << " % -- (" << sum << "/" << args.streamSize << ") -- (" << duration << "s / " << timeLeft << "s)";
    cout.flush();

  }
  cout << endl;
  double streamDuration = (clock() - startStream)/(double)CLOCKS_PER_SEC;
  cout << "Availability stream read in:  " << cleanTime(streamDuration, "ms") << endl;
  saveSeedSet();
  cout << "Computing influence score of seed set of size: " << seedSet.size() << endl;
  start = clock();
  double seedScore;
  if (seedSet.size() >= 20000){
      seedScore = graph.influenceScore(seedSet, 2);
  }else if (seedSet.size() >= 300){
      seedScore = graph.influenceScore(seedSet, 3);
  }else{
      seedScore = graph.influenceScore(seedSet);
  }
  double seedDuration = (clock() - start)/(double)CLOCKS_PER_SEC;
  cout << "Seed set score: " << seedScore << "/" << nodes << ", computed in: " << cleanTime(seedDuration, "ms") << endl;

  double liveDuration = (clock() - startLive)/(double)CLOCKS_PER_SEC;
  saveLiveLog(seedScore, streamDuration, seedDuration, max_time, sum, liveDuration);
  saveLiveCSV(graph, seedScore, streamDuration, seedDuration, max_time, sum, liveDuration);
  cout << "Live complete!" << endl;
  cout << "------------------------------" << endl;
};


void RTIM::printScores(){
  cout << "Influence Scores: " << infScores.size() << endl;
  if (infScores.size() < 50){
    for (int i = 0; i < infScores.size() ; i++){
      cout << "(" << i << " : " << infScores[i] << ")" << endl;
    }
  }else{
    for (int i = 0; i < 20 ; i++){
      cout << "(" << i << " : " << infScores[i] << ")" << endl;
    }
  }
}


void RTIM::saveScores(){
  string file = "../../data/" + args.dataset + "/rtim/" + args.dataset + "_infscores_d" + to_string(args.depth) + "m" + to_string(args.min_weight) + ".txt";
  cout << "Saving influence scores to: " << file << endl;
  // save scores
  ofstream infScoresFile;
  infScoresFile.open(file);
  for (int i = 0; i < infScores.size() ; i++){
     infScoresFile << i << " " << infScores[i] << " " << nodeTime[i] << endl;
  }
  infScoresFile.close();
  cout << "Scores saved successfully!" << endl;
}


void RTIM::saveSeedSet(){
  string file = "../../data/" + args.dataset + "/availability_models/" + args.streamModel + "/" + args.streamModel + "_m" + to_string(args.streamVersion) + "/" + args.dataset + "_seedSet_s" + to_string(seedSet.size()) + "r" + to_string(args.reach) + "ap" + to_string(args.theta_ap) + ".txt";
  cout << "Saving seed set to: " << file << endl;
  ofstream seedSetFile;
  seedSetFile.open(file);
  for (int i = 0; i < seedSet.size() ; i++){
     seedSetFile << seedSet[i] << endl;
  }
  seedSetFile.close();
  cout << "Seed set saved successfully!" << endl;
}


void RTIM::saveLiveLog(double& score, double& streamTime, double& seedTime, double& maxTime, int& progress, double& runtime){
  string file = "../../data/" + args.dataset + "/availability_models/" + args.streamModel + "/" + args.streamModel + "_m" + to_string(args.streamVersion) + "/" + args.dataset + "_liveLog.txt";
  cout << "Saving live log to: " << file << endl;
  ofstream liveLogFile;
  liveLogFile.open(file, fstream::app);
  liveLogFile << "<Stream>" << endl;
  liveLogFile << "- model: " << args.streamModel << endl;
  liveLogFile << "- version: " << args.streamVersion << endl;
  liveLogFile << "- size: " << args.streamSize << endl;
  liveLogFile << "- progress: " << progress << endl;
  liveLogFile << "- runtime: " << cleanTime(streamTime, "ms") << endl;
  liveLogFile << "<Seed set>" << endl;
  liveLogFile << "- size: " << seedSet.size() << endl;
  liveLogFile << "- score: " << score << endl;
  liveLogFile << "- score compute time: " << cleanTime(seedTime, "ms") << endl;
  liveLogFile << "<Args>" << endl;
  liveLogFile << "- reach: " << args.reach << endl;
  liveLogFile << "- theta_ap: " << args.theta_ap << endl;
  liveLogFile << "Runtime: " << cleanTime(runtime, "ms") << endl;
  liveLogFile << "Max update time: " << cleanTime(maxTime,"ms") << endl;
  liveLogFile << "----------------------------------------------------" << endl;
  liveLogFile.close();
  cout << "Live log saved successfully!" << endl;
}


void RTIM::saveLiveCSV(const Graph& graph, double& score, double& streamTime, double& seedTime, double& maxTime, int& progress, double& runtime){
  string file = "../../data/live_log.csv";
  cout << "Saving live csv to: " << file << endl;
  ofstream liveCSV;
  liveCSV.open(file, fstream::app);
  /*Order of values: dataset,nodes,edges,reach,theta_ap,ap_algo,ap_depth,streamModel,streamVersion,streamRuntime,streamProgress,maxAPTime,seedSizeLimit,seedSize,seedScore,seedScoreRuntime,liveRuntime */
  liveCSV << args.dataset << ",";
  liveCSV << graph.nodes << ",";
  liveCSV << graph.edges << ",";
  liveCSV << args.reach << ",";
  liveCSV << args.theta_ap << ",";
  liveCSV << "APShort" << ",";
  liveCSV << args.depth << ",";
  liveCSV << args.streamModel << ",";
  liveCSV << args.streamVersion << ",";
  liveCSV << streamTime << ",";
  liveCSV << progress << ","; // NaN if file saved outside of live
  liveCSV << maxTime << ",";
  liveCSV << args.k << ",";
  liveCSV << seedSet.size() << ",";
  liveCSV << score << ",";
  liveCSV << seedTime << ",";
  liveCSV << runtime << endl;
  liveCSV.close();
  cout << "Live CSV saved! " << endl;
}


void RTIM::importScores(){
  string folder = "../../data/" + args.dataset + "/rtim/" + args.dataset + "_infscores.txt";
  cout << "Importing influence scores from: " << folder << endl;
  infScores.resize(nodes, 0);
  int user;
  double infScore;
  double scoreTime;

  ifstream infile(folder.c_str());
  while(infile >> user >> infScore >> scoreTime){
    infScores[user] = infScore;
  }
  cout << "Import successful" << endl;
  printScores();
}


void RTIM::availabilityStream(){
  cout << "Generating availability stream: " << args.streamModel << "_m"<< args.streamVersion << endl;
  string file = args.streamModel + "_m" + to_string(args.streamVersion);
  string folder = "../../data/" + args.dataset + "/availability_models/" + args.streamModel + "/" + file + "/" + file + ".txt";
  int user;
  ofstream availabilityFile;
  availabilityFile.open(folder);
  for (int i = 0; i < args.streamSize; i++){
    availabilityFile << 0 << endl;
  }
  availabilityFile.close();
  cout << "Availability file saved!" << endl;
}


void RTIM::getInfIndex(vector<double> & sorted){
  infIndex = sorted.size() - sorted.size() * args.reach / 100;
}


void RTIM::outgoing(){
  string file = "../../data/" + args.dataset + "/" + args.dataset + "_outgoing.txt";
  ofstream outgoingFile;
  outgoingFile.open(file);
  if (outgoingFile.is_open()){
    cout << "Saving # of outgoing neighbors" << endl;
    // for each node in graph compute outgoing and save to dataset_outgoing.txt
    for(int i = 0; i < graph.graph.size(); i++){
      outgoingFile << i  << " " << graph.graph[i].size() << endl;
    }
    outgoingFile.close();
    cout << "Number of outgoing neighbors saved successfully" << endl;
  } else {
    cerr << file << " not opened!" << endl;
  }

}


void RTIM::mergeOutgoingScores(){
  string dir = "../../data/" + args.dataset + "/";
  // import outgoing to vector
  string file1 = dir + args.dataset + "_outgoing.txt";
  vector<int> outgoing;
  outgoing.resize(nodes, 0);
  ifstream outgoingFile;
  outgoingFile.open(file1);
  if (outgoingFile.is_open()){
    int user, out;
    while (outgoingFile >> user >> out){
      cout << user << "," << out << endl;
      outgoing[user] = out;
    }
    outgoingFile.close();
  } else{
    cerr << file1 << " not opened!" << endl;
  }


  // import scores to vector
  importScores();

  // save both to file
  string file3 = dir + args.dataset + "_nodeinfo.txt";

  ofstream nodeInfoFile;

  nodeInfoFile.open(file3);
  if (nodeInfoFile.is_open()){
    for (int i = 0; i < infScores.size(); i++){
      nodeInfoFile << i << " " << infScores[i] << " " << outgoing[i] << endl;
    }
    cout << file3 << " saved successfully!" << endl;
  }else{
    cerr << file3 << " not opened!" << endl;
  }
}


void RTIM::convergenceScore(){
  double newScore, diff;
  cout << "Computing inf scores with convergence. " << endl;
  infScores.resize(nodes, 1);
  set<int> converged;
  set<int> newConverged;
  for(int i = 0; i < nodes; i++){
    converged.insert(i);
  }

  while (!converged.empty()){
    for(int user: converged){
      if (graph.graph[user].size() == 0){ // node has no neighbors
        infScores[user] = 1;
        //converged.erase(user);
        newConverged.insert(user);
        cout << "Node " << user << " has no neighbors score is 1" << endl;
      } else {
        double product = 1.0;
        bool convergedAll = true;
        for(pair<int, double> neighbor: graph.graph[user]){
          if (converged.find(neighbor.first) != converged.end()){
            convergedAll = false;
          }
          product = product * (1 - neighbor.second * infScores[neighbor.first]);
        }
        newScore = 2 - product;
        cout << "user: " << user << "---" << product << "--> " << newScore << endl;
        diff = abs(newScore - infScores[user]);
        if ( diff <= 0.01){
          cout << "Node " << user << " convergence at " << diff << " with  score " << newScore << endl;
          //converged.erase(user);
          newConverged.insert(user);
          infScores[user] = newScore;// + 1;
        } else if (convergedAll){ // all neighbors have converged
          cout << "Node " << user << " has all neighbors converged, score is " << infScores[user] << endl;
          //converged.erase(user);
          newConverged.insert(user);
          infScores[user] = newScore;// + 1;
        } else {
          infScores[user] = newScore;
        }

      }
    }
    for (int user: newConverged){
      converged.erase(user);
      newConverged.erase(user);
    }
  }
  cout << "--------------------------" << endl;
}


void RTIM::mcConvergenceTest(int sampleSize){
  // select a random sample of node from the graph, these nodes have to have at
  // least one neighbor
  cout << "Starting Monte Carlo Convergence test on " << args.dataset << endl;
  if (sampleSize > graph.nodes){
    cerr << "Error: Sample size larger than graph size!" << endl;
    exit(1);
  }
  cout << "Sampling " << sampleSize << " nodes." << endl;
  vector<int> nodes;
  double nodesNeeded = sampleSize;
  double nodesLeft = graph.nodes;
  double selectProb;// = nodesNeeded / nodesLeft;
  double r;
  int i = 0;
  // we select sample with reservoir sampling
  while (nodes.size() != sampleSize && i < graph.nodes){
    if(graph.graph[i].size() >= 1){ // ignore nodes that have no neighbors
      selectProb = nodesNeeded / nodesLeft;
      r = rand()/(double)RAND_MAX;
      if (r < selectProb){
        // cout << i << " - " << r << " <= " << selectProb << " = " << nodesNeeded << " / " << nodesLeft << endl;
        nodes.push_back(i);
        nodesNeeded --;
      }
    }
    nodesLeft --;
    i ++;
  }
  // for each node run 10,000 simulations record all of the results.
  // compute the average of various size,record these values.
  string file = "../../data/" + args.dataset + "/montecarlo_convergence.txt";
  ofstream convergenceFile;
  // int sims [10] = {1, 10, 100, 1000, 2000, 3000, 5000, 7000, 9000, 10000};
  // double results [10] = { };
  double score;
  cout << "Computing scores. " << endl;
  for (int node: nodes){
    // cout << "Saving convergence data of " << node << endl;// << " to: " << file << endl;
    convergenceFile.open(file, fstream::app);
    if (convergenceFile.is_open()){
      vector<double> values;
      graph.influenceScoreValues(values, {node}); // runs at max depth 10_000 times
      // compute score for various simulation sizes, record results
      int sum = 0;
      double score;
      for(int i = 1; i <= 10000; i++){
        sum += values[i];
        if (i == 1 || i == 10 || i % 100 == 0){
          // compute average
          score = sum/(double)i;
          convergenceFile << node << "," << i << "," << score << endl;
        }
      }
      // convergenceFile << node << "," << sims[i] << "," << results[i] << endl;
    }else{
      cerr << "Convergence file " << file << " not opened! " << endl;
      exit(1);
    }
    convergenceFile.close();
  }
}


void RTIM::seedComputationTest(int seedSize, int depth, double minEdgeWeight){
  // select random nodes from graph to generate seed
  cout << "------------------------------------------------------------------------------------------" << endl;
  cout << "Starting seed computation test on " << args.dataset << " > seed size: " << seedSize << " | depth: " << depth << "| minEdgeWeight: " << minEdgeWeight  <<  endl;
  if (seedSize > graph.nodes){
    cerr << "Error: Seed set size larger than graph size!" << endl;
    exit(1);
  }
  cout << "Sampling " << seedSize << " nodes for seed." << endl;
  vector<int> seedSet;
  double nodesNeeded = seedSize;
  double nodesLeft = graph.nodes;
  double selectProb;// = nodesNeeded / nodesLeft;
  double r;
  int i = 0;
  // we select sample with reservoir sampling
  while (seedSet.size() != seedSize && i < graph.nodes){
    if(graph.graph[i].size() >= 1){ // ignore nodes that have no neighbors
      selectProb = nodesNeeded / nodesLeft;
      r = rand()/(double)RAND_MAX;
      if (r < selectProb){
        // cout << i << " - " << r << " <= " << selectProb << " = " << nodesNeeded << " / " << nodesLeft << endl;
        seedSet.push_back(i);
        nodesNeeded --;
      }
    }
    nodesLeft --;
    i ++;
  }
  // test computation using monte carlo simulations
  cout << "Computing seed set score." << endl;
  double start = omp_get_wtime();
  double score;
  score = graph.influenceScore(seedSet, depth, minEdgeWeight);
  double delta = omp_get_wtime() - start;
  cout << "Seed set score is: " << score << " / " << graph.nodes << " computed in: " << cleanTime(delta, "s") << endl;
  cout << "------------------------------------------------------------------------------------------" << endl;
}


void datasetMenu(){
  string dataset;
  cout << "________________________________________" << endl;
  if(args.dataset != ""){
    cout << "Current imported dataset: " << args.dataset << endl;
  }
  cout << "Choose a [dataset] (nodes, edges): " << endl;
  cout << "   [test]        (20, 30])"<< endl;
  cout << "   [nethept]     (15_229, 62_752)" << endl;
  cout << "   [dblp]        (317_080, 2.09M)" << endl;
  cout << "   [youtube]     (1.13M, 5.97M)" << endl;
  cout << "   [orkut]       (3.07M, 234.37M)" << endl;
  cout << "   [livejournal] (3.99M, 69.36M)" << endl;
  cout << "   [twitter]     (41.65M, 1.46B)" << endl;
  while(1){
    cout <<  "> dataset name: ";
    cin >> dataset;
    if(dataset != "test" && dataset != "nethept" && dataset != "dblp" && dataset != "orkut" && dataset != "youtube" && dataset != "twitter" && dataset != "livejournal"){
      cerr << "Dataset not recognized: " << dataset << endl;
    } else if (args.dataset == dataset){
      cout << "Dataset has already been imported!" << endl;
      break;
    }
    else{
      args.dataset = dataset;
      break;
    }
  }
  // read attributes
  // import dataset
  stageMenu()
}


void stageMenu(){
  int choice;
  cout << "________________________________________" << endl;
  cout << "Choose a stage: " << endl;
  cout << "   [1] pre-process"<< endl;
  cout << "   [2] live " << endl;
  cout << "   [3] IMM seed set test" << endl;
  cout << "   [4] test " << endl;
  while(1){
    cout <<  "> choice: ";
    cin >> choice;
    switch(choice){
      case 1:
        stageMenu('pre_process');
        break;
      case 2:
        stageMenu('live');
        break;
      case 3:
        stageMenu('imm_seed_test');
        break;
      case 4:
        cout << "Test!" << endl;
        break;
      default:
        cout << "Error: stage not recognized!" << endl;
    }
  }
}


void stageArgumentsMenu(string stage){
  string input;
  cout << "Input " << stage << " arguments: ";
  getline(cin, input);
  // args.getInput(input);
  switch(stage){
    case 'pre_process':
      //   RTIM rtim = RTIM(args, true);
      //   double start = omp_get_wtime();
      //   rtim.pre_process();//g);
      //   double duration = omp_get_wtime() - start;
      //   cout << "Pre-process stage done in: " << cleanTime(duration, "s") << endl;
      cout << "Test: Launching pre_process!" << endl;
      break;
    case 'live':
      // live();
      cout << "Test: Launching live!" << endl;
      break;
    case 'imm_seed_test':
      cout << "Not implemented yet!" << endl;
      break;
    default:
      cout << "Error: stage not recognized!" << endl;
      exit(1);
      break;
  }
}


void continueMenu(){
  int choice;
  cout << "________________________________________" << endl;
  cout << "Continue: " << endl;
  cout << "   [1] Repeat previous stage with same arguments" << endl;
  cout << "   [2] Repeat previous stage with new arguments" << endl;
  cout << "   [3] Change stage" << endl;
  cout << "   [4] Change dataset" << endl;
  cout << "   [5] End Program" << endl;
  while(1){
    cout << "> choice: ";
    cin >> choice;
    switch(choice){
      case 1:
        cout << "Test: Repeat previous stage, same args!" << endl;
        break;
      case 2:
        stageArgumentsMenu();
        break;
      case 3:
        stageMenu();
        break;
      case 4:
        datasetMenu();
        break;
      default:
        cout << "Error: choice not recognized!" << endl;
    }
  }
}


void run(RTIM& rtim){
  datasetMenu();
}


int main(int argn, char **argv)
{
    clock_t start;
    double duration;
    run();
    // Arguments args = Arguments();
    // args.getArguments(argn, argv);
    // args.printArguments();
    //
    // if (args.stage == "pre"){
    //   RTIM rtim = RTIM(args, true);
    //   double start = omp_get_wtime();
    //   rtim.pre_process();//g);
    //   double duration = omp_get_wtime() - start;
    //   cout << "Pre-process stage done in: " << cleanTime(duration, "s") << endl;
    // } else if (args.stage == "live"){
    //   //
    //   RTIM rtim = RTIM(args, true);
    //   start = clock();
    //   rtim.live();//, args.k, args.streamModel, args.streamVersion, args.streamSize, args.theta_ap, args.reach);
    //   duration = (clock() - start)/(double)CLOCKS_PER_SEC;
    //   cout << "Live stage done in: " << cleanTime(duration, "ms") << endl;
    // } else if (args.stage == "newStream"){
    //   //
    //   // RTIM rtim = RTIM(args, false);
    //   // start = clock();
    //   // rtim.availabilityStream();//args.streamModel, args.streamVersion, args.streamSize);
    //   //
    //   // duration = (clock() - start)/(double)CLOCKS_PER_SEC;
    //   // cout << "Stream generated in: " << cleanTime(duration, "ms") << endl;
    //   cout << "Availability generator not implemented! " << endl;
    // } else if (args.stage == "special"){
    //
    //   /* Testing seed computation time
    //   RTIM rtim = RTIM(args, true);
    //   if(args.k == -1){
    //     args.k = 10;
    //   }
    //   int  sizes[7] = {10, 100, 300, 500, 1000, 2000, 5000};
    //   for (int i = 0; i < 7; i++){
    //     rtim.seedComputationTest(sizes[i]);
    //   }
    //   */
    //   RTIM rtim = RTIM(args, true);
    //   if(args.k == -1){
    //     args.k = 1;
    //   }
    //   // rtim.seedComputationTest(200, 2, 1.0);
    //   // rtim.seedComputationTest(200, 1, 1.0);
    //   // rtim.seedComputationTest(1000, 10000, 1.0);
    //   rtim.seedComputationTest(5000, 3, 1.0);
    //   rtim.seedComputationTest(5000, 2, 1.0);
    //   rtim.seedComputationTest(10000, 3, 1.0);
    //   rtim.seedComputationTest(10000, 2, 1.0);
    //   rtim.seedComputationTest(10000, 1, 1.0);
    // } else {
    //   cerr << "Error stage not recognized!" << endl;
    //   exit(1);
    // }
}
