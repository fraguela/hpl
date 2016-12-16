#include <iostream>
#include <cstring>
#include "HPL.h"

#define N 400
#define PI 3.14159f

using namespace HPL;

/// This code is used both by the host and by the kernels
template<typename T, typename T2>
void surfaceCircle(T&& result, const T2& radius)
{
  result = PI * radius * radius;
}

template<typename T>
void kernel(Array<T, 1>& surface, const Array<T, 1>& radius)
{
  surfaceCircle(surface[idx], radius[idx]);  
}

int main()
{ Array<float, 1> sv(N), rv(N);
  float s[N];
  
  for(int i =0; i < N; i++) {
    rv(i) = (float)(i+1);
    sv(i) = 0.0; 
    surfaceCircle(s[i], rv(i));
  }

  eval(kernel<float>)(sv, rv);

  //only used for debugging.
  TheGlobalState().getFHandle((void*)kernel<float>).print();

  
  if( memcmp(s, sv.getData(), N * sizeof(float)) == 0 ) {
    std::cout << "Test is PASSED. \n";
    return 0;
  } else {
    std::cout << "Test is FAILED. \n";
    return -1;
  }
  
}
