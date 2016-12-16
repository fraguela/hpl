/**
@mainpage  Heterogeneous Programming Library (HPL)

@authors   Moisés Viñas        <moises.vinas@udc.es>
@authors   Zeki Bozkus         <zeki.bozkus@khas.edu.tr>
@authors   Basilio B. Fraguela <basilio.fraguela@udc.es>
 
A library for the simplifying the development of heterogeous applications on top of OpenCL

Website: http://hpl.des.udc.es

@section HPLIntro Introduction

HPL is an open source, cross platform development library that aims to
facilitate the exploitation of heterogeneous computing on top of
 the OpenCL standard. HPL can be used in two ways that can be mixed:

- on top of existing OpenCL kernels, largely simplifying their usage, or
- developing the heterogeneous kernels in the embedded language it provides, which is naturally integrated in C++

@section HPLexample Example
 
 The code below computes the SAXPY function Y = alpha * X + Y,
which is described in the C++ function saxpy using the HPL embedded language.

@code
 #include "HPL.h"
 
 using namespace HPL;
 
 //SAXPY kernel in which thread idx computes y[idx]
 void saxpy(Array<float,1> y, Array<float,1> x, Float alpha)
 {
   y[idx] = alpha * x[idx] + y[idx];
 }
 
 
 int main(int argc, char **argv)
 {  Array<float, 1> x(1000), y(1000);
    float alpha;
 
    //the vectors x and y are filled in with data (not shown)
 
    //Run SAXPY on an accelerator, or the CPU if no OpenCL capable accelerator is found
    eval(saxpy)(y, x, alpha);
 }
@endcode
 
 If an OpenCL C kernel is already available, it can be associated to a C++ function that is used to invoke it in HPL. The function parameter list specifies whether each non scalar arguments is an input, an output or both. HPL can get the kernel from a file or a string in the program. The following example illustrates this second possibility.

@code
 #include "HPL.h"
 
 using namespace HPL;
 
 //string with the OpenCL C kernel for SAXPY
 const char *saxpy_kernel =
   "__kernel void saxpy(__global float *y, __global float *x, float alpha) {\n \
      int i = get_global_id(0);                                             \n \
      y[i] = alpha * x[i] + y[i];                                           \n \
    }";
 
 
 //Function whose arguments define the kernel for HPL
 void saxpy_handle(InOut< Array<float,1> > y, In< Array<float,1> > x, Float alpha)
 { }
 
 
 int main(int argc, char **argv)
 {  Array<float, 1> x(1000), y(1000);
    float alpha;
 
    //the vectors x and y are filled in with data (not shown)
 
    //associate the kernel string with the HPL function handle
    nativeHandle(saxpy_handle, “saxpy”, saxpy_kernel);
 
    eval(saxpy_handle)(y, x, alpha);
 }

@endcode
 
*/
