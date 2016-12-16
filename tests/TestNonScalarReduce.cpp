#include <iostream>

#include "HPL.h"

#define N 256
#define Witems 16
#define NGroup N/Witems

using namespace HPL;

bool dounroll;

void f1(Array<float, 1> v1, Array<float, 1> v2) 
{
  
  for(std::size_t i = 0; i <= 10; ++i)
    reduce(v1[0], v2[idx*8], "+").ndims(1).groupSize(i).nElems(8).unroll(dounroll).inTree();
  
  reduce(v1[0], v2[idx*8], "+").ndims(1).groupSize(16).nElems(8).unroll(dounroll).inTree().toAll();
  
  reduce(v1[0], v2[idx*8], "+").ndims(1).groupSize(32).nElems(8).unroll(dounroll).inTree().toAll();

}

void f2(Array<float, 1> v1) 
{ Float f;
  
  for(std::size_t i = 0; i <= 10; ++i)
    reduce(v1[0], v1[idx*8], "fmax").ndims(1).groupSize(i).nElems(8).unroll(dounroll).inTree();
  
  reduce(f, v1[idx*8], "fmax").ndims(1).groupSize(16).nElems(8).unroll(dounroll).inTree();
  
  reduce(f, v1[idx*8], "fmax").ndims(1).groupSize(32).nElems(8).unroll(dounroll).inTree();
  
}

void fgeneric(Array<float, 1> v1)
{ Float f;
  
  reduce(f, v1[idx*8], "+").nElems(8).unroll(dounroll).inTree();
    
  reduce(f, v1[idx*8], "+").nElems(8).unroll(dounroll).inTree().toAll();
  
  reduce(f, v1[idx*8], "+").ndims(2).nElems(8).unroll(dounroll).inTree();
  
  reduce(f, v1[idx*8], "+").ndims(2).groupSize(16).nElems(8).unroll(dounroll).inTree();
    
  reduce(f, v1[idx*8], "+").ndims(2).localMem(64).nElems(8).unroll(dounroll).inTree();
  
  reduce(f, v1[idx*8], "+").ndims(2).minGroupSize(16).nElems(8).unroll(dounroll).inTree();
  
  reduce(f, v1[idx*8], "+").ndims(2).minGroupSize(16).localMem(16).nElems(8).unroll(dounroll).inTree();
  
  reduce(f, v1[idx*8], "+").ndims(2).minGroupSize(16).localMem(32).nElems(8).unroll(dounroll).inTree();
}


//Must be run with Witems threads per block to get correct result
void dotproduct(Array<float, 1> v1, Array<float, 1> v2, Array<float, 1> partialSums) 
{ Array<float, 1, Local> mx(Witems * 4);
  
  mx[4*lidx]   =  v1[idx*4]   * v2[idx*4];
  mx[4*lidx+1] =  v1[idx*4+1] * v2[idx*4+1];
  mx[4*lidx+2] =  v1[idx*4+2] * v2[idx*4+2];
  mx[4*lidx+3] =  v1[idx*4+3] * v2[idx*4+3];
  
  reduce(partialSums[gidx*4], mx[4*lidx], "+").ndims(1).groupSize(Witems).nElems(4).unroll(dounroll).inTree(); 
}

//Allows any number of theads per block
void generic_dotproduct(Array<float, 1> v1, Array<float, 1> v2, Array<float, 1> v3, Array<float, 1> partialSums) 
{ 
  v3[4*idx]   =  v1[idx*4]   * v2[idx*4];
  v3[4*idx+1] =  v1[idx*4+1] * v2[idx*4+1];
  v3[4*idx+2] =  v1[idx*4+2] * v2[idx*4+2];
  v3[4*idx+3] =  v1[idx*4+3] * v2[idx*4+3];
  
  barrier(LOCAL);
  
  reduce(partialSums[gidx*4], v3[4*idx], "+").ndims(1).nElems(4).unroll(dounroll).inTree(); 
}

int main()
{ int resi = 0; 
  Array<float, 1> v1(N), v2(N), v3(N);
  Array<float, 1> partialSums((N/4)/Witems*4);
  float dotp = 0.0;

  for(int i =0; i < N; i++) {
      v1(i) = 5.0;
      v2(i) = 6.0; 
    }
  
  dounroll = true;
  
  eval(f1).getFhandle()->print();
  
  eval(f2).getFhandle()->print();

  eval(fgeneric).getFhandle()->print();
  
  dounroll = false;
  
  reeval(f1).getFhandle()->print();
  
  reeval(f2).getFhandle()->print();
  
  reeval(fgeneric).getFhandle()->print();
  
  if(Device().getPlatformId() == INTEL) {
    std::cout << "Test skipped in Intel platforms due to bug in some OpenCL SDKs\n";
    return resi;
  }

  /*** Check scalarReduce specialized for a given work-group size ***/
  
  eval(dotproduct).global(N/4).local(Witems)(v1, v2, partialSums);
  
  dotp = partialSums.reduce(std::plus<float>());
  
  if( dotp == 7680.0 )
    std::cout << "Test is PASSED. \n";
  else {
    std::cout << dotp << " Test is FAILED. \n";
    resi--;
  }
  
  /*** Check scalarReduce generalized for any work-group size ***/

  for(int s = 32; s <= N/4; s = 2 * s){
    eval(generic_dotproduct).global(N/4).local(s)(v1, v2, v3, partialSums);
    dotp = 0.;
    for(int i = 0; i < ((N/4)/s)*4; i++) dotp += partialSums(i);

    if( dotp == 7680.0 )
      std::cout << "Test is PASSED. \n";
    else {
      std::cout << dotp << " Test is FAILED. \n";
      resi--;
    }
  }

  return resi;
}
