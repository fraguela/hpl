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
{ HPL::Array<float, 1> x(4), y(4);
  
  for(int i = 0; i < 4; ++i) {
    x(i) = i + 1;         //1  2  3  4
    y(i) = 5 * (i +1);    //5 10 15 20
  }

  HPL_clblasSetup();
  
  clblasSaxpy(10.f, x, y); //15 30 45 60

  HPL_clblasTeardown();

  return check((y(0) == 15.f) && (y(1) == 30.f) && (y(2) == 45.f) && (y(3) == 60.f)); 
}
