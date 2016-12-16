#include <iostream>
#include "HPL_clBLAS.h"

#define N 4

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

int main()
{ HPL::Array<HPL::float2, 1> x(N), y(N);
  
  for(int i = 0; i < N; ++i) {
    x(i) = i * (i+1);   //0 2 6 12
    x(i)(1) = 1;
    y(i) = i;           //0 1 2 3
    y(i)(1) = 0;
  }

  HPL_clblasSetup();
  
  clBlasCcopy(x, y);

  HPL_clblasTeardown();
  
  return check((y(0)(0) == 0.f) && (y(1)(0) == 2.f) && (y(2)(0) == 6.f) && (y(3)(0) == 12.f) &&
               (y(0)(1) == 1.f) && (y(1)(1) == 1.f) && (y(2)(1) == 1.f) && (y(3)(1) == 1.f));
}
