/*
 HPL : Heterogeneous Programming Library
 Copyright (c) 2012-2016 The HPL Team as listed in CREDITS.txt

 This file is part of HPL

 HPL is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 HPL is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with HPL.  If not, see <http://www.gnu.org/licenses/>.
*/

///
/// \file     Stringizer.cpp
/// \author   Moisés Viñas        <moises.vinas@udc.es>
/// \author   Zeki Bozkus         <zeki.bozkus@khas.edu.tr>
/// \author   Basilio B. Fraguela <basilio.fraguela@udc.es>
///

#include <cstdio>
#include "Stringizer.h"

namespace HPL {
  
  String_t stringize(int v)
  { char tmp[32];
    
    sprintf(tmp, "%d", v);
    return String_t(tmp); 
  }
  
  String_t stringize(unsigned int v)
  { char tmp[32];
    
    sprintf(tmp, "%u", v);
    return String_t(tmp); 
  }
  
  String_t stringize(float v)
  { char tmp[32];
    
    sprintf(tmp, "%ff", v);
    return String_t(tmp); 
  }
  
  String_t stringize(double v)
  { char tmp[32];
    
    sprintf(tmp, "%25.15E", v);
    return String_t(tmp); 
  }
  
  String_t stringize(const std::string& v)
  {
    return v;
  }
  
  String_t stringize(const char *v)
  {
    return String_t(v);
  }

#ifdef HPL_64BIT_MACHINE  
  String_t stringize(std::size_t v)
  { char tmp[32];
    
    sprintf(tmp, "%zu", v);
    return String_t(tmp);
  }
#endif
  
  /*
  String_t stringize(Vf16 v)
  {
	  char tmp[64];

	  sprintf(tmp, "{%.f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f}",
			  v.s0, v.s1, v.s2, v.s3, v.s4, v.s5, v.s6, v.s7, v.s8,
			  v.s9, v.s10, v.s11, v.s12, v.s13, v.s14, v.s15);
	  return String_t(tmp);
  }

  String_t stringize(Vf8 v)
  {
	  char tmp[32];

	  sprintf(tmp, "{%f,%f,%f,%f,%f,%f,%f,%f}",
			  v.s0, v.s1, v.s2, v.s3, v.s4, v.s5, v.s6, v.s7);
	  return String_t(tmp);
  }

  String_t stringize(Vf4 v)
  {
	  char tmp[32];

	  sprintf(tmp, "{%f,%f,%f,%f}",
			  v.s0, v.s1, v.s2, v.s3);
	  return String_t(tmp);
  }

  String_t stringize(Vf2 v)
  {
	  char tmp[32];

	  sprintf(tmp, "{%f,%f}",
			  v.s0, v.s1);
	  return String_t(tmp);
  }

  String_t stringize(Vi16 v)
  {
	  char tmp[64];

	  sprintf(tmp, "{%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d}",
			  v.s0, v.s1, v.s2, v.s3, v.s4, v.s5, v.s6, v.s7, v.s8,
			  v.s9, v.s10, v.s11, v.s12, v.s13, v.s14, v.s15);
	  return String_t(tmp);
  }

  String_t stringize(Vi8 v)
  {
	  char tmp[32];

	  sprintf(tmp, "{%d,%d,%d,%d,%d,%d,%d,%d}",
			  v.s0, v.s1, v.s2, v.s3, v.s4, v.s5, v.s6, v.s7);
	  return String_t(tmp);
  }

  String_t stringize(Vi4 v)
  {
	  char tmp[32];

	  sprintf(tmp, "{%d,%d,%d,%d}",
			  v.s0, v.s1, v.s2, v.s3);
	  return String_t(tmp);
  }

  String_t stringize(Vi2 v)
  {
	  char tmp[32];

	  sprintf(tmp, "{%d,%d}",
			  v.s0, v.s1);
	  return String_t(tmp);
  }

*/
  
  template<> const char*  TypeInfo<double>::String = "double";
  template<> const char*  TypeInfo<float>::String = "float";
  template<> const char*  TypeInfo<float16>::String = "float16";
  template<> const char*  TypeInfo<float8>::String = "float8";
  template<> const char*  TypeInfo<float4>::String = "float4";
  template<> const char*  TypeInfo<float2>::String = "float2";
  template<> const char*  TypeInfo<int16>::String = "int16";
  template<> const char*  TypeInfo<int8>::String = "int8";
  template<> const char*  TypeInfo<int4>::String = "int4";
  template<> const char*  TypeInfo<int2>::String = "int2";
  template<> const char*  TypeInfo<int>::String = "int";
  template<> const char*  TypeInfo<unsigned int>::String = "uint";
  template<> const char*  TypeInfo<char>::String = "char";
  template<> const char*  TypeInfo<unsigned char>::String = "uchar";
  template<> const char*  TypeInfo<void>::String = "void";
#ifdef HPL_64BIT_MACHINE
  template<> const char*  TypeInfo<size_t>::String = "size_t";
#endif
  
}
