#include "Counter.h"

using namespace std;

Counter::Counter(int id=0){
  cnt[id]++;
}

static void Counter::show(){
  for (int i = 0;i < 1000;i++){
    if (cnt[i] > 0){
      INFO("Counter", i, cnt[i]);
    }
  }
}
