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
  HPL::Array<float, 1> X(N), Y(N), SPARAM(5);

  for(int i=0;i<N;i++) {
      X(i) = 1 << i + 1;
      Y(i) = 1 << i + 2;
  }

  SPARAM(0) = -1;
  SPARAM(1) = 10;
  SPARAM(2) = 12;
  SPARAM(3) = 14;
  SPARAM(4) = 16;

  HPL_clblasSetup();
  
  clblasSrotm(X, Y, SPARAM);

  HPL_clblasTeardown();
  
  return check((X(0) == 76.f) && (X(1) == 152.f) &&
               (X(2) == 304.f) && (X(3) == 608.f) &&
               (X(4) == 1216.f) &&
               (Y(0) == 88.f) && (Y(1) == 176.f) &&
               (Y(2) == 352.f) && (Y(3) == 704.f) &&
               (Y(4) == 1408.f));
}
