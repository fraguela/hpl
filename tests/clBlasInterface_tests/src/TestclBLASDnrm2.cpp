#include <iostream>
#include "HPL_clBLAS.h"

#include <stdio.h>

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
  HPL::Array<double, 1> NRM2(1), X(4);

  X(0) = 1;
  X(1) = 2;
  X(2) = 4;
  X(3) = 2;
  
  HPL_clblasSetup();
  
  try { clblasDnrm2(NRM2, X); } catch (HPL::HPLException& e) { std::cout << e.what();  if(e.getErrCode() == -33) std::cout << "The test device probably does not support double precision\n"; }

  HPL_clblasTeardown();

  return check(NRM2(0) == 5.f);
}
