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

void RTIM::pre_process(const Graph& graph){
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
  #pragma omp parallel private(score, num_thread, startTime, save) shared(dataset, infScores, graph, nb_nodes, nb_threads, numNodes)
  {
    nb_threads = omp_get_num_threads();
    num_thread = omp_get_thread_num();
    if (num_thread==0){
      if (nb_nodes == 0){
        nb_nodes = (int*)calloc (sizeof(int),nb_threads*8);
      }
      time ( &startTime );
    }
    #pragma omp for schedule(dynamic)
    for(int i = 0; i < numNodes; i++){
      if (num_thread==0){
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
        std::cout << "] " << int(progress * 100.0) << " % (" << sum << "/" << numNodes << ") -- (" << duration << "s / " << timeLeft << "s)";
        //std::cout.flush();
        
        int testPourcent = (int)(progress*100.0);
        if (testPourcent%10 == 0 && save==0){
          save = 1;
          string fileName = "../../data/" + dataset + "/rtim/infScores_"+ std::to_string(testPourcent) + ".csv";
          saveToCSV(fileName);
        } else if (testPourcent%10 != 0){
          save = 0;
        }
      }
      score = graph.influenceScore({i}, 2);
      infScores[i] = score;
      nb_nodes[num_thread*8]++;
    }
  }
  cout << endl;
  double time = omp_get_wtime() - start;
  /* 
  if (numNodes <= 20){
    printScores();
  }*/
  cout << "Pre_process done in: " << time << " seconds." << endl;
  saveScores();
};

void RTIM::live(const Graph& graph){
  cout << "Running live on " << dataset << endl;
  // run live
  cout << "Live complete!" << endl;
};

void RTIM::printScores(){
  cout << "Influence Scores: " << endl;
  for (int i = 0; i < numNodes ; i++){
    cout << "(" << i << " : " << infScores[i] << ")" << endl;
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
  cout << "Importing influence scores" << endl;
  // import scores
  cout << "Import successful" << endl;
}

int main(int argn, char **argv)
{
    clock_t start;
    double duration;

    Arguments args = Arguments();
    args.getArguments(argn, argv);
    args.printArguments();
    start = clock();
    Graph g = Graph(args.dataset);
    duration = (clock() - start)/(double)CLOCKS_PER_SEC;
    cout << "Import done in: " << duration << " seconds." << endl;
    RTIM rtim = RTIM(args.dataset);

    //start = clock();
    rtim.pre_process(g);
    //duration = (clock() - start)/(double)CLOCKS_PER_SEC; // duration in seconds
    // cout << "Pre_process ran in: " << duration << " seconds." << endl;
    //g.print();
/*
    vector<int> v;
    v.push_back(0);
    double s = g.influenceScore(v);
*/
}
