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
{ HPL::Array<HPL::float2, 1> x(4), y(4);

  cl_float2 alpha = {10.f, 5.f};
  
  for(int i = 0; i < 4; ++i) {
    x(i) = i + 1;         //1  2  3  4
    x(i)(1) = 1;
    y(i) = 5 * (i +1);    //5 10 15 20
    y(i)(1) = 2;
  }

  HPL_clblasSetup();
  
  clblasCaxpy(alpha, x, y);

  HPL_clblasTeardown();

  return check((y(0)(0) == 10.f) && (y(1)(0) == 25.f) && (y(2)(0) == 40.f) && (y(3)(0) == 55.f) &&
               (y(0)(1) == 17.f) && (y(1)(1) == 22.f) && (y(2)(1) == 27.f) && (y(3)(1) == 32.f)); 
}
