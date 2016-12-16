#include <iostream>
#include "HPL_clBLAS.h"

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
#define N 32

int test0()
{
  HPL::Array<double, 2> x(8,8);
  HPL::Array<double, 2> y(8,8);
  HPL::Array<double, 2> z(8,8);
  
  HPL_clblasSetup();
  
  for(int i = 0; i < 8; i++) {
   for(int j = 0; j < 8; j++)
   {
    x(i,j) = i + j + 1;        
    y(i,j) = 5 * (i+ j +1);    
   }
  }
  
  try { clblasDgemm( x, y, z); } catch (HPL::HPLException& e) { std::cout << e.what();  if(e.getErrCode() == -33) std::cout << "The test device probably does not support double precision\n"; } 
  

  HPL_clblasTeardown();
  
  return check((z(0,0) == 1020.f) && (z(1,1) == 1420.f) && (z(5,5) == 3820.f) && (z(7,7) == 5500.f)); 

}

int test1()
{
  HPL::Array<double, 2> x(4,8);
  HPL::Array<double, 2> y(8,6);
  HPL::Array<double, 2> z(4,6);
  
  HPL_clblasSetup();
  
  for(int i = 0; i < 4; i++) {
   for(int j = 0; j < 8; j++)
   {
    x(i,j) = i + j + 1;        
   }
  }
  for(int i = 0; i < 8; i++)
  {
   for(int j = 0; j < 6; j++)
    y(i,j) = 5 * (i+ j +1) ;
  }


  /*
   * Example of clBlasHPL with subarrays.
   * The following product is:
   * X = | X1 X2 |    Y = | Y1 Y2 |  Z = | X1Y1 X2Y2 |
   *     | X3 X4 |        | Y3 Y4 |      | X3Y3 X4Y4 |
   */

  try { clblasDgemm( x(HPL::Range(0,1), HPL::Range(0,3)), y(HPL::Range(0,3), HPL::Range(0,2)), z(HPL::Range(0,1), HPL::Range(0,2))); } catch (HPL::HPLException& e) { std::cout << e.what();  if(e.getErrCode() == -33) std::cout << "The test device probably does not support double precision\n"; }  //X1Y1
  try { clblasDgemm( x(HPL::Range(0,1), HPL::Range(4,7)), y(HPL::Range(0,3), HPL::Range(3,5)), z(HPL::Range(0,1), HPL::Range(3,5))); } catch (HPL::HPLException& e) { std::cout << e.what();  if(e.getErrCode() == -33) std::cout << "The test device probably does not support double precision\n"; }  //X2Y2
  try { clblasDgemm( x(HPL::Range(2,3), HPL::Range(0,3)), y(HPL::Range(4,7), HPL::Range(0,2)), z(HPL::Range(2,3), HPL::Range(0,2))); } catch (HPL::HPLException& e) { std::cout << e.what();  if(e.getErrCode() == -33) std::cout << "The test device probably does not support double precision\n"; }  //X3Y3
  try { clblasDgemm( x(HPL::Range(2,3), HPL::Range(4,7)), y(HPL::Range(4,7), HPL::Range(3,5)), z(HPL::Range(2,3), HPL::Range(3,5))); } catch (HPL::HPLException& e) { std::cout << e.what();  if(e.getErrCode() == -33) std::cout << "The test device probably does not support double precision\n"; }  //X4Y4
  

  HPL_clblasTeardown();
  
  return check((z(0,0) == 150.f) && (z(1,5) == 1150.f) && (z(3,0) == 740.f) && (z(3,5) == 2210.f)); 

}

int test2()
{
  const int NN = 1024;
  const int MM = 1024;
  const int PP = 256;
  HPL::Array<double, 2> x(NN,MM);
  HPL::Array<double, 2> y(MM,PP);
  HPL::Array<double, 2> z(NN,PP);
  
  HPL_clblasSetup();
  
  for(int i = 0; i < NN; i++) {
   for(int j = 0; j < MM; j++)
   {
    x(i,j) = i + j + 1;        
   }
  }
  for(int i = 0; i < MM; i++)
  {
   for(int j = 0; j < PP; j++)
    y(i,j) = (i+j+1) % 5;
  }

  /*
   * Sgemm 
   * Z = X * Y
   */
  try { clblasDgemm( x, y, z); } catch (HPL::HPLException& e) { std::cout << e.what();  if(e.getErrCode() == -33) std::cout << "The test device probably does not support double precision\n"; } 
  
  int ret = check((z(0,0) == 1051650.f) && (z(1,1) == 1051649.f) && (z(5,5) == 1061900.f) && (z(7,7) == 1062911.f));
  for(int i = 0; i < NN; i++)
    for(int j = 0; j < PP; j++)
      z(i,j) = 0.0;

  return ret; // BBF

  /*
   * Example of clBlasHPL with subarrays.
   * The following product is:
   * X = | X1 X2 |    Y = | Y1 Y2 |  Z = | X1Y1 X2Y2 |
   *     | X3 X4 |        | Y3 Y4 |      | X3Y3 X4Y4 |
   */
  try { clblasDgemm( x(HPL::Range(0,NN/2-1), HPL::Range(0,MM/2-1)), y(HPL::Range(0,MM/2-1), HPL::Range(0,PP/2-1)), z(HPL::Range(0, NN/2-1), HPL::Range(0,PP/2-1))); } catch (HPL::HPLException& e) { std::cout << e.what();  if(e.getErrCode() == -33) std::cout << "The test device probably does not support double precision\n"; }      //X1Y1
  try { clblasDgemm( x(HPL::Range(0,NN/2-1), HPL::Range(MM/2,MM-1)), y(HPL::Range(0,MM/2-1), HPL::Range(PP/2,PP-1)), z(HPL::Range(0, NN/2-1), HPL::Range(PP/2,PP-1))); } catch (HPL::HPLException& e) { std::cout << e.what();  if(e.getErrCode() == -33) std::cout << "The test device probably does not support double precision\n"; }   //X2Y2
  try { clblasDgemm( x(HPL::Range(NN/2,NN-1), HPL::Range(0,MM/2-1)), y(HPL::Range(MM/2,MM-1), HPL::Range(0,PP/2-1)), z(HPL::Range(NN/2, NN-1), HPL::Range(0,PP/2-1))); } catch (HPL::HPLException& e) { std::cout << e.what();  if(e.getErrCode() == -33) std::cout << "The test device probably does not support double precision\n"; }   //X3Y3
  try { clblasDgemm( x(HPL::Range(NN/2,NN-1), HPL::Range(MM/2,MM-1)), y(HPL::Range(MM/2,MM-1), HPL::Range(PP/2,PP-1)), z(HPL::Range(NN/2, NN-1), HPL::Range(PP/2,PP-1))); } catch (HPL::HPLException& e) { std::cout << e.what();  if(e.getErrCode() == -33) std::cout << "The test device probably does not support double precision\n"; }//X4Y4

  HPL_clblasTeardown();
  
  return ret + check((z(0,0) == 262145.f) && (z(1,240) == 786944.f) && (z(600,6) == 877054.f) && (z(768,250) == 1578241.f));

}

int main()
{
  test0();
  test1();
  test2();
}

