#include <vector>
#include <cstdio>
#include <ctime>
#include <time.h>
#include <omp.h>
#include <string>
#include <algorithm>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <ctime>
#include <sstream>
#include <stdexcept>

#include "RTIM.h"

/**
  * Special color codes: https://stackoverflow.com/questions/2616906/how-do-i-output-coloured-text-to-a-linux-terminal
  */

using namespace std;

const int SLEEP = 2; // duration of sleep


RTIM::RTIM(Graph& g):graph(g){
  srand(time(NULL));
}


// RTIM::RTIM(Arguments& arguments, bool loadGraph):graph(arguments, loadGraph){
//   args = arguments;
//
//   if (streamSize == -1){
//     streamSize = graph.nodes;
//   }
//   if (maxSeedSize == -1){
//     maxSeedSize = graph.nodes;
//   }
//   nodes = graph.nodes;
//   srand(time(NULL));
// }


void RTIM::importIMMSeed(){
  immTargeted.resize(graph.nodes, 0);
  string path = "../../data/" + graph.dataset + "/imm/basic/" + datasets[graph.dataset] + "_k" + to_string(maxSeedSize) + "_e" + "0,1" + "_ss.txt";
  if (!pathExists(path)){
    cerr << "Error path doesn't exist: " << path << endl;
    exit(1);
  }
  cout << "Importing IMM Seed Set from " + path << endl;
  ifstream infile(path.c_str());
  int user;
  while (infile >> user){
    immTargeted[user] = 1;
  }
  cout << "Done import!" << endl;
  // sleep(3);
  // clearLines(2);
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
  // cout << "Running pre_process on " << graph.dataset << endl;
  double score;
  // nodes = graph.graph.size();

  // infScores.reserve(nodes);
  infScores.resize(graph.nodes, 0);
  nodeTime.resize(graph.nodes, 0);
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
    for(int i = 0; i < graph.nodes; i++){
      if (num_thread == 0){
        save = print_progress(nb_threads, finishedProcess, graph.nodes, startTime, nb_nodes, save);
      }
      // Compute the influence score of a node in G
      clock_t nodeStart = clock();
      score = graph.influenceScore({i}, maxDepth);
      // score = graph.influenceScorePath(i, maxDepth,"shortest", args.edge_weight, minEdgeWeight);
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
        save = print_progress(nb_threads, finishedProcess, graph.nodes, startTime, nb_nodes, save);
      }
      save = print_progress(nb_threads, finishedProcess, graph.nodes, startTime, nb_nodes, save);
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
  graph.importDegrees();
  importIMMSeed();
  vector<double> tmpInfScores;
  for(double score: infScores){
    tmpInfScores.push_back(score);
  }
  clock_t startLive = clock();
  string startDatetime = getLocalDatetime();
  // nodes = graph.graph.size();
  // cout << "-------------------------------" << endl;
  // cout << "Running live on " << graph.dataset << endl;
  seedSet.clear(); // in case live was already run with different params
  initiateProgressLog();
  initiateStreamLog();
  activationProbabilities.clear();
  activationProbabilities.resize(graph.nodes, 0);

  // printScores();
  cout << "getting inf index" << endl;
  getInfIndex(sortedScores);
  cout << "Starting influence score threshold: " << sortedScores[infIndex] << endl;

  // read availability stream
  string path = generateDataFilePath("stream") + generateFileName("stream");
  int user;
  // cout << "Reading availability stream: " << folder << endl;
  if (!pathExists(path)){
    cerr << "Error path doesn't exist: " << path << endl;
    exit(1);
  }
  printInColor("cyan", "Reading stream from : " + path);
  ifstream infile(path.c_str());
  double max_time = 0;
  clock_t start;
  clock_t startStream = clock();
  time_t startTime;
  time ( &startTime );
  double duration;
  double inf_duration;
  int sum = 0;
  int previousProgress = -1;
  bool newProgress = true;
  while (infile >> user){
    sum ++;

    if (immTargeted[user] == 1){
      immSeedSet.insert(user);
    }
    if (graph.dataset == "test"){
      cout << "User: " << user << " is online: old_ap = " << activationProbabilities[user] << ", score = " << tmpInfScores[user] << endl;
    }
    if (activationProbabilities[user] < theta_ap){ // CHECK ACTIVATION PROBABILITY
      // ADD FUNCTION TO PROPERLY UPDATE INFLUENCE SCORE, RECORD TIME
      // check neighbors
      double tot = 0;
      start = clock();
      for(pair<int, double> neighbor: graph.graph[user]){
        // if(activationProbabilities[neighbor.first] == 1){
        //   tot ++;
        // }
        tot += activationProbabilities[neighbor.first];
      }
      inf_duration = (clock() - start)/(double)CLOCKS_PER_SEC;
      double old_score = tmpInfScores[user];
      if (tot == graph.graph[user].size()){
        // cout << "All neighbors are activated: " << user << " > ap= " << activationProbabilities[user] << "| old_score= " << old_score << endl;
        tmpInfScores[user] = 1;
        if (old_score > sortedScores[infIndex] && tmpInfScores[user] < sortedScores[infIndex]){
          infIndex --;
        }
      }else if (tot > 0){
        tmpInfScores[user] = tmpInfScores[user] - tot;
        // cout << "New score for : " << user << " > old_score = " << old_score << " > new_score = " << tmpInfScores[user] << endl;
        if (old_score > sortedScores[infIndex] && tmpInfScores[user] < sortedScores[infIndex]){
          infIndex --;
        }
      }

      if(tmpInfScores[user] >= sortedScores[infIndex]){ // CHECK INFLUENCE SCORE
        seedSet.push_back(user); // add user to seed set
        saveProgress(user,activationProbabilities[user], tmpInfScores[user], sum, sortedScores[infIndex], seedSet.size());
        double tmpAP = activationProbabilities[user];
        activationProbabilities[user] = 1.0;

        // measure update time
        start = clock();
        //graph.updateNeighborsAP(user, activationProbabilities, {}, 1.0, 1);
        //graph.updateNeighborsAPShort(user, activationProbabilities);
        graph.updateNeighborsAPDepth(user, activationProbabilities, 1);
        duration = (clock() - start)/(double)CLOCKS_PER_SEC;
        if (duration > max_time){
          max_time = duration;
        }
        // RECORD TARGETED USER
        saveStreamLog(sum, user, tmpAP, duration, old_score, tmpInfScores[user], inf_duration, sortedScores[infIndex], "targeted", seedSet.size(), immTargeted[user], graph.inDegrees[user], graph.outDegrees[user]);
        infIndex --;
        if (graph.dataset == "test" || maxSeedSize < 20){
          cout << "Targeted user: " << user << ": old_ap = " << tmpAP << ", score = " << infScores[user] << endl;
        }
        if (seedSet.size() == 1){
        	cout << "First user targeted: " << user << ": pos = " << sum << ", old_ap = " << tmpAP << ", score = " << tmpInfScores[user] << endl;
        }else if (seedSet.size() == maxSeedSize){
        	cout << "Last user targeted: " << user << ": pos = " << sum << ", old_ap = " << tmpAP << ", score = " << tmpInfScores[user] << endl;
        }
      }else{
        //cout <<   "User not targeted : " << user << ": pos = " << sum << ", old_ap = " << activationProbabilities[user] << ", score = " << infScores[user] << endl;
        // RECORD USER IGNORED BECAUSE SCORE TOO LOW
        saveStreamLog(sum, user, activationProbabilities[user], -1, old_score, tmpInfScores[user], inf_duration, sortedScores[infIndex], "ig. (sig. too low)", -1, immTargeted[user], graph.inDegrees[user], graph.outDegrees[user]);
      }
      if (seedSet.size() >= maxSeedSize && immSeedSet.size() >= maxSeedSize){
        break;
      }
    }else{
      if (activationProbabilities[user] == 1){
        // RECORD USER IGNORED BECAUSE ALREADY TARGETED
        saveStreamLog(sum, user, activationProbabilities[user], -1, tmpInfScores[user], -1, -1, sortedScores[infIndex], "ig. (targeted)", -1, immTargeted[user], graph.inDegrees[user], graph.outDegrees[user]);
      }else{
        // RECORD USER IGNORED BECAUSE AP TOO HIGH
        saveStreamLog(sum, user, activationProbabilities[user], -1, tmpInfScores[user], -1, -1, sortedScores[infIndex], "ig. (ap too high)", -1, immTargeted[user], graph.inDegrees[user], graph.outDegrees[user]);
      }
    }
    if (immTargeted[user] == 1){
      immTargeted[user] = 2;
    }
  }
  cout << endl;
  clearLines(1);
  double streamDuration = (clock() - startStream)/(double)CLOCKS_PER_SEC;
  cout << "Availability stream read in:  " << cleanTime(streamDuration, "s") << endl;
  saveSeedSet();

  string endDatetime = getLocalDatetime();
  double liveDuration = (clock() - startLive)/(double)CLOCKS_PER_SEC;
  saveLiveLog(max_time, liveDuration, startDatetime, endDatetime);
  saveLiveCSV(graph, streamDuration, max_time, liveDuration);

  // COMPUTE SEED SET INFLUENCE SCORE ESTIMATION BY SUMMING ACTIVATION PROBS
  double estScore = 0;
  for(double ap: activationProbabilities){
    estScore += ap;
  }
  printInColor("red", "\u03C3(seed) = " + properStringDouble(estScore));

  // COMPUTE SEED SET INFLUENCE SCORE WITH MC SIMULATIONS
  printInColor("magenta", string(60, '-'));
  printLocalTime("magenta", "Compute RTIM seed score", "starting");
  string seedFile = generateDataFilePath("rtim_seedSet") + generateFileName("rtim_seedSet", seedSet.size());
  double score;
  string scoreStartDate = getLocalDatetime();
  clock_t scoreStartTime = clock();
  if (graph.dataset == "twitter"){
    score = graph.influenceScoreParallel(seedSet, 2);
  }else{
    score = graph.influenceScoreParallel(seedSet);
  }

  double scoreDuration = (clock() - startTime)/(double)CLOCKS_PER_SEC;
  string scoreEndDate = getLocalDatetime();
  printInColor("red", "RTIM: \u03C3_MC(seed) = " + properStringDouble(score));
  saveSeedScoreLog(seedFile, scoreStartDate, scoreEndDate, scoreDuration, score);
  saveSeedScoreCSV(seedFile, scoreStartDate, scoreEndDate, scoreDuration, score);
  // printLocalTime("magenta", "IMM seed test", "ending");
  printLocalTime("magenta", "Compute RTIM seed score", "ending");
  printInColor("magenta", string(60, '-'));

  printInColor("red", "IMM seed set size: " + to_string(immSeedSet.size()));
  printInColor("magenta", string(60, '-'));
  printLocalTime("magenta", "Compute RTIM seed score", "starting");
  vector<int> vecImmSeedSet;
  for(int seed: immSeedSet){
    vecImmSeedSet.push_back(seed);
  }
  if (graph.dataset == "twitter"){
    score = graph.influenceScoreParallel(vecImmSeedSet, 2);
  }else{
    score = graph.influenceScoreParallel(vecImmSeedSet);
  }
  printInColor("red", "IMM: \u03C3_MC(seed) = " + properStringDouble(score));
  printLocalTime("magenta", "Compute RTIM seed score", "ending");
  printInColor("magenta", string(60, '-'));
};


void RTIM::initializeInfluenceScores(){
  // ideally we should not repeat this if live is run more than once
  importScores();
  sortedScores = infScores;
  sort(sortedScores.begin(), sortedScores.end());
}


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
  string file = "../../data/" + graph.dataset + "/rtim/pre_process/";
  if (!pathExists(file)){
    cerr << "Error path doesn't exist: " << file << endl;
    exit(1);
  }
  file += generateFileName("save_infScores");
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
  // string file = "../../data/" + graph.dataset + "/rtim/live/";
  string file;
  if(progress){
    file = generateDataFilePath("rtim_progress_seedSet");
    if (!pathExists(file)){
      cerr << "Error path doesn't exist: " << file << endl;
      exit(1);
    }
    file += generateFileName("rtim_progress_seedSet", progPercentage);
    printInColor("cyan", "Saving progress seed set to: " + file);
  }else{
    file = generateDataFilePath("rtim_seedSet");
    if (!pathExists(file)){
      cerr << "Error path doesn't exist: " << file << endl;
      exit(1);
    }
    file += generateFileName("rtim_seedSet", seedSet.size());
    printInColor("cyan", "Saving seed set to  : " + file);
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
  // string file = "../../data/" + graph.dataset + "/" + file_path;
  cout << "Importing from: " << file_path << endl;
  if (!pathExists(file_path)){
    cerr << "Error path doesn't exist: " << file_path << endl;
    exit(1);
  }
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
    score = graph.influenceScoreParallel(seedSet);
  }else{
    cout << "Computing influence score of seed set with depth of 3." << endl;
    score = graph.influenceScoreParallel(seedSet, 3);
  }
  cout << "Seed score is: " << score << endl;
}


void RTIM::saveLiveLog(double& maxTime, double& runtime, string startDatetime, string endDatetime){
  // string file = "../../data/" + graph.dataset + "/streams/" + streamModel + "/" + streamModel + "_m" + to_string(streamVersion) + "/" + graph.dataset + "_liveLog.txt";
  string path = "../../data/" + graph.dataset + "/logs/";
  if (!pathExists(path)){
    cerr << "Error path doesn't exist: " << path << endl;
    exit(1);
  }
  path += "rtim_live.log";
  //cout << "\033[33mSaving live log to: " << file << "\033[0m" << endl;
  printInColor("cyan", "Saving live log to  : " + path);
  ofstream liveLogFile;
  liveLogFile.open(path, fstream::app);
  liveLogFile << "File name       : " << generateFileName("rtim_seedSet", seedSet.size()) << endl;
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
  string path = "../../data/" + graph.dataset + "/logs/";
  if (!pathExists(path)){
    cerr << "Error path doesn't exist: " << path << endl;
    exit(1);
  }
  path += "rtim_live.csv";
  // cout << "\033[33mSaving live csv to: " << file << "\033[0m" << endl;
  printInColor("cyan", "Saving live csv to  : " + path);
  ofstream liveCSV;
  liveCSV.open(path, fstream::app);
  /*Order of values: dataset, streamModel, streamVersion, reahc, theta_ap, depth, maxSeedSize, actualSeedSize, maxUpdateTime, runtime*/
  liveCSV << graph.dataset << ",";
  liveCSV << streamModel << ",";
  liveCSV << streamVersion << ",";
  liveCSV << streamSize << ",";
  liveCSV << reach << ",";
  liveCSV << theta_ap << ",";
  liveCSV << maxDepth << ",";
  liveCSV << maxSeedSize << ",";
  liveCSV << seedSet.size() << ",";
  liveCSV << maxTime << ",";
  liveCSV << runtime << endl;
  liveCSV.close();
  // cout << "\033[33mLive CSV saved!\033[0m" << endl;
  // printInColor("cyan", "Live CSV saved!");
}


void RTIM::saveSeedScoreLog(string file, string startDate, string endDate, double& runtime, double& score){
  string path = "../../data/" + graph.dataset + "/logs/";
  if (!pathExists(path)){
    cerr << "Error path doesn't exist: " << path << endl;
    exit(1);
  }
  path += "seed_set_score.log";
  printInColor("cyan", "Saving seed score log to: " + path);
  ofstream seedScoreLogFile;
  seedScoreLogFile.open(path, fstream::app);
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
  string path = "../../data/" + graph.dataset + "/logs/";
  if (!pathExists(path)){
    cerr << "Error path doesn't exist: " << path << endl;
    exit(1);
  }
  path += "seed_set_score.csv";
  printInColor("cyan", "Saving seed score csv to: " + path);

  // dataset,file_name, startDate, endDate, runtime, seed size, score
  ofstream seedScoreCSVFile;
  seedScoreCSVFile.open(path, fstream::app);
  seedScoreCSVFile << graph.dataset << ",";
  seedScoreCSVFile << file << ",";
  seedScoreCSVFile << startDate << ",";
  seedScoreCSVFile << endDate << ",";
  seedScoreCSVFile << runtime << ",";
  seedScoreCSVFile << seedSet.size() << ",";
  seedScoreCSVFile << score << endl;
  seedScoreCSVFile.close();
}


void RTIM::initiateProgressLog(){
  string path = generateDataFilePath("rtim_progress");
  if (!pathExists(path)){
    cerr << "Error path doesn't exist: " << path << endl;
    exit(1);
  }
  path += generateFileName("rtim_progress");
  printInColor("cyan", "New progress log    : " + path);
  ofstream progressFile;
  progressFile.open(path);
  progressFile << "seen,influence_threshold,user_index,ap, infScore, seed_size" << endl;
  progressFile.close();
}


void RTIM::saveProgress(int user_index, double ap, double score, int seen, double infTheta, int seedSize){
  string path = generateDataFilePath("rtim_progress");
  if (!pathExists(path)){
    cerr << "Error path doesn't exist: " << path << endl;
    exit(1);
  }
  path += generateFileName("rtim_progress");
  // printInColor("cyan","Saving progress: " + to_string(progress));
  ofstream progressFile;
  progressFile.open(path, fstream::app);
  /* progress | nodes seen | seed size */
  progressFile << seen << ",";
  progressFile << infTheta << ",";
  progressFile << user_index << ",";
  progressFile << ap << ",";
  progressFile << score << ",";
  progressFile << seedSize;
  progressFile << endl;
  progressFile.close();
}


void RTIM::initiateStreamLog(){
  string path = generateDataFilePath("stream_log");
  if (!pathExists(path)){
    cerr << "Error path doesn't exist: " << path << " at line 611"<< endl;
    exit(1);
  }
  path += generateFileName("stream_log");
  printInColor("cyan", "New stream log    :" + path);
  ofstream streamLog;
  streamLog.open(path);
  streamLog << "Dataset: " << graph.dataset << endl;
  streamLog << "STREAM: model = " << streamModel << "|  size = " << streamSize << endl;
  streamLog << "RTIM: k = " << maxSeedSize << " | theta_ap = " << theta_ap << " | reach = " << reach << endl;
  streamLog << "------------------------------------------------------------" << endl;
  streamLog << left << setw(8 + 1) << "position";
  streamLog << left << setw(8 + 1) << "user_id";
  streamLog << left << setw(10 + 1) << "ap";
  streamLog << left << setw(10 + 1) << "ap_time(s)";
  streamLog << left << setw(10 + 1) << "inf_old";
  streamLog << left << setw(10 + 1) << "inf_new";
  streamLog << left << setw(11 + 1) << "inf_time(s)";
  streamLog << left << setw(8 + 1) << "theta_I";
  streamLog << left << setw(18 + 1) << "rtim_status";
  streamLog << left << setw(9 + 1) << "seed_size";
  streamLog << left << setw(14 + 1) << "imm_status";
  streamLog << left << setw(9 + 1) << "in_degree";
  streamLog << left << setw(9 + 1) << "out_degree";
  streamLog << endl;
}


void RTIM::saveStreamLog(int pos, int user, double ap, double ap_time, double oScore, double nScore, double inf_time, double theta_I, string rtim_status, int seedSize, int imm_targeted, int inDeg, int outDeg){
  string path = generateDataFilePath("stream_log");
  if (!pathExists(path)){
    cerr << "Error path doesn't exist: " << path << " at line 642 " << endl;
    exit(1);
  }
  path += generateFileName("stream_log");
  ofstream streamLog;
  streamLog.open(path, fstream::app);
  streamLog << setprecision(5);
  streamLog << left << setw(8 + 1) << pos;
  streamLog << left << setw(8 + 1) << user;
  streamLog << left << setw(10 + 1) << ap;
  if (ap_time == -1){
    streamLog << left << setw(10 + 1) << "-";
  }else{
    streamLog << left << setw(10 + 1) << ap_time;
  }
  streamLog << left << setw(10 + 1) << oScore;
  if (nScore == -1 || nScore == oScore){
    streamLog << left << setw(10 + 1) << "-";
  }else{
    streamLog << left << setw(10 + 1) << nScore;
  }
  if (inf_time == -1){
    streamLog << left << setw(11 + 1) << "-";
  }else{
    streamLog << left << setw(11 + 1) << inf_time;
  }

  streamLog << left << setw(8 + 1) << theta_I;
  streamLog << left << setw(18 + 1) << rtim_status;
  if ( seedSize == -1){
    streamLog << left << setw(9 + 1) << "-";
  }else{
    streamLog << left << setw(9 + 1) << seedSize;
  }
  if (imm_targeted == 1){
    streamLog << left << setw(14 + 1) << "targeted";
  }else if(imm_targeted == 2){
    streamLog << left << setw(14 + 1) << "ig. (targeted)";
  }else{
    streamLog << left << setw(14 + 1) << "-";
  }
  streamLog << left << setw(9 + 1) << inDeg;
  streamLog << left << setw(9 + 1) << outDeg;
  streamLog << endl;
  streamLog.close();
}


void RTIM::importScores(){
  string path = generateDataFilePath("get_infScores") + generateFileName("get_infScores");
  if (!pathExists(path)){
    cerr << "Error path doesn't exist: " << path << endl;
    exit(1);
  }
  // cout << "Importing influence scores from: " << folder << endl;
  printInColor("cyan", "Importing influence scores from: " + path);
  infScores.resize(graph.nodes, 0);
  int user;
  double infScore;
  double scoreTime;

  ifstream infile(path.c_str());
  while(infile >> user >> infScore >> scoreTime){
    infScores[user] = infScore;
  }
  // cout << "Import successful" << endl;
  printInColor("cyan", "Import successful");
  printScores();
}


void RTIM::getInfIndex(vector<double> & sorted){
  infIndex = (int)(sorted.size() - sorted.size() * reach / 100);
}


void RTIM::seedComputationTest(int seedSize, int depth, double minEdgeWeight){
  // select random nodes from graph to generate seed
  cout << "------------------------------------------------------------------------------------------" << endl;
  cout << "Starting seed computation test on " << graph.dataset << " > seed size: " << seedSize << " | depth: " << depth << "| minEdgeWeight: " << minEdgeWeight  <<  endl;
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
  score = graph.influenceScoreParallel(seedSet, maxDepth, minEdgeWeight);
  double delta = omp_get_wtime() - start;
  cout << "Seed set score is: " << score << " / " << graph.nodes << " computed in: " << cleanTime(delta, "s") << endl;
  cout << "------------------------------------------------------------------------------------------" << endl;
}


void RTIM::printStageParams(){
  // stages = pre-process, live, compute_seed_score
  if( stage == "live"){
    cout << string(27, '-') << toColor("red", " live ") << string(27, '-') << endl;
    cout << "STREAM" << endl;
    cout << "- model        : "; printInColor("yellow", streamModel);
    if (streamModel != "inNOut_repeat"){
      cout << "- version      : "; printInColor("yellow", to_string(streamVersion));
    }
    cout << "- size         : "; printInColor("yellow", to_string(streamSize));
    cout << "RTIM" << endl;
    cout << "- k            : "; printInColor("yellow", to_string(maxSeedSize));
    cout << "- reach        : "; printInColor("yellow", properStringDouble(reach));
    cout << "- ap           : "; printInColor("yellow", properStringDouble(theta_ap));
  }else if ( stage == "pre-process scores"){
    cout << string(24, '-') << toColor("red", " pre-process ") << string(23, '-') << endl;
    cout << "RTIM" << endl;
    cout << "- depth      : "; printInColor("yellow", to_string(maxDepth));
    cout << "- min weight : "; printInColor("yellow", properStringDouble(minEdgeWeight));
  }else if (stage == "compute_seed_score"){
    cout << string(20, '-') + " " << toColor("red", stage) << " " + string(20, '-') << endl;
    cout << "- file path    : "; printInColor("yellow", seedSetPath);
  }
  cout << string(60, '-') << endl;
}


int RTIM::stagesMenu(){
  int result = 0;
  while (result == 0){
    int choice = -1;
    cout << string(26,'_') + " Stages " + string(26,'_') << endl;
    cout << "Choose a stage: " << endl;
    cout << "   [1] Pre-process scores"<< endl;
    cout << "   [2] Pre-process probabilities" << endl;
    cout << "   [3] Live" << endl;
    cout << "   [4] Test " << endl;
    cout << "   [5] EXIT PROGRAM " << endl;
    while(choice == -1){
      cout <<  "> choice: ";
      string val;
      getline(cin, val);
      choice = stoi(val);
      switch(choice){
        case 1:
          // pre-process scores menu
          clearLines(8);
          stage = "pre-process scores";
          result = preProcessMenu();
          break;
        case 2:
          // pre-process probabilities menu
          clearLines(8);
          stage = "pre-process APs";
          break;
        case 3:
          // live menu
          clearLines(8);
          stage = "live";
          result = liveMenu();
          break;
        case 4:
          // test menu
          clearLines(8);
          // result = testMenu();
          break;
        case 5:
          // EXIT
          clearLines(8);
          return 3;
        default:
          cout << "Error: choice not recognized!" << endl;
          sleep(SLEEP);
          clearLines(2);
          choice = -1;
      }
    }
    //clearLines(8);
  }
  return result - 1;
}


void RTIM::stageArgumentsMenu(){
  if (stage == "pre"){
    preProcessMenu();
  }else if (stage == "live"){
    liveMenu();
  }else if (stage == "compute_seed_score"){
    computeSeedScoreMenu();
  }else if (stage != "test"){
    cout << "Error: Stage not recognized!" << endl;
    exit(1);
  }
}


int RTIM::preProcessMenu(){
  int result = 0;
  while (result == 0 || result == -1){
    if(result == 0){ // ask for new arguments
      int iChoice;
      double dChoice;
      string input;
      cout << string(60,'_') << endl;
      cout << "Input pre_process arguments" << endl;
      // asking for max search depth
      while(1){
        cout << "> depth (" << maxDepth << "): ";
        getline(cin, input);
        if(input != ""){
          try{
            iChoice = stoi(input);

            if (iChoice >= 1 && iChoice <= 10000){
              maxDepth = iChoice;
              clearLines(1);
              cout << "> depth (" << maxDepth << "): ";
              printInColor("yellow", to_string(maxDepth));
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
          cout << "> depth (" << maxDepth << "): ";
          printInColor("yellow", to_string(maxDepth));
          break;
        }
      }
      // asking for minimum path weight
      while(1){
        cout << "> minimum path weight(" << minEdgeWeight << "): ";
        getline(cin, input);
        if(input != ""){
          try{
            dChoice = stod(input);
            if (dChoice >= 0 && dChoice <= 1.0){
              minEdgeWeight = dChoice;
              clearLines(1);
              cout << "> minimum path weight(" << minEdgeWeight << "): ";
              printInColor("yellow", properStringDouble(minEdgeWeight));
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
          cout << "> minimum path weight(" << minEdgeWeight << "): ";
          printInColor("yellow", properStringDouble(minEdgeWeight));
          break;
        }
      }
      sleep(SLEEP);
      clearLines(4);
    }
    printStageParams();
    if(!graph.loaded){
      graph.loadGraph();
      graph.loaded = true;
    }
    pre_process();
    result = continueMenu();
  }
  return result - 1;
}


// TEST PRE-PROCESS MENU
void RTIM::testPreProcessScoresMenu(){
  int iChoice;
  double dChoice;
  string input;
  cout << string(60,'_') << endl;
  cout << "Input pre_process arguments" << endl;
  // asking for max search depth
  while(1){
    cout << "> depth (" << maxDepth << "): ";
    getline(cin, input);
    if(input != ""){
      try{
        iChoice = stoi(input);

        if (iChoice >= 1 && iChoice <= 10000){
          maxDepth = iChoice;
          clearLines(1);
          cout << "> depth (" << maxDepth << "): ";
          printInColor("yellow", to_string(maxDepth));
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
      cout << "> depth (" << maxDepth << "): ";
      printInColor("yellow", to_string(maxDepth));
      break;
    }
  }
  // asking for minimum path weight
  while(1){
    cout << "> minimum path weight(" << minEdgeWeight << "): ";
    getline(cin, input);
    if(input != ""){
      try{
        dChoice = stod(input);
        if (dChoice >= 0 && dChoice <= 1.0){
          minEdgeWeight = dChoice;
          clearLines(1);
          cout << "> minimum path weight(" << minEdgeWeight << "): ";
          printInColor("yellow", properStringDouble(minEdgeWeight));
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
      cout << "> minimum path weight(" << minEdgeWeight << "): ";
      printInColor("yellow", properStringDouble(minEdgeWeight));
      break;
    }
  }
  sleep(SLEEP);
  clearLines(4);
}


int RTIM::liveMenu(){
  int result = 0;
  while (result == -1 || result == 0){
    if(result == 0){ // ask for new arguments
      int iChoice;
      double dChoice;
      string input;
      cout << string(60,'_') << endl;
      cout << "Input live arguments: [seed size | reach | activation probability threshold | stream model | stream version | stream size]" << endl;
      // asking for seed size
      while(1){
        cout << "> seed size (" << maxSeedSize << "): ";
        getline(cin, input);
        if(input != ""){
          try{
            iChoice = stoi(input);
            if(iChoice > graph.nodes){
              cout << "Error: seed size larger than current graph: " << iChoice << " / " << graph.nodes << endl;
              sleep(SLEEP + 2);
              clearLines(2);
            }else{
              maxSeedSize = iChoice;
              clearLines(1);
              cout << "> seed size (" << maxSeedSize << "): ";
              printInColor("yellow", to_string(maxSeedSize));
              break;
            }
          }catch(invalid_argument& e){
            cout << "Error: invalid input!" << endl;
            sleep(SLEEP);
            clearLines(2);
          }
        }else{
          if(maxSeedSize > graph.nodes){
            cout << "Error: seed size larger than current graph: " << maxSeedSize << " / " << graph.nodes << endl;
            sleep(SLEEP + 2);
            clearLines(2);
          }else{
            clearLines(1);
            cout << "> seed size (" << maxSeedSize << "): ";
            printInColor("yellow", to_string(maxSeedSize));
            break;
          }
        }
      }
      // asking for reach
      while(1){
        cout << "> reach (" << properStringDouble(reach) << "): ";
        getline(cin, input);
        if(input != ""){
          try{
            dChoice = stof(input);
            if (dChoice > 0 && dChoice < 100){
              reach = dChoice;
              clearLines(1);
              cout << "> reach (" << properStringDouble(reach) << "): ";
              printInColor("yellow", properStringDouble(reach));
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
          cout << "> reach (" << properStringDouble(reach) << "): ";
          printInColor("yellow", properStringDouble(reach));
          break;
        }
      }
      // asking for activation probability
      while(1){
        cout << "> activation probability threshold (" << properStringDouble(theta_ap) << "): ";
        getline(cin, input);
        if(input != ""){
          dChoice = stod(input);
          if(dChoice >= 0.0 && dChoice <= 1.0){
            theta_ap = dChoice;
            clearLines(1);
            cout << "> activation probability threshold (" << properStringDouble(theta_ap) << "): ";
            printInColor("yellow", properStringDouble(theta_ap));
            break;
          }else{
            cout << "Error: Input must be a probability!" << endl;
            sleep(SLEEP);
            clearLines(2);
          }
        }else{
          clearLines(1);
          cout << "> activation probability threshold (" << properStringDouble(theta_ap) << "): ";
          printInColor("yellow", properStringDouble(theta_ap));
          break;
        }
      }
      // asking for stream model
      while(1){
        cout << "> stream model [uniform_rand_repeat, uniform_rand_no_repeat, inNOut_repeat](" << streamModel << "): ";
        getline(cin, input);
        if(input != ""){
          if(input == "uniform_rand_repeat" || input == "uniform_rand_no_repeat" || input == "inNOut_repeat"){
            streamModel = input;
            clearLines(1);
            cout << "> stream model [uniform_rand_repeat, uniform_rand_no_repeat, inNOut_repeat](" << streamModel << "): ";
            printInColor("yellow", streamModel);
            break;
          }else{
            cout << "Error: Input must be a valid stream model!" << endl;
            sleep(SLEEP);
            clearLines(2);
          }
        }else{
          clearLines(1);
          cout << "> stream model [uniform_rand_repeat, uniform_rand_no_repeat, inNOut_repeat](" << streamModel << "): ";
          printInColor("yellow", streamModel);
          break;
        }
      }
      // asking for stream version
      while(1){
        cout << "> stream version [1, 2, 3] (" << streamVersion << "): ";
        getline(cin, input);
        if(input != ""){
          try{
            iChoice = stoi(input);
            if(iChoice == 1 || iChoice == 2 || iChoice == 3){
              streamVersion = iChoice;
              clearLines(1);
              cout << "> stream version [1, 2, 3]: ";
              printInColor("yellow", to_string(streamVersion));
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
          cout << "> stream version [1, 2, 3]: ";
          printInColor("yellow", to_string(streamVersion));
          break;
        }
      }
      // asking for stream size
      while(1){
        if(streamSize == -1){
          streamSize = graph.nodes / 10;
        }
        cout << "> stream size (" << streamSize << "): ";
        getline(cin, input);
        if(input != ""){
          try{
            iChoice = stoi(input);
            streamSize = iChoice;
            clearLines(1);
            cout << "> stream size (" << streamSize << "): ";
            printInColor("yellow", to_string(streamSize));
            break;
          }catch(invalid_argument& e){
            cout << "Error: invalid input!" << endl;
            sleep(SLEEP);
            clearLines(2);
          }
        }else{
          // streamSize = nodes;
          clearLines(1);
          cout << "> stream size (" << streamSize << "): ";
          printInColor("yellow", to_string(streamSize));
          break;
        }
      }
      sleep(SLEEP+1);
      clearLines(8);
    }
    printStageParams();
    if(!graph.loaded){
      graph.loadGraph();
      graph.loaded = true;
    }
    // cout << graph.graph.size() << endl;
    live();
    result = continueMenu();
  }
  return result - 1;
}


void RTIM::computeSeedScoreMenu(){
  // seedSet.clear(); //in case it's a re-run
  int choice = -1;
  string file_path = "../../data/" + graph.dataset + "/";
  cout << string(60,'_') << endl;
  cout << "Choose a folder: " << endl;
  cout << "   [1] rtim/live/" << endl;
  cout << "   [2] imm/basic/" << endl;
  cout << "   [3] imm/" << endl;
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
        file_path += "imm/";
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
  seedSetPath = file_path;
  importSeedSet(file_path);
  sleep(SLEEP);
  clearLines(5);
  seedFile = input;
}


int RTIM::continueMenu(){
  int choice = -1;
  cout << string(60, '_') << endl;
  cout << "Continue: " << endl;
  cout << "   [1] Repeat previous stage with same arguments (" << stage << ")" << endl;
  cout << "   [2] Repeat previous stage with new arguments (" << stage << ")" << endl;
  cout << "   [3] Change stage" << endl;
  cout << "   [4] Change algorithm" << endl;
  cout << "   [5] Change dataset" << endl;
  cout << "   [6] Main Menu" << endl;
  cout << "   [7] End Program" << endl;
  while(choice == -1){
    cout << "> choice: ";
    string val;
    getline(cin, val);
    choice = stoi(val);
    switch(choice){
      case 1:
        // repeat stage with same arguments
        clearLines(10);
        return -1;
      case 2:
        // repeat stage with new arguments
        clearLines(10);
        return 0;
      case 3:
        // choose stage
        clearLines(10);
        return 1;
      case 4:
        // change algorithm
        clearLines(10);
        return 2;
      case 5:
        // change dataset
        clearLines(10);
        return 3;
      case 6:
        // go to main menu
        clearLines(10);
        return 4;
      case 7:
        // exit program
        clearLines(10);
        return 5;
      default:
        cout << "Error: choice not recognized!" << endl;
        choice = -1;
        clearLines(2);
    }
  }
  // clearLines(10);
}


int RTIM::run(){
  return stagesMenu();
//   cout << endl;
//   printLocalTime("red", "Program", "starting");
//   int choice = 0;
//   int loadDataset;
//   int loadScores;
//   while (choice != -1){
//     loadScores = 0;
//     loadDataset = 0;
//     switch(choice){
//       case 1:
//         // repeat previous stage with same arguments
//         break;
//       case 2:
//         // repeat previous stage with new arguments
//         stageArgumentsMenu();
//         printStageParams();
//         break;
//       case 3:
//         // change stage
//         stageMenu();
//         stageArgumentsMenu();
//         printStageParams();
//         break;
//       case 4:
//         // change dataset
//         loadDataset = datasetMenu();
//         loadScores = 1; // inf. scores need to be imported for new dataset
//         // printDatasetArguments(graph.nodes, graph.edges);
//         stageMenu();
//         stageArgumentsMenu();
//         printStageParams();
//         break;
//       default:
//         // choose dataset
//         loadDataset = datasetMenu();
//         loadScores = 1; // inf. scores need to be imported for new dataset
//         // args.printDatasetArguments(graph.nodes, graph.edges);
//         stageMenu();
//         stageArgumentsMenu();
//         printStageParams();
//     }
//     if (stage == "test"){
//       loadDataset = false; // ?
//     }
//     if(loadDataset){
//       // graph.args = args;
//       graph.graph.resize(graph.nodes);
//       graph.loadGraph();
//       clearLines(3);
//     }
//     if (stage == "pre"){
//       printInColor("magenta", string(60, '-'));
//       printLocalTime("magenta", "Pre_processing", "starting");
//       pre_process();
//       printLocalTime("magenta", "Pre_processing", "ending");
//       // Live starting at: Mon Nov 19 17:49:25 2018
//       // ---------------------------------------------
//       printInColor("magenta", string(60, '-'));
//     }else if (stage == "live"){
//       if(loadScores){
//         initializeInfluenceScores();
//       }
//       printInColor("magenta", string(60, '-'));
//       printLocalTime("magenta", "Live", "starting");
//       live();
//       printLocalTime("magenta", "Live", "ending");
//       printInColor("magenta", string(60, '-'));
//     }else if (stage == "compute_seed_score"){
//       printInColor("magenta", string(60, '-'));
//       printLocalTime("magenta", "Compute seed score", "starting");
//       double score;
//       string startDate = getLocalDatetime();
//       clock_t startTime = clock();
//       if (graph.dataset == "twitter"){
//         score = graph.influenceScoreParallel(seedSet, 2);
//       }else{
//         score = graph.influenceScoreParallel(seedSet);
//       }
//
//       double duration = (clock() - startTime)/(double)CLOCKS_PER_SEC;
//       string endDate = getLocalDatetime();
//       string txt = "Influence score of seed set is : " + to_string(score);
//       printInColor("cyan", txt);
//       saveSeedScoreLog(seedFile, startDate, endDate, duration, score);
//       saveSeedScoreCSV(seedFile, startDate, endDate, duration, score);
//       // printLocalTime("magenta", "IMM seed test", "ending");
//       printLocalTime("magenta", "Compute seed score", "ending");
//       printInColor("magenta", string(60, '-'));
//     }else if (stage == "test"){
//       printInColor("magenta", string(60, '-'));
//       printLocalTime("magenta", "Test", "starting");
//       runTest();
//       printLocalTime("magenta", "Test", "ending");
//       printInColor("magenta", string(60, '-'));
//     } else{
//       cout << "Error! stage not recognized: " << stage << endl;
//       exit(1);
//     }
//     choice = continueMenu();
//   }
//
//   printLocalTime("red", "Program", "ending");
//   cout << endl;
}


string RTIM::generateDataFilePath(string type){
  string file_path = "../../data/" + graph.dataset + "/";
  if (type == "save_infScores" || type == "get_infScores"){
    file_path += "rtim/pre_process/";
  }else if (type == "rtim_seedSet"){
    file_path += "rtim/live/" + keyword[streamModel] + "/";
  }else if (type == "rtim_progress_seedSet"){
    file_path += "rtim/live/" + keyword[streamModel] + "/";
  }else if (type == "stream"){
    file_path += "streams/" + streamModel + "/v" + to_string(streamVersion) + "/";
  }else if (type == "rtim_progress"){
    file_path += "rtim/live/progress/" + keyword[streamModel] + "/";
  }else if (type == "stream_log"){
    file_path += "streams/" + streamModel + "/v" + to_string(streamVersion) + "/log/";
  }else{
    cout << "Type not recognized!" << endl;
  }
  return file_path;
}


string RTIM::generateFileName(string type, int param){
  string file_name = "";
  if (type == "save_infScores"){
    file_name = datasets[graph.dataset] + "_d" + properStringDouble(maxDepth) + "_m" + properStringDouble(minEdgeWeight) + "_infS.txt";
  }else if (type == "get_infScores"){
    file_name = datasets[graph.dataset] + "_infS.txt";
  }else if (type == "rtim_seedSet"){
    file_name = datasets[graph.dataset] + "_k" + to_string(maxSeedSize) + "_r" + properStringDouble(reach) + "_ap" + properStringDouble(theta_ap) + "_" + keyword[streamModel] + "_v" + to_string(streamVersion) + "_s" + to_string(streamSize) + "_ss" + to_string(param) + ".txt";
  }else if (type == "rtim_progress_seedSet"){
    file_name = datasets[graph.dataset] + "_" + keyword[streamModel] + "_v" + to_string(streamVersion) + "_s" + to_string(streamSize) + "_r" + properStringDouble(reach) + "_ap" + properStringDouble(theta_ap) + "_prg" + to_string(param) + "_ss.txt";
  }else if (type == "stream"){
    file_name = datasets[graph.dataset] + "_" + keyword[streamModel] + "_v" + to_string(streamVersion) + "_s" + to_string(streamSize) + "_st.txt";
  }else if (type == "rtim_progress"){
    file_name = datasets[graph.dataset] + "_k" + to_string(maxSeedSize) + "_r" + properStringDouble(reach) + "_ap" + properStringDouble(theta_ap) + "_" + keyword[streamModel] + "_v" + to_string(streamVersion) + "_s" + to_string(streamSize) + "_prg.csv";
  }else if (type == "stream_log"){
    file_name = datasets[graph.dataset] + "_k" + to_string(maxSeedSize) + "_r" + properStringDouble(reach) + "_ap" + properStringDouble(theta_ap) + "_" + keyword[streamModel] + "_v" + to_string(streamVersion) + "_s" + to_string(streamSize) + "_prg.log";
  }
  return file_name;
}

// int main(int argn, char **argv)
// {
//   // int cores = omp_get_max_threads();
//   // Arguments args = Arguments();
//   // minEdgeWeight.loadDatasetsData();
//   Graph graph = Graph();
//   RTIM rtim = RTIM(graph);
//   // rtim.rtimRun();
//   // rtim.runTest();
// }
