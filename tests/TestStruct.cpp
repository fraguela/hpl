#include <iostream>
#include "HPL.h"

using namespace HPL;

#define N 100

HPL_DEFINE_STRUCT(mystruct, {int i_; float f_;} );

void myf1(Array<mystruct, 1> m)
{
  m[idx].at(f_) += m[idx].at(i_) + idx;
}

void myf2(Array<mystruct, 1> m, Array<mystruct, 0> as)
{
  m[idx].at(f_) += m[idx].at(i_) + idx * as.at(i_) - as.at(f_);
}

Array<mystruct, 1> m(N);
Array<mystruct, 0> as;

void init_vectors()
{
  for(int i = 0; i < N; i++) {
    field(m(i), i_) = i ;
    field(m(i), f_) = 100.25f;
  }
}

int main()
{ int i, result=0;
  
  std::cout << "Using array of structs with " << m.getNumberOfDimensions() << " dims\n";
  std::cout << "Element size= " << m.getDataItemSize() << std::endl;
  
  field(as, i_) = 7;
  field(as, f_) = 99.9f;
  
  bool ok = (as.value().i_ == 7) && (as.value().f_ == 99.9f);
  
  as.value().i_ = 1;
  as.value().f_ = 100.0f;
  
  ok = ok && (as.value().i_ == 1) && (as.value().f_ == 100.0f);

  std::cout << "\nRunning Test1\n";
  
  init_vectors();
  
  eval(myf1) (m);

  //only used for debugging.
  TheGlobalState().getFHandle((void*)myf1).print();
  
  for(i = 0; (i < N) && (field(m(i), f_) == (100.25f + 2 * i)); i++);
  
  if (ok && i == N) {
    std::cout << "Test1 is PASSED.\n";
  } else {
    std::cout << "m(" << i << ").f_=" << field(m(i), f_) << " Should be " << (100.25f + 2 * i) << std::endl;
    std::cout << "Test1 is FAILED.\n";
    result--;
  }

  ////////////
  
  /** @bug this one fails in GPU in Apple platform 
      because as of 2013-7-1 this bug in Apple OpenCL applies:
      http://www.khronos.org/message_boards/showthread.php/8220-problem-passing-struct-kernel-arguments-(Apple)
   */
  std::cout << "\nRunning Test2\n";
  
  std::cout << '(' << as.value().i_ << ", " << as.value().f_ << ")\n";

  init_vectors();
  
  mystruct &kk = as.value();
  HPL::Device d(getDeviceNumber(HPL::CPU) ? HPL::CPU : HPL::GPU);
  eval(myf2).device(d) (m, kk);
  
  //only used for debugging.
  TheGlobalState().getFHandle((void*)myf2).print();
  
  for(i = 0; (i < N) && (field(m(i), f_) == (0.25f + 2 * i)); i++);
  
  if (i == N) {
    std::cout << "Test2 is PASSED.\n";
  } else {
    std::cout << "m(" << i << ").f_=" << field(m(i), f_) << " Should be " << (0.25f + 2 * i) << std::endl;
    std::cout << "Test2 is FAILED.\n";
    result--;
  }
  
  return result;
}
