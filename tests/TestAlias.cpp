#include <iostream>
#include "HPL.h"

#define N 400

using namespace HPL;

void addition(Array<float, 1> a, Array<float, 1> b, Array<float, 1> c)
{
  a[idx] =  b[idx] + c[idx];  
}

void display4(const Array<float, 1>& av)
{
  std::cout << "Array<float, 1> : [";

  for(int i =0; i < 4; i++) {
    std::cout << av(i) << "  ";
  }
  std::cout << " ...]\n";
}

int main()
{ Array<float, 1> av(N), bv(N);
  Array<float, 1> cv(N);

  for(int i =0; i < N; i++) {
      av(i) = 5.0;
      cv(i) = 6.0;
    }

  eval(addition)(av, av, cv);  // Aliases: here av is used two times

  display4(av);

  if( av.reduce(std::plus<float>()) == 4400.f ) {
    std::cout << "Test is PASSED. \n";
    return 0;
  } else {
    std::cout << "Test is FAILED. \n";
    return -1;
  }
  
}
