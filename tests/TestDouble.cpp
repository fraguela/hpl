#include <iostream>
#include "HPL.h"

#define N 400

using namespace HPL;

void display4(const Array<double, 1>& av, const char *name)
{
  std::cout << "Array<double, 1> " << name << ": [";
  
  for(int i =0; i < 4; i++) {
    std::cout << av(i) << "  ";
  }
  std::cout << "]\n";
}

void myf2(Array<double, 1> a, Array<double, 1> b, Array<double, 1> c)
{
  a[idx] =  b[idx] + c[idx];  
}

void myf1(Array<double, 1> a, Double b, Double c )
{
  a[idx] = b + c ;  
}

int testtest(bool condition)
{
  if( condition ) {
    std::cout << "Test is PASSED. \n";
    return 0;
  } else {
    std::cout << "Test is FAILED. \n";
    return -1;
  }
}

int main()
{ Array<double, 1> av(N), bv(N), cv(N);
  int res = 0;
  
  if (!HPL::getDeviceNumber(CPU)) {
    std::cout << "This test runs in CPU but it is not available, so it is skipped\n";
    return 0;
  }
  
  for(int i =0; i < N; i++) {
    av(i) = 5.0;
    bv(i) = 6.0; 
    cv(i) = 0.0;
  }
  
  eval(myf2).device(CPU,0)(cv, av, bv);
  
  display4(cv,"cv");
  
  res += testtest( cv.reduce(std::plus<double>())  == 4400.0 );
  
  Double s = 3.0, v = 4.0;
  
  eval(myf1).device(CPU,0)(cv, s, v);
  
  display4(cv, "cv");
  
  res += testtest( cv.reduce(std::plus<double>()) == 2800.0 );
  
  return res;
}
