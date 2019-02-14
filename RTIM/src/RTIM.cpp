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
  string file_name = datasets[graph.dataset]["id"] + "_k" + to_string(maxSeedSize) + "_e" + "0,1" + "_ss.txt";
  string path = "../../data/" + graph.dataset + "/imm/basic/" + file_name;
  if (!pathExists(path)){
    cerr << "Error path doesn't exist: " << path << " in " << __FILE__ << " at line " << __LINE__ << endl;
    exit(1);
  }
  ifstream infile(path.c_str());
  int user;
  while (infile >> user){
    immTargeted[user] = 1;
  }
  cout << "IMM seed set : " << toColor("cyan", file_name) << endl;
}

void RTIM::runIMMLive(){
  InfScore infScore = InfScore(graph);
  double immScore;
  if (graph.dataset != "test"){
    importIMMSeed();
  }
  initiateIMMProgressLog();

  string path = generateDataFilePath("stream") + generateFileName("stream");
  int user;
  // cout << "Reading availability stream: " << folder << endl;
  if (!pathExists(path)){
    cerr << "Error path doesn't exist: " << path << " in " << __FILE__ << " at line " << __LINE__ << endl;
    exit(1);
  }
  printInColor("cyan", "Reading stream from : " + path);
  ifstream infile(path.c_str());
  int sum = 0;
  while (infile >> user){
    sum ++;
    if (immTargeted[user] == 1){
      immSeedSet.push_back(user);
      // COMPUTE INFLUENCE SCORE OF IMM SEED SET IF SIZE % 100 = 0
      if(immSeedSet.size()%100 == 0){
        infScore.seedSet = immSeedSet;
        immScore = infScore.mcInfScoreParallel();
        saveIMMProgress(sum, immSeedSet.size(), immScore);
      }
    }

    if (immTargeted[user] == 1){
      immTargeted[user] = 2;
    }

  }

  infScore.seedSet = immSeedSet;
  immScore = infScore.mcInfScoreParallel();
  saveIMMProgress(sum, immSeedSet.size(), immScore);
}

void RTIM::initiateIMMProgressLog(){
  string path = generateDataFilePath("imm_progress");
  if (!pathExists(path)){
    cerr << "Error path doesn't exist: " << path << " in " << __FILE__ << " at line " << __LINE__ << endl;
    exit(1);
  }
  path += generateFileName("imm_progress");
  printInColor("cyan", "New progress log    : " + path);
  ofstream progressFile;
  progressFile.open(path);
  progressFile << "seen, seed_size, seed_score" << endl; //
  progressFile.close();
}

void RTIM::saveIMMProgress(int seen, int seedSize, double seedScore){
  string path = generateDataFilePath("imm_progress");
  if (!pathExists(path)){
    cerr << "Error path doesn't exist: " << path << " in " << __FILE__ << " at line " << __LINE__ << endl;
    exit(1);
  }
  path += generateFileName("imm_progress");
  // printInColor("cyan","Saving progress: " + to_string(progress));
  ofstream progressFile;
  progressFile.open(path, fstream::app);
  /* progress | nodes seen | seed size */
  progressFile << seen << ", ";
  progressFile << seedSize << ", ";
  progressFile << seedScore << endl;
  progressFile.close();
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

void RTIM::infScorePreProcess(){
  // for each node in graph compute influence score
  // cout << "Running infScorePreProcess on " << graph.dataset << endl;
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
      InfScore infScore = InfScore(graph);
      clock_t nodeStart = clock();
      infScore.seedSet = {i};
      infScore.depth = maxDepth;
      score = infScore.mcInfScore();
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

void RTIM::actProbPreProcess(){
  preActProbs.resize(graph.nodes, 0);
  graph.importDegrees();
  // for each node in the graph, compute it's activation probability
  // with the "WC" model we only need to use the in degree
  for(int i = 0; i < graph.nodes; i++){
    if(graph.inDegrees[i] > 1){
      preActProbs[i] = 1.0 - pow(1.0 / graph.inDegrees[i], graph.inDegrees[i]);
    }else if (graph.inDegrees[i] == 1){
      preActProbs[i] = 1;
    }else{
      preActProbs[i] = 0;
    }

  }
  // save results to file
  cout << "Activation Probabilities calculated!" << endl;
  saveActProbs();
}

void RTIM::saveActProbs(){
  string file = "../../data/" + graph.dataset + "/rtim/pre_process/";
  if (!pathExists(file)){
    cerr << "Error path doesn't exist: " << file << " in " << __FILE__ << " at line " << __LINE__ << endl;
    exit(1);
  }
  file += datasets[graph.dataset]["id"] + "_aps.txt";
  printInColor("cyan", "Saving activation probabilities to " + file);

  ofstream actProbsFile;
  actProbsFile.open(file);
  for (int i = 0; i < preActProbs.size() ; i++){
     actProbsFile << i << " " << preActProbs[i] << endl;
  }
  actProbsFile.close();
}

void RTIM::importActProbs(){
  string name = datasets[graph.dataset]["id"] + "_aps.txt";
  string file = "../../data/" + graph.dataset + "/rtim/pre_process/";
  file += name;
  if (!pathExists(file)){
    cerr << "Error path doesn't exist: " << file << " in " << __FILE__ << " at line " << __LINE__ << endl;
    exit(1);
  }
  // printInColor("cyan", "Importing activation probabilities from: " + file);
  preActProbs.resize(graph.nodes, 0);
  int user;
  double ap;

  ifstream infile(file.c_str());
  while(infile >> user >> ap){
    preActProbs[user] = ap;
  }
  cout << "Activation Probs : " << toColor("cyan", name) << endl;
}

// void updateNeighborsAP(int src, vector<double>& activationProbs, set<int> path, double path_weight, int depth);

void RTIM::liveExploration(int& sum, int currUser, double currPathWeight, bool activated, int depth){
  // cout << sum << " | " << currUser << " | " << currPathWeight << " | " << activated << " | " << depth << " | " << theta_ap << endl;
  if(activationProbabilities[currUser] >= theta_ap){
    activated = 1;
    sum += currPathWeight;
  }
  // check all neighbors
  for(pair<int, double> neighbor: graph.graph[currUser]){
    if(depth - 1 >= 0){
      int newD = depth - 1;
      double newPathWeight = currPathWeight * neighbor.second;
      liveExploration(sum, neighbor.first, newPathWeight, activated, newD);
    }
  }
}

void RTIM::live(){
  InfScore infScore = InfScore(graph);

  // initialize all live variables
  int currRTIMSize = 0;
  int currIMMSize = 0;
  double rtimScore;
  double immScore = -1;
  double max_time = 0;
  double duration;
  double inf_duration;
  int sum = 0;
  bool rtimProgressLine = false;
  bool immProgressLine = false;
  int previousProgress = -1;
  bool newProgress = true;

  seedSet.clear(); // in case live was already run with different arguments
  graph.importDegrees();
  if (graph.dataset != "test" && useIMM){
    immSeedSet.clear();
    importIMMSeed();
  }
  if (preActProbs.size() == 0){
    importActProbs();
  }

  // duplicate influence scores so we can modify them during the live process
  vector<double> tmpInfScores;
  for(double score: infScores){
    tmpInfScores.push_back(score);
  }

  if(rtimProgress){
    initiateProgressLog();
  }
  if(immProgress){
    initiateIMMProgressLog();
  }
  initiateStreamLog();
  activationProbabilities.clear();
  activationProbabilities.resize(graph.nodes, 0);

  // printScores();
  getInfIndex(sortedScores);
  cout << "Influence score threshold: " << toColor("yellow", to_string(sortedScores[infIndex])) << endl;

  // read availability stream
  string path = generateDataFilePath("stream") + generateFileName("stream");
  int user;
  // cout << "Reading availability stream: " << folder << endl;
  if (!pathExists(path)){
    cerr << "Error path doesn't exist: " << path << " in " << __FILE__ << " at line " << __LINE__ << endl;
    exit(1);
  }

  //============================== LIVE ========================================
  // start live timer
  clock_t startLive = clock();
  string startDatetime = getLocalDatetime();

  printInColor("magenta", string(60, '-'));
  printLocalTime("magenta", "Live", "starting");
  cout << endl;
  // printInColor("cyan", "Reading stream from : " + path);
  ifstream infile(path.c_str());

  clock_t start;
  time_t startTime;
  time ( &startTime );
  while (infile >> user){
    sum ++;

    //---RECORD IMM PROGRESS---------------------------------------------------
    if(useIMM){
      if (immTargeted[user] == 1){
        immSeedSet.push_back(user);
        if(immSeedSet.size()%100 == 0 && immProgress){
          currIMMSize = immSeedSet.size();
          infScore.seedSet = immSeedSet;
          immScore = infScore.mcInfScoreParallel();
          saveIMMProgress(sum, currIMMSize, immScore);
          if(rtimProgressLine){
            clearLines(2);
            rtimProgressLine = false;
            cout << "IMM (size = " << currIMMSize << ", score = " << immScore << ")" << endl;
            cout << "RTIM (size = " << currRTIMSize << ", score = " << rtimScore << ")" << endl;
          }else{
            clearLines(1);
            cout << "IMM (size = " << currIMMSize << ", score = " << immScore << ")" << endl;
          }

          immProgressLine = true;
	      }
      }
    }
    //-------------------------------------------------------------------------

    if (graph.dataset == "test"){
      cout << "User: " << user << " is online: old_ap = " << activationProbabilities[user] << ", score = " << tmpInfScores[user] << endl;
    }

    // PHASE 1: CHECK ACTIVATION PROBABILITY
    if (activationProbabilities[user] < theta_ap && seedSet.size() < maxSeedSize){

      // ---------------------update influence score --------------------------
      double tot = 0;
      double old_score = tmpInfScores[user];
      start = clock();
      // check all first neighbors first
      for(pair<int, double> neighbor: graph.graph[user]){
        if(activationProbabilities[neighbor.first] == 1){
          tot ++;
        }
      }
      if (tot == graph.graph[user].size()){
        // all neighbors are activated, user influence score = 1
        tmpInfScores[user] = 1;
      }
      // we need to update the inf. threshold if the new score is below it
      // this allows the top % to be the same
      // if (old_score > sortedScores[infIndex] && tmpInfScores[user] < sortedScores[infIndex]){
      //   if(infIndex > 0){
      //     //infIndex --;
      //   }
      // }
      inf_duration = (clock() - start)/(double)CLOCKS_PER_SEC;
      // ----------------------------------------------------------------------

      // PHASE 2: CHECK INFLUENCE SCORE & START MAKING TARGETING DECISIONS
      if(tmpInfScores[user] >= sortedScores[infIndex]){
        seedSet.push_back(user);

        // COMPUTE INFLUENCE SCORE OF SEED SET
        if(seedSet.size() % 100 == 0 && rtimProgress){
          currRTIMSize = seedSet.size();
          infScore.seedSet = seedSet;
          rtimScore = infScore.mcInfScoreParallel();
          saveProgress(sum, currRTIMSize, rtimScore);
          if(immProgress){
            clearLines(2);
            immProgressLine = false;
            // print both results
            cout << "IMM (size = " << currIMMSize << ", score = " << immScore << ")" << endl;
            cout << "RTIM (size = " << currRTIMSize << ", score = " << rtimScore << ")" << endl;
          }else{
            clearLines(1);
            cout << "RTIM (size = " << currRTIMSize << ", score = " << rtimScore << ")" << endl;
          }

          rtimProgressLine = true;
        }
        //---------------------------------------------------------------------

        double tmpAP = activationProbabilities[user];
        activationProbabilities[user] = 1.0;

        //--UPDATE ACTIVATION PROBABILITIES OF TARGETED USER-------------------
        start = clock();
        updateNeighborsAPDepth(user, 2);
        duration = (clock() - start)/(double)CLOCKS_PER_SEC;
        if (duration > max_time){
          max_time = duration;
        }
        //---------------------------------------------------------------------

        // RECORD TARGETED USER
        saveStreamLog(sum, user, tmpAP, duration, old_score, tmpInfScores[user], inf_duration, sortedScores[infIndex], "targeted", seedSet.size(), rtimScore, immScore, graph.inDegrees[user], graph.outDegrees[user]);
        // if (infIndex > 0){
        //   infIndex --;
        // }
        if (graph.dataset == "test" || maxSeedSize < 20){
          cout << "Targeted user: " << user << ": old_ap = " << tmpAP << ", score = " << infScores[user] << endl;
        }
        if (seedSet.size() == 1){
        	cout << "First user targeted: " << user << ": pos = " << sum << ", old_ap = " << tmpAP << ", score = " << tmpInfScores[user] << endl;
        }else if (seedSet.size() == maxSeedSize){
        	cout << "Last user targeted: " << user << ": pos = " << sum << ", old_ap = " << tmpAP << ", score = " << tmpInfScores[user] << endl;
        }
      }else{
        // RECORD USER IGNORED BECAUSE SCORE TOO LOW
        saveStreamLog(sum, user, activationProbabilities[user], -1, old_score, tmpInfScores[user], inf_duration, sortedScores[infIndex], "ig. (sig. too low)", -1, -1, immScore, graph.inDegrees[user], graph.outDegrees[user]);
      }
      //--STREAM STOPPING CONDITIONS-------------------------------------------
      if (seedSet.size() >= maxSeedSize){
        if(useIMM){
          if(immSeedSet.size() >= maxSeedSize){
            break;
          }
        }else{
          break;
        }
      }
      //----------------------------------------------------------------------
    }else{
      if(seedSet.size() >= maxSeedSize){
        saveStreamLog(sum, user, activationProbabilities[user], -1, tmpInfScores[user], -1, -1, sortedScores[infIndex], "seed size reached", -1, -1, immScore, graph.inDegrees[user], graph.outDegrees[user]);
      }else{
        if (activationProbabilities[user] == 1){
          // RECORD USER IGNORED BECAUSE ALREADY TARGETED
          saveStreamLog(sum, user, activationProbabilities[user], -1, tmpInfScores[user], -1, -1, sortedScores[infIndex], "ig. (targeted)", -1, -1, immScore, graph.inDegrees[user], graph.outDegrees[user]);
        }else{
          // RECORD USER IGNORED BECAUSE AP TOO HIGH
          saveStreamLog(sum, user, activationProbabilities[user], -1, tmpInfScores[user], -1, -1, sortedScores[infIndex], "ig. (ap too high)", -1, -1, immScore, graph.inDegrees[user], graph.outDegrees[user]);
        }
      }
    }
    if(useIMM){
      if (immTargeted[user] == 1){
        immTargeted[user] = 2;
      }
    }
  }
  // cout << endl;
  // clearLines(1);
  saveSeedSet();

  string endDatetime = getLocalDatetime();
  double liveDuration = (clock() - startLive)/(double)CLOCKS_PER_SEC;
  printLocalTime("magenta", "Live", "ending");
  printInColor("magenta", string(60, '-'));
  //===========================================================================

  //----COMPUTE RTIM SEED SET INFLUENCE SCORE WITH MC SIMULATIONS--------------
  infScore.seedSet = seedSet;
  printInColor("magenta", string(60, '-'));
  printLocalTime("magenta", "Compute RTIM seed score", "starting");
  string seedFile = generateDataFilePath("rtim_seedSet") + generateFileName("rtim_seedSet", seedSet.size());
  string scoreStartDate = getLocalDatetime();
  clock_t scoreStartTime = clock();
  rtimScore = infScore.mcInfScoreParallel();
  double scoreDuration = (clock() - scoreStartTime)/(double)CLOCKS_PER_SEC;
  string scoreEndDate = getLocalDatetime();
  clearLines(2);
  printInColor("white", "RTIM: |S| = " + to_string(seedSet.size()) + " ; \u03C3_MC(seed) = " + properStringDouble(rtimScore) + " ; duration: " + cleanTime(scoreDuration, "ms"));

  infScore.saveSeedScoreLog(seedFile, scoreStartDate, scoreEndDate, scoreDuration, rtimScore);
  infScore.saveSeedScoreCSV(seedFile, scoreStartDate, scoreEndDate, scoreDuration, rtimScore);
  //---------------------------------------------------------------------------

  //----COMPUTE INFLUENCE SCORE OF IMM SEED SET WITH MC SIMULATIONS------------
  if(useIMM){
    printInColor("magenta", string(60, '-'));
    printLocalTime("magenta", "Compute IMM seed score", "starting");
    infScore.seedSet = immSeedSet;
    immScore = infScore.mcInfScoreParallel();
    clearLines(2);
    printInColor("white", "IMM : |S| = " + to_string(immSeedSet.size()) + " ; \u03C3_MC(seed) = " + properStringDouble(immScore));

    saveIMMProgress(sum, immSeedSet.size(), immScore);
  }
  //---------------------------------------------------------------------------

  saveLiveLog(max_time, liveDuration, startDatetime, endDatetime, rtimScore, immScore);
  saveLiveCSV(graph, liveDuration, max_time, liveDuration);
  if(rtimProgress){
    saveProgress(sum, seedSet.size(), rtimScore);
  }
};

void RTIM::initializeInfluenceScores(){
  // ideally we should not repeat this if live is run more than once
  cout << "Initializing influence scores" << endl;
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
    cerr << "Error path doesn't exist: " << file << " in " << __FILE__ << " at line " << __LINE__ << endl;
    exit(1);
  }
  file += generateFileName("save_infScores");
  //string txt = "Saving inlfuence score to " + file;
  printInColor("cyan", "Saving inlfuence scores to " + file);
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
      cerr << "Error path doesn't exist: " << file << " in " << __FILE__ << " at line " << __LINE__ << endl;
      exit(1);
    }
    file += generateFileName("rtim_progress_seedSet", progPercentage);
    printInColor("cyan", "Saving progress seed set to: " + file);
  }else{
    file = generateDataFilePath("rtim_seedSet");
    if (!pathExists(file)){
      cerr << "Error path doesn't exist: " << file << " in " << __FILE__ << " at line " << __LINE__ << endl;
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
    cerr << "Error path doesn't exist: " << file_path << " in " << __FILE__ << " at line " << __LINE__ << endl;
    exit(1);
  }
  ifstream infile(file_path.c_str());
  while(infile >> user){
    seedSet.push_back(user);
  }
  cout << "Seed set imported correctly!" << endl;
}

void RTIM::saveLiveLog(double& maxTime, double& runtime, string startDatetime, string endDatetime, double rtimScore, double immScore){
  // string file = "../../data/" + graph.dataset + "/streams/" + streamModel + "/" + streamModel + "_m" + to_string(streamVersion) + "/" + graph.dataset + "_liveLog.txt";
  string path = "../../data/" + graph.dataset + "/logs/";
  if (!pathExists(path)){
    cerr << "Error path doesn't exist: " << path << " in " << __FILE__ << " at line " << __LINE__ << endl;
    exit(1);
  }
  path += "rtim_live.log";
  //cout << "\033[33mSaving live log to: " << file << "\033[0m" << endl;
  // printInColor("cyan", "Saving live log to  : " + path);
  ofstream liveLogFile;
  liveLogFile.open(path, fstream::app);
  liveLogFile << "File name       : " << generateFileName("rtim_seedSet", seedSet.size()) << endl;
  liveLogFile << "Start date      : " << startDatetime << endl;
  liveLogFile << "End date        : " << endDatetime << endl;
  liveLogFile << "Duration        : " << cleanTime(runtime, "s") << endl;
  liveLogFile << "Max update time : " << cleanTime(maxTime, "s") << endl;
  liveLogFile << "RTIM \u03C3_MC  : " << rtimScore << endl;
  liveLogFile << "IMM  \u03C3_MC  : " << immScore << endl;
  liveLogFile << "----------------------------------------------------" << endl;
  liveLogFile.close();
  //cout << "\033[33mLive log saved successfully!\033[0m" << endl;
  // printInColor("cyan", "Live log saved successfully");
}

void RTIM::saveLiveCSV(const Graph& graph, double& streamTime, double& maxTime, double& runtime){
  string path = "../../data/" + graph.dataset + "/logs/";
  if (!pathExists(path)){
    cerr << "Error path doesn't exist: " << path << " in " << __FILE__ << " at line " << __LINE__ << endl;
    exit(1);
  }
  path += "rtim_live.csv";
  // cout << "\033[33mSaving live csv to: " << file << "\033[0m" << endl;
  // printInColor("cyan", "Saving live csv to  : " + path);
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

void RTIM::initiateProgressLog(){
  string path = generateDataFilePath("rtim_progress");
  if (!pathExists(path)){
    cerr << "Error path doesn't exist: " << path << " in " << __FILE__ << " at line " << __LINE__ << endl;
    exit(1);
  }
  string name = generateFileName("rtim_progress");
  path += name;
  // printInColor("cyan", "New progress log    : " + path);
  cout << "Progress log : " << toColor("cyan", name) << endl;
  ofstream progressFile;
  progressFile.open(path);
  progressFile << "seen, seed_size, seed_score" << endl; //
  progressFile.close();
}

void RTIM::saveProgress(int seen, int seedSize, double seedScore){
  string path = generateDataFilePath("rtim_progress");
  if (!pathExists(path)){
    cerr << "Error path doesn't exist: " << path << " in " << __FILE__ << " at line " << __LINE__ << endl;
    exit(1);
  }
  path += generateFileName("rtim_progress");
  // printInColor("cyan","Saving progress: " + to_string(progress));
  ofstream progressFile;
  progressFile.open(path, fstream::app);
  /* progress | nodes seen | seed size */
  progressFile << seen << ", ";
  progressFile << seedSize << ", ";
  progressFile << seedScore << endl;
  progressFile.close();
}

void RTIM::initiateStreamLog(){
  string path = generateDataFilePath("stream_log");
  if (!pathExists(path)){
    cerr << "Error path doesn't exist: " << path << " in " << __FILE__ << " at line " << __LINE__<< endl;
    exit(1);
  }
  string name = generateFileName("stream_log");
  path += name;
  // printInColor("cyan", "New stream log      : " + path);
  cout << "Stream log : " << toColor("cyan", name) << endl;
  ofstream streamLog;
  streamLog.open(path);
  streamLog << "Dataset: " << graph.dataset << endl;
  streamLog << "STREAM: model = " << streamModel << "|  size = " << streamSize << endl;
  streamLog << "RTIM: k = " << maxSeedSize << " | theta_ap = " << theta_ap << " | reach = " << reach << endl;
  streamLog << "------------------------------------------------------------" << endl;
  streamLog << left << setw(8 + 1) << "position";
  streamLog << left << setw(8 + 1) << "user_id";
  streamLog << left << setw(10 + 1) << "preAp";
  streamLog << left << setw(10 + 1) << "ap";
  streamLog << left << setw(10 + 1) << "ap_time(s)";
  streamLog << left << setw(10 + 1) << "inf_old";
  streamLog << left << setw(10 + 1) << "inf_new";
  streamLog << left << setw(11 + 1) << "inf_time(s)";
  streamLog << left << setw(8 + 1) << "theta_I";
  streamLog << left << setw(18 + 1) << "rtim_status";
  streamLog << left << setw(10 + 1) << "rtim_size";
  streamLog << left << setw(10 + 1) << "rtim_score";
  streamLog << left << setw(14 + 1) << "imm_status";
  streamLog << left << setw(9 + 1) << "imm_size";
  streamLog << left << setw(10 + 1) << "imm_score";
  streamLog << left << setw(9 + 1) << "in_degree";
  streamLog << left << setw(9 + 1) << "out_degree";
  streamLog << endl;
}

void RTIM::saveStreamLog(int pos, int user, double ap, double ap_time, double oScore, double nScore, double inf_time, double theta_I, string rtim_status, int rtimSize, double rtimScore, double immScore, int inDeg, int outDeg){
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
  streamLog << left << setw(10 + 1) << preActProbs[user];
  streamLog << left << setw(10 + 1) << ap;
  // ACTIVATION PROBABILITY TIME
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
  // INFLUENCE TIME
  if (inf_time == -1){
    streamLog << left << setw(11 + 1) << "-";
  }else{
    streamLog << left << setw(11 + 1) << inf_time;
  }
  streamLog << left << setw(8 + 1) << theta_I;
  streamLog << left << setw(18 + 1) << rtim_status;
  // RTIM SIZE
  if (rtimSize == -1){
    streamLog << left << setw(10 + 1) << "-";
  }else{
    streamLog << left << setw(10 + 1) << rtimSize;
  }
  // RTIM SCORE
  if (rtimScore == -1 || rtimSize % 100 != 0){
    streamLog << left << setw(10 + 1) << "-";
  }else{
    streamLog << left << setw(10 + 1) << rtimScore;
  }
  // IMM TARGETED
  int imm_targeted = 0;
  if(useIMM){
    imm_targeted = immTargeted[user];
  }
  if (imm_targeted == 1){
    streamLog << left << setw(14 + 1) << "targeted";
  }else if(imm_targeted == 2){
    streamLog << left << setw(14 + 1) << "ig. (targeted)";
  }else{
    streamLog << left << setw(14 + 1) << "-";
  }
  // IMM SIZE
  int immSize = 0;
  if(useIMM){
    immSize = immSeedSet.size();
  }
  if (immSize == -1 || imm_targeted != 1){
    streamLog << left << setw(9 + 1) << "-";
  }else{
    streamLog << left << setw(9 + 1) << immSize;
  }
  // IMM SCORE
  if (immScore == -1 || immScore == 0 || immSize % 100 != 0 || imm_targeted != 1){
    streamLog << left << setw(9 + 1) << "-";
  }else{
    streamLog << left << setw(9 + 1) << immScore;
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
    cout << "- progress     : "; printInColor("yellow", (rtimProgress? "true" : "false"));
    cout << "IMM" << endl;
    cout << "- using        : "; printInColor("yellow", (useIMM ? "true" : "false"));
    cout << "- progress     : "; printInColor("yellow", (immProgress? "true" : "false"));
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

int RTIM::stagesMenu(string prevClass){
  int result = 0;
  const int LINES = 10;
  while (result == 0){
    int choice = -1;
    cout << string(26,'_') + " Stages " + string(26,'_') << endl;
    cout << "Choose a stage: " << endl;
    cout << "\t[1] Pre-process scores"<< endl;
    cout << "\t[2] Pre-process probabilities" << endl;
    cout << "\t[3] Live" << endl;
    cout << "\t[4] IMM Live" << endl;
    cout << "\t[5] Test " << endl;
    cout << "\t[6] Return to " << prevClass << endl;
    cout << "\t[7] EXIT PROGRAM " << endl;
    while(choice == -1){
      cout <<  "> choice: ";
      string val;
      getline(cin, val);
      choice = stoi(val);
      switch(choice){
        case 1:
          // pre-process scores menu
          clearLines(LINES);
          stage = "pre-process scores";
          result = infScorePreProcessMenu(prevClass);
          break;
        case 2:
          // pre-process probabilities menu
          clearLines(LINES);
          stage = "pre-process APs";
          result = apPreProcessMenu(prevClass);
          break;
        case 3:
          // live menu
          clearLines(LINES);
          stage = "live";
          result = liveMenu(prevClass);
          break;
        case 4:
          clearLines(LINES);
          result = liveIMMMenu(prevClass);
          break;
        case 5:
          // test menu
          clearLines(LINES);
          // result = testMenu();
          break;
        case 6:
          // go to prevClass
          clearLines(LINES);
          return -1;
        case 7:
          // EXIT Program
          clearLines(LINES);
          return -2;
        default:
          cout << "Error: choice not recognized!" << endl;
          sleep(SLEEP);
          clearLines(2);
          choice = -1;
      }
    }
    //clearLines(9);
  }
  if (result < 0){
    return result;
  }else{
    return result - 1;
  }
}

int RTIM::infScorePreProcessMenu(string prevClass){
  int result = 0;
  while (result == 0){
    if(result == 0){ // ask for new arguments
      int iChoice;
      double dChoice;
      string input;
      cout << string(60,'_') << endl;
      cout << "Input infScorePreProcess arguments" << endl;
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
    printInColor("magenta", string(60, '-'));
    printLocalTime("magenta", "infScorePreProcess", "starting");
    infScorePreProcess();
    printLocalTime("magenta", "infScorePreProcess", "ending");
    result = continueMenu(prevClass);
  }
  if(result < 0){
    return result;
  }else{
    return result - 1;
  }
}

int RTIM::apPreProcessMenu(string prevClass){
  if(!graph.loaded){
    cout << "loading graph" << endl;
    graph.loadGraph();
    graph.loaded = true;
  }
  printInColor("magenta", string(60, '-'));
  printLocalTime("magenta", "Pre-process Activation Probabilities", "starting");
  actProbPreProcess();
  printLocalTime("magenta", "Pre-process Activation Probabilities", "ending");
  printInColor("magenta", string(60, '-'));
  return 0;
}

int RTIM::liveMenu(string prevClass){
  int result = 0;
  while (result == 0){
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
        cout << "> stream model [urr, urnr, inoutr, log](" << streamModel << "): ";
        getline(cin, input);
        if(input != ""){
          if(input == "urr" || input == "urnr" || input == "inoutr" || input == "log" || input == "loginoutr"){
            streamModel = input;
            clearLines(1);
            cout << "> stream model [urr, urnr, inoutr, log](" << streamModel << "): ";
            printInColor("yellow", streamModel);
            break;
          }else{
            cout << "Error: Input must be a valid stream model!" << endl;
            sleep(SLEEP);
            clearLines(2);
          }
        }else{
          clearLines(1);
          cout << "> stream model [urr, urnr, inoutr, log](" << streamModel << "): ";
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
      // does user want to compare RTIM with IMM?
      while(1){
        cout << "> use IMM (" << (useIMM ? "true" : "false") << "): ";
        getline(cin, input);
        if(input != ""){
          try{
            if(input == "true"){
              useIMM = true;
            }else{
              useIMM = false;
            }
            clearLines(1);
            cout << "> use IMM (" << (useIMM ? "true" : "false")  << "): ";
            printInColor("yellow", (useIMM ? "true" : "false"));
            break;
          }catch(invalid_argument& e){
            cout << "Error: invalid input!" << endl;
            sleep(SLEEP);
            clearLines(2);
          }
        }else{
          // streamSize = nodes;
          clearLines(1);
          cout << "> use IMM (" << (useIMM ? "true" : "false")  << "): ";
          printInColor("yellow", (useIMM ? "true" : "false"));
          break;
        }
      }
      // does user want to record RTIM progress
      while(1){
        cout << "> record RTIM progress (" << (rtimProgress ? "true" : "false") << "): ";
        getline(cin, input);
        if(input != ""){
          try{
            if(input == "true"){
              rtimProgress = true;
            }else{
              rtimProgress = false;
            }
            clearLines(1);
            cout << "> record RTIM progress (" << (rtimProgress ? "true" : "false")  << "): ";
            printInColor("yellow", (rtimProgress ? "true" : "false"));
            break;
          }catch(invalid_argument& e){
            cout << "Error: invalid input!" << endl;
            sleep(SLEEP);
            clearLines(2);
          }
        }else{
          // streamSize = nodes;
          clearLines(1);
          cout << "> record RTIM progress (" << (rtimProgress ? "true" : "false")  << "): ";
          printInColor("yellow", (rtimProgress ? "true" : "false"));
          break;
        }
      }
      // does user want to record IMM progress
      while(1){
        cout << "> record IMM progress (" << (immProgress ? "true" : "false") << "): ";
        getline(cin, input);
        if(input != ""){
          try{
            if(input == "true"){
              immProgress = true;
            }else{
              immProgress = false;
            }
            clearLines(1);
            cout << "> record IMM progress (" << (immProgress ? "true" : "false")  << "): ";
            printInColor("yellow", (immProgress ? "true" : "false"));
            break;
          }catch(invalid_argument& e){
            cout << "Error: invalid input!" << endl;
            sleep(SLEEP);
            clearLines(2);
          }
        }else{
          // streamSize = nodes;
          clearLines(1);
          cout << "> record IMM progress (" << (immProgress ? "true" : "false")  << "): ";
          printInColor("yellow", (immProgress ? "true" : "false"));
          break;
        }
      }
      sleep(SLEEP+1);
      clearLines(11);
    }
    printStageParams();
    if(!graph.loaded){
      graph.loadGraph();
      graph.loaded = true;
    }
    if(infScores.size() == 0 || infScores.size() != graph.nodes){
      initializeInfluenceScores();
    }
    live();
    result = continueMenu(prevClass);
  }
  return result - 1;
}

int RTIM::liveIMMMenu(string prevClass){
  int result = 0;
  while (result == 0){
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
      // asking for stream model
      while(1){
        cout << "> stream model [uniform_rand_repeat, uniform_rand_no_repeat, inNOut_repeat, log](" << streamModel << "): ";
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
      clearLines(6);
    }
    printStageParams();
    if(!graph.loaded){
      graph.loadGraph();
      graph.loaded = true;
    }
    printInColor("magenta", string(60, '-'));
    printLocalTime("magenta", "IMM Live", "starting");
    runIMMLive();
    printLocalTime("magenta", "IMM Live", "ending");
    printInColor("magenta", string(60, '-'));
    result = continueMenu(prevClass);
  }
  return result - 1;
}

int RTIM::continueMenu(string prevClass){
  int choice = -1;
  cout << string(60, '_') << endl;
  cout << "Continue: " << endl;
  // cout << "   [1] Repeat previous stage with same arguments (" << stage << ")" << endl;
  cout << "   [1] Repeat previous stage -" << stage << "-" << endl;
  cout << "   [2] Change stage" << endl;
  cout << "   [3] Return to " << prevClass << endl;
  cout << "   [4] End Program" << endl;
  while(choice == -1){
    cout << "> choice: ";
    string val;
    getline(cin, val);
    choice = stoi(val);
    switch(choice){
      case 1:
        // repeat stage with new arguments
        clearLines(7);
        return 0;
      case 2:
        // change stage
        clearLines(7);
        return 1;
      case 3:
        // return to previous menu
        clearLines(7);
        return -1;
      case 4:
        // exit program
        clearLines(7);
        return -2;
      default:
        cout << "Error: choice not recognized!" << endl;
        choice = -1;
        clearLines(2);
    }
  }
  // clearLines(10);
}

int RTIM::run(string prevClass){
  return stagesMenu(prevClass);
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
//         break;
//       case 3:
//         // change stage
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
//     }else if (stage == "test"){
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
    file_path += "rtim/live/" + keyword[streamModel] + "/v" + to_string(streamVersion) + "/";
  }else if (type == "rtim_progress_seedSet"){
    file_path += "rtim/live/" + keyword[streamModel] + "/" + to_string(streamVersion) + "/progress/";
  }else if (type == "stream"){
    file_path += "streams/" + keyword[streamModel] + "/v" + to_string(streamVersion) + "/";
  }else if (type == "rtim_progress"){
    file_path += "rtim/live/" + keyword[streamModel] + "/v" + to_string(streamVersion) + "/progress/";
  }else if (type == "imm_progress"){
    file_path += "imm/live/" + keyword[streamModel] + "/v" + to_string(streamVersion) + "/progress/";
  }else if (type == "stream_log"){
    file_path += "streams/" + keyword[streamModel] + "/v" + to_string(streamVersion) + "/log/";
  }else{
    cout << "Type not recognized!" << endl;
  }
  return file_path;
}

string RTIM::generateFileName(string type, int param){
  string file_name = "";
  if (type == "save_infScores"){
    file_name = datasets[graph.dataset]["id"] + "_d" + properStringDouble(maxDepth) + "_m" + properStringDouble(minEdgeWeight) + "_infS.txt";
  }else if (type == "get_infScores"){
    file_name = datasets[graph.dataset]["id"] + "_infS.txt";
  }else if (type == "rtim_seedSet"){
    file_name = datasets[graph.dataset]["id"] + "_k" + to_string(maxSeedSize) + "_r" + properStringDouble(reach) + "_ap" + properStringDouble(theta_ap) + "_" + keyword[streamModel] + "_v" + to_string(streamVersion) + "_s" + to_string(streamSize) + "_ss" + to_string(param) + ".txt";
  }else if (type == "rtim_progress_seedSet"){
    file_name = datasets[graph.dataset]["id"] + "_" + keyword[streamModel] + "_v" + to_string(streamVersion) + "_s" + to_string(streamSize) + "_r" + properStringDouble(reach) + "_ap" + properStringDouble(theta_ap) + "_prg" + to_string(param) + "_ss.txt";
  }else if (type == "stream"){
    file_name = datasets[graph.dataset]["id"] + "_" + keyword[streamModel] + "_v" + to_string(streamVersion) + "_s" + to_string(streamSize) + "_st.txt";
  }else if (type == "rtim_progress"){
    file_name = datasets[graph.dataset]["id"] + "_k" + to_string(maxSeedSize) + "_r" + properStringDouble(reach) + "_ap" + properStringDouble(theta_ap) + "_" + keyword[streamModel] + "_v" + to_string(streamVersion) + "_s" + to_string(streamSize) + "_prg.csv";
  }else if (type == "imm_progress"){
    file_name = datasets[graph.dataset]["id"] + "_k" + to_string(maxSeedSize) + "_e0,1_" + keyword[streamModel] + "_v" + to_string(streamVersion) + "_s" + to_string(streamSize) + "_prg.csv";
  }else if (type == "stream_log"){
    file_name = datasets[graph.dataset]["id"] + "_k" + to_string(maxSeedSize) + "_r" + properStringDouble(reach) + "_ap" + properStringDouble(theta_ap) + "_" + keyword[streamModel] + "_v" + to_string(streamVersion) + "_s" + to_string(streamSize) + "_prg.log";
  }
  return file_name;
}

void RTIM::updateNeighborsAP(int src, set<int> path, double path_weight, int depth){
  path.insert(src);
  double new_path_weight;
  for(pair<int, double> neighbor: graph.graph[src]){
    if(path.find(neighbor.first) == path.end()){
      new_path_weight = path_weight * neighbor.second;
      activationProbabilities[neighbor.first] = 1 - (1 - activationProbabilities[neighbor.first])*(1 - new_path_weight);
      if (depth > 1){
        updateNeighborsAP(neighbor.first, path, new_path_weight, depth - 1);
      }
    }
  }
  path.erase(src);
}

void RTIM::updateNeighborsAPDepth(int src, int maxDepth){
  for(pair<int, double> neighbor: graph.graph[src]){
    if (activationProbabilities[neighbor.first] < 1){
      activationProbabilities[neighbor.first] = 1 - (1 - activationProbabilities[neighbor.first])*(1 - neighbor.second * activationProbabilities[src]);
      if (maxDepth > 1){
        updateNeighborsAPDepth(neighbor.first, --maxDepth);
      }
    }
  }
}

void RTIM::updateInfluenceScore(double &infScore,int src, vector<double>& tmpAPs, int depth){
  int activated = 0;
  tmpAPs[src] = 1;
  queue< pair<int, int> > queue;
  // queue.push(src);
  double sum = 0;
  // first check all neighbors of src
  for(pair<int, double> neighbor: graph.graph[src]){
    if(activationProbabilities[neighbor.first] == 1){ //neighbor is activated
      activated += 1;
      sum += neighbor.second;
    }
    tmpAPs[neighbor.first] = 1 - (1 - tmpAPs[neighbor.first])*(1 - neighbor.second*tmpAPs[src]);
    sum += tmpAPs[neighbor.first];
    queue.push(make_pair(neighbor.first, 1));
  }
  if( activated == graph.graph[src].size()){ // if all neighbors are activated score = 1
    infScore = 1;
    return;
  }
  if (depth > 1){
    while(!queue.empty()){
      pair<int, int> curr = queue.front();
      queue.pop();
      for(pair<int, double> neighbor: graph.graph[curr.first]){
        tmpAPs[neighbor.first] = 1 - (1 - tmpAPs[neighbor.first])*(1 - neighbor.second*tmpAPs[curr.first]);
        sum += tmpAPs[neighbor.first];
        if(curr.second + 1 <= depth){
          queue.push(make_pair(neighbor.first, curr.second + 1));
        }
      }
    }
    infScore -= sum;
    return;
  }


}

#ifndef MAIN
int main(int argn, char **argv)
{
  // int cores = omp_get_max_threads();
  // Arguments args = Arguments();
  // minEdgeWeight.loadDatasetsData();
  Graph graph = Graph();
  RTIM rtim = RTIM(graph);
  cout << "Running RTIM alone!" << endl;
  // rtim.run();
  // rtim.runTest();
}
#endif
