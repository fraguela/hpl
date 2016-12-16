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
    x(i) = (i * 2);         //0,1  2,1  4,1  6,1
    x(i)(1) = 1.f;
    y(i) = (5 * (i+1));     //5,2  10,2 15,2 20,2
    y(i)(1) = 2.f;
  }

  HPL_clblasSetup();
  
  clBlasCswap(x, y);

  HPL_clblasTeardown();

  return check((y(0)(0) == 0.f) && (y(1)(0) == 2.f) && (y(2)(0) == 4.f) && (y(3)(0) == 6.f) &&
               (y(0)(1) == 1.f) && (y(1)(1) == 1.f) && (y(2)(1) == 1.f) && (y(3)(1) == 1.f) &&
               (x(0)(0) == 5.f) && (x(1)(0) == 10.f) && (x(2)(0) == 15.f) && (x(3)(0) == 20.f) &&
               (x(0)(1) == 2.f) && (x(1)(1) == 2.f) && (x(2)(1) == 2.f) && (x(3)(1) == 2.f));
}
