#include <iostream>
#include "HPL.h"

#define N 400

using namespace HPL;


void myf(Array<float, 1> a, Array<float, 1> b, Array<float, 1> c)
{
  a[idx] =  b[idx] + c[idx];  
}


int main()
{
  Array<float, 1> a1(N), a2(N), a3(N), a4(N), a5(N);
  
  for(int i =0; i < N; i++) {
    a1(i) = 1.0f;
    a2(i) = 2.0f;
    a3(i) = 3.0f;
    a4(i) = 4.0f;
    a5(i) = 5.0f;
  }
  
  //We obtain the FRunner object with and execute it to
  // set correctly the suitable arrays.
  FRunner fr = eval(myf);
  fr(a5,a1,a4);

  //We perform other execution with another arrays with the same
  //fhandle to sure that run() method runs with the correct parameters.
  eval(myf)(a1,a3,a3);

  //This call has the same behavior that fr(a5,a1,a4)
  fr.run();

  if( a5.reduce(std::plus<float>()) == 4000.0 ) {
    std::cout << "Test is PASSED. \n";
    return 0;
  } else {
    std::cout << "Test is FAILED. \n";
    return -1;
  }
  
}
