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
  cout << "\r                                                         ";
  for(int i=0; i < l; i++){
    cout << "\e[A\r                                                                                                      ";
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
