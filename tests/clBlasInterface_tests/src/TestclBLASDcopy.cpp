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
{ HPL::Array<double, 1> x(N), y(N);
  
  for(int i = 0; i < N; ++i) {
    x(i) = i * (i+1);   //0 2 6 12
    y(i) = i;           //0 1 2 3
  }

  HPL_clblasSetup();
  
  clBlasDswap(x, y);

  HPL_clblasTeardown();

  return check((y(0) == 0.f) && (y(1) == 2.f) && (y(2) == 6.f) && (y(3) == 12.f));
}

