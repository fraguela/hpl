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
/// \file     HPL.h
/// \author   Moisés Viñas        <moises.vinas@udc.es>
/// \author   Zeki Bozkus         <zeki.bozkus@khas.edu.tr>
/// \author   Basilio B. Fraguela <basilio.fraguela@udc.es>
///

#ifndef HPL_H
#define HPL_H

#include "Array.h"
#include "FHandle.h"
#include "FRunner.h"
#include "HPLHelpers.h"
#include <functional>
#include <string.h>

/** @file
 @brief Top level header file to be included in the client code.
 */

namespace HPL {

  /** @name Control Macros 
   * Macros for control structures inside user functions to be built and compiled by the library.
   */
  ///@{
#define HPL_common_block_macro for(int _hpl_tmp = 0; _hpl_tmp < 1; TheGlobalState().getCodifier().endBlock(), ++_hpl_tmp)
  
#define if_(x)        TheGlobalState().getCodifier().if_(stringize(x));      HPL_common_block_macro
#define else_if_(x)   TheGlobalState().getCodifier().else_if_(stringize(x)); HPL_common_block_macro
#define else_         TheGlobalState().getCodifier().else_();                HPL_common_block_macro        
#define while_(x)     TheGlobalState().getCodifier().while_(stringize(x));   HPL_common_block_macro
#define for_(a,b,c)   TheGlobalState().getCodifier().for_(stringize(a), stringize(b), stringize(c)); HPL_common_block_macro
#define endif_
#define endwhile_     
#define endfor_     
#define end_        
#define break_ TheGlobalState().getCodifier().break_();
  ///@}

  /// Empty return statement
  void return_();
  
  /// Return statement with argument
  template<typename T>
  void return_(const T& retval) {
    TheGlobalState().getCodifier().return_(stringize(retval));
    //ArgBehavior<T>::remove(retval); is not needed because return_ performs a BaseExpression::clearPendingExpressions()
  }
  
  /** @name Identification Functions.
   * They retrieve the ID of the current thread and the working space size.
   */
  ///@{
  
  void get_global_id(const Array<int, 0>& i);
  void get_global_id(const Array<int, 0>& i, const Array<int, 0>& j);
  void get_global_id(const Array<int, 0>& i, const Array<int, 0>& j, const Array<int, 0>& k);
  
  void get_local_id(const Array<int, 0>& i);
  void get_local_id(const Array<int, 0>& i, const Array<int, 0>& j);
  void get_local_id(const Array<int, 0>& i, const Array<int, 0>& j, const Array<int, 0>& k);
  
  void get_group_id(const Array<int, 0>& i);
  void get_group_id(const Array<int, 0>& i, const Array<int, 0>& j);
  void get_group_id(const Array<int, 0>& i, const Array<int, 0>& j, const Array<int, 0>& k);
  
  void get_num_groups(const Array<int, 0>& i);
  void get_num_groups(const Array<int, 0>& i, const Array<int, 0>& j);
  void get_num_groups(const Array<int, 0>& i, const Array<int, 0>& j, const Array<int, 0>& k);
  
  void get_global_size(const Array<int, 0>& i);
  void get_global_size(const Array<int, 0>& i, const Array<int, 0>& j);
  void get_global_size(const Array<int, 0>& i, const Array<int, 0>& j, const Array<int, 0>& k);
  
  void get_local_size(const Array<int, 0>& i);
  void get_local_size(const Array<int, 0>& i, const Array<int, 0>& j);
  void get_local_size(const Array<int, 0>& i, const Array<int, 0>& j, const Array<int, 0>& k);
  ///@}
  
  
  /** @name Synchronization interface.
   *  By now it consists of barriers and a datatype to indicate the level of synchronization.
   */
  ///@{
  
  /// Level of synchronization in a barrier.
  enum Sync_t { LOCAL = 1, GLOBAL = 2 };
  
  inline Sync_t operator|( Sync_t lhs, Sync_t rhs )
  {
    return Sync_t( int(lhs) | int(rhs) );
  }
  
  /// Barrier synchronization.
  void barrier(Sync_t flags);
  
  ///@}
  
  /** @name Interface to express intent of arguments in native kernels */
  ///@{
  template<typename T>
  struct In {};
  
  template<typename T>
  struct Out {};
  
  template<typename T>
  struct InOut {};
  ///@}
  
  /** @name External kernel invocation interface.
   *  Finds the device object for a function. It builds the object if it does not exists.
   */
  ///@{
  
  namespace internal {
    
    template<typename T>
    struct IntentArg {
      static const bool isWrite           = true;
      static const bool isWriteBeforeRead = false;
    };
    
    template<typename T>
    struct IntentArg<Out<T> > {
      static const bool isWrite           = true;
      static const bool isWriteBeforeRead = true;
    };
    
    template<typename T>
    struct IntentArg<In<T> > {
      static const bool isWrite           = false;
      static const bool isWriteBeforeRead = false;
    };
    
    template<typename T>
    struct RemoveIntent {
      typedef T type;
    };
    
    template<typename T>
    struct RemoveIntent< In<T> > {
      typedef T type;
    };
    
    template<typename T>
    struct RemoveIntent< Out<T> > {
      typedef T type;
    };
    
    template<typename T>
    struct RemoveIntent< InOut<T> > {
      typedef T type;
    };
    
    template<int N, int NARGS, int SCALARSBYVALUE>
    struct internal_eval {
    	template<typename RET, typename... Args, typename... BuiltArgs>
    	static void eval(std::function<RET(Args&...)>& f, FHandle& ret, BuiltArgs&&... args) {
    		typedef typename Select_type<N, Args...>::type actual_type;
    		typename std::remove_reference<actual_type>::type new_arg;
    		typedef typename RemoveIntent<typename std::remove_const<typename std::remove_reference<actual_type>::type>::type>::type arg_type;
    		static_assert(std::is_base_of<BaseArray, arg_type>::value, "Arguments must be Arrays");
    		internal_eval<N+1, NARGS, SCALARSBYVALUE + ISSCALARBYVALUE(actual_type)>::eval(f, ret, std::forward<BuiltArgs>(args)..., std::forward<decltype(new_arg)>(new_arg));
    		ret.copyFormalArrayInfo(N, (arg_type *)&new_arg);
    	}
    };

    template<int NARGS, int SCALARSBYVALUE>
    struct internal_eval<NARGS, NARGS, SCALARSBYVALUE> {
    	template<typename RET, typename... Args, typename... BuiltArgs>
    	static void eval(std::function<RET(Args&...)>& f, FHandle& ret, BuiltArgs&&... args) {
    		Codifier &cod = TheGlobalState().getCodifier();
    		cod.setnScalarsByValue(SCALARSBYVALUE);
    		cod.state(BuildingFBody);
    		f(args...);
    		ret.endDefinition();
    	}
    };

    template<typename... Args>
    void common_eval(std::function<void(Args...)>& f, FHandle &ret)
    {
    	HPL_PROFILEACTION(TheGlobalState().timer.start());
    	ret.fillIn(sizeof...(Args));
    	internal_eval<0, sizeof...(Args), 0>::eval(f, ret);
    	HPL_PROFILEACTION(TheGlobalState().secs_kernel_creation = TheGlobalState().timer.stop());
    	HPL_PROFILEACTION(TheGlobalState().total_secs_kernel_creation += TheGlobalState().secs_kernel_creation);
    }

    template<int N, int NARGS>
    struct native_internal_eval {
    	template<typename... Args>
    	static void eval(void (&f)(Args...), FHandle &ret) {
    		typedef typename Select_type<N, Args...>::type actual_type;
    		ret.copyFormalArrayInfo(N, IntentArg<actual_type>::isWrite, IntentArg<actual_type>::isWriteBeforeRead);
    		native_internal_eval<N+1, NARGS>::eval(f, ret);
    	}
    };

    template<int NARGS>
    struct native_internal_eval<NARGS, NARGS> {
    	template<typename... Args>
    	static void eval(void (&f)(Args...), FHandle &ret) { }
    };
    
    template<typename T>
    struct method_type { /*typedef T type;*/ };
    
    template<typename R, typename T, typename... Args>
    struct method_type<R (T::*)(Args...) const> {
      typedef R type (Args...);
      typedef R reftype (typename std::remove_reference<Args>::type&...);
    };
    
    template<typename R, typename T, typename... Args>
    struct method_type<R (T::*)(Args...)> {
      typedef R type (Args...);
      typedef R reftype (typename std::remove_reference<Args>::type&...);
    };
    
  }
  


  /** @brief Searches GlobalState::FHandle for std::function \p f, building it if required
   * and returns an FRunner to execute it over a concrete Device.
   * @param[in]  f     std::function to build.
   * @tparam     Args  type of the arguments to function \p f.
   */
  template<typename... Args>
  FRunner eval(std::function<void(Args...)>& f)
  {
    FHandle &ret = TheGlobalState().getFHandle((void*)(&f));
    HPL_PROFILEACTION(utils::clearProfilerStatistics());
    HPL_PROFILEACTION(TheGlobalState().secs_kernel_creation = 0.0);
    if(!ret.numArgs_) {
      std::function<void(typename std::remove_reference<Args>::type&...)> ff = f;
      internal::common_eval(ff, ret);
    }
    return FRunner(&ret);
  }
  
  /** @brief Searches GlobalState::FHandle for function \p f, building it if required
   * and returns an FRunner to execute it over a concrete Device.
   * @param[in]  f     pointer to the user function to build.
   * @tparam     Args  type of the arguments to function \p f.
   */
  template<typename... Args>
  FRunner eval(void (&f)(Args...))
  {
    FHandle &ret = TheGlobalState().getFHandle((void*)(&f));
    HPL_PROFILEACTION(utils::clearProfilerStatistics());
    HPL_PROFILEACTION(TheGlobalState().secs_kernel_creation = 0.0);
    if(!ret.numArgs_) {
      std::function<void(typename std::remove_reference<Args>::type&...)> ff = f;
      internal::common_eval(ff, ret);
    }
    return FRunner(&ret);
  }
  
  /** @brief Searches GlobalState::FHandle for functor \p f, building it if required
   * and returns an FRunner to execute it over a concrete Device.
   * @param[in]  f     functor whose operator() is to be run in HPL.
   * @tparam     T     type of the functor \p f.
   */
  template<typename T>
  FRunner eval(T& f)
  {
    FHandle &ret = TheGlobalState().getFHandle((void*)(&f));
    HPL_PROFILEACTION(utils::clearProfilerStatistics());
    HPL_PROFILEACTION(TheGlobalState().secs_kernel_creation = 0.0);
    if(!ret.numArgs_) {
      std::function<typename internal::method_type<decltype(&T::operator())>::reftype> ff = std::ref(f);
      internal::common_eval(ff, ret);
    }
    return FRunner(&ret);
  }
 
  
  /** @brief Searches the GlobalState::FHandle for function \p f, building it if required
   * and returns an FRunner to execute it over a concrete Device. The difference with the eval
   * method is that it removes the GlobalState::FHandle if it had been already created. This
   * behavior forces the reevaluation of the kernel and it generates a new OpenCL kernel.
   * @param[in]  f   pointer to the user function to build.
   * @tparam     T  type of the only input to function \p f.
   */
  template<typename T>
  FRunner reeval(T&& f)
  {
    TheGlobalState().deleteFHandle((void*)(&f));
    return eval(std::forward<T>(f));
  }
    
  ///@}
  
  template<typename... Args>
  void nativeHandle(void (&f)(Args...), const String_t& name, const String_t& native_code)
  {
    FHandle &fh = TheGlobalState().getNativeFHandle((void*)(&f), name, native_code);
    fh.fillIn(sizeof...(Args));
    internal::native_internal_eval<0, sizeof...(Args)>::eval(f, fh);
  }
  
  /**
   * @brief Obtains the number of available devices in the system.
   */
  unsigned int getDeviceNumber(Device_t type_n);

  /**
   * @brief Obtains the number of devices suported by a given platform in the system.
   */
  unsigned int getDeviceNumber(Platform_t platform_id, Device_t type_n);
  
  /**
   * @brief List the information of the devices
   */
  String_t getDeviceInfo(Device_t type_n);

  /// Return profiling data for most recent kernel run
  ProfilingData getProfile();
  
  /// Return profiling data for all kernel runs
  ProfilingData getTotalProfile();
  
}

#endif
