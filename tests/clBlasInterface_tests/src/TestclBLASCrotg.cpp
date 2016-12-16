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

bool tol(float a, float b)
{
    return abs(a - b) < 0.0001;
}

int main()
{
  HPL::Array<HPL::float2, 1> SA(1), SB(1), S(1);
  HPL::Array<float, 1> C(1);

  SA(0)(0) = 3.f;
  SA(0)(1) = 1.f;
  SB(0)(0) = 4.f;
  SB(0)(1) = 2.f;
  
  HPL_clblasSetup();
  
  clblasCrotg(SA, SB, C, S);
  
  HPL_clblasTeardown();
  
  return check(tol(C(0), 0.577344f) &&
               tol(S(0)(0), 0.819255f) && tol(S(0)(1), 0.03862f));
}
