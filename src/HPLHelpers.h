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
/// \file     HPLHelpers.h
/// \author   Moisés Viñas        <moises.vinas@udc.es>
/// \author   Zeki Bozkus         <zeki.bozkus@khas.edu.tr>
/// \author   Basilio B. Fraguela <basilio.fraguela@udc.es>
///

#ifndef HPLHELPERS_H
#define HPLHELPERS_H

#include "FCall.h"

namespace HPL {
  
  
  /// Helper function. where(a,b,c) is expanded to "(a) ? (b) : (c)"
  template<typename T1, typename T2, typename T3>
  Expression<InternalKernelCaller> where(const T1& cond, const T2& a, const T3& b) {
    InternalKernelCaller tmp("((" + stringize(cond) + ") ? (" +  stringize(a) + ") : (" + stringize(b) + "))");
    ArgBehavior<T1>::remove(cond);
    ArgBehavior<T2>::remove(a);
    ArgBehavior<T3>::remove(b);
    return Expression<InternalKernelCaller>(tmp);
  }
  
  template<typename T, typename T2>
  Expression<InternalKernelCaller> cast(const T2& input) {
    char tmpstr[1024];
    sprintf(tmpstr, "((%s)(%s))", TypeInfo<T>::String, stringize(input).c_str());
    const String_t str(tmpstr);
    InternalKernelCaller tmp(str);
    ArgBehavior<T2>::remove(input);
    return Expression<InternalKernelCaller>(tmp);
  }

#define ATOMFUNCTION1ARG(def)	template<typename T> \
  	  	  	  	  	  	  		Expression<InternalKernelCaller> def(const T& input) { \
  	  	  	  	  	  	  		InternalKernelCaller tmp(#def"(&"+ stringize(input) + ")"); \
  	  	  	  	  	  	  		TheGlobalState().getArrayInfo(input.getMultiDimArray())->addAccess(); \
  	  	  	  	  	  	  		TheGlobalState().getArrayInfo(input.getMultiDimArray())->set_isWritten(); \
  	  	  	  	  	  	  		ArgBehavior<T>::remove(input); \
  	  	  	  	  	  	  	  	return Expression<InternalKernelCaller>(tmp); \
  	  	  	  	  	  	  	}
#define ATOMFUNCTION2ARGS(def)	template<typename T1, typename T2> \
  	  	  	  	  	  	  		Expression<InternalKernelCaller> def(const T1& a, const T2& b) { \
  	  	  	  	  	  	  		InternalKernelCaller tmp(#def"(&"+ stringize(a) + ","+ stringize(b)+")"); \
  	  	  	  	  	  	  		TheGlobalState().getArrayInfo(a.getMultiDimArray())->addAccess(); \
  	  	  	  	  	  	  		TheGlobalState().getArrayInfo(a.getMultiDimArray())->set_isWritten(); \
  	  	  	  	  	  	  		ArgBehavior<T1>::remove(a); \
  	  	  	  	  	  	  	  	ArgBehavior<T2>::remove(b); \
  	  	  	  	  	  	  	  	return Expression<InternalKernelCaller>(tmp); \
  	  	  	  	  	  	  	}

ATOMFUNCTION1ARG(atomic_inc);
ATOMFUNCTION1ARG(atomic_dec);
ATOMFUNCTION2ARGS(atomic_add);
ATOMFUNCTION2ARGS(atomic_sub);
ATOMFUNCTION2ARGS(atomic_xchg);
ATOMFUNCTION2ARGS(atomic_min);
ATOMFUNCTION2ARGS(atomic_max);
ATOMFUNCTION2ARGS(atomic_and);
ATOMFUNCTION2ARGS(atomic_or);
ATOMFUNCTION2ARGS(atomic_xor);

  template<typename T1, typename T2, typename T3>
  Expression<InternalKernelCaller> atomic_cmpxchg(const T1& a, const T2& b, const T3& c ) {
    InternalKernelCaller tmp("atomic_cmpxchg(&"+ stringize(a) + ","+ stringize(b)+ "," + stringize(c)+")");
    TheGlobalState().getArrayInfo(a.getMultiDimArray())->addAccess();
    TheGlobalState().getArrayInfo(a.getMultiDimArray())->set_isWritten();
    ArgBehavior<T1>::remove(a);
    ArgBehavior<T2>::remove(b);
    ArgBehavior<T3>::remove(c);
    return Expression<InternalKernelCaller>(tmp);
  }

  /// Generates code to perform a reduction among all the threads in a group, each thread providing a scalar
  /** By default, only thread 0 stores the result.
    * The constructor provides the minimum information needed to generate the code. 
    * All the other methods are optional and they provide information that can optimize the
    * code generated and tune the amount of local memory used for the reduction.
    * Examples:
    * @code
    *   reduce(v[0], var, "+");
    * @endcode
    *   reduces variable \c var from each thread into variable \c v[0] in thread 0 using operator \c "+"
    * @code
    *   reduce(f, var, "max").minGroupSize(16).ndims(1).toAll();
    * @endcode
    *   reduces variable \c var from each thread into variable \c f in all the threads using operator \c "max"
    *   The code is optimized for a 1-dimensional mesh of threads in which there will be at least 16 threads
    *   per local domain.
    */
  class reduce {
    
    static const std::size_t DEFAULT_LOCALMEM = 16;
    
    const String_t dest_;        ///< represents final element where the data will be stored
    const char* const typeName_; ///< datatype of the scalar to reduce
    const String_t expression_;  ///< input expression
    const char* const op_;       ///< reduction operation
    std::size_t grpSz_;          ///< number of threads in the group
    std::size_t ndims_;          ///< number of dimensions of the thread grid
    std::size_t localMem_;       ///< maximum amount of local mem to use
    std::size_t nTeams_;         ///< if != 0, how many groups of consecutive threads reduce a different value
    std::size_t nelems_;         ///< how many elements to reduce from each thread
    std::size_t syncReq_;        ///< lower limit for number of threads that require synchronization
    bool        isMinGrpSz_;     ///< whether grpSz_ is the exact or the minimum group size
    bool        inBinaryTree_;   ///< whether reduction in the local array is performed as binary tree or sequentially
    bool        toAll_;          ///< whether everyone or only thread 0 writes the result
    bool        unroll_;         ///< whether loops must be unrolled
    
    char buf[2048];
    int pos;
    
    void default_initialization();
    
    void docopy(const char *indexdest, const char *value);
    void dolocalarraydef(const char * name, std::size_t nrows, const char *source = 0);
    void dobarrier();
    
  public:
    
    /// Reduce into \c dest the value \c v provided by each thread in a group using operation \c  op
    template<typename TDEST, typename T>
    reduce(const Array<TDEST, 0>& dest, const T& v, const char *op)
    : dest_(dest.name()), typeName_(TypeInfo<TDEST>::String), expression_(stringize(v)), op_(op)
    {
      ArgBehavior<T>::remove(v);
      default_initialization();
    }
    
    /// Reduce into \c dest the value \c v provided by each thread in a group using operation \c  op
    /** @internal Notice that \c dest_ is a String_t because of the need to store dest.string(), 
                  which is a temporary */
    template<typename TDEST, int NDIM, typename T>
    reduce(const IndexedArray<TDEST, NDIM>& dest, const T& v, const char *op)
    : dest_(dest.string()), typeName_(TypeInfo<TDEST>::String), expression_(stringize(v)), op_(op)
    {
      ArgBehavior<T>::remove(v);
      default_initialization();
      TheGlobalState().getArrayInfo(dest.getMultiDimArray())->set_isWritten();
    }
    
    /// Specify the exact number of threads in the local domain
    reduce& groupSize(std::size_t grpSz) { grpSz_ = grpSz; isMinGrpSz_ = false; return *this; }
    
    /// Specify a minimum for the number of threads in the local domain
    reduce& minGroupSize(std::size_t grpSz) { grpSz_ = grpSz; isMinGrpSz_ = true; return *this; }
    
    /// Specify how many groups of consecutive threads in the group act as a unit reducing a different value
    reduce& nTeams(std::size_t nTeams) { nTeams_ = nTeams; return *this; }
    
    /// Indicate how many elements to reduce from each thread
    reduce& nElems(std::size_t nElems) { nelems_ = nElems; return *this; }

    /// Indicate the number of dimensions of the domain in which the kernel is run
    reduce& ndims(std::size_t ndims) { ndims_ = ndims; return *this; }
    
    /// Indicate minimum number of threads that require a barrier synchronization
    reduce& syncReq(std::size_t syncReq) { syncReq_ = syncReq; return *this; }

    /// Request loops to be unrolled or not
    reduce& unroll(bool b) { unroll_ = b; return *this; }
    
    /// Maximum number of elements of the type used in the reduction to reserve in local memory
    /** The more elements allocated, the faster the reduction can take place.
        The helper array is deallocated after the reduction */
    reduce& localMem(std::size_t localmem) { localMem_ = localmem; return *this; }

    /// Requests to perform the reduction of the local array as a parallel binary tree. Otherwise it is made sequentially
    reduce& inTree() { inBinaryTree_ = true; return *this; }
    
    /// Requests that every thread writes the result. By default only thread 0 writes it
    reduce& toAll() { toAll_ = true; return *this; }
    
    ~reduce();
  };

}

#endif
