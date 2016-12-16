#include <iostream>
#include "HPL.h"

#define N 400
#define M 2

using namespace HPL;

void vecAdd(Array<float, 1> a, Array<float, 1> b, Array<float, 1> c)
{
  Int i;
  for_(i = 0, i < M, i++)   // just to consume time
    a[idx] =  b[idx] + c[idx];  
}

float mysum(Array<float, 1>& av)
{

  float results = 0.0;
  for(int i =0; i < N; i++)
    results += av(i);

  return results;
}

int main()
{ Array<float, 1> av(N), bv(N), cv(N);
  Array<float, 1> dv(N), ev(N), fv(N);
  Timer tm;


  int GPUs_number;
  GPUs_number = HPL::getDeviceNumber(GPU);


  std::cout << "There are "<< GPUs_number << " GPUs.\n";
  if(GPUs_number < 2 ) {
    std::cout << "Test needs 2 GPUs. \n";
    std::cout << "Test is PASSED. \n";
    return 0;
  }

  for(int i =0; i < N; i++) {
      av(i) = 5.f;
      bv(i) = 6.f; 
      cv(i) = 0.f;

      ev(i) = 3.f;
      dv(i) = 4.f; 
      fv(i) = 0.f;

    }
  
  HPL::Device d0 = HPL::Device(GPU, 0);
  HPL::Device d1 = HPL::Device(GPU, 1);

  // Block version
  tm.start();
  eval(vecAdd).device(d0)(cv, av, bv);
  eval(vecAdd).device(d1)(fv, dv, ev);
  double sync_secs = tm.stop();

  // Non-block version
  tm.start();
  eval(vecAdd).device(d0)/*.async()*/(cv, av, bv);
  eval(vecAdd).device(d1)/*.async()*/(fv, dv, ev);
  d0.sync();
  d1.sync();
  double async_secs = tm.stop();

  if( mysum(cv) == 4400.f && mysum(fv) == 2800.f) {
    std::cout << "Test is PASSED(Sync) "<< " in "<< sync_secs <<" seconds.\n"; 
    std::cout << "Test is PASSED(Async) "<< " in "<< async_secs <<" seconds.\n";
    return 0;
  } 
  else {
    std::cout << "Test is FAILED. \n";
    return -1;
  }
}
