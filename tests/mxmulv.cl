
#define BSIZE  2048
#define MXWSIZE 256

__kernel void mxmulv(__global float *y,    
		     __global const float *a,     
		     __global const float *v,     
		     const uint global_rows,
		     const uint global_cols)         
{
  const uint nrows = (global_rows +  get_global_size(0) - 1) / get_global_size(0);
  
  const uint iend = min(global_rows, ((int)get_global_id(0) + 1) * nrows );
  
  for(uint i = get_global_id(0) * nrows; i < iend; ++i) {
    float f = 0.f;
    for(uint j = 0; j < global_cols; ++j)    
      f += a[i*global_cols+j] * v[j];
    y[i] = f;
  }
  
} 

__kernel void mxmulv_alt(__global float *y,    
		     __global const float *a,     
		     __global const float *v,     
		     const uint global_rows,
		     const uint global_cols)         
{
  for(uint i = get_global_id(0); i < global_rows; i+=(uint)(get_global_size(0))) {
    float f = 0.f;
    for(uint j = 0; j < global_cols; ++j)    
      f += a[i*global_cols+j] * v[j];
    y[i] = f;
  }
  
} 


/*********************************************************************/
/** little improvement in NVIDIA */

__kernel void mxmulv_blocked(__global float *y,    
			     __global const float *a,     
			     __global const float *v,     
			     const uint global_rows,
			     const uint global_cols)         
{ __local float tmp[BSIZE];
  
  const uint nrows = (global_rows +  get_global_size(0) - 1) / get_global_size(0);
  
  const uint iend = min(global_rows, ((int)get_global_id(0) + 1) * nrows );
  
  if(global_cols <= BSIZE) {
    
    for(uint j = get_local_id(0); j < global_cols; j += get_local_size(0))
      tmp[j] = v[j];
    
    barrier(CLK_LOCAL_MEM_FENCE);
    
    for(uint i = get_global_id(0) * nrows; i < iend; ++i) {
      float f = 0.f;
      for(uint j = 0; j < global_cols; ++j)    
        f += a[i*global_cols+j] * tmp[j];
      y[i] = f;
    }
    
  } else {
    
    for(uint i = get_global_id(0) * nrows; i < iend; ++i)
      { y[i] = *v - *v; } /* y[i] = 0.f; Breaks with Apple drivers */
    
    for(uint j2=0; j2 < global_cols; j2 += BSIZE) {
    
      const uint j2end = min(j2+BSIZE, global_cols);
      
      for(uint j = (uint)(j2 + get_local_id(0)); j < j2end; j += get_local_size(0))
        tmp[j - j2] = v[j];
      
      barrier(CLK_LOCAL_MEM_FENCE);
      
      for(uint i = (uint)(get_global_id(0) * nrows); i < iend; ++i) {
        float f = y[i];
        for(uint j = j2; j < j2end; ++j)    
          f += a[i * global_cols + j] * tmp[j - j2];
        y[i] = f;
      }
      
      barrier(CLK_LOCAL_MEM_FENCE);
      
    }
   
  }
  
}

/*********************************************************************/
/** little improvement in NVIDIA */

__kernel void mxmulv_blocked_alt(__global float *y,    
			     __global const float *a,     
			     __global const float *v,     
			     const uint global_rows,
			     const uint global_cols)         
{ __local float tmp[BSIZE];

  if(global_cols <= BSIZE) {
    
    for(uint j = get_local_id(0); j < global_cols; j += get_local_size(0))
      tmp[j] = v[j];
    
    barrier(CLK_LOCAL_MEM_FENCE);
    
    for(uint i = get_global_id(0); i < global_rows; i+=get_global_size(0)) {
      float f = 0.f;
      for(uint j = 0; j < global_cols; ++j)    
        f += a[i*global_cols+j] * tmp[j];
      y[i] = f;
    }
    
  } else {
    
    for(uint i = get_global_id(0); i < global_rows; i+=get_global_size(0))
      y[i] = 0.f;
    
    for(uint j2=0; j2 < global_cols; j2 += BSIZE) {
    
      const uint j2end = min(j2+BSIZE, global_cols);
      
      for(uint j = j2 + get_local_id(0); j < j2end; j += get_local_size(0))
        tmp[j - j2] = v[j];
      
      barrier(CLK_LOCAL_MEM_FENCE);
      
      for(uint i = get_global_id(0); i < global_rows; i+=get_global_size(0)) {
        float f = y[i];
        for(uint j = j2; j < j2end; ++j)    
          f += a[i * global_cols + j] * tmp[j - j2];
        y[i] = f;
      }
      
      barrier(CLK_LOCAL_MEM_FENCE);
      
    }
    
  }
  
}

/*********************************************************************/
/** improvement in NVIDIA */

__kernel void mxmulv_blocked2(__global float *y,    
			     __global const float *a,     
			     __global const float *v,     
			     const uint global_rows,
			     const uint global_cols)         
{ __local float tmp[BSIZE], reduce[MXWSIZE];

  const uint nrows = (global_rows +  get_global_size(0) - 1) / get_global_size(0);

  if(global_cols <= BSIZE) {
    
    const uint lidx = get_local_id(0);

    const uint ibegin = get_group_id(0) * get_local_size(0) * nrows;
    
    const uint iend = min(global_rows, ibegin + (uint)get_local_size(0) * nrows);
    
    for(uint j = lidx; j < global_cols; j += get_local_size(0))
      tmp[j] = v[j];
    
    barrier(CLK_LOCAL_MEM_FENCE);
    
    for(uint i = ibegin; i < iend; ++i) {
      float f = 0.f;
      for(uint j = lidx; j < global_cols; j += get_local_size(0))    
        f += a[i*global_cols+j] * tmp[j];
 
      reduce[lidx] = f;
      
      barrier(CLK_LOCAL_MEM_FENCE);
      for(uint j = get_local_size(0) >> 1; j > 1; j = j >> 1) {
        if(lidx < j) reduce[lidx] += reduce[lidx + j];
	      barrier(CLK_LOCAL_MEM_FENCE);
      }
      if(!lidx) y[i] = reduce[0] + reduce[1];
    }
    
  } else { /* not adapted */

    const uint iend = min(global_rows, ((int)get_global_id(0) + 1) * nrows );
  
    for(uint i = get_global_id(0) * nrows; i < iend; ++i)
      { y[i] = *v - *v; } /* y[i] = 0.f; Break with Apple drivers */
    
    for(uint j2=0; j2 < global_cols; j2 += BSIZE) {
    
      const uint j2end = min(j2+BSIZE, global_cols);
      
      for(uint j = j2 + get_local_id(0); j < j2end; j += get_local_size(0))
        tmp[j - j2] = v[j];
      
      barrier(CLK_LOCAL_MEM_FENCE);
      
      for(uint i = get_global_id(0) * nrows; i < iend; ++i) {
        float f = y[i];
        for(uint j = j2; j < j2end; ++j)    
          f += a[i * global_cols + j] * tmp[j - j2];
        y[i] = f;
      }
      
      barrier(CLK_LOCAL_MEM_FENCE);
      
    }

  }
  
}

