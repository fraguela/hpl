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
  HPL::Array<HPL::float2, 1> X(N), Y(N);

  static cl_float C = 2.0f;
  static cl_float S = 3.0f;

  for(int i=0;i<N;i++) {
      X(i)(0) = 1 << i + 1;
      X(i)(1) = 1;
      Y(i) = 1 << i + 2;
      Y(i)(1) = 2;
  }

  HPL_clblasSetup();
  
  clblasCsrot(X, Y, C, S);

  HPL_clblasTeardown();

  return check((X(0)(0) == 16.f) && (X(1)(0) == 32.f) && (X(2)(0) == 64.f) && (X(3)(0) == 128.f) && 
             (X(4)(0) == 256.f) && (X(0)(1) == 8.f) && (X(1)(1) == 8.f) && (X(2)(1) == 8.f) && 
             (X(3)(1) == 8.f) && (X(4)(1) == 8.f) && (Y(0)(0) == 2.f) && (Y(1)(0) == 4.f) && 
             (Y(2)(0) == 8.f) && (Y(3)(0) == 16.f) && (Y(4)(0) == 32.f) && (Y(0)(1) == 1.f) &&
             (Y(1)(1) == 1.f) && (Y(2)(1) == 1.f) && (Y(3)(1) == 1.f) && (Y(4)(1) == 1.f));

}
