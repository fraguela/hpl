#include <iostream>

#include "HPL.h"

#define N 256
#define Witems 8
#define NGroup N/Witems

using namespace HPL;


/*===================================================================================
 | USE CASES OF 2D REDUCTION
 | --> A 2D reduction that stores its result in a 2D array (dotproduct2)
 | --> A 2D reduction that stores its result in a 1D array (dotproduct)
 ==================================================================================== */

void dotproduct(Array<float, 2> v1, Array<float, 2> v2, Array<float, 1> partialSums)
{ Float f;
  
  f =  v1[idy][idx] * v2[idy][idx];
  
  reduce(partialSums[gidy*NGroup+gidx], f, "+").ndims(2).groupSize(Witems*Witems).inTree();
}

void dotproduct2(Array<float, 2> v1, Array<float, 2> v2, Array<float, 2> partialSums)
{ Float f;

  f =  v1[idy][idx] * v2[idy][idx];

  reduce(partialSums[gidy][gidx], f, "+").ndims(2).groupSize(Witems*Witems).inTree();
}

int main()
{ int resi = 0;
  Array<float, 2> v1(N,N), v2(N,N);
  Array<float, 2> partialSums(NGroup, NGroup);
  Array<float, 1> partialSums2(NGroup * NGroup);
  float dotp = 0.0;

  for(int i =0; i < N; i++) {
	  for(int j = 0; j < N; j++)
	  {
		  v1(i,j) = 5.0;
		  v2(i,j) = 6.0;
	  }
    }
  
  eval(dotproduct2).global(N,N).local(Witems,Witems)(v1, v2, partialSums);

  for(int i = 0; i < NGroup; i++)
	  for(int j = 0; j < NGroup; j++)
		  dotp += partialSums(i,j);


  if( dotp == 30*N*N )
    std::cout << "Test is PASSED. \n";
  else {
    std::cout << dotp << " Test is FAILED. \n";
    resi--;
  }
  
  dotp = 0.0;
  eval(dotproduct).global(N,N).local(Witems,Witems)(v1, v2, partialSums2);

  for(int i = 0; i < NGroup; i++)
	  for(int j = 0; j < NGroup; j++)
		  dotp += partialSums2(i*NGroup+j);

  if( dotp == 30*N*N )
    std::cout << "Test is PASSED. \n";
  else {
    std::cout << dotp << " Test is FAILED. \n";
    resi--;
  }
  
  return resi;
}
