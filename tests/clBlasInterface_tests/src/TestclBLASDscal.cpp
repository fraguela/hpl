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
{ HPL::Array<double, 1> x(N);
  
  for(int i = 0; i < N; ++i) {
    x(i) = i * 3;         //0  3  6  9
  }

  HPL_clblasSetup();
  
  clBlasDscal(1.5f, x);

  HPL_clblasTeardown();
  
  return check((x(0) == 0.f) && (x(1) == 4.5f) && (x(2) == 9.f) && (x(3) == 13.5f));
}
