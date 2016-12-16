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
  HPL::Array<float, 1> SA(1), SB(1), C(1), S(1);

  SA(0) = 3.f;
  SB(0) = 4.f;
  
  HPL_clblasSetup();
  
  clblasSrotg(SA, SB, C, S);
  
  HPL_clblasTeardown();

  return check(tol(C(0), 0.6f) && tol(S(0), 0.8f));
}
