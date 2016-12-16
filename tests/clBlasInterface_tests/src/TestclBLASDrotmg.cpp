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
  HPL::Array<double, 1> SD1(1), SD2(1), SX1(4), SY1(4), SPARAM(5);

  SD1(0) = 10;
  SD2(0) = 21;
  SX1(0) = 1;
  SY1(0) = -1;

  SPARAM(0) = -1;
  SPARAM(1) = 10;
  SPARAM(2) = 12;
  SPARAM(3) = 20;
  SPARAM(4) = 2;

  HPL_clblasSetup();
  
  try { clblasDrotmg(SD1, SD2, SX1, SY1, SPARAM); } catch (HPL::HPLException& e) { std::cout << e.what();  if(e.getErrCode() == -33) std::cout << "The test device probably does not support double precision\n"; }

  HPL_clblasTeardown();

  return check(tol(SD1(0), 14.2258f) && tol(SD2(0), 6.7741f) && 
               tol(SX1(0), -1.4761f) && tol(SY1(0), -1.f) &&
               tol(SPARAM(0), 1.f) && tol(SPARAM(1), -0.476190f) &&
               tol(SPARAM(2), 12.f) && tol(SPARAM(3), 20.f) &&
               tol(SPARAM(4), -1.f));
}
