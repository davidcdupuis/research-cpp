#ifndef __TOOLS_H__
#define __TOOLS_H__

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

struct timezone{
    int  tz_minuteswest; /* minutes W of Greenwich */
    int  tz_dsttime;     /* type of dst correction */
};

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

std::string nowStr(){
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char str[100];
    sprintf(str,"%d_%d_%d_%d_%d_%d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    return std::string(str);
}

void timer_init(std::string arg="default"){
    timeval ts;
    gettimeofday(&ts,NULL);
    __head_h_start[arg]=ts;
}

int64 timer_elapse(std::string arg="default"){ // unit ms
    cout << "fuck " <<endl;
    struct timeval now;
    gettimeofday(&now,NULL);
    int64 sec=now.tv_sec-__head_h_start[arg].tv_sec;
    int64 usec=now.tv_usec-__head_h_start[arg].tv_usec;
    return sec * 1000 + usec/1000;
}

std::string currentTimestampStr() {
    time_t t = time(0);   // get time now
    struct tm * now = localtime( & t );
    char buf[1000];
    sprintf(buf, "%04d-%02d-%02d %02d:%02d:%02d", now->tm_year+1900, now->tm_mon+1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);
    return std::string(buf);
}

class OutputInfo
{
  public:
    OutputInfo(int argn, char ** argv);
    ~OutputInfo();
};

#endif
