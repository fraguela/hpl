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
{ HPL::Array<float, 1> x(N), y(N);
  
  for(int i = 0; i < N; ++i) {
    x(i) = i * 2;         //0  2  4  6
    y(i) = 5 * (i+1);     //5  10 15 20
  }

  HPL_clblasSetup();
  
  clBlasSswap(x, y);

  HPL_clblasTeardown();
  
  return check((y(0) == 0.f) && (y(1) == 2.f) && (y(2) == 4.f) && (y(3) == 6.f) &&
               (x(0) == 5.f) && (x(1) == 10.f) && (x(2) == 15.f) && (x(3) == 20.f)); 
}
