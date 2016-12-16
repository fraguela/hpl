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
{ HPL::Array<HPL::float2, 1> x(N), y(N), dotProduct(1);

  for(int i = 0; i < N; ++i) {
    x(i) = (i+1) * (i+1);   //1 4 9 16
    x(i)(1) = 1;
    y(i) = i + 1;           //1 2 3 4
    y(i)(1) = 2;
  }

  HPL_clblasSetup();
  
  clblasCdotu(dotProduct, x, y);

  HPL_clblasTeardown();

  return check((dotProduct(0)(0) == 92.f) && (dotProduct(0)(1) == 70.f));
}
