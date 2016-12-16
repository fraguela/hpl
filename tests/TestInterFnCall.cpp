#include <iostream>

#include "HPL.h"

#define N 256
#define M 32

using namespace HPL;

int az(Array<float, 1> a, Int i) { a[i] = a[i]+a[i]; return_(i); return 0; }

int au(Array<float, 1> a, Int i) { a[i] = a[i]+a[i]; call(az)(a,i); return_(i); return 0; }

void af(Array<float, 1> a, Int i, Float f) { a[i] = i + f; }

void atfun(Array<float, 1> a, Int i, Float f) { if_(idx < N) { a[idx] = idx + call(au)(a,idx); } }

void localf(Array<float,1,Local> mx, Float c) {
  mx[lidx] = c;
}

void myf1(Array<float, 1> a, Float f)
{ Float t;
  
  if_(idx < N & idy == 0){
    Array<float,1,Local> SharedM(M);
    call(af)(a, idx, f);
    call(atfun)(a, idx, f);
    t = call(az)(a,idx) + call(az)(a,idx);
    call(localf)(SharedM, t * t);
  }
}

void display4(const Array<float, 1>& av)
{
  std::cout << "Array<float, 1> : [";
  
  for(int i =0; i < 4; i++) {
    std::cout << av(i) << "  ";
  }
  std::cout << " ...]\n";
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
{ Array<float, 1> cv(N);
  Float s = 2.0;
  Int i = 0;
  int result = 0;
  
  for(int j = 0; j < N; j++)
    cv(j) = 0.f;

  display4(cv);
  
  eval(myf1).local(M)(cv, s);

  display4(cv);

  result += testtest( cv.reduce(std::plus<float>()) == 261120.f );

  eval(atfun)(cv, i, s);

  display4(cv);

  result += testtest( cv.reduce(std::plus<float>()) == 65280.f );

  return result;
  
}
