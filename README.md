## Heterogenerous Programming Libray (HPL) </p>

 The Heterogeneous Programming Library (HPL) is a C++ framework that provides an easy and portable way to exploit heterogeneous computing systems on top of the [OpenCL standard](http://www.khronos.org/opencl/). HPL can be used in two ways that can be mixed:

- on top of existing OpenCL kernels, largely simplifying their usage, or
- developing the heterogeneous kernels in the embedded language it provides, which is naturally integrated in C++

As an example, the code below computes the SAXPY function `Y = alpha * X + Y`, which is described in the C++ function saxpy using the HPL embedded language.
 
 ```cpp 
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
 ```

 If an OpenCL C kernel is already available, it can be associated to a C++ function that is used to invoke it in HPL. The function parameter list specifies whether each non scalar arguments is an input, an output or both. HPL can get the kernel from a file or a string in the program. The following example illustrates this second possibility.

```cpp 
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
```

### Requirements

  - At least one OpenCL distribution that supports version 1.1 of the standard. HPL currently supports AMD, Apple, Intel and NVIDIA implementations.
  - A C++ compiler that supports C++11
 
### Documentation

 - [HPL Programming Manual](http://hpl.des.udc.es/page1_assets/HPL_programming_manual.pdf)
 - [A tutorial](http://gac.udc.es/~tonino/webmoises/public_html/courses_en.html)
 - [Example comparison with OpenCL](http://gac.udc.es/~tonino/webmoises/public_html/index_en.html)

### Publications

<ul>
<li>
<i>M. Vi&ntilde;as, B.B. Fraguela, D. Andrade, R. Doallo</i>.
<a href="http://gac.udc.es/~basilio/papers/Vinas18-HTAHPL.pdf">
Heterogeneous Distributed Computing based on High Level Abstractions</a>.
<b> Concurrency and Computation: Practice and Experience</b>, 30(17):e4664, September 2018.
</li>

<li>
<i>M. Vi&ntilde;as, B.B. Fraguela, D. Andrade, R. Doallo</i>.
<a href="http://gac.udc.es/~basilio/papers/Vinas17-HPLStencils.pdf">
Facilitating the development of stencil applications using the Heterogeneous Programming Library</a>.
<b> Concurrency and Computation: Practice and Experience</b>, 29(12):e4152, June 2017.
</li>

<li>
<i>N. Losada, B.B. Fraguela, P. Gonz&aacute;lez, M.J. Mart&iacute;n</i>.
<a href="http://gac.udc.es/~basilio/papers/Losada17-CPPC_HPL.pdf">
A portable and adaptable fault tolerance solution for heterogeneous applications</a>.
<b> Journal of Parallel and Distributed Computing</b>, 104:146-158, June 2017.
</li>

<li>
<i>M. Vi&ntilde;as, B.B. Fraguela, D. Andrade, R. Doallo</i>.
<a href="http://gac.udc.es/~basilio/papers/Vinas17-HPL.pdf">
High Productivity Multi-device Exploitation with the Heterogeneous Programming Library</a>.
<b> Journal of Parallel and Distributed Computing</b>, 101:51-68, March 2017.
</li>

<li>
<i>M. Vi&ntilde;as, B.B. Fraguela, D. Andrade, R. Doallo</i>.
<a href="http://gac.udc.es/~basilio/papers/Vinas16-clusters.pdf">
Towards a high level approach for the programming of heterogeneous clusters</a>.
<b> 45th Intl. Conf. on Parallel Processing Workshops (ICPPW 2016)</b>, pp. 106-114. Philadelphia (USA), August 2016.
</li>

<li>
<i>J. F. Fabeiro, D. Andrade,  B.B. Fraguela, R. Doallo</i>.
<a href="https://www.researchgate.net/profile/Jorge_Fernandez_Fabeiro/publication/305556818_How_to_Write_Performance_Portable_Codes_using_the_Heterogeneous_Programming_Library/links/57935f2108aeb0ffccddc0e0.pdf">
How to write performance portable codes using the Heterogeneous Programming Library</a>.
<b> 19th Workshop on  Compilers for Parallel Computing (CPC 2016)</b>. Valladolid (Spain), July 2016.
</li>

<li>
<i>M. Vi&ntilde;as</i> (Advisors: <i>B.B. Fraguela</i> and <i>D. Andrade</i>).
<a href="http://gac.udc.es/tesis/MoisesVinasBuceta.pdf">Improving the programmability of heterogeneous systems by means of libraries</a>. <b>PhD Thesis</b>, Department of Electronics and Systems, University of A Coru&ntilde;a (Spain). July 2016.
</li>

<li>
<i>S. Altuntaş, Z. Bozkus, B.B. Fraguela</i>.
<a href="http://gac.udc.es/~basilio/papers/Altuntas16-Docking.pdf">
GPU accelerated molecular docking simulation with genetic algorithms</a>.
<b> 19th European Conf. on Applications of Evolutionary Computation (EvoApps 2016)</b>, pp. 134-146. Porto (Portugal), April 2016.
</li>

<li>
<i>J. F. Fabeiro, D. Andrade,  B.B. Fraguela</i>.
<a href="http://gac.udc.es/~basilio/papers/Fabeiro16-HPLmxproduct.pdf">
Writing a performance-portable matrix multiplication</a>.
<b> Parallel Computing</b>, 52:65-77, February 2016.
</li>

<li>
<i>M. Vi&ntilde;as, B.B. Fraguela, Z. Bozkus, D. Andrade</i>.
<a href="http://gac.udc.es/~basilio/papers/Vinas15-HPLOpenCL.pdf">
Improving OpenCL programmability with the Heterogeneous Programming Library</a>.
<b> International Conference on Computational Science (ICCS 2015)</b>, pp. 110-119. Reykjavik (Iceland), June 2015</li>
</li>

<li>
<i>M. Vi&ntilde;as, Z. Bozkus, B.B. Fraguela, D. Andrade, R. Doallo</i>.
<a href="http://gac.udc.es/~basilio/papers/Vinas15-HPLmultidevice.pdf">
Developing adaptive multi-device applications with the Heterogeneous Programming Library</a>.
<b> The Journal of Supercomputing</b>, 71(6):2204-2220, June 2015.
The final publication is available at <a href="http://link.springer.com">http://link.springer.com</a> <a href="http://link.springer.com/article/10.1007%2Fs11227-014-1352-1">here</a>
</li>

<li>
<i>J.F. Fabeiro, D. Andrade, B.B. Fraguela, R. Doallo</i>.
<a href="http://gac.udc.es/~basilio/papers/Fabeiro14-HPL.pdf">
Writing self-adaptive codes for heterogeneous systems</a>.
<b> 20th International Euro-par Conference (Euro-Par 2014)</b>,  
Lecture Notes in Computer Science Vol. 8632, Springer, pp. 800-811. 
Porto (Portugal), August 2014.
</li>

<li>
<i>M. Vi&ntilde;as, Z. Bozkus, B.B. Fraguela, D. Andrade, R. Doallo</i>.
<a href="http://gac.udc.es/~basilio/papers/Vinas14-multiGPU.pdf">
Exploiting multi-GPU systems using the Heterogeneous Programming Library</a>.
<b> 14th Intl. Conf. on Computational and Mathematical Methods in Science and Engineering (CMMSE 2014)</b>, pp. 1280-1291. Rota (Spain), July 2014.
</li>

<li>
<i>M. Vi&ntilde;as, Z. Bozkus, B.B. Fraguela</i>.
<a href="http://gac.udc.es/~basilio/papers/Vinas13-HPL.pdf">
Exploiting heterogeneous parallelism with the Heterogeneous Programming Library</a>.
<b> Journal of Parallel and Distributed Computing</b>, 73(12):1627-1638, December 2013.
</li>

<li>
<i>Z. Bozkus, B.B. Fraguela.</i>
   <a href="http://gac.udc.es/~basilio/papers/Bozkus12-HPL.pdf">A Portable High-Productivity Approach to Program Heterogeneous Systems</a>. <b>21st International Heterogeneity in Computing Workshop (HCW 2012)</b>, in conjunction with IPDPS'12, pp. 163-173. Shanghai (China). May 2012.
</li>
</lu>


### License

The HPL library is available under the GPL license v3.
