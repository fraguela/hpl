#include <iostream>
#include "HPL.h"

#define N 400
#define EPS cast<float>(1e-10)

using namespace HPL;

void myf1(Array<float, 1> a, Float b, Float c )
{
  Float d = b + c; // Initialization from expression
  Float e = d;     // Initialization from another scalar
  Float f = 0;     // Initialization from a 0 constant
  Float g = 1.0;   // Initialization from a nonzero constant
  Float h;         // Initialization from nothing
  a[idx] = g * (b + c) + f ;
  h = 2.0f + EPS * e;
}


int main()
{ Array<float, 1> cv(N);
  float s0 = 3.0f, v0 = 4.0f;
  Float s = 3.0f, v = 4.0f;
  
  eval(myf1) (cv, s, v);
  
  int success = ( cv.reduce(std::plus<float>()) == 2800.f );
  
  eval(myf1) (cv, s0, v0);
  
  success += ( cv.reduce(std::plus<float>()) == 2800.f );
  
  eval(myf1) (cv, s, v0);
  
  success += ( cv.reduce(std::plus<float>()) == 2800.f );
  
  s0 = 10.0f;
  v0 = 1.0f;
  
  s.value() = 10.0f;
  v.value() = 1.0f;
  
  eval(myf1) (cv, s, v);
  
  success += ( cv.reduce(std::plus<float>()) == 4400.f );
  
  eval(myf1) (cv, s0, v0);
  
  success += ( cv.reduce(std::plus<float>()) == 4400.f );
  
  eval(myf1) (cv, s, v0);
  
  success += ( cv.reduce(std::plus<float>()) == 4400.f );
  
  if(success == 6) {
    std::cout << "Test is PASSED. \n";
    return 0;
  } else {
    std::cout << "Test is FAILED. \n";
    return -1;
  }
}
