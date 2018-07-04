#include <vector>
#include <cstdio>
#include <ctime>
#include <time.h>
#include <omp.h>
#include <string>

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
}

void RTIM::saveToCSV(string fileName){
  int j;
  ofstream myfile;
  myfile.open (fileName);
  myfile << "i,score\n";
  for (j=0; j<numNodes; j++){
      myfile << j << "," << infScores[j] << "\n";
  }
  myfile << "semi;colon";
  myfile.close();
}

int RTIM::print_progress(int nb_threads, int finishedProcess, int numNodes, time_t startTime, int* nb_nodes, int save){
  int j, sum = 0;
  for (j=0; j<nb_threads; j++){
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
  if (testPourcent%10 == 0 && save==0){
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
    if (num_thread==0){
      if (nb_nodes == 0){
        nb_nodes = (int*)calloc (sizeof(int),nb_threads*8);
      }
      time ( &startTime );
    }
    #pragma omp for schedule(dynamic) nowait
    for(int i = 0; i < numNodes; i++){
      if (num_thread==0){
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
    if (num_thread==0){
      while (finishedProcess!=nb_threads) {
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

void RTIM::live(const Graph& graph, int max_size){
  numNodes = graph.graph.size();
  cout << "Running live on " << dataset << endl;
  // run live
  /*
  - import influence scores of dataset
  - while import availability stream
    - while seed_set.size() < k:
      - target or not user
  - save seed set to file
  - compute influence score of seed set and save data to file
  */
  activationProbabilities.resize(numNodes, 0);
  importScores();
  // define inf threshold

  // read availability stream
  string folder = "../../data/" + dataset + "/rtim/rand_repeat.txt";
  int user;
  cout << "Reading availability stream: " << folder << endl;
  ifstream infile(folder.c_str());
  while (infile >> user){
    while (seedSet.size() < max_size){
      cout << "User: " << user << " is online." << endl;
      if (activationProbabilities[user] < 0.8 && infScores[user] > 50){
        activationProbabilities[user] = 1.0;
        // update neighbor activationProbabilities
        seedSet.push_back(user);
      }
    }
  }
  double seedScore = graph.influenceScore(seedSet);
  cout << "Live complete!" << endl;
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
  for (int i = 0; i < numNodes ; i++){
     infScoresFile << i << " " << infScores[i] << endl;
  }
  infScoresFile.close();
  cout << "Scores saved successfully!" << endl;
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

int main(int argn, char **argv)
{
    clock_t start;
    double duration;

    Arguments args = Arguments();
    args.getArguments(argn, argv);
    args.printArguments();

    RTIM rtim = RTIM(args.dataset);

    if (args.stage == "pre"){
      start = clock();
      Graph g = Graph(args.dataset, true);
      duration = (clock() - start)/(double)CLOCKS_PER_SEC;
      cout << "Import done in: " << cleanTime(duration) << endl;

      start = clock();
      rtim.pre_process(g, args.depth);
      duration = (clock() - start)/(double)CLOCKS_PER_SEC;
      cout << "Pre-process stage done in: " << cleanTime(duration) << endl;
    }else if (args.stage == "live"){
      start = clock();
      Graph g = Graph(args.dataset, true);
      duration = (clock() - start)/(double)CLOCKS_PER_SEC;
      cout << "Import done in: " << cleanTime(duration) << endl;

      start = clock();
      rtim.live(g, g.graph.size());
      duration = (clock() - start)/(double)CLOCKS_PER_SEC;
      cout << "Live stage done in: " << cleanTime(duration) << endl;
    }else if (args.stage == "newStream"){
      Graph g = Graph(args.dataset, false);
      if (args.k == -1){
        rtim.availabilityStream(args.availability, args.version, g.nodes);
      }else{
        rtim.availabilityStream(args.availability, args.version, args.k);
      }
    }else{
      cerr << "Error stage not recognized!" << endl;
      exit(1);
    }
}
