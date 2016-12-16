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
/// \file     Stringizer.h
/// \author   Moisés Viñas        <moises.vinas@udc.es>
/// \author   Zeki Bozkus         <zeki.bozkus@khas.edu.tr>
/// \author   Basilio B. Fraguela <basilio.fraguela@udc.es>
///

#ifndef STRINGIZER_H
#define STRINGIZER_H

#include "HPL_utils.h"
#include "VTypes.h"

/** @file
    @brief functions/traits to get a string representation of objects/basic dadatypes
*/

namespace HPL {

  /** @brief Trait to get a const char* name for data type \p T
   @tparam T datatype whose name is requested
   */
  template<typename T>
  struct TypeInfo {
    static const char* String;
    static const int Length;
  };
  
  template<typename T, int N> 
  struct  TypeInfo<HPL::InternalVector<T, N> >{
    static const char* String;
    static const int Length = N;
  };
  
  /// Default empty string returned. The trait must be specialized for each data type
  template<typename T> const char* TypeInfo<T>::String = "";
  template<typename T> const int TypeInfo<T>::Length = 0;
  
  template<> const char* TypeInfo<double>::String;
  template<> const char* TypeInfo<float>::String;
  template<> const char* TypeInfo<float16>::String;
  template<> const char* TypeInfo<float8>::String;
  template<> const char* TypeInfo<float4>::String;
  template<> const char* TypeInfo<float2>::String;
  template<> const char* TypeInfo<int16>::String;
  template<> const char* TypeInfo<int8>::String;
  template<> const char* TypeInfo<int4>::String;
  template<> const char* TypeInfo<int2>::String;
  template<> const char* TypeInfo<int>::String;
  template<> const char* TypeInfo<unsigned int>::String;
  template<> const char* TypeInfo<char>::String;
  template<> const char* TypeInfo<unsigned char>::String;
  template<> const char* TypeInfo<void>::String;
#ifdef HPL_64BIT_MACHINE
  template<> const char* TypeInfo<std::size_t>::String;
#endif
  
  
  
  /** @name Stringization functions
   * Return string representation of the input object \p v
   */
  ///@{
  template<class T>
  String_t stringize(const T& v)
  {
    return v.string();
  }
  
  String_t stringize(int v);
  
  String_t stringize(unsigned int v);
  
  String_t stringize(float v);
  
  String_t stringize(double v);
  
  String_t stringize(const std::string& v);
  
  String_t stringize(const char *v);

#ifdef HPL_64BIT_MACHINE
  String_t stringize(std::size_t v);
#endif

  /*
  String_t stringize(Vf16 v);
  String_t stringize(Vf8 v);
  String_t stringize(Vf4 v);
  String_t stringize(Vf2 v);
  String_t stringize(Vi16 v);
  String_t stringize(Vi8 v);
  String_t stringize(Vi4 v);
  String_t stringize(Vi2 v);
   */
  
  ///@}
  
  
};

#endif
