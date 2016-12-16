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

bool tol(float a, float b)
{
    return abs(a - b) < 0.0001;
}

int main()
{
  HPL::Array<HPL::float2, 1> X(4);
  HPL::Array<float, 1> NRM2(1);

  X(0)(0) = 1;
  X(1)(0) = 2;
  X(2)(0) = 4;
  X(3)(0) = 2;

  X(0)(1) = 1;
  X(1)(1) = 2;
  X(2)(1) = 4;
  X(3)(1) = 2;
  
  HPL_clblasSetup();
  
  clblasScnrm2(NRM2, X);

  HPL_clblasTeardown();
  
  return check(tol(NRM2(0), 7.07107f));
}
