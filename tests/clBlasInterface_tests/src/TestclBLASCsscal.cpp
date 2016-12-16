#include <iostream>
#include "HPL_clBLAS.h"

#define N 5

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
{ HPL::Array<HPL::float2, 1> x(N);
  
  for(int i = 0; i < N; i++) {
    x(i)(0) = i+1;
    x(i)(1) = 2;
  }

  HPL_clblasSetup();
  
  clBlasCsscal(10.f, x);

  HPL_clblasTeardown();
  
  return check((x(0)(0) == 10.f) && (x(1)(0) == 20.f) && (x(2)(0) == 30.f) && (x(3)(0) == 40.f) &&
               (x(0)(1) == 20.f) && (x(1)(1) == 20.f) && (x(2)(1) == 20.f) && (x(3)(1) == 20.f));
}
