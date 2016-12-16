#include <iostream>

#include "HPL.h"

#define N 400

using namespace HPL;

float a[N], b[N], c[N], d[N];

void myf(Array<float, 1> a, Array<float, 1> b, Array<float, 1> c, Array<float, 1> d)
{
  a[idx] += b[idx] + (c[idx]=d[idx]);
  b[idx] += d[idx];
  d[idx] = a[idx]+b[idx];
}

int main()
{ Array<float, 1> av(N, a), bv(N, b), cv(N, c), dv(N, d);

  eval(myf)(av, bv, cv, dv);

  std::cout << "Kernel to check array intent:" << std::endl;
  std::cout << "-----------------------------" << std::endl;
  std::cout << "void myf(Array<float, 1> a, Array<float, 1> b, Array<float, 1> c, Array<float, 1> d)\n";
  std::cout << "{" << std::endl;
  std::cout << "    Int i;" << std::endl;
  std::cout << "    get_global_id(i);" << std::endl;
  std::cout << "    a[i] += b[i] + (c[i]=d[i]);"<< std::endl;
  std::cout << "    b[i] += d[i];" << std::endl;
  std::cout << "    d[i] = a[i]+b[i];" << std::endl;
  std::cout << "}" << std::endl;

  //Checking intent
  std::cout << "Checking intent...\n";

  FHandle &fh_myf = TheGlobalState().getFHandle((void*)myf);
  if(fh_myf.fhandleTable[0].isWrite)
  {
	  if(fh_myf.fhandleTable[0].isWriteBeforeRead)
	  	  std::cout << "The array a is written before being read. \n";
	  else
		  std::cout << "The array a is read before written. \n";
  }
  else
	  std::cout << "The array a is only read. \n";

  if(fh_myf.fhandleTable[1].isWrite)
  {
	  if(fh_myf.fhandleTable[1].isWriteBeforeRead)
		  std::cout << "The array b is written before being read. \n";
	  else
		  std::cout << "The array b is read before written. \n";
  }
  else
	  std::cout << "The array b is only read. \n";

  if(fh_myf.fhandleTable[2].isWrite)
  {
	  if(fh_myf.fhandleTable[2].isWriteBeforeRead)
		  std::cout << "The array c is written before being read. \n";
	  else
		  std::cout << "The array c is read before written. \n";
  }
  else
	  std::cout << "The array c is only read. \n";

  if(fh_myf.fhandleTable[3].isWrite)
  {
	  if(fh_myf.fhandleTable[3].isWriteBeforeRead)
		  std::cout << "The array d is written before being read. \n";
	  else
		  std::cout << "The array d is read before written. \n";
  }
  else
	  std::cout << "The array d is only read. \n";

  //This avoids that program finishes before the eval
  d[0] = dv(0);

  if(fh_myf.fhandleTable[0].isWrite &
     !fh_myf.fhandleTable[0].isWriteBeforeRead &
     fh_myf.fhandleTable[1].isWrite &
     !fh_myf.fhandleTable[1].isWriteBeforeRead &
     fh_myf.fhandleTable[2].isWrite &
     fh_myf.fhandleTable[2].isWriteBeforeRead &
     fh_myf.fhandleTable[3].isWrite &
     !fh_myf.fhandleTable[3].isWriteBeforeRead) {
    std::cout << "Test is PASSED. \n";
    return 0;
  } else {
    std::cout << "Test is FAILED. \n";
    return -1;
  }
}




