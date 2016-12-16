#include <iostream>

#include "HPL.h"

#define N 400
#define M 40

using namespace HPL;

int check(bool is_ok)
{
  if(is_ok) {
    std::cout << "Test is PASSED. \n";
    return 0;
  } else {
    std::cout << "Test is FAILED. \n";
    return -1;
  }
}

void transpose(Array<float, 2> m1, Array<float, 2> m2)
{
  m1[idy][idx] = m2[idx][idy];
}

void add(Array<float, 1> a, Array<float, 1> b, Array<float, 1> c)
{
  a[idx] =  b[idx] + c[idx];
}

int main()
{ bool ok = true;
  Array<float, 1> av(N), bv(N);
  Array<float, 1> cv(N);
  Array<float, 2> m1(M, M);
  Array<float, 2> m2(M, M);
  
  for(int i =0; i < N; i++) {
    av(i) = 5.f;
    bv(i) = 6.f;
    cv(i) = 0.f;
  }

  for(int i = 0; i< M; i++)
    for(int j = 0; j < M; j++) {
      m1(i,j) = (float)(i%10);
      m2(i,j) = (float)(j%10);
    }
  
  Array<float, 1> &da = av(Range(0,4));
  Array<float, 1> &db = bv(Range(5,9));
  
  for(int i = 0; i < 5; i++)
  {
    da(i) = 1.f; //These accesses do not cause copies in GPU because they are updated in the host
    db(i) = 2.f; //These accesses do not cause copies in GPU because they are updated in the host
  }
  
  eval(add)(cv, av, bv);
  
  std::cout << "Ten first elements of a and c obtained through their delegate:" << std::endl;
  for(int i = 0; i < 10; i++)
  {
    std::cout << "a[" << i << "]=" << da(i) << ", c[" << i << "]= " << cv(i) << std::endl;  //The c content is copied to its hostside
    ok = ok && (((i < 5) && (da(i) == 1.f)) || ((i >=5) && (da(i) == 5.f)));
    ok = ok && (cv(i) == 7.f);
    if (!ok) {
      return check(ok);
    }
  }
  
  Array<float,2> &dm1 = m1(Range(10,29), Range(10,29)); //Central square
  
  for(int i = 0; i < 20; i++)
    for(int j = 0; j < 20; j++)
      dm1(i, j) = ((i>=10 & j>=10 & i<15) ? 1.0 : 0.0); //These accesses only modify the host side of m1
  
  std::cout << "Before trasposing..." << std::endl;
  for(int i = 0; i < M; i++)
  {
    for(int j = 0; j < M; j++)
    {
      std::cout << m1(i,j); //These accesses do not 'go' to the device
    }
    std::cout << std::endl;
  }
  
  eval(transpose)(m2, m1);	//m1 is copied to device
  
  Array<float, 2> &dm2 = m2(Range(10,29), Range(10,29)); //Central square
  
  dm2 = dm1;   //We bring dm1 (it is already in the hostside) and we copy the selected subarray in m2.
  
  std::cout << "After trasposing... and after substitution of the delegate of dm1" << std::endl;
  for(int i = 0; i < M; i++)
  {
    for(int j = 0; j < M; j++)
    {
      std::cout << m2(i,j); //The bring all the m2
      ok = ok && ( ((i>=10) && (i<=29) && (j>=10) && (j<=29)) ? (m2(i,j) == m1(i,j)) : (m2(i,j) == m1(j,i)) );
      if (!ok) {
        return check(ok);
      }
    }
    std::cout << std::endl;
  }
  
  eval(transpose)(m1, m2); //This call does not causes any copy because m2 is updated in the device
  
  std::cout << "After trasposing again... " << std::endl;
  for(int i = 0; i < M; i++)
  {
    for(int j = 0; j < M; j++)
    {
      std::cout << m1(i,j);
      ok = ok &&  (m1(i,j) == m2(j,i));
      if (!ok) {
        return check(ok);
      }
    }
    std::cout << std::endl;
  }
  
  /* Here we copy a part of av in the last row of the matrix,
     it causes a copy to device of that part from m1 that is in host side.
   */
  m1(Range(M-1, M-1), Range(0, M-1)) = av(Range(0, M-1));
  
  std::cout << "...and after substitution of the last row" << std::endl;
  for(int i = 0; i < M; i++)
  {
    for(int j = 0; j < M; j++)
    {
      std::cout << m1(i,j); //This access does not cause a copy of memory because m1 is updated in host
      ok = ok && ( (i < M-1) ? (m1(i,j) == m2(j,i)) : (m1(i,j) == av(j)) );
      if (!ok) {
        return check(ok);
      }
    }
    std::cout << std::endl;
  }
  
  std::cout << "new content of bv (bv = av) \n";
  
  bv = av; //This access does not cause a copy of memory because av is updated in host
  
  for(int i = 0; i < M; i++) {
    std::cout << bv(i); //This access does not cause a copy of memory because av is updated in host
    ok = ok && (bv(i) == av(i));
    if (!ok) {
      return check(ok);
    }
  }
  std::cout << std::endl;
  
  std::cout << "cv = av + bv (this is, cv = 2*av)\n";
  
  eval(add)(cv, av, bv); //bv is copied to device
  for(int i = 0; i < M; i++) {
    std::cout << cv(i);
    ok = ok && (cv(i) == (av(i) + bv(i)));
    if (!ok) {
      return check(ok);
    }
  }
  
  cv(Range(0,N-1)) = bv; //cv is updated in device?? with the data of bv
  
  std::cout << "\ncv(0,N) = bv (this is, cv == bv == av)\n";
  for(int i = 0; i < M; i++) {
    std::cout << cv(i); // Causes D --> H??
    ok = ok && (cv(i) == bv(i));
    if (!ok) {
      return check(ok);
    }
  }
  std::cout << std::endl;
  for(int i = 0; i < M; i++)
    std::cout << av(i);
  std::cout << std::endl;
  for(int i = 0; i < M; i++)
    std::cout << bv(i);
  std::cout << std::endl;
  
  Array<float,2> &dm3 = m1(Range(M-1,M-1), Range(0,M-1));
  
  int i;
  for(i = 0; i < M; i++)
  {
    if(cv(i) != dm3(0,i)) //This access does not cause copy of memory because m1 is in hostside
      break;
  }
  
  return check(i== M);
  
}




