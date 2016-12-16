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
  HPL::Array<double, 1> X(5), asum(1);

  X(0) = 0.f;
  X(1) = 1.f;
  X(2) = 2.f;
  X(3) = 3.f;
  X(4) = 4.f;
  
  HPL_clblasSetup();
  
  try { clblasDasum(asum, X); } catch (HPL::HPLException& e) { std::cout << e.what(); if(e.getErrCode() == -33) std::cout << "The test device probably does not support double precision\n"; }

  HPL_clblasTeardown();
  
  return check(asum(0) == 10.f);
}
