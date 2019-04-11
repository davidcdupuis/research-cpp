#include "head.h"

map<string, timeval> __head_h_start;
string __head_version = "";
map<string, string> datasets = {
  { "test", "TS" },
  { "nethept", "NE" },
  { "dblp", "DB" },
  { "youtube", "YO" },
  { "livejournal", "LJ" },
  { "orkut", "OR" },
  { "twitter", "TW"}
};


#ifdef WIN32
int gettimeofday(struct timeval *tv, struct timezone *tz){
    FILETIME ft;
    unsigned __int64 tmpres = 0;
    static int tzflag;

    if (NULL != tv)
    {
        GetSystemTimeAsFileTime(&ft);

        tmpres |= ft.dwHighDateTime;
        tmpres <<= 32;
        tmpres |= ft.dwLowDateTime;

        /*converting file time to unix epoch*/
        tmpres -= DELTA_EPOCH_IN_MICROSECS;
        tmpres /= 10;  /*convert into microseconds*/
        tv->tv_sec = (long)(tmpres / 1000000UL);
        tv->tv_usec = (long)(tmpres % 1000000UL);
    }

    if (NULL != tz)
    {
        if (!tzflag)
        {
            _tzset();
            tzflag++;
        }
        tz->tz_minuteswest = _timezone / 60;
        tz->tz_dsttime = _daylight;
    }

    return 0;
}
#endif

#ifndef WIN32
#ifdef __CYGWIN__

//CYGWIN
uint64 rdtsc(){
  uint64 t0;
  asm volatile("rdtsc" : "=A"(t0));
  return t0;
}
#else
//LINUX
uint64 rdtsc(void){
  unsigned a, d;
  //asm("cpuid");
  asm volatile("rdtsc" : "=a" (a), "=d" (d));
  return (((uint64)a) | (((uint64)d) << 32));
}
#endif
#endif


string nowStr(){
  time_t t = time(NULL);
  struct tm tm = *localtime(&t);
  char str[100];
  sprintf(str,"%d_%d_%d_%d_%d_%d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
  return string(str);
}


// map<string, timeval> __head_h_start;
void timer_init(string arg){
  timeval ts;
  gettimeofday(&ts,NULL);
  __head_h_start[arg]=ts;
}

int64 timer_elapse(string arg){ // unit ms
  struct timeval now;
  gettimeofday(&now,NULL);
  int64 sec=now.tv_sec-__head_h_start[arg].tv_sec;
  int64 usec=now.tv_usec-__head_h_start[arg].tv_usec;
  return sec * 1000 + usec/1000;
}

// static inline string &ltrim(string &s) { s.erase(s.begin(), find_if(s.begin(), s.end(), not1(ptr_fun<int, int>(isspace)))); return s; }
// static inline string &rtrim(string &s) { s.erase(find_if(s.rbegin(), s.rend(), not1(ptr_fun<int, int>(isspace))).base(), s.end()); return s; }
// static inline string &trim(string &s) { return ltrim(rtrim(s)); }
string __n_variable(string t, int n){ t=t+','; int i=0; if(n) for(; i<SIZE(t)&&n; i++) if(t[i]==',') n--; n=i; for(;t[i]!=',';i++); t=t.substr(n, i-n); trim(t);if(t[0]=='"') return ""; return t+"="; }
// #define __expand_nv(x) __n_variable(t, x)<< t##x << " "
// template<class T0>
// void ___debug(string t,deque<T0> t0,ostream&os){os<<__n_variable(t,0);FOR(i, SIZE(t0))os<<t0[i]<<" ";}
// template<class T0>
// void ___debug(string t,set<T0> t0,ostream&os){os<<__n_variable(t,0);FORE(i,t0)os<<*i<<" ";}
// template<class T0>
// void ___debug(string t,vector<T0> t0,ostream&os){os<<__n_variable(t,0);FOR(i, SIZE(t0))os<<t0[i]<<" ";}
// template<class T0,class T1>
// void ___debug(string t,vector<pair<T0,T1> > t0,ostream&os){os<<__n_variable(t,0);FOR(i, SIZE(t0))os<<t0[i].F<<","<<t0[i].S<<" ";}
// template<class T0>
// void ___debug(string t,T0 t0,ostream&os){os<<__expand_nv(0);}
// template<class T0,class T1>
// void ___debug(string t,T0 t0,T1 t1,ostream&os){os<<__expand_nv(0)<<__expand_nv(1);}
// template<class T0,class T1,class T2>
// void ___debug(string t,T0 t0,T1 t1,T2 t2,ostream&os){os<<__expand_nv(0)<<__expand_nv(1)<<__expand_nv(2);}
// template<class T0,class T1,class T2,class T3>
// void ___debug(string t,T0 t0,T1 t1,T2 t2,T3 t3,ostream&os){os<<__expand_nv(0)<<__expand_nv(1)<<__expand_nv(2)<<__expand_nv(3);}
// template<class T0,class T1,class T2,class T3,class T4>
// void ___debug(string t,T0 t0,T1 t1,T2 t2,T3 t3, T4 t4,ostream&os){os<<__expand_nv(0)<<__expand_nv(1)<<__expand_nv(2)<<__expand_nv(3)<<__expand_nv(4);}

// template<class T>
// string toStr(T t){ stringstream ss; ss<<t; return ss.str(); }
string strcat2(const string a, const string b){return (a+b); }

string currentTimestampStr() {
  time_t t = time(0);   // get time now
  struct tm * now = localtime( & t );
  char buf[1000];
  sprintf(buf, "%04d-%02d-%02d %02d:%02d:%02d", now->tm_year+1900, now->tm_mon+1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);
  return string(buf);
}


// return the output of the command by string
string exec(const char* cmd){
  FILE* pipe = popen(cmd, "r");
  if (!pipe) return "ERROR";
  char buffer[128];
  std::string result = "";
  while(!feof(pipe)) {
      if(fgets(buffer, 128, pipe) != NULL)
          result += buffer;
  }
  pclose(pipe);
  return result;
}

string getIpAddress(){
  string cmd=" ifconfig |grep 'inet addr' |grep -v 127.0.0.1 |awk '{print $2}' |python -c 'print raw_input().split(\":\")[-1]' ";
  return exec(cmd.c_str());
}

// string __head_version = "";
// map<string, string> datasets = {
//   { "test", "TS" },
//   { "nethept", "NE" },
//   { "dblp", "DB" },
//   { "youtube", "YO" },
//   { "livejournal", "LJ" },
//   { "orkut", "OR" },
//   { "twitter", "TW"}
// };

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


string getLocalDatetime(){
  time_t tt;
  struct tm * ti;
  time (&tt);
  ti = localtime(&tt);
  char *dateTime = asctime(ti);
  dateTime[strlen(dateTime) - 1] = 0;
  return dateTime;
}
