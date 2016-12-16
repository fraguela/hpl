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

bool tol(double a, double b)
{
    return abs(a - b) < 0.0001;
}

int main()
{
  HPL::Array<double, 1> SA(1), SB(1), C(1), S(1);

  SA(0) = 3.f;
  SB(0) = 4.f;
  
  HPL_clblasSetup();
  
  try { clblasDrotg(SA, SB, C, S); } catch (HPL::HPLException& e) { std::cout << e.what();  if(e.getErrCode() == -33) std::cout << "The test device probably does not support double precision\n"; }
  
  HPL_clblasTeardown();
  
  return check(tol(C(0), 0.6f) && tol(S(0), 0.8f));
}
