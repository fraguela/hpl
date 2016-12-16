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
{
  HPL::Array<float, 1> X(N), Y(N);

  static cl_float C = 2.0f;
  static cl_float S = 3.0f;

  for(int i=0;i<N;i++) {
      X(i) = 1 << i + 1;
      Y(i) = 1 << i + 2;
  }

  HPL_clblasSetup();
  
  clblasSrot(X, Y, C, S);

  HPL_clblasTeardown();
  
  return check((X(0) == 16.f) && (X(1) == 32.f) && (X(2) == 64.f) && (X(3) == 128.f) && (X(4) == 256.f) &&
               (Y(0) == 2.f) && (Y(1) == 4.f) && (Y(2) == 8.f) && (Y(3) == 16.f) && (Y(4) == 32.f));
}
