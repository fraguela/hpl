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
  HPL::Array<HPL::float2, 1> X(5);
  HPL::Array<unsigned int, 1> iMax(1);

  X(0)(0) = -13.3f;
  X(1)(0) = 0.f;
  X(2)(0) = 123.4f;
  X(3)(0) = -124.1f; // Max absolute value
  X(4)(0) = 52.f;

  X(0)(1) = 0.f;
  X(1)(1) = -22.f;
  X(2)(1) = -122.f;
  X(3)(1) = 122.f; // Max absolute value
  X(4)(1) = 120.f;
  
  HPL_clblasSetup();
  
  clblasiCamax(iMax, X);

  HPL_clblasTeardown();

  return check(iMax(0) == 4); // Index start in 1
}
