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
  HPL::Array<float, 1> NRM2(1), X(4);

  X(0) = 1;
  X(1) = 2;
  X(2) = 4;
  X(3) = 2;
  
  HPL_clblasSetup();
  
  clblasSnrm2(NRM2, X);

  HPL_clblasTeardown();

  return check(NRM2(0) == 5.f);
}
