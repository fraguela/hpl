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
/// \file     VTypes.h
/// \author   Moisés Viñas        <moises.vinas@udc.es>
/// \author   Zeki Bozkus         <zeki.bozkus@khas.edu.tr>
/// \author   Basilio B. Fraguela <basilio.fraguela@udc.es>
///

#ifndef VTYPES_H
#define VTYPES_H

#include <iostream>
#include <cassert>
#include "HPL_utils.h"

/** @file
    @brief Vector datatypes.
*/

namespace HPL {
  
  /// Common ancestor to all the vector datatypes.
  struct VectorType {
    static const String_t& getFieldText(int i);
    static String_t string(const int* v_, int n);
    static String_t string(const float* v_, int n);
    static String_t string(const double* v_, int n);
  };

  
  template<typename T, int N>
  struct InternalVector : public VectorType {
    
    T v_[N];
    
    InternalVector(T v0 = T()) {
      for(int i = 0; i < N; ++i)
	v_[i] = v0;
    }
    
    InternalVector(T v0, T v1) {
      static_assert(N == 2, "Constructor for vectors of length 2");
      v_[0] = v0;
      v_[1] = v1;
    }
    
    InternalVector(T v0, T v1, T v2, T v3) {
      static_assert(N == 4, "Constructor for vectors of length 4");
      v_[0] = v0;
      v_[1] = v1;
      v_[2] = v2;
      v_[3] = v3;
    }
    
    InternalVector(T v0, T v1, T v2, T v3, T v4, T v5, T v6, T v7) {
      static_assert(N == 8, "Constructor for vectors of length 8");
      v_[0] = v0;
      v_[1] = v1;
      v_[2] = v2;
      v_[3] = v3;
      v_[4] = v4;
      v_[5] = v5;
      v_[6] = v6;
      v_[7] = v7;
    }
    
    InternalVector(T v0, T v1, T v2, T v3, T v4, T v5, T v6, T v7,
		   T v8, T v9, T va, T vb, T vc, T vd, T ve, T vf) {
      static_assert(N == 16, "Constructor for vectors of length 16");
      v_[0] = v0;
      v_[1] = v1;
      v_[2] = v2;
      v_[3] = v3;
      v_[4] = v4;
      v_[5] = v5;
      v_[6] = v6;
      v_[7] = v7;
      v_[8] = v8;
      v_[9] = v9;
      v_[10] = va;
      v_[11] = vb;
      v_[12] = vc;
      v_[13] = vd;
      v_[14] = ve;
      v_[15] = vf;
    }
    
    /// Copy constructor
    InternalVector(const InternalVector& other) {
      for(int i = 0; i < N; ++i)
	v_[i] = other.v_[i];
    }

    /// Assignment operator
    InternalVector& operator= (const InternalVector& other) {
      for(int i = 0; i < N; ++i)
	v_[i] = other.v_[i];
      return *this;
    }
    
    InternalVector& operator=(T v) {
      for(int i = 0; i < N; ++i)
	v_[i] = v;
      return *this;
    }
    
    String_t string() const { return VectorType::string(v_, N); }
    
    std::size_t length() const { return N; }
    
    T& operator()(int i) { assert((i>=0) && (i < N)); return v_[i]; }
    
    T operator()(int i) const { assert((i>=0) && (i < N)); return v_[i]; }
    
    bool operator!= (const InternalVector& other) const {
      for(int i =0; i < N; ++i)
	if(v_[i] != other.v_[i]) return true;
      return false;
    }
  };
  
  /// Dumps any InternalVector to a std::ostream
  template<typename T, int NDIM>
  std::ostream &operator<<(std::ostream &os, const InternalVector<T, NDIM> &a)
  {
    os << a.string();
    return os;
  }
  
  /// Equivalent to float16 of OpenCL
  typedef InternalVector<float, 16> float16;
    
  /// Equivalent to float8 of OpenCL.
  typedef InternalVector<float, 8> float8;
  
  /// Equivalent to float4 of OpenCL.
  typedef InternalVector<float, 4> float4;
  
  /// Equivalent to float2 of OpenCL.
  typedef InternalVector<float, 2> float2;
  
  /// Equivalent to int16 of OpenCL.
  typedef InternalVector<int, 16> int16;
  
  /// Equivalent to int8 of OpenCL.
  typedef InternalVector<int, 8> int8;
  
  /// Equivalent to int4 of OpenCL.
  typedef InternalVector<int, 4> int4;
  
  /// Equivalent to int2 of OpenCL.
  typedef InternalVector<int, 2> int2;

}


#endif /* VTYPES_H */
