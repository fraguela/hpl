#include <iostream>

#include "HPL.h"

#define N 512
#define Witems 128

using namespace HPL;

Array<int, 1> v1(N);
Array<int, 2> res(4, 4 * (N/Witems)); // 4 x 16 matrix

int res_cpu[4][4 * (N/Witems)];       // 4 x 16 matrix

//This one is only for printing
void f1(Array<float, 1> v1, Array<float, 1> d1) 
{
  Float f=v1[idx];
  
  reduce(d1[gidx*4 + lidx], f, "+").ndims(1).nTeams(4).inTree();
  
  reduce(v1[gidx+lidx/6], f, "+").ndims(1).nTeams(6).inTree();
  
  reduce(v1[gidx+lidx/8], f, "+").ndims(1).nTeams(8).inTree();
  
  reduce(v1[gidx+lidx/4], f, "+").ndims(1).nTeams(4);
  
  reduce(v1[gidx+lidx/6], f, "+").ndims(1).nTeams(6);
  
  reduce(v1[gidx+lidx/8], f, "+").ndims(1).nTeams(8);
  
}

void f2(Array<int, 1> v1, Array<int, 2> res) 
{
  Int f=v1[idx];
  Int ix = 4 * gidx + (lidx / 32); // 32 = 128 / 4

  reduce(res[0][ix], f, "+").ndims(1).nTeams(4).groupSize(128).inTree();
    
  reduce(res[1][ix], f, "+").ndims(1).nTeams(4).groupSize(128);
  
  reduce(res[2][ix], f, "+").ndims(1).nTeams(4).minGroupSize(128).inTree();
    
  reduce(res[3][ix], f, "+").ndims(1).nTeams(4).minGroupSize(128);
}

void f3(Array<int, 1> v1, Array<int, 2> res) 
{

  Int f=v1[idx];
  Int ix = 4 * gidx + (lidx / 2); // 2 = 8 / 4
  
  reduce(res[0][ix], f, "+").ndims(1).nTeams(4).groupSize(8).inTree();
  
  reduce(res[1][ix], f, "+").ndims(1).nTeams(4).groupSize(8);
  
  reduce(res[2][ix], f, "+").ndims(1).nTeams(4).minGroupSize(8).inTree();
  
  reduce(res[3][ix], f, "+").ndims(1).nTeams(4).minGroupSize(8);
}

void initialize(int nteams, int nthreads, int wgsize)
{
  int els_per_team = wgsize / nteams;
  
  int *p = v1.data();
  
  for(int i =0; i < nthreads; i++) {
    p[i] = i;
  }
  
  for(int i = 0; i < 4; i++) {
    for(int j = 0; j < (nteams * (nthreads / wgsize)); j++) {
      res_cpu[i][j] = 0;
      for(int k = j * els_per_team; k < (j+1) * els_per_team; k++)
	res_cpu[i][j] += p[k];
      
      res(i, j) = 0;
    }
  }
  
}

void print(const Array<int, 2>& res, int nteams, int nthreads, int wgsize)
{
  for(int i = 0; i < 4; i++) {
    printf("[");
    for(int j = 0; j < (nteams * (nthreads / wgsize)); j++)
      printf("%d ", res(i,j));
    printf("]\n");
  }
}

bool test(const Array<int, 2>& res, int nteams, int nthreads, int wgsize) 
{
  for(int i = 0; i < 4; i++) 
    for(int j = 0; j < (nteams * (nthreads / wgsize)); j++) {
      if(res_cpu[i][j] != res(i,j)) {
	printf("Result[%d][%d]: CPU=%d kernel=%d\n", i, j, res_cpu[i][j], res(i,j));
	print(res, nteams, nthreads, wgsize);
	return false;
      }
    }
  
  return true;
}

void pre_test()
{
  std::cout <<  idx.name() << ' ' <<  idy.name() << ' ' <<  idz.name() << std::endl;
  std::cout << lidx.name() << ' ' << lidy.name() << ' ' << lidz.name() << std::endl;
  std::cout << gidx.name() << ' ' << gidy.name() << ' ' << gidz.name() << std::endl;
  std::cout <<  szx.name() << ' ' <<  szy.name() << ' ' <<  szz.name() << std::endl;
  std::cout << lszx.name() << ' ' << lszy.name() << ' ' << lszz.name() << std::endl;
  std::cout << ngroupsx.name() << ' ' <<  ngroupsy.name() << ' ' <<  ngroupsz.name() << std::endl;
  std::cout << v1.name() << ' ' << res.name() << std::endl;
}

int main()
{ int resi = 0;

  pre_test();
  
  /** @bug breaks if these ones are uncommented */
  eval(f1).getFhandle()->print();
  
  eval(f2).getFhandle()->print();

  eval(f3).getFhandle()->print();
  
  initialize(4, N, Witems);
  
  eval(f2).global(N).local(Witems)(v1, res);
  
  if(test(res, 4, N, Witems))
    std::cout << "Test is PASSED. \n";
  else {
    std::cout << "Test is FAILED. \n";
    resi--;
  }
  
  initialize(4, 32, 8);
  
  eval(f3).global(32).local(8)(v1, res);
  
  if(test(res, 4, 32, 8)) {
    std::cout << "Test is PASSED. \n";
  } else {
    std::cout << "Test is FAILED. \n";
    resi--;
  }
  
  return resi;
}
