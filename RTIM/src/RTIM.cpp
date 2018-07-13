#include <vector>
#include <cstdio>
#include <ctime>
#include <time.h>
#include <omp.h>
#include <string>
#include <algorithm>

#include "RTIM.h"
#include "Arguments.h"
#include "Graph.h"

#include <iostream>
#include <fstream>

using namespace std;

string cleanTime(double t){
  string cleanT;
  if (t < 1){
    t = t * 1000;
    cleanT = to_string(t) + " ms";
    return cleanT;
  }
  if (t > 60){
    t = t / 60; // time is in minutes
    if (t > 60){
      t = t / 60; // time is in hours
      if (t > 24){
        t = t / 24; // time is in days
        cleanT = to_string(t) + " days";
      }else{
        cleanT = to_string(t) + " hours";
      }
    }else{
      cleanT = to_string(t) + " minutes";
    }
  }else{
    cleanT = to_string(t) + " seconds";
  }
  return cleanT;
}

RTIM::RTIM(string d){
  dataset = d;
  srand(time(NULL));
}

void RTIM::saveToCSV(string fileName){
  ofstream myfile;
  myfile.open(fileName);
  myfile << "i,score\n";
  for (int j = 0; j < numNodes; j++){
      myfile << j << "," << infScores[j] << "\n";
  }
  myfile << "semicolon"; // ?
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

void RTIM::pre_process(const Graph& graph, int max_depth){
  // for each node in graph compute influence score
  cout << "Running pre_process on " << dataset << endl;
  double score;
  numNodes = graph.graph.size();

  infScores.reserve(numNodes);
  for(int i = 0; i < numNodes; i++){
    infScores.push_back(0);
  }
  double start = omp_get_wtime();
  int* nb_nodes = 0;
  int nb_threads = 0;
  int num_thread = 0;
  time_t startTime;
  int save = 0;
  int finishedProcess = 0;
  #pragma omp parallel private(score, num_thread, startTime, save) shared(graph, nb_nodes, nb_threads, finishedProcess) //shared(infScores, numNodes)
  {
    nb_threads = omp_get_num_threads();
    num_thread = omp_get_thread_num();
    if (num_thread == 0){
      if (nb_nodes == 0){
        nb_nodes = (int*)calloc (sizeof(int),nb_threads*8);
      }
      time ( &startTime );
    }
    #pragma omp for schedule(dynamic) nowait
    for(int i = 0; i < numNodes; i++){
      if (num_thread == 0){
        save = print_progress(nb_threads, finishedProcess, numNodes, startTime, nb_nodes, save);
      }
      // Compute the influence score of a node in G
      // score = graph.influenceScore({i}, 1);
      score = graph.influenceScorePath(i, max_depth);
      // score = graph.influenceScoreNeighbors(i);
      infScores[i] = score;
      nb_nodes[num_thread*8]++;
    }

    #pragma omp critical
    {
      finishedProcess ++;
      //printf("\r[%d/%d] Finished !\n",num_thread, nb_threads);
    }
    if (num_thread == 0){
      while (finishedProcess != nb_threads) {
        save = print_progress(nb_threads, finishedProcess, numNodes, startTime, nb_nodes, save);
      }
      save = print_progress(nb_threads, finishedProcess, numNodes, startTime, nb_nodes, save);
    }

  }
  cout << endl;
  double time = omp_get_wtime() - start;
  /*
  if (numNodes <= 20){
    printScores();
  }
  */
  saveScores();
};

void RTIM::live(const Graph& graph, int max_size, string model, int version, int size, double ap, int infReach){
  clock_t startLive = clock();
  numNodes = graph.graph.size();
  streamModel = model;
  streamVersion = version;
  streamSize = size;
  reach = infReach;
  theta_ap = ap;
  cout << "-------------------------------" << endl;
  cout << "Running live on " << dataset << endl;
  activationProbabilities.resize(numNodes, 0);
  importScores();
  sortedScores = infScores;
  sort(sortedScores.begin(), sortedScores.end());
  getInfIndex(sortedScores);
  cout << "Starting influence score threshold: " << sortedScores[infIndex] << endl;

  // read availability stream
  string file = streamModel + "_" + to_string(streamSize) + "_m" + to_string(streamVersion);
  string folder = "../../data/" + dataset + "/availability_models/" + streamModel + "/" + streamModel + "_m" + to_string(streamVersion) + "/" + streamModel + "_" + to_string(streamSize) + "_m" + to_string(streamVersion) + ".txt";
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
    // if (sum == 15162 || sum == 15163){
    //   cout << "(" << user << ") ";
    // }
    if (dataset == "test"){
      cout << "User: " << user << " is online." << endl;
    }
    if (activationProbabilities[user] < theta_ap && infScores[user] >= sortedScores[infIndex]){
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
      if (dataset == "test" || max_size < 20){
        cout << "Targeted user: " << user << endl;
      }
    }
    if (seedSet.size() >= max_size){
      break;
    }
    // print progress

    float progress = (float)sum/streamSize;

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
    cout << "] " << int(progress * 100.0) << " % -- (" << sum << "/" << streamSize << ") -- (" << duration << "s / " << timeLeft << "s)";
    cout.flush();

  }
  cout << endl;
  double streamDuration = (clock() - startStream)/(double)CLOCKS_PER_SEC;
  cout << "Availability stream read in:  " << cleanTime(streamDuration) << endl;
  saveSeedSet();
  cout << "Computing influence score of seed set of size: " << seedSet.size() << endl;
  start = clock();
  double seedScore = graph.influenceScore(seedSet);
  double seedDuration = (clock() - start)/(double)CLOCKS_PER_SEC;
  cout << "Seed set score: " << seedScore << "/" << numNodes << ", computed in: " << cleanTime(seedDuration) << endl;

  double liveDuration = (clock() - startLive)/(double)CLOCKS_PER_SEC;
  saveLiveLog(seedScore, streamDuration, seedDuration, max_time, sum, liveDuration);
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
  string file = "../../data/" + dataset + "/rtim/" + dataset + "_infscores.txt";
  cout << "Saving influence scores to: " << file << endl;
  // save scores
  ofstream infScoresFile;
  infScoresFile.open(file);
  for (int i = 0; i < infScores.size() ; i++){
     infScoresFile << i << " " << infScores[i] << endl;
  }
  infScoresFile.close();
  cout << "Scores saved successfully!" << endl;
}

void RTIM::saveSeedSet(){
  string file = "../../data/" + dataset + "/availability_models/" + streamModel + "/" + streamModel + "_m" + to_string(streamVersion) + "/" + dataset + "_seedSet.txt";
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
  string file = "../../data/" + dataset + "/availability_models/" + streamModel + "/" + streamModel + "_m" + to_string(streamVersion) + "/" + dataset + "_liveLog.txt";
  cout << "Saving live log to: " << file << endl;
  ofstream liveLogFile;
  liveLogFile.open(file, fstream::app);
  liveLogFile << "<Stream>" << endl;
  liveLogFile << "- model: " << streamModel << endl;
  liveLogFile << "- version: " << streamVersion << endl;
  liveLogFile << "- size: " << streamSize << endl;
  liveLogFile << "- progress: " << progress << endl;
  liveLogFile << "- runtime: " << cleanTime(streamTime) << endl;
  liveLogFile << "<Seed set>" << endl;
  liveLogFile << "- size: " << seedSet.size() << endl;
  liveLogFile << "- score: " << score << endl;
  liveLogFile << "- score compute time: " << cleanTime(seedTime) << endl;
  liveLogFile << "<Args>" << endl;
  liveLogFile << "- reach: " << reach << endl;
  liveLogFile << "- theta_ap: " << theta_ap << endl;
  liveLogFile << "Runtime: " << cleanTime(runtime) << endl;
  liveLogFile << "Max update time: " << cleanTime(maxTime) << endl;
  liveLogFile << "----------------------------------------------------" << endl;
  liveLogFile.close();
  cout << "Live log saved successfully!" << endl;
}

void RTIM::importScores(){
  string folder = "../../data/" + dataset + "/rtim/" + dataset + "_infscores.txt";
  cout << "Importing influence scores from: " << folder << endl;
  infScores.resize(numNodes, 0);
  int user;
  double infScore;
  ifstream infile(folder.c_str());
  while(infile >> user >> infScore){
    infScores[user] = infScore;
  }
  cout << "Import successful" << endl;
  printScores();
}

void RTIM::availabilityStream(string model, int version, int size){
  cout << "Generating availability stream: " << model << "_m"<< version << endl;
  string file = model + "_m" + to_string(version);
  string folder = "../../data/" + dataset + "/availability_models/" + model + "/" + file + "/" + file + ".txt";
  int user;
  ofstream availabilityFile;
  availabilityFile.open(folder);
  for (int i = 0; i < size; i++){
    availabilityFile << 0 << endl;
  }
  availabilityFile.close();
  cout << "Availability file saved!" << endl;
}

void RTIM::getInfIndex(vector<double> & sorted){
  infIndex = sorted.size() - sorted.size() * reach / 100;
}

int main(int argn, char **argv)
{
    clock_t start;
    double duration;

    Arguments args = Arguments();
    args.getArguments(argn, argv);
    args.printArguments();

    RTIM rtim = RTIM(args.dataset);

    if (args.stage == "pre"){
      //
      start = clock();
      Graph g = Graph(args.dataset, true);
      duration = (clock() - start)/(double)CLOCKS_PER_SEC;
      cout << "Import done in: " << cleanTime(duration) << endl;

      start = clock();
      rtim.pre_process(g, args.depth);
      duration = (clock() - start)/(double)CLOCKS_PER_SEC;
      cout << "Pre-process stage done in: " << cleanTime(duration) << endl;
    }else if (args.stage == "live"){
      //
      start = clock();
      Graph g = Graph(args.dataset, true);
      duration = (clock() - start)/(double)CLOCKS_PER_SEC;
      cout << "Import done in: " << cleanTime(duration) << endl;
      if(args.streamSize == -1){
        args.streamSize = g.nodes;
      }
      if (args.k == -1){
        args.k = g.graph.size();
      }
      start = clock();
      rtim.live(g, args.k, args.streamModel, args.streamVersion, args.streamSize, args.activation_threshold, args.reach);
      duration = (clock() - start)/(double)CLOCKS_PER_SEC;
      cout << "Live stage done in: " << cleanTime(duration) << endl;
    }else if (args.stage == "newStream"){
      //
      Graph g = Graph(args.dataset, false);
      start = clock();
      if (args.streamSize == -1){
        args.streamSize = g.nodes;
      }
      rtim.availabilityStream(args.streamModel, args.streamVersion, args.streamSize);

      duration = (clock() - start)/(double)CLOCKS_PER_SEC;
      cout << "Stream generated in: " << cleanTime(duration) << endl;
    }else{
      cerr << "Error stage not recognized!" << endl;
      exit(1);
    }

    //Graph g = Graph(args.dataset, true);
    //rtim.numNodes = g.nodes;
    /*rtim.importScores();
    /vector<double> sortedScores = rtim.infScores;
    sort(sortedScores.begin(), sortedScores.end());
    cout << "Sorted scores: " << endl;
    for (int i = 0; i < sortedScores.size() ; i++){
      cout << "(" << i << " : " << sortedScores[i] << ")" << endl;
    }
    rtim.reach = 20;
    rtim.getInfIndex(sortedScores);
    cout << rtim.infIndex << endl;
    */
    /*
    rtim.activationProbabilities.resize(rtim.numNodes, 0);

    start = clock();
    rtim.activationProbabilities[0] = 1.0;
    cout << "Target user: 0" << endl;
    g.updateNeighborsAP(0, rtim.activationProbabilities, {}, 1.0, 1);
    duration = (clock() - start)/(double)CLOCKS_PER_SEC;
    cout << "Time taken at depth 1 for user 0 with updateNeighborsAP: " << cleanTime(duration) << endl;

    start = clock();
    rtim.activationProbabilities[0] = 1.0;
    cout << "Target user: 0" << endl;
    g.updateNeighborsAPShort(0, rtim.activationProbabilities);
    duration = (clock() - start)/(double)CLOCKS_PER_SEC;
    cout << "Time taken at depth 1 for user 0 with updateNeighborsAPShort: " << cleanTime(duration) << endl;


    start = clock();
    rtim.activationProbabilities[1] = 1.0;
    cout << "Target user: 1" << endl;
    g.updateNeighborsAP(1, rtim.activationProbabilities, {}, 1.0, 1);
    duration = (clock() - start)/(double)CLOCKS_PER_SEC;
    cout << "Time taken at depth 1 for user 1 with updateNeighborsAP: " << cleanTime(duration) << endl;

    start = clock();
    rtim.activationProbabilities[1] = 1.0;
    cout << "Target user: 1" << endl;
    g.updateNeighborsAPShort(1, rtim.activationProbabilities);
    duration = (clock() - start)/(double)CLOCKS_PER_SEC;
    cout << "Time taken at depth 1 for user 1 with updateNeighborsAPShort: " << cleanTime(duration) << endl;
    */
    // rtim.printScores();

}
