#include <iostream>
#include "HPL.h"

#define N 256
#define Witems 32
#define NGroup N/Witems

using namespace HPL;

void dotproduct_seq(Array<float, 1> v1, Array<float, 1> v2, Array<float, 1> partialSums) 
{
  Int i;
  Array<float, 1, Local> sharedM(128); 
  sharedM[lidx] =  v1[idx] * v2[idx];  
  barrier(LOCAL);
  if_(lidx == 0 ) {
	  partialSums[gidx] = sharedM[0];
	  for_( i = 1, i < Witems, i++ ) {
		  partialSums[gidx] += sharedM[i];
	  } 
  } 
}

void dotproduct_manual(Array<float, 1> v1, Array<float, 1> v2, Array<float, 1> partialSums) 
{
  Int i;
  Array<float, 1, Local> sharedM(128); 
  sharedM[lidx] =  v1[idx] * v2[idx];  
  barrier(LOCAL);
  
  // Reduction
  if_ (lidx < 16) {
    sharedM[lidx] += sharedM[lidx+16]; 
  }
  barrier(LOCAL);
  if_ (lidx <  8) {
    sharedM[lidx] += sharedM[lidx+ 8];
  }
  barrier(LOCAL);
  if_ (lidx <  4) {
    sharedM[lidx] += sharedM[lidx+ 4];
  }
  barrier(LOCAL);
  if_ (lidx <  2) {
    sharedM[lidx] += sharedM[lidx+ 2];
  }
  barrier(LOCAL);
  if_ (lidx <  1) {
    sharedM[lidx] += sharedM[lidx+ 1];
  }
  barrier(LOCAL);
  
  if_(lidx == 0 ) {
    partialSums[gidx] = sharedM[lidx];
  } 
}

void dotproduct_generic(Array<float, 1> v1, Array<float, 1> v2, Array<float, 1> partialSums)
{
  reduce(partialSums[gidx], v1[idx] * v2[idx], "+");
}

void dotproduct_genericopt(Array<float, 1> v1, Array<float, 1> v2, Array<float, 1> partialSums)
{
  reduce(partialSums[gidx], v1[idx] * v2[idx], "+").ndims(1).groupSize(Witems);
}

void dotproduct_genericoptintree(Array<float, 1> v1, Array<float, 1> v2, Array<float, 1> partialSums)
{
  reduce(partialSums[gidx], v1[idx] * v2[idx], "+").ndims(1).groupSize(Witems).inTree();
}

int test(Array<float, 1>& partialSums)
{
  if( partialSums.reduce(std::plus<float>()) == 7680.0f ) {
    std::cout << "Test is PASSED. \n";
    return 0;
  } else {
    std::cout << "Test is FAILED. \n";
    return -1;
  }
}

int main()
{ Array<float, 1> v1(N), v2(N);
  Array<float, 1> partialSums(NGroup);
  int result = 0;

  for(int i =0; i < N; i++) {
    v1(i) = 5.0;
    v2(i) = 6.0; 
  }
  
  
  eval(dotproduct_seq).global(N).local(Witems)(v1, v2, partialSums);

  result += test(partialSums);
  
  eval(dotproduct_manual).global(N).local(Witems)(v1, v2, partialSums);
  
  result += test(partialSums);
  
  eval(dotproduct_generic).global(N).local(Witems)(v1, v2, partialSums);
  
  result += test(partialSums);
  
  eval(dotproduct_genericopt).global(N).local(Witems)(v1, v2, partialSums);
  
  result += test(partialSums);
  
  eval(dotproduct_genericoptintree).global(N).local(Witems)(v1, v2, partialSums);
  
  result += test(partialSums);
  
  return result;
}
