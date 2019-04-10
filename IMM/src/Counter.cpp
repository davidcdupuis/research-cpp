#include "Counter.h"

using namespace std;

Counter::Counter(int id){
  cnt[id]++;
}

void Counter::show(){
  for (int i = 0;i < 1000;i++){
    if (cnt[i] > 0){
      INFO("Counter", i, cnt[i]);
    }
  }
}

int Counter::cnt[1000]={0};
