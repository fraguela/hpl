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
/// \file     FCall.h
/// \author   Moisés Viñas        <moises.vinas@udc.es>
/// \author   Zeki Bozkus         <zeki.bozkus@khas.edu.tr>
/// \author   Basilio B. Fraguela <basilio.fraguela@udc.es>
///

#ifndef FCALL_H
#define FCALL_H

#include "ExprManipulation.h"
#include "FHandle.h"
#include "Array.h"

namespace HPL {

  /**
   * @brief Utility class to manage the functions calls from kernel codes.
   */
  class InternalKernelCaller {

    FHandle * const fh_;	     ///< Object HPL::FHandle that represents this function.
    String_t s_;	             ///< Code that represents the calling instruction in the kernel code.
    int accesses[MAX_KERNEL_NARG];

    // Terminates recursion
    void internal_arg_process(unsigned int n) { }
    
    template<typename Head, typename... Args>
    void internal_arg_process(unsigned int n, const Head& arg1, const Args&... args) {
      s_ +=  stringize(arg1) + ", ";
      internal_arg_process(n + 1, args...);
      updateCallerIntents(n, &arg1);
    }
    
  public:
    
    /**
     * @brief Default constructor
     * @param[in] f Called function.
     */
    InternalKernelCaller(void *f);
    
    /// Allows to use InternalKernelCaller to represent any string expression
    InternalKernelCaller(const String_t& s);
    
    /**
     * @name These function calls in kernels are another type of expressions.
     */
    ///@{

    /** @internal This version works a bit differently internally, so it has a separate implementation */
    Expression<InternalKernelCaller> operator()();
    
    template<typename Head, typename... Args>
    Expression<InternalKernelCaller> operator()(const Head& arg1, const Args&... args) {
      
      internal_arg_process(0, arg1, args...);
      
      s_.erase(s_.size() - 2, 2); //remove last ", "
      
      s_ = fh_->name_ + '(' + s_ + ')';
      
      return Expression<InternalKernelCaller>(*this);
    }
    
    ///@}

    FHandle* getFHandle()
    {
    	return fh_;
    }

    /**
     * @brief Returns the string representation of this expression.
     */
    String_t string() const { return s_; }
    
    /**
     * @brief Sets the accesses made on the arguments of this function.
     */
    void setAccesses(int index, int num_accesses)
    {
    	accesses[index] = num_accesses;
    }

  private:
    
    /**
     * @brief This method updates the caller intents from the
     * called function.
     */
    void updateCallerIntents(int index, const BaseArray* arg)
    {
    	if(fh_->fhandleTable[index].isWrite && !fh_->fhandleTable[index].isWriteBeforeRead)
    	{
    		TheGlobalState().getArrayInfo(arg)->force_isWritten();
    	}
    	if(fh_->fhandleTable[index].isWriteBeforeRead)
    	{
    		TheGlobalState().getArrayInfo(arg)->addAccess();
    		TheGlobalState().getArrayInfo(arg)->set_isWritten();
    	}
    	if(!fh_->fhandleTable[index].isWrite)
    	{
    		TheGlobalState().getArrayInfo(arg)->addAccess(accesses[index]);
    	}
    }
    
    template<typename T, int NDIM>
    void updateCallerIntents(int index, const IndexedArray<T, NDIM> *ciia) {
      const BaseArray* const p = ciia->getBaseArray();
      if(p)
	updateCallerIntents(index, p);
    }
    
    template<typename T>
    void updateCallerIntents(int index, const AliasArray<T> *ciaa) {
      updateCallerIntents(index, ciaa->getUnderlyingBaseArray());
    }
    
    template<typename T>
    void updateCallerIntents(int index, const Expression<T> *expptr) {
      BaseExpression::removeLast();
    }
    
  };

  
  
  /// Interface to call functions inside the kernels

  /** @name Device functions invocation interface.
   *  Finds the FHandle object for a function. It builds the object if it does not exists.
   *  The call method can be launched recursively. In each iteration the call method
   *  increments the nesting level and selecting a distinct codifier each time. In this way,
   *  we can store the codifier state of each nesting level separately guaranteeing the correct
   *  write of the kernel codes.
   */
  ///@{
  
  /// Convenience macro to test whether data type \c TYPE is not a reference and derives from InternalScalar
#define ISSCALARBYVALUE(TYPE) (is_nonref_derived_from<TYPE, InternalScalar>::value)
  
  template<int N, int NARGS, int SCALARSBYVALUE>
  struct internal_call {
    template<typename RET, typename... Args, typename... BuiltArgs>
    static void eval(RET (&&f) (Args...), InternalKernelCaller& res, BuiltArgs&&... args) {
      typedef typename Select_type<N, Args...>::type actual_type;
      typename std::remove_reference<actual_type>::type new_arg;
      internal_call<N+1, NARGS, SCALARSBYVALUE + ISSCALARBYVALUE(actual_type)>::eval(std::forward<RET(Args...)>(f), res, args..., new_arg);
      res.getFHandle()->copyFormalArrayInfo(N, &new_arg);
      res.setAccesses(N, TheGlobalState().getArrayInfo(&new_arg)->getAccesses());
    }
  };
  
  template<int NARGS, int SCALARSBYVALUE>
  struct internal_call<NARGS, NARGS, SCALARSBYVALUE> {
    template<typename RET, typename... Args, typename... BuiltArgs>
    static void eval(RET (&&f) (Args...), InternalKernelCaller& res, BuiltArgs&&... args) {
      Codifier &cod = TheGlobalState().getCodifier();
      cod.setnScalarsByValue(SCALARSBYVALUE);
      cod.state(BuildingFBody);
      f(args...);
      res.getFHandle()->endDefinition(TypeInfo<RET>::String);
      res.getFHandle()->checkPrivateArgs();
    }
  };
  
  template<typename RET, typename... Args>
  InternalKernelCaller call(RET (*f)(Args...))
  {
    InternalKernelCaller res = InternalKernelCaller(reinterpret_cast<void *>(f));
    if(!res.getFHandle()->numArgs_) {
      TheGlobalState().addCodifierNest();
      BaseExpression::pushLiveExpressions();
      res.getFHandle()->fillIn(sizeof...(Args));
      internal_call<0, sizeof...(Args), 0>::eval(std::forward<RET(Args...)>(*f), res);
      BaseExpression::popLiveExpressions();
      TheGlobalState().subCodifierNest();
      res.getFHandle()->fprint();
    }
    return res;
  }
  
  ///@}
  
};

/**
 * @brief Processes an InternalKernelCaller leaf in a PETE expression template to
 *  return a string representing it.
 */
template<>
struct LeafFunctor<HPL::InternalKernelCaller, StringizeExpr>
{
  typedef HPL::String_t Type_t;
  static Type_t apply(const HPL::InternalKernelCaller &ikc, const StringizeExpr &)
  { return ikc.string(); }
};

#endif
