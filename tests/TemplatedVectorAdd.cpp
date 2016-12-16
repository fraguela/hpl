#include <iostream>

#include "HPL.h"

#define N 400

using namespace HPL;

template<typename T>
void addv(Array<T, 1> a, Array<T, 1> b, Array<T, 1> c)
{
  a[idx] =  b[idx] + c[idx];  
}

int main()
{ Array<float, 1> av(N), bv(N), cv(N);
  Array<int, 1> avi(N), bvi(N), cvi(N);

  for(int i =0; i < N; i++) {
    av(i) = 5.f;
    bv(i) = 6.f; 
    cv(i) = 0.f;
    avi(i) = i;
    bvi(i) = i+1; 
    cvi(i) = 0;
  }
  
  //We use addv to add floats
  eval(addv<float>)(cv, av, bv);
  
  //We use addv to add ints
  eval(addv<int>)(cvi, avi, bvi);

  if( (cv.reduce(std::plus<float>()) == (N*11.0)) && 
     (cvi.reduce(std::plus<int>()) == ((N*(N-1))/2 + (N*(N+1))/2)) ) {
    std::cout << "Test is PASSED. \n";
    return 0;
  } else {
    std::cout << "Test is FAILED. \n";
    return -1;
  }
}
