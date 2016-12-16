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
  HPL::Array<float, 1> X(5), asum(1);

  X(0) = 0.f;
  X(1) = 1.f;
  X(2) = 2.f;
  X(3) = 3.f;
  X(4) = 4.f;
  
  HPL_clblasSetup();
  
  clblasSasum(asum, X);

  HPL_clblasTeardown();

  return check(asum(0) == 10.f);
}
