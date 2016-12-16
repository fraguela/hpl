#include <iostream>

#include "HPL.h"

#define N 256
#define Witems 32
#define NGroup N/Witems

using namespace HPL;

void f1(Array<float, 1> v1, Array<float, 1> v2) 
{
  Float f;
  f =  v1[idx] * v2[idx];
  
  for(std::size_t i = 0; i <= 10; ++i)
    reduce(v1[0], f, "+").ndims(1).groupSize(i).inTree();
  
  reduce(v1[0], f, "+").ndims(1).groupSize(16).inTree().toAll();
  
  reduce(v1[0], f, "+").ndims(1).groupSize(32).inTree().toAll();

}

void f2(Array<float, 1> v1) 
{
  Float f;
  f =  v1[idx];
  
  for(std::size_t i = 0; i <= 10; ++i)
    reduce(v1[0], f, "fmax").ndims(1).groupSize(i).inTree();
  
  reduce(f, f, "fmax").ndims(1).groupSize(16).inTree();
  
  reduce(f, f, "fmax").ndims(1).groupSize(32).inTree();
  
}

void fgeneric(Array<float, 1> v1)
{ Float f;
  
  reduce(f, 8, "+").inTree();
  
  reduce(f, 8+f*idx, "+").inTree();
  
  reduce(f, f, "+").inTree().toAll();
  
  reduce(f, f, "+").ndims(2).inTree();
  
  reduce(f, f, "+").ndims(2).groupSize(16).inTree();
    
  reduce(f, f, "+").ndims(2).localMem(64).inTree();
  
  reduce(f, f, "+").ndims(2).minGroupSize(16).inTree();
  
  reduce(f, f, "+").ndims(2).minGroupSize(16).localMem(16).inTree();
  
  reduce(f, f, "+").ndims(2).minGroupSize(16).localMem(32).inTree();
  
  reduce(f, v1[idx], "+").inTree();
}


//Must be run with Witems threads per block to get correct result
void dotproduct(Array<float, 1> v1, Array<float, 1> v2, Array<float, 1> partialSums) 
{ Float f;
  
  f =  v1[idx] * v2[idx];
  
  reduce(partialSums[gidx], f, "+").ndims(1).groupSize(Witems).inTree(); 
}

//Allows any number of theads per block
void generic_dotproduct(Array<float, 1> v1, Array<float, 1> v2, Array<float, 1> partialSums) 
{ Float f;
  
  f =  v1[idx] * v2[idx];
  
  reduce(partialSums[gidx], f, "+").ndims(1).inTree(); 
}

int main()
{ int resi = 0;

  Array<float, 1> v1(N), v2(N);
  Array<float, 1> partialSums(NGroup);
  float dotp = 0.0;

  for(int i =0; i < N; i++) {
      v1(i) = 5.0;
      v2(i) = 6.0; 
    }
  
  FHandle *fh = eval(f1).getFhandle();
  
  fh->print();
  
  eval(f2).getFhandle()->print();

  eval(fgeneric).getFhandle()->print();
 
  std::cout << "===============\n";

  eval(dotproduct).getFhandle()->print();
 
  if(Device().getPlatformId() == INTEL) {
	std::cout << "Test skipped in Intel platforms due to bug in some OpenCL SDKs\n";
	return resi;
  }
  
  /*** Check scalarReduce specialized for a given work-group size ***/
  
  eval(dotproduct).global(N).local(Witems)(v1, v2, partialSums);
  
  for(int i = 0; i < NGroup; i++) dotp += partialSums(i);
  
  if( dotp == 7680.0 )
    std::cout << "Test is PASSED. \n";
  else {
    std::cout << dotp << " Test is FAILED. \n";
    resi--;
  }
  
  
  /*** Check scalarReduce generalized for any work-group size ***/

  for(int s = 32; s <= 256; s = 2 * s){
    eval(generic_dotproduct).global(N).local(s)(v1, v2, partialSums);
    dotp = 0.;
    for(int i = 0; i < (N/s); i++) dotp += partialSums(i);

    if( dotp == 7680.0 )
      std::cout << "Test is PASSED. \n";
    else {
      std::cout << dotp << " Test is FAILED. \n";
      resi--;
    }
    
  }

  return resi;
  
}
