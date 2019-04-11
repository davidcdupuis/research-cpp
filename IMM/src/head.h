#ifndef __HEAD_H__
#define __HEAD_H__



// #ifdef DEBUG_INFO
// this_is_deprecated
// #endif
// #ifdef DEBUG_TRACE
// this_is_deprecated
// #endif


#if defined(WIN32)
#elif defined(__CYGWIN__) // cygwin
#include <sys/time.h>
#else //linux
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

#include <iostream>
#include <set>
#include <list>
#include <sstream>
#include <cmath>
#include <queue>
#include <fstream>
#include <string>
#include <cstdio>
#include <functional>
#include <algorithm>
#include <climits>
#include <cstring>
#include <algorithm>
#include <cstdlib>
#include <cstdio>
#include <vector>
#include <map>
#include <deque>
using namespace std;


extern map<string, timeval> __head_h_start;
extern string __head_version;
extern map<string, string> datasets;

typedef unsigned int uint;
typedef unsigned char uint8;
typedef long long int64;
typedef unsigned long long uint64;
typedef pair<int,int> ipair;
typedef pair<double,double> dpair;
#define MP make_pair
#define F first
#define S second

#ifndef TIMES_PER_SEC
#define TIMES_PER_SEC (2393.910e6)
#endif

typedef char int8;
typedef unsigned char uint8;
typedef long long int64;
typedef unsigned long long uint64;

#ifdef WIN32
#include <time.h>
#include <windows.h> //I've ommited this line.
#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
#define DELTA_EPOCH_IN_MICROSECS  11644473600000000Ui64
#else
#define DELTA_EPOCH_IN_MICROSECS  11644473600000000ULL
#endif
struct timezone
{
    int  tz_minuteswest; /* minutes W of Greenwich */
    int  tz_dsttime;     /* type of dst correction */
};

int gettimeofday(struct timeval *tv, struct timezone *tz);
#endif

#ifndef WIN32
#ifdef __CYGWIN__

//CYGWIN
uint64 rdtsc();
#else
//LINUX
uint64 rdtsc(void);
#endif
#endif


string nowStr();


void timer_init(string arg="default");

int64 timer_elapse(string arg="default");

#define SIZE(t) (int)(t.size())
#define ALL(t) (t).begin(), (t).end()
#define FOR(i,n) for(int (i)=0; (i)<((int)(n)); (i)++)
#ifdef WIN32
#define FORE(i, x) for (typeid((x).begin()) i = (x).begin(); (i) != (x).end(); (i)++)
#else
#define FORE(i, x) for (__typeof((x).begin()) i = (x).begin(); (i) != (x).end(); (i)++)
#endif


static inline string &ltrim(string &s) { s.erase(s.begin(), find_if(s.begin(), s.end(), not1(ptr_fun<int, int>(isspace)))); return s; }
static inline string &rtrim(string &s) { s.erase(find_if(s.rbegin(), s.rend(), not1(ptr_fun<int, int>(isspace))).base(), s.end()); return s; }
static inline string &trim(string &s) { return ltrim(rtrim(s)); }
string __n_variable(string t, int n);
#define __expand_nv(x) __n_variable(t, x)<< t##x << " "
template<class T0>
void ___debug(string t,deque<T0> t0,ostream&os){os<<__n_variable(t,0);FOR(i, SIZE(t0))os<<t0[i]<<" ";}
template<class T0>
void ___debug(string t,set<T0> t0,ostream&os){os<<__n_variable(t,0);FORE(i,t0)os<<*i<<" ";}
template<class T0>
void ___debug(string t,vector<T0> t0,ostream&os){os<<__n_variable(t,0);FOR(i, SIZE(t0))os<<t0[i]<<" ";}
template<class T0,class T1>
void ___debug(string t,vector<pair<T0,T1> > t0,ostream&os){os<<__n_variable(t,0);FOR(i, SIZE(t0))os<<t0[i].F<<","<<t0[i].S<<" ";}
template<class T0>
void ___debug(string t,T0 t0,ostream&os){os<<__expand_nv(0);}
template<class T0,class T1>
void ___debug(string t,T0 t0,T1 t1,ostream&os){os<<__expand_nv(0)<<__expand_nv(1);}
template<class T0,class T1,class T2>
void ___debug(string t,T0 t0,T1 t1,T2 t2,ostream&os){os<<__expand_nv(0)<<__expand_nv(1)<<__expand_nv(2);}
template<class T0,class T1,class T2,class T3>
void ___debug(string t,T0 t0,T1 t1,T2 t2,T3 t3,ostream&os){os<<__expand_nv(0)<<__expand_nv(1)<<__expand_nv(2)<<__expand_nv(3);}
template<class T0,class T1,class T2,class T3,class T4>
void ___debug(string t,T0 t0,T1 t1,T2 t2,T3 t3, T4 t4,ostream&os){os<<__expand_nv(0)<<__expand_nv(1)<<__expand_nv(2)<<__expand_nv(3)<<__expand_nv(4);}


//#define DO_ONCE


#define RUN_TIME(...) { int64 t=rdtsc();  __VA_ARGS__; t=rdtsc()-t; cout<<  #__VA_ARGS__ << " : " << t/TIMES_PER_SEC <<"s"<<endl;  }

#ifdef HEAD_TRACE
#define TRACE(...) {{ ___debug( #__VA_ARGS__,  __VA_ARGS__,cerr); cerr<<endl;  } }
#define IF_TRACE(args) args
#define TRACE_LINE(...) { ___debug( #__VA_ARGS__,  __VA_ARGS__,cerr); cerr<<"                    \033[100D";  }
#define TRACE_SKIP(a, ...) { static int c=-1; c++; if(c%a==0)TRACE( __VA_ARGS__); }
#define TRACE_LINE_SKIP(a, ...) { static int c=-1; c++; if(c%a==0) TRACE_LINE(__VA_ARGS__);  }
#define TRACE_LINE_END(...) {cerr<<endl; }
ofstream __HEAD_H__LOG("log.txt");
#define TRACE_LOG(...) { __HEAD_H__LOG.close(); ofstream cerr("log.txt", ofstream::out|ofstream::app); ___debug( #__VA_ARGS__,  __VA_ARGS__, cerr); cerr<<endl;  }
#else
#define TRACE(...) ;
#define IF_TRACE(args) ;
#define TRACE_LINE(...) ;
#define TRACE_SKIP(a, ...) ;
#define TRACE_LINE_SKIP(a, ...) ;
#define TRACE_LINE_END(...) ;
#define TRACE_LOG(...) ;
#endif //HEAD_TRACE


#ifdef HEAD_INFO
ofstream __HEAD_H_FOUT;
void setInfoFile(string s){__HEAD_H_FOUT.open(s.c_str()); }
#define ASSERT(v) {if (!(v)) {cerr<<"ASSERT FAIL @ "<<__FILE__<<":"<<__LINE__<<endl; exit(1);}}
#define INFO(...) do { ___debug( #__VA_ARGS__,  __VA_ARGS__,cout); cout<<endl; if(__HEAD_H_FOUT.is_open()){___debug( #__VA_ARGS__,  __VA_ARGS__,__HEAD_H_FOUT); __HEAD_H_FOUT<<endl;}  } while(0)
#define ASSERTT(v, ...) {if (!(v)) {cerr<<"ASSERT FAIL @ "<<__FILE__<<":"<<__LINE__<<endl; INFO(__VA_ARGS__); exit(1);}}
#else
#define ASSERTT(v, ...) ;
#define ASSERT(v ) ;
#define INFO(...) ;
#endif

template<class T>
string toStr(T t){ stringstream ss; ss<<t; return ss.str(); }
string strcat2(const string a, const string b);

string currentTimestampStr();

// return the output of the command by string
string exec(const char* cmd);

string getIpAddress();

string properStringDouble(double value);


string getLocalDatetime();

#endif //__HEAD_H__
