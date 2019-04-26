#include "Tools.h"

using namespace std;

void printInColor(string color, string txt){
  string colorVal;
  if(color == "red"){
    colorVal = "31";
  }else if (color == "green"){
    colorVal = "32";
  }else if (color == "yellow"){
    colorVal = "33";
  }else if (color == "blue"){
    colorVal = "34";
  }else if (color == "magenta"){
    colorVal = "35";
  }else if (color == "cyan"){
    colorVal = "36";
  }else if (color == "white"){
    colorVal = "37";
  }else{
    cerr << "Error: color specified not recognized!" << endl;
  }
  cout << "\033[1;" + colorVal + "m" + txt + "\033[0m" << endl;
}

string toColor(string color, string txt){
  string colorVal;
  if(color == "red"){
    colorVal = "31";
  }else if (color == "green"){
    colorVal = "32";
  }else if (color == "yellow"){
    colorVal = "33";
  }else if (color == "blue"){
    colorVal = "34";
  }else if (color == "magenta"){
    colorVal = "35";
  }else if (color == "cyan"){
    colorVal = "36";
  }else if (color == "white"){
    colorVal = "37";
  }else{
    cerr << "Error: color specified not recognized!" << endl;
  }
  return "\033[1;" + colorVal + "m" + txt + "\033[0m";
}

string cleanTime(double t, string type){
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

void clearLines(int l){
  cout << "\r                                                                                                                                           ";
  for(int i=0; i < l; i++){
    cout << "\e[A\r                                                                                                                                     ";
  }
  cout << "\r";
}

void printLocalTime(string color, string name, string status){
  time_t tt;
  struct tm * ti;
  time (&tt);
  ti = localtime(&tt);
  char *dateTime = asctime(ti);
  dateTime[strlen(dateTime) - 1] = 0;
  //cout << "\033[" + args + "m" + name + " " + status + " at: " << asctime(ti) << "\033[0m";
  string txt = name + " " + status + " at: " + dateTime;
  printInColor(color, txt);
}

string getLocalDatetime(){
  time_t tt;
  struct tm * ti;
  time (&tt);
  ti = localtime(&tt);
  char *dateTime = asctime(ti);
  dateTime[strlen(dateTime) - 1] = 0;
  return dateTime;
}

string printSeed(vector<int> seed){
  string s = "[";
  for (int i = 0; i < seed.size(); i++){
    if (i < seed.size() - 1){
      s += to_string(seed[i]) + ", ";
    }else{
      s += to_string(seed[i]);
    }
  }
  s += "]";
  return s;
}

string properStringDouble(double value){
  string original = to_string(value);
  string result = "";
  bool cont = true;
  int index = original.length() - 1;
  for(int i=original.length()-1; i >= 0; i--){
    if(original[i] != '0'){
      cont = false;
    }
    if(original[i] == '.'){
      original[i] = ',';
      if (index == i){
        index--;
      }
      break;
    }
    if(cont){
      index--;
    }
  }
  for(int i=0; i <= index; i++){
    result += original[i];
  }
  return result;
}

string cleanLongInt(int value){
  string result = to_string(value);
  string reverse = "";
  int step = 0;
  for(int i = result.length() - 1; i >= 0; i--){
    if(step == 3){
      step = 0;
      reverse += "_";
    }
    reverse += result[i];
    step ++;
  }
  result = "";
  for(int i = reverse.length() - 1; i >= 0; i--){
    result += reverse[i];
  }
  return result;
}

void listFolderFiles(string folder){
  DIR *dir;
  struct dirent *ent;
  if ((dir = opendir (folder.c_str())) != NULL) {
    /* print all the files and directories within directory */
    while ((ent = readdir (dir)) != NULL) {
      printf ("%s\n", ent->d_name);
    }
    closedir (dir);
  } else {
    /* could not open directory */
    perror ("");
    cout << "Error!" << endl;
  }
  std::string path = "/path/to/directory";
}

bool pathExists(const std::string& filename){
  ifstream infile(filename.c_str());
  return (bool)infile;
}

vector<int> randomSeedGenerator(int N, int k){
  random_device rd;
  mt19937 gen(rd());
  unordered_set<int> elems = setGenerator(N, k, gen);
  vector<int> result(elems.begin(), elems.end());
  std::shuffle(result.begin(), result.end(), gen);
  return result;
}

unordered_set<int> setGenerator(int N, int k, std::mt19937& gen){
  uniform_int_distribution<> dis(1, N);
  unordered_set<int> elems;

  while (elems.size() < k) {
    elems.insert(dis(gen));
  }

  return elems;
}

void printTitle(int size, string text){
  text = " " + text + " ";
  size -= text.length();
  cout << string(size/2, '-') << toColor("red", text) << string(size/2+1, '-') << endl;
}
