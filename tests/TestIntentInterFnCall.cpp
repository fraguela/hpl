#include <iostream>
#include "HPL.h"

#define N 400
#define M 32

using namespace HPL;


int az(Array<float, 1> a, Int i) { a[i] = a[i] + a[i]; return_(i); return 0; }

int au(Array<float, 1> a, Int i, Float f) { f = a[i]; call(az)(a,i); return_(i); return 0; }

void af(Array<float, 1> a, Int i, Float f) { i += a[i]; }

void atfun(Array<float, 1> a, Int i, Float f) { if_(idx < N) { call(au)(a,idx,f); a[i] = i; } }

void myf1(Array<float, 1> a, Float f)
{
  Float t;
  if_(idx < N & idy == 0)
	  call(atfun)(a, idx, f);

}

float mysum(Array<float, 1>& av)
{

  float results = 0.0;
  for(int i =0; i < N; i++)
    results += av(i);
  return results;
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
{ Array<float, 1> cv(N);
  Float s = 2.0;
  Int i = 0;

  for(int j = 0; j < N; j++)
    cv(j) = 0.f;

  display4(cv);

  eval(myf1) (cv, s);
  std::cout << "f1 run" << std::endl << std::flush;
  eval(af) (cv, i, s);
  std::cout << "af run" << std::endl << std::flush;
  eval(atfun) (cv, i, s);
  std::cout << "atfun run" << std::endl << std::flush;

//  std::cout << "<All Code>" << std::endl;
//  std::cout << TheGlobalState().clbinding().allCode << std::endl;
//  std::cout << "</All Code>" << std::endl;

  FHandle &fh_myf1 = TheGlobalState().getFHandle((void*)myf1);
  FHandle &fh_af = TheGlobalState().getFHandle((void*)af);
  FHandle &fh_atfun = TheGlobalState().getFHandle((void*)atfun);

  //Vector a is the first argument of the three kernels
  if((fh_myf1.fhandleTable[0].isWrite && !fh_myf1.fhandleTable[0].isWriteBeforeRead)
     && (!fh_af.fhandleTable[0].isWrite && !fh_af.fhandleTable[0].isWriteBeforeRead)
     && (fh_atfun.fhandleTable[0].isWrite && !fh_atfun.fhandleTable[0].isWriteBeforeRead))

  {
    std::cout << "Test is PASSED. \n";
    return 0;
  }
  else {
    std::cout << "Test is FAILED. \n";
    return -1;
  }
  
}

