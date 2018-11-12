#include <vector>
#include <cstdio>
#include <ctime>
#include <time.h>
#include <omp.h>
#include <string>
#include <algorithm>
#include <sys/stat.h>
#include <unistd.h>

#include "RTIM.h"

#include <iostream>
#include <fstream>
#include <iomanip>
#include <ctime>
#include <sstream>
#include <stdexcept>

/**
  * Special color codes: https://stackoverflow.com/questions/2616906/how-do-i-output-coloured-text-to-a-linux-terminal
  */

using namespace std;

const int SLEEP = 2; // duration of sleep


RTIM::RTIM(Arguments& arguments):graph(arguments){
  args = arguments;
  srand(time(NULL));
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

  // int testPourcent = (int)(progress*100.0);
  // if (testPourcent%10 == 0 && save == 0){
  //   save = 1;
  //   string fileName = "../../data/algoritmhs/rtim/infScores_"+ std::to_string(testPourcent) + ".csv";
  //   saveToCSV(fileName);
  // } else if (testPourcent%10 != 0){
  //   save = 0;
  // }

  return save;
}


void RTIM::pre_process(){
  // for each node in graph compute influence score
  // cout << "Running pre_process on " << args.dataset << endl;
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
      clock_t nodeStart = clock();
      score = graph.influenceScore({i}, args.depth);
      // score = graph.influenceScorePath(i, args.depth,"shortest", args.edge_weight, args.min_weight);
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
  string startDatetime = getLocalDatetime();
  // nodes = graph.graph.size();
  // cout << "-------------------------------" << endl;
  // cout << "Running live on " << args.dataset << endl;
  seedSet.clear(); // in case live was already run with different params
  initiateProgressLog();
  activationProbabilities.clear();
  activationProbabilities.resize(nodes, 0);
  // ideally we should not repeat this if live is run more than once
  importScores();
  sortedScores = infScores;
  sort(sortedScores.begin(), sortedScores.end());
  getInfIndex(sortedScores);
  cout << "Starting influence score threshold: " << sortedScores[infIndex] << endl;

  // read availability stream
  string folder = "../../data/" + args.dataset + "/streams/" + args.streamModel + "/v" + to_string(args.streamVersion) + "/" + args.generateFileName("stream");
  int user;
  // cout << "Reading availability stream: " << folder << endl;
  printInColor("cyan", "Reading availability stream: " + folder);
  ifstream infile(folder.c_str());
  double max_time = 0;
  clock_t start;
  clock_t startStream = clock();
  time_t startTime;
  time ( &startTime );
  double duration;
  int sum = 0;
  int previousProgress = -1;
  bool newProgress = true;
  while (infile >> user){
    sum ++;
    cout << "User: " << user << " dataset: " << args.dataset << endl;
    if (args.dataset == "test"){
      cout << "User: " << user << " is online: old_ap = " << activationProbabilities[user] << ", score = " << infScores[user] << endl;
    }
    if (activationProbabilities[user] < args.theta_ap && infScores[user] >= sortedScores[infIndex]){
      seedSet.push_back(user);
      saveProgress(user, sum, seedSet.size());
      double tmpAP = activationProbabilities[user];
      activationProbabilities[user] = 1.0;
      // measure update time
      start = clock();
      //graph.updateNeighborsAP(user, activationProbabilities, {}, 1.0, 1);
      graph.updateNeighborsAPShort(user, activationProbabilities);
      duration = (clock() - start)/(double)CLOCKS_PER_SEC;
      if (duration > max_time){
        max_time = duration;
      }

      infIndex --;
      if (args.dataset == "test" || args.k < 20){
        cout << "Targeted user: " << user << ": old_ap = " << tmpAP << ", score = " << infScores[user] << endl;
      }
      if (seedSet.size() == 1){
      	cout << "First user targeted: " << user << ": old_ap = " << tmpAP << ", score = " << infScores[user] << endl;
      }else if (seedSet.size() == args.k){
      	cout << "Last user targeted: " << user << ": old_ap = " << tmpAP << ", score = " << infScores[user] << endl;
      }
    }
    if (seedSet.size() >= args.k){
      break;
    }

    // print progress
    /*
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
    int progressPercentage = (int)(progress * 100.0);
    cout << "] " << progressPercentage << " % -- (" << sum << "/" << args.streamSize << ") -- (" << duration << "s / " << timeLeft << "s)";
    cout.flush();
    */

  }
  cout << endl;
  clearLines(1);
  double streamDuration = (clock() - startStream)/(double)CLOCKS_PER_SEC;
  cout << "Availability stream read in:  " << cleanTime(streamDuration, "ms") << endl;
  saveSeedSet();

  string endDatetime = getLocalDatetime();
  double liveDuration = (clock() - startLive)/(double)CLOCKS_PER_SEC;
  saveLiveLog(max_time, liveDuration, startDatetime, endDatetime);
  saveLiveCSV(graph, streamDuration, max_time, liveDuration);
  // cout << "Live complete!" << endl;
  // cout << "------------------------------" << endl;
};


void RTIM::printScores(){
  cout << "Influence Scores: " << infScores.size() << endl;
  if (infScores.size() < 50){
    for (int i = 0; i < infScores.size() ; i++){
      cout << "(" << i << " : " << infScores[i] << ")" << endl;
    }
    sleep(SLEEP + 2);
    clearLines(infScores.size() + 1);
  }else{
    for (int i = 0; i < 20 ; i++){
      cout << "(" << i << " : " << infScores[i] << ")" << endl;
    }
    sleep(SLEEP + 5); // pause 6 seconds to let user time to review inf scores
    clearLines(21);
  }
}


void RTIM::saveScores(){
  string file = "../../data/" + args.dataset + "/rtim/pre_process/" + args.generateFileName("save_infScores");
  //string txt = "Saving inlfuence score to " + file;
  printInColor("cyan", "Saving inlfuence score to " + file);
  //cout << "\033[33mSaving influence scores to: " << file << "\033[0m" << endl;
  // save scores
  ofstream infScoresFile;
  infScoresFile.open(file);
  for (int i = 0; i < infScores.size() ; i++){
     infScoresFile << i << " " << infScores[i] << " " << nodeTime[i] << endl;
  }
  infScoresFile.close();
  // printInColor("cyan", "Scores saved successfully!");
  //cout << "\033[1;33mScores saved successfully!\033[0m" << endl;
}


void RTIM::saveSeedSet(bool progress, int progPercentage){
  string file = "../../data/" + args.dataset + "/rtim/live/";
  if(progress){
    file += "progress/" + args.generateFileName("rtim_progress_seedSet", progPercentage);
    printInColor("cyan", "Saving progress seed set to: " + file);
  }else{
    file += args.generateFileName("rtim_seedSet", seedSet.size());
    printInColor("cyan", "Saving seed set to: " + file);
  }
  ofstream seedSetFile;
  seedSetFile.open(file);
  for (int i = 0; i < seedSet.size() ; i++){
     seedSetFile << seedSet[i] << endl;
  }
  seedSetFile.close();
}


void RTIM::importSeedSet(string file_path){
  int user;
  seedSet.clear();
  // string file = "../../data/" + args.dataset + "/" + file_path;
  cout << "Importing from: " << file_path << endl;

  ifstream infile(file_path.c_str());
  while(infile >> user){
    seedSet.push_back(user);
  }
  cout << "Seed set imported correctly!" << endl;
}


void RTIM::seedSetTest(string file_path){
  double score;
  importSeedSet(file_path);
  if(seedSet.size() <= 300){
    cout << "Computing influence score of seed set with max depth." << endl;
    score = graph.influenceScore(seedSet);
  }else{
    cout << "Computing influence score of seed set with depth of 3." << endl;
    score = graph.influenceScore(seedSet, 3);
  }
  cout << "Seed score is: " << score << endl;
}


void RTIM::saveLiveLog(double& maxTime, double& runtime, string startDatetime, string endDatetime){
  // string file = "../../data/" + args.dataset + "/streams/" + args.streamModel + "/" + args.streamModel + "_m" + to_string(args.streamVersion) + "/" + args.dataset + "_liveLog.txt";
  string file = "../../data/" + args.dataset + "/logs/rtim_live.log";
  //cout << "\033[33mSaving live log to: " << file << "\033[0m" << endl;
  printInColor("cyan", "Saving live log to: " + file);
  ofstream liveLogFile;
  liveLogFile.open(file, fstream::app);
  liveLogFile << "File name       : " << args.generateFileName("rtim_seedSet") << endl;
  liveLogFile << "Start date      : " << startDatetime << endl;
  liveLogFile << "End date        : " << endDatetime << endl;
  liveLogFile << "Duration        : " << cleanTime(runtime, "s") << endl;
  liveLogFile << "Max update time : " << cleanTime(maxTime, "s") << endl;
  liveLogFile << "----------------------------------------------------" << endl;
  liveLogFile.close();
  //cout << "\033[33mLive log saved successfully!\033[0m" << endl;
  // printInColor("cyan", "Live log saved successfully");
}


void RTIM::saveLiveCSV(const Graph& graph, double& streamTime, double& maxTime, double& runtime){
  string file = "../../data/" + args.dataset + "/logs/rtim_live.csv";
  // cout << "\033[33mSaving live csv to: " << file << "\033[0m" << endl;
  printInColor("cyan", "Saving live csv to: " + file);
  ofstream liveCSV;
  liveCSV.open(file, fstream::app);
  /*Order of values: dataset, streamModel, streamVersion, reahc, theta_ap, depth, maxSeedSize, actualSeedSize, maxUpdateTime, runtime*/
  liveCSV << args.dataset << ",";
  liveCSV << args.streamModel << ",";
  liveCSV << args.streamVersion << ",";
  liveCSV << args.streamSize << ",";
  liveCSV << args.reach << ",";
  liveCSV << args.theta_ap << ",";
  liveCSV << args.depth << ",";
  liveCSV << args.k << ",";
  liveCSV << seedSet.size() << ",";
  liveCSV << maxTime << ",";
  liveCSV << runtime << endl;
  liveCSV.close();
  // cout << "\033[33mLive CSV saved!\033[0m" << endl;
  // printInColor("cyan", "Live CSV saved!");
}


void RTIM::saveSeedScoreLog(string file, string startDate, string endDate, double& runtime, double& score){
  string file_path = "../../data/" + args.dataset + "/logs/seed_set_score.log";
  printInColor("cyan", "Saving seed score log to: " + file_path);
  ofstream seedScoreLogFile;
  seedScoreLogFile.open(file_path, fstream::app);
  seedScoreLogFile << "File name  : " << file << endl;
  seedScoreLogFile << "Start date : " << startDate << endl;
  seedScoreLogFile << "End date   : " << endDate << endl;
  seedScoreLogFile << "Runtime    : " << runtime << endl;
  seedScoreLogFile << "Seed size  : " << seedSet.size() << endl;
  seedScoreLogFile << "Inf. score : " << score << endl;
  seedScoreLogFile << "-----------------------------------------------" << endl;
  seedScoreLogFile.close();
}


void RTIM::saveSeedScoreCSV(string file, string startDate, string endDate, double& runtime, double& score){
  string file_path = "../../data/" + args.dataset + "/logs/seed_set_score.csv";
  printInColor("cyan", "Saving seed score csv to: " + file_path);

  // dataset,file_name, startDate, endDate, runtime, seed size, score
  ofstream seedScoreCSVFile;
  seedScoreCSVFile.open(file_path, fstream::app);
  seedScoreCSVFile << args.dataset << ",";
  seedScoreCSVFile << file << ",";
  seedScoreCSVFile << startDate << ",";
  seedScoreCSVFile << endDate << ",";
  seedScoreCSVFile << runtime << ",";
  seedScoreCSVFile << seedSet.size() << ",";
  seedScoreCSVFile << score << endl;
  seedScoreCSVFile.close();
}


void RTIM::initiateProgressLog(){
  string folder = "../../data/" + args.dataset + "/rtim/live/progress/";
  string file = folder + args.generateFileName("rtim_progress");
  printInColor("cyan", "Initiating progress log: " + file);
  ofstream progressFile;
  progressFile.open(file);
  progressFile << "seen,user_index,seed_size" << endl;
  progressFile.close();
}


void RTIM::saveProgress(int user_index, int seen, int seedSize){
  string folder = "../../data/" + args.dataset + "/rtim/live/progress/";
  string file = folder + args.generateFileName("rtim_progress");
  // printInColor("cyan","Saving progress: " + to_string(progress));
  ofstream progressFile;
  progressFile.open(file, fstream::app);
  /* progress | nodes seen | seed size */
  progressFile << seen << ",";
  progressFile << user_index << ",";
  progressFile << seedSize;
  progressFile << endl;
  progressFile.close();
}


void RTIM::importScores(){
  string folder = "../../data/" + args.dataset + "/rtim/pre_process/" + args.generateFileName("get_infScores");
  // cout << "Importing influence scores from: " << folder << endl;
  printInColor("cyan", "Importing influence scores from: " + folder);
  infScores.resize(nodes, 0);
  int user;
  double infScore;
  double scoreTime;

  ifstream infile(folder.c_str());
  while(infile >> user >> infScore >> scoreTime){
    infScores[user] = infScore;
  }
  // cout << "Import successful" << endl;
  printInColor("cyan", "Import successful");
  printScores();
}


void RTIM::availabilityStream(){
  cout << "Generating availability stream: " << args.streamModel << "_m"<< args.streamVersion << endl;
  string folder = "../../data/" + args.dataset + "/streams/" + args.streamModel + "/v" + to_string(args.streamVersion) + "/";
  string file = folder + args.generateFileName("stream");
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
  infIndex = (int)(sorted.size() - sorted.size() * args.reach / 100);
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


int RTIM::datasetMenu(){
  int lines = 10;
  string dataset;
  cout << "________________________________________" << endl;
  if(args.dataset != "" && graph.graph.size() != 0){
    cout << "Current imported dataset: " << args.dataset << endl;
    lines++;
  }
  cout << "Choose a [dataset] (nodes, edges): " << endl;
  cout << "   [test]        (      20,      30)"<< endl;
  cout << "   [nethept]     (  15_229,  62_752)" << endl;
  cout << "   [dblp]        ( 317_080,   2.09M)" << endl;
  cout << "   [youtube]     (   1.13M,   5.97M)" << endl;
  cout << "   [orkut]       (   3.07M, 234.37M)" << endl;
  cout << "   [livejournal] (   3.99M,  69.36M)" << endl;
  cout << "   [twitter]     (  41.65M,   1.46B)" << endl;
  while(1){
    cout <<  "> dataset name: ";
    getline(cin, dataset);
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
      graph.directory = "../../data/" + args.dataset; //necessary to readAttributes
      graph.readAttributes();
      nodes = graph.nodes;
      sleep(2);
      lines += 3;
      break;
    }
  }
  clearLines(lines);
  return 1;
}


void RTIM::stageMenu(){
  int choice = -1;
  cout << "________________________________________" << endl;
  cout << "Choose a stage: " << endl;
  cout << "   [1] pre-process"<< endl;
  cout << "   [2] live " << endl;
  cout << "   [3] compute seed score" << endl;
  cout << "   [4] test " << endl;
  cout << "   [5] EXIT PROGRAM " << endl;
  while(choice == -1){
    cout <<  "> choice: ";
    string val;
    getline(cin, val);
    choice = stoi(val);
    switch(choice){
      case 1:
        args.stage = "pre";
        break;
      case 2:
        args.stage = "live";
        break;
      case 3:
        args.stage = "compute_seed_score";
        break;
      case 4:
      args.stage = "test";
        break;
      case 5:
        cout << "Program ending: Have a nice day!" << endl;
        printLocalTime("red", "Program", "ending");
        cout << endl;
        exit(1);
      default:
        cout << "Error: stage not recognized!" << endl;
        sleep(SLEEP);
        clearLines(2);
        choice = -1;
    }
  }
  clearLines(8);
}


void RTIM::stageArgumentsMenu(){
  if (args.stage == "pre"){
    preProcessMenu();
  }else if (args.stage == "live"){
    liveMenu();
  }else if (args.stage == "compute_seed_score"){
    computeSeedScoreMenu();
  }else if (args.stage != "test"){
    cout << "Error: Stage not recognized!" << endl;
    exit(1);
  }
}


void RTIM::preProcessMenu(){
  int iChoice;
  double dChoice;
  string input;
  cout << "________________________________________" << endl;
  cout << "Input pre_process arguments" << endl;
  // asking for max search depth
  while(1){
    cout << "> depth (" << args.depth << "): ";
    getline(cin, input);
    if(input != ""){
      try{
        iChoice = stoi(input);

        if (iChoice >= 1 && iChoice <= 10000){
          args.depth = iChoice;
          break;
        }else{
          cout << "Error: depth must be int between 1 and 10000!" << endl;
          sleep(SLEEP);
          clearLines(2);
        }
      }catch(invalid_argument& e){
        cout << "Error: invalid input!" << endl;
        sleep(SLEEP);
        clearLines(2);
      }
    }else{
      clearLines(1);
      cout << "> depth (" << args.depth << "): " << args.depth << endl;
      break;
    }
  }
  // asking for minimum path weight
  while(1){
    cout << "> minimum path weight(" << args.min_weight << "): ";
    getline(cin, input);
    if(input != ""){
      try{
        dChoice = stod(input);
        if (dChoice >= 0 && dChoice <= 1.0){
          args.min_weight = dChoice;
          break;
        }else{
          cout << "Error: minimum path weight must be a probability!" << endl;
          sleep(SLEEP);
          clearLines(2);
        }
      }catch(invalid_argument& e){
        cout << "Error: invalid input!" << endl;
        sleep(SLEEP);
        clearLines(2);
      }
    }else{
      clearLines(1);
      cout << "> minimum path weight(" << args.min_weight << "): " << args.min_weight << endl;
      break;
    }
  }
  sleep(SLEEP);
  clearLines(4);
}


void RTIM::liveMenu(){
  int iChoice;
  double dChoice;
  string input;
  cout << "________________________________________" << endl;
  cout << "Input live arguments: [seed size | reach | activation probability threshold | stream model | stream version | stream size]" << endl;
  // asking for seed size
  while(1){
    cout << "> seed size (" << args.k << "): ";
    getline(cin, input);
    if(input != ""){
      try{
        iChoice = stoi(input);
        if(iChoice > nodes){
          cout << "Error: seed size larger than current graph: " << iChoice << " / " << nodes << endl;
          sleep(SLEEP + 2);
          clearLines(2);
        }else{
          args.k = iChoice;
          break;
        }
      }catch(invalid_argument& e){
        cout << "Error: invalid input!" << endl;
        sleep(SLEEP);
        clearLines(2);
      }
    }else{
      if(args.k > nodes){
        cout << "Error: seed size larger than current graph: " << args.k << " / " << nodes << endl;
        sleep(SLEEP + 2);
        clearLines(2);
      }else{
        clearLines(1);
        cout << "> seed size (" << args.k << "): " << args.k << endl;
        break;
      }
    }
  }
  // asking for reach
  while(1){
    cout << "> reach (" << properStringDouble(args.reach) << "): ";
    getline(cin, input);
    if(input != ""){
      try{
        dChoice = stof(input);
        if (dChoice > 0 && dChoice < 100){
          args.reach = dChoice;
          break;
        }else{
          cout << "Error: expecting int value between 1 and 100!" << endl;
          sleep(SLEEP);
          clearLines(2);
        }
      }catch(invalid_argument& e){
        cout << "Error: invalid input!" << endl;
        sleep(SLEEP);
        clearLines(2);
      }
    }else{
      clearLines(1);
      cout << "> reach (" << properStringDouble(args.reach) << "): " << properStringDouble(args.reach) << endl;
      break;
    }
  }
  // asking for activation probability
  while(1){
    cout << "> activation probability threshold (" << properStringDouble(args.theta_ap) << "): ";
    getline(cin, input);
    if(input != ""){
      dChoice = stod(input);
      if(dChoice >= 0.0 && dChoice <= 1.0){
        args.theta_ap = dChoice;
        break;
      }else{
        cout << "Error: Input must be a probability!" << endl;
        sleep(SLEEP);
        clearLines(2);
      }
    }else{
      clearLines(1);
      cout << "> activation probability threshold (" << properStringDouble(args.theta_ap) << "): " << properStringDouble(args.theta_ap) << endl;
      break;
    }
  }
  // asking for stream model
  while(1){
    cout << "> stream model [rand_repeat, rand_no_repeat](" << args.streamModel << "): ";
    getline(cin, input);
    if(input != ""){
      if(input == "rand_repeat" || input == "rand_no_repeat"){
        args.streamModel = input;
        break;
      }else{
        cout << "Error: Input must be a valid stream model!" << endl;
        sleep(SLEEP);
        clearLines(2);
      }
    }else{
      clearLines(1);
      cout << "> stream model [rand_repeat, rand_no_repeat](" << args.streamModel << "): " << args.streamModel << endl;
      break;
    }
  }
  // asking for stream version
  while(1){
    cout << "> stream version [1, 2, 3] (" << args.streamVersion << "): ";
    getline(cin, input);
    if(input != ""){
      try{
        iChoice = stoi(input);
        if(iChoice == 1 || iChoice == 2 || iChoice == 3){
          args.streamVersion = iChoice;
          break;
        }else{
          cout << "Error: stream version doesn't exist!" << endl;
          sleep(SLEEP);
          clearLines(2);
        }
      }catch(invalid_argument& e){
        cout << "Error: invalid input!" << endl;
        sleep(SLEEP);
        clearLines(2);
      }
    }else{
      clearLines(1);
      cout << "> stream version [1, 2, 3]: " << args.streamVersion << endl;
      break;
    }
  }
  // asking for stream size
  while(1){
    cout << "> stream size (" << nodes << "): ";
    getline(cin, input);
    if(input != ""){
      try{
        iChoice = stoi(input);
        args.streamSize = iChoice;
        break;
      }catch(invalid_argument& e){
        cout << "Error: invalid input!" << endl;
        sleep(SLEEP);
        clearLines(2);
      }
    }else{
      args.streamSize = nodes;
      //clearLines(1);
      cout << "> stream size (" << args.streamSize << "): ";
      break;
    }
  }
  sleep(SLEEP+1);
  clearLines(8);
}


void RTIM::computeSeedScoreMenu(){
  // seedSet.clear(); //in case it's a re-run
  int choice = -1;
  string file_path = "../../data/" + args.dataset + "/";
  cout << "________________________________________" << endl;
  cout << "Choose a folder: " << endl;
  cout << "   [1] rtim/live/" << endl;
  cout << "   [2] imm/basic/" << endl;
  cout << "   [3] imm/intersect/" << endl;
  while(choice == -1){
    cout <<  "> choice: ";
    string val;
    getline(cin, val);
    choice = stoi(val);
    switch(choice){
      case 1:
        file_path += "rtim/live/";
        break;
      case 2:
        file_path += "imm/basic/";
        break;
      case 3:
        file_path += "imm/intersect/";
        break;
      default:
        cout << "Error: choice not recognized!" << endl;
        sleep(SLEEP);
        clearLines(2);
        choice = -1;
    }
  }
  sleep(SLEEP);
  clearLines(7);

  string input;
  cout << "________________________________________" << endl;
  cout << "Input seed set file name" << endl;
  while (1){
    cout << "> seed file ["<< file_path <<"] : ";
    getline(cin, input);
    break;
  }
  file_path += input;
  importSeedSet(file_path);
  sleep(SLEEP);
  clearLines(5);
  seedFile = input;
}


int RTIM::continueMenu(){
  int choice = -1;
  cout << "________________________________________" << endl;
  cout << "Continue: " << endl;
  cout << "   [1] Repeat previous stage with same arguments" << endl;
  cout << "   [2] Repeat previous stage with new arguments" << endl;
  cout << "   [3] Change stage" << endl;
  cout << "   [4] Change dataset" << endl;
  cout << "   [5] End Program" << endl;
  while(choice == -1){
    cout << "> choice: ";
    string val;
    getline(cin, val);
    choice = stoi(val);
    switch(choice){
      case 1:
        break;
      case 2:
        break;
      case 3:
        break;
      case 4:
        break;
      case 5:
        clearLines(8);
        cout << "Programming ending: \"Have a nice day!\"      "<< endl;
        return -1;
      default:
        cout << "Error: choice not recognized!" << endl;
        choice = -1;
        clearLines(2);
    }
  }
  clearLines(8);
  return choice;
}


void RTIM::run(){
  cout << endl;
  printLocalTime("red", "Program", "starting");
  int choice = 0;
  int loadDataset;
  while (choice != -1){
    loadDataset = 0;
    switch(choice){
      case 1:
        break;
      case 2:
        stageArgumentsMenu();
        args.printArguments();
        break;
      case 3:
        stageMenu();
        stageArgumentsMenu();
        args.printArguments();
        break;
      case 4:
        loadDataset = datasetMenu();
        stageMenu();
        stageArgumentsMenu();
        args.printArguments();
        break;
      default:
        loadDataset = datasetMenu();
        stageMenu();
        stageArgumentsMenu();
        args.printArguments();
        break;
    }
    if(args.stage == "test"){
      loadDataset = false;
    }
    if(loadDataset){
      graph.args = args;
      graph.graph.resize(graph.nodes);
      graph.loadGraph();
      clearLines(3);
    }
    if (args.stage == "pre"){
      printLocalTime("magenta", "Pre_processing", "starting");
      pre_process();
      printLocalTime("magenta", "Pre_processing", "ending");
    }else if (args.stage == "live"){
      printLocalTime("magenta", "Live", "starting");
      live();
      printLocalTime("magenta", "Live", "ending");
    }else if (args.stage == "compute_seed_score"){
      printLocalTime("magenta", "Compute seed score", "starting");
      double score;
      string startDate = getLocalDatetime();
      clock_t startTime = clock();
      score = graph.influenceScore(seedSet);
      double duration = (clock() - startTime)/(double)CLOCKS_PER_SEC;
      string endDate = getLocalDatetime();
      string txt = "> Influence score of seed set is: " + to_string(score);
      printInColor("cyan", txt);
      saveSeedScoreLog(seedFile, startDate, endDate, duration, score);
      saveSeedScoreCSV(seedFile, startDate, endDate, duration, score);
      // printLocalTime("magenta", "IMM seed test", "ending");
    }else if (args.stage == "test"){
      printLocalTime("magenta", "Test", "starting");
      runTest();
      printLocalTime("magenta", "Test", "ending");
    } else{
      cout << "Error! stage not recognized: " << args.stage << endl;
      exit(1);
    }
    choice = continueMenu();
  }

  printLocalTime("red", "Program", "ending");
  cout << endl;
}


void RTIM::runTest(){
  // import scores

  string folder = "../../data/" + args.dataset + "/rtim/live/" + args.dataset + "_infS.txt";
  // cout << "Importing influence scores from: " << folder << endl;
  printInColor("cyan", "Importing influence scores from: " + folder);
  infScores.resize(nodes, 0);
  cout << "Size of infScores after resize: " << infScores.size() << endl;
  int user;
  double infScore;
  double scoreTime;

  ifstream infile(folder.c_str());
  int count = 0;
  while(infile >> user >> infScore >> scoreTime){
    if (infScore < 1){
      count ++;
      if(count < 50){
        cout << user << " | " << infScore << " | " << scoreTime << endl;
      }
    }
    infScores[user] = infScore;

  }
  cout << "Number of scores < 1 in file: " << count << endl;
  // cout << "Import successful" << endl;
  printInColor("cyan", "Import successful");
  printScores();
}

int main(int argn, char **argv)
{
  // int cores = omp_get_max_threads();
  Arguments args = Arguments();
  RTIM rtim = RTIM(args);
  rtim.run();
}
