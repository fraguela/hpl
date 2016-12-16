#include <iostream>
#include "HPL.h"

#define N 400

using namespace HPL;


void myf1(Array<int, 1> a)
{
  atomic_inc(a[0]);
  atomic_dec(a[1]);
  atomic_add(a[2],2);
  atomic_sub(a[3],2);
  atomic_xchg(a[4],2);
  atomic_min(a[5],idx);
  atomic_max(a[6],idx);
  atomic_and(a[7],0xffffffff);
  atomic_or(a[8],0x0);
  atomic_xor(a[9],1);
  atomic_cmpxchg(a[10],idx,500);
}

void myf2(Array<int, 1> a)
{
  Int i;
  i = atomic_dec(a[0]);
}

int test1()
{ Array<int, 1> a(11);

  //atomic_inc
  a(0) = 0;
  //atomic_dec
  a(1) = N;
  //atomic_add
  a(2) = 0;
  //atomic_sub
  a(3) = 2*N;
  //atomic_xchg
  a(4) = 0;
  //atomic_min
  a(5) = N;
  //atomic_max
  a(6) = 0;
  //atomic_and
  a(7) = 1;
  //atomic_or
  a(8) = 1;
  //atomic_xor
  a(9) = 1;
  //atomic_cmpxchg
  a(10) = 5;
  eval(myf1).global(N).local(1) (a);

  if( (a(0) == N) &(a(1) == 0) & (a(2) == 2*N) &
     (a(3) == 0) & (a(4) == 2) & (a(5) == 0) &
     (a(6) == (N-1)) & (a(7) == 1) & (a(8) == 1) &
     (a(9) == 1) & (a(10) == 500)) {
    std::cout << "Test1 is PASSED. \n";
    return 0;
  } else {
    std::cout << "Test1 is FAILED. \n";
    return -1;
  }
}

int test2()
{ Array<int, 1> a(1);
  
  a(0) = N;
  eval(myf2).global(N).local(1) (a);

  if( a(0) == 0 ) {
    std::cout << "Test2 is PASSED. \n";
    return 0;
  } else {
    std::cout << "Test2 is FAILED. \n";
    return -1;
  }
}

int main()
{
  return test1() + test2();
}