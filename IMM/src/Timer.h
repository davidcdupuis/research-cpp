#ifndef __TIMER_H__
#define __TIMER_H__

#include <sys/time.h>

class Timer{
public:
	static std::vector<int64> timeUsed;
        static std::vector< std::string> timeUsedDesc;
        int id;
        uint64 startTime;
        bool showOnDestroy;

	Timer(int id, string desc="", bool showOnDestroy=false);
	~Timer();
	static void show(bool debug=false);
	static void clearAll();	
};

#endif

