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
  HPL::Array<float, 1> X(5);
  HPL::Array<unsigned int, 1> iMax(1);

  X(0) = -13.3f;
  X(1) = 0.f;
  X(2) = 123.4f;
  X(3) = -124.1f; // Max absolute value
  X(4) = 52.f;
  
  HPL_clblasSetup();
  
  clblasiSamax(iMax, X);

  HPL_clblasTeardown();

  return check(iMax(0) == 4); // Index start in 1
}
