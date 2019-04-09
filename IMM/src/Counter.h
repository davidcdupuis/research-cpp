#ifndef __COUNTER_H__
#define __COUNTER_H__

#include "head.h"

class Counter{
public:
    static int cnt[1000];
    Counter(int id=0);
    ~Counter();
    static void show();
};

#endif
