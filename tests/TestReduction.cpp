#include <iostream>
#include <fstream>
#include "HPL.h"

#define N 1<<20
#define BLOCKDIM 64

using namespace HPL;

//The commented piece of code is an optimization that causes the Device-->Host transfer of only one word.

void reduceGPU(Array<float, 1> v1, Array<float, 1> v2, Array<float, 1, Local> sdata, Int n/*, Array<float, 1> res*/)
{

	Size_t tid = lidx;
	Int s;

	Int i = gidx * lszx*2 + lidx;

	sdata[tid] =  where(i < n, v1[i], 0.0f);

	if_((i + lszx) < n)
		sdata[tid] += v1[i + lszx];
	

	barrier(LOCAL);
	// do reduction in shared mem
	for_(s=(lszx>>1), s>0, s>>=1){
		if_(tid < s)
			sdata[tid] += sdata[tid+s];
		
		barrier(LOCAL);
	}

	// write result for this block to global mem
	if_(tid == 0)
		v2[gidx] = sdata[0];
	

//    if_(n <= (lszx<<1)) {
//		res[0] = sdata[0];
//	} endif_
}

//void reduceGPU(Array<float, 1> v1, Array<float, 1> v2, Array<float, 1, Local> sdata, Array<int, 0> n)
//{
//	Size_t tid = lidx;
//	Int s;
//
//	Int i = gidx * lszx*2 + lidx;
//	Float f = where(i <n, v1[i], 0.0f);
//	if_((i + lszx) < n){
//		f += v1[i + lszx];
//	}endif_
//
//	reduce(v2[gidx], f, "+").ndims(1).groupSize(64);
//}

float reduceCPU(float* input)
{
  float res = 0;
  for (int i = 0; i < N; i++)
    res += input[i];
  return res;
}

// Round Up Division function
int shrRoundUp(int group_size, int global_size)
{
    int r = global_size % group_size;
    if(r == 0)
    {
        return global_size;
    } else
    {
        return global_size + group_size - r;
    }
}

int main()
{
	float v1[N];
	float cpu_output, h_out_element;

	for(int i =0; i < N; i++) v1[i] = 1.0f;
	cpu_output = reduceCPU(v1);

	Array<float, 1> d_input(N, v1), d_output(N);//, res(1);
	Array<float, 1, Local> localv(BLOCKDIM);
	

	int par = 0;
	int nn = N;
	
	int global = shrRoundUp((int)(BLOCKDIM<<1), N)>>1;

	while(nn > 1)
	{

		if(par%2==0)
		{
			eval(reduceGPU).global(global).local(BLOCKDIM)(d_input, d_output, localv, nn/*, res*/);
		}
		else
		{
			eval(reduceGPU).global(global).local(BLOCKDIM)(d_output, d_input, localv, nn/*, res*/);
		}

		nn = (nn + (BLOCKDIM<<1) - 1) / (BLOCKDIM<<1);
		global = shrRoundUp((int)(BLOCKDIM<<1), nn	)>>1;

		par++;
	}
	if(par%2==0)
		h_out_element = d_input(0);
	else
		h_out_element = d_output(0);

	//h_out_element = res(0);

        if(h_out_element == cpu_output) {
	        std::cout << "Test is PASSED. \n";
	        return 0;
	} else {
		std::cout << "Test is FAILED, CPU: " << cpu_output << ", GPU: " << h_out_element << "\n";
	        return -1;
	}
}




