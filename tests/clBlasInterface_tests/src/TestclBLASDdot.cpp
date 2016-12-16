#include <iostream>
#include "HPL_clBLAS.h"

#define N 4

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
{ HPL::Array<double, 1> x(N), y(N), dotProduct(1);
  
  for(int i = 0; i < N; ++i) {
    x(i) = i * (i+1);   //0 2 6 12
    y(i) = i;           //0 1 2 3
  }

  HPL_clblasSetup();
  
  try { clblasDdot(dotProduct, x, y); } catch (HPL::HPLException& e) { std::cout << e.what();  if(e.getErrCode() == -33) std::cout << "The test device probably does not support double precision\n"; }

  HPL_clblasTeardown();

  return check(dotProduct(0) == 50);
}
