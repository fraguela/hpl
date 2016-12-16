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
{ HPL::Array<float, 1> x(5), y(5);
  
  HPL_clblasSetup();
  
  try {
    //clblasSaxpy(5,
    //            1.0f,
    //            x,
    //            1, // Wrong offset
    //            1,
    //            y,
    //            0,
    //            1);
  } catch (int e) {
   std::cout << "Exception" << std::endl; 
  }

  return 0; 
}
