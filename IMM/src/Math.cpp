#include "Math.h"

using namespace std;

static double Math::log2(int n){
  return log(n) / log(2);
}

static double Math::logcnk(int n, int k){
  double ans = 0;
  for(int i = n - k + 1; i <= n; i++){
    ans += log(i);
  }
  for(int i = 1; i <= k; i++){
    ans -= log(i);
  }
  return ans;
}
