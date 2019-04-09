#include "Timer.h"

using namespace std;

Timer::Timer(int id, string desc="", bool showOnDestroy=false){
  this->id=id;
  while((int)timeUsed.size() <= id){
    timeUsed.push_back(0);
    timeUsedDesc.push_back("");
  }
  timeUsedDesc[id]=desc;
  startTime=rdtsc();
  this->showOnDestroy=showOnDestroy;
}

Timer::~Timer(){
  timeUsed[id]+=(rdtsc()-startTime);
}

static void Timer::show(bool debug=false){
  //INFO("########## Timer ##########");

  for (int i = 0;i < (int)timeUsed.size(); i++){
    if (timeUsed[i] > 0){
      char str[100];
      sprintf(str,"%.6lf",timeUsed[i]/TIMES_PER_SEC );
      string s = str;
      if ((int)s.size()<15){
        s=" "+s;
      }
      char t[100];
      memset(t, 0, sizeof t);
      sprintf(t,"Spend %s seconds on %s",s.c_str(), timeUsedDesc[i].c_str());
      cout << t << endl;
    }
  }
}

static void Timer::clearAll(){
  timeUsed.clear();
  timeUsedDesc.clear();
}
