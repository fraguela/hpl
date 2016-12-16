#include <iostream>
#include "HPL.h"

#define N (64*1048576)

using namespace HPL;


int check(bool is_ok)
{
  if(is_ok) {
    std::cout << "Test is PASSED. \n";
    return 0;
  } else {
    std::cout << "Test is FAILED. \n";
    return -1;
  }
}

void increase(Array<float, 1> r)
{
  r[idx] = r[idx] + 1.f;
}


double benchmark(Array<float, 1>& av, float correct, int& ret)
{ Timer tm;
  
  tm.start();
  eval(increase).global(1024)(av);
  if(av(0) != correct) ret++;
  return tm.stop();
}

int main()
{ Array<float, 1> av(N), bv(N, HPL_FAST_MEM);
  int ret = 0;
  
  float *a = (float *)av.getData(HPL_WR);
  float *b = (float *)bv.getData(HPL_WR);
  
  for(int i =0; i < N; i++) {
    a[i] = 1.f;
    b[i] = 1.f; 
  }
  
  eval(increase).global(1024)(av);
  if(av(0) != 2.f) ret++;
  eval(increase).global(1024)(bv);
  if(bv(0) != 2.f) ret++;
  
  av(0) = 3.f;
  bv(0) = 3.f;
  
  std::cout << "Normal time " << benchmark(av, 4.f, ret) << std::endl;
  std::cout << "Fast   time " << benchmark(bv, 4.f, ret) << std::endl;

  return check(ret == 0);
 
}
