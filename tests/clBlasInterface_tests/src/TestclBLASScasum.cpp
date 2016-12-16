#include <iostream>
#include "HPL_clBLAS.h"

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
  HPL::Array<HPL::float2, 1> X(5), asum(1);

  X(0)(0) = 0.f;
  X(1)(0) = 1.f;
  X(2)(0) = 2.f;
  X(3)(0) = 3.f;
  X(4)(0) = 4.f;

  X(0)(1) = 0.f;
  X(1)(1) = 1.f;
  X(2)(1) = 2.f;
  X(3)(1) = 3.f;
  X(4)(1) = 4.f;
  
  HPL_clblasSetup();
  
  clblasScasum(asum, X);

  HPL_clblasTeardown();

  return check(asum(0)(0) == 20.f);
}
