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
/// \file     FCall.cpp
/// \author   Moisés Viñas        <moises.vinas@udc.es>
/// \author   Zeki Bozkus         <zeki.bozkus@khas.edu.tr>
/// \author   Basilio B. Fraguela <basilio.fraguela@udc.es>
///

#include "FCall.h"

namespace HPL {
  
  InternalKernelCaller::InternalKernelCaller(void *f)
  : fh_(&(TheGlobalState().getFHandle(f)))
  { }
  
  InternalKernelCaller::InternalKernelCaller(const String_t& s)
  : fh_(0), s_(s)
  { }
  
  Expression<InternalKernelCaller> InternalKernelCaller::operator()() {
    if(fh_) //This is a regular InternalKernelCaller. Otherwise it just stores a string
      s_ = fh_->name_ + "()";
    
    return Expression<InternalKernelCaller>(*this);
  }
  
  /*
  template<> 
  void InternalKernelCaller::remove<BaseExpression> (const BaseExpression& v) {
    fprintf(stderr, "RemovingLast!\n");
    BaseExpression::removeLast();
  }
  */
}
