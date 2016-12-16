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
/// \file     HPL_utils.h
/// \author   Moisés Viñas        <moises.vinas@udc.es>
/// \author   Zeki Bozkus         <zeki.bozkus@khas.edu.tr>
/// \author   Basilio B. Fraguela <basilio.fraguela@udc.es>
///

#ifndef HPL_UTILS_H
#define HPL_UTILS_H

#include <string>
#include <cstring>
#include <cctype>
#include <sys/time.h>
#include <type_traits>
#include "exceptions/HPLException.hpp"

/** @file
    @brief Debugging macros, basic datatypes and helper functions
*/

#if defined(__amd64) || defined(__amd64__) || defined(__x86_64) || defined(__x86_64__)  || defined(_LP64) || defined(__LP64__)
#define HPL_64BIT_MACHINE 1
#endif

#if defined(WIN32) || defined(WINNT) || defined(__WIN32) || defined(__WIN32__) || defined(__WINNT) || defined(__WINNT__)
#define HPL_WINDOWS 1
#endif

#define TOSTRING(...) #__VA_ARGS__

/// Contains all the HPL library
namespace HPL {

  /// Defines kind of access to data
  enum AccessType { HPL_NONE = 0, HPL_RD = 1, HPL_WR = 2, HPL_RDWR = 3 };

  /// Defines properties of an Array such as the memory space in which it is storaged
  enum ArrayFlags { Global = 0, Local = 1, Constant = 2, Private = 3 };

  enum BufferType { None, OwnBuffer, SubBuffer };

  enum DistribType { D_NONE = 0, D_FST = 1, D_SND = 2, D_TRD = 3 };

  /*
   inline ArrayFlags operator|( ArrayFlags lhs, ArrayFlags rhs )
   {
     return ArrayFlags( int(lhs) | int(rhs) );
   }
   */
  
  template<typename T, int NDIM>
  class IndexedArray;
    
  /// Array or scalar to be used in/with parallel functions
  /** @tparam T    type of the elements
      @tparam NDIM number of dimensions (0 for scalars) */
  template<typename T, int NDIM = 0, ArrayFlags AF = Global>
  class Array;
  
  typedef std::string String_t; ///< String type manipulated by the library
  
  
  /// Returns the wall clock
  double Wtime();
  
  /// Utility class to measure time
  class Timer {
  private:
    
    struct timeval begin, end;
    
  public:
    
    Timer();
    
    /// Start measuring time
    void start();
    
    // Return time in second since previous invocation to start()
    double stop();   
  };
  
  /// Check whether type \c D is a non-reference type derived from type \c B
  template<typename D, typename B>
  struct is_nonref_derived_from {
    static const bool value = !std::is_reference<D>::value && 
                               std::is_base_of<B, typename std::remove_reference<D>::type>::value;
  };
  
  /// internal useful functions
  namespace utils {
    
    /// Comparator for using char * as key in containers
    struct ltstr
    {
      bool operator()(const char* s1, const char* s2) const
      {
	return strcmp(s1, s2) < 0;
      }
    };
    
    /// Whether c is a character part of a C++ identifier or not
    inline bool notInWord(int c)
    {
      //This is because of Mavericks g++ isalnum bug http://stackoverflow.com/questions/19649421/something-odd-happened-to-c-11-in-mavericks
#if defined(__APPLE__) && defined(__GNUC__) && !defined(__clang__)
      bool inword =   ((c >= '0') && (c <= '9'))
                    || ((c >= 'A') && (c <= 'Z'))
                    || ((c >= 'a') && (c <= 'z'))
                    || (c == '_');
      return !inword;
#else
      return !std::isalnum(c) && (c != '_');
#endif
    }
    
    /// Find an instance of \p substr inside \p str that is not part of a word
    bool findWord(const std::string &str, const char* substr);
    
    /** @brief If the profiler is activated, this function stores the profiling information
     * in the file HPL_PROFILER_OUTPUT.txt.
     */
    void showTotalProfilerStatistics();
    void clearProfilerStatistics();

  }
  // end namespace utils
  
  
  //////////////////////////////////////////////////////////////
  ///Selects the i-th type from a variadic list of template arguments
  template<int I, typename... Tail>
  struct Select_type;
  
  template<typename Head, typename... Tail>
  struct Select_type<0, Head, Tail...> {
    typedef Head type;
  };
  
  template<int I, typename Head, typename... Tail>
  struct Select_type<I, Head, Tail...> {
    typedef typename Select_type<I-1, Tail...>::type type;
  };

}

#define HPL_STACK_STORAGE(TYPE, VAR, LEN) char _hpl_tmp_ ## VAR[(LEN)*sizeof(TYPE)]; TYPE *const VAR=(TYPE*)_hpl_tmp_ ## VAR;

#ifdef DEBUG
#include <iostream>
#define DEBUGSTREAM std::cerr
#define LOG(...)   do{ DEBUGSTREAM << __VA_ARGS__ << std::endl; }while(0)
#define GLOG(g,...) do{ if(g) do{ DEBUGSTREAM << __VA_ARGS__ << std::endl; } }while(0)
#define DEBUGACTION(...) do{ __VA_ARGS__ ; }while(0)
#else
#define LOG(...)              /* no debug */
#define GLOG(g,...)           /* nodebug */
#define DEBUGACTION(...)      /* nodebug */
#endif


#ifdef HPL_PROFILE
#include <iostream>
#define HPL_PROFILEACTION(...) do{ __VA_ARGS__ ; }while(0)
#define HPL_PROFILEDEFINITION(def) def
#define GET(variable) double get___VA_ARGS__ ();
#define PROFILELOG(...)   do{ std::cerr << __VA_ARGS__ << std::endl; }while(0)
/// @deprecated since getters are deprecated in favor of object-oriented interface
#define ADD_PROFILER_GETTER(MEMBER) \
double get_ ## MEMBER(){ \
return  MEMBER; \
}
#else
#define HPL_PROFILEACTION(...) /* no profiling */
#define PROFILELOG(...)  /* no profiling */
#define HPL_PROFILEDEFINITION(def) /* no profiling */
/// @deprecated since getters are deprecated in favor of object-oriented interface
#define ADD_PROFILER_GETTER(MEMBER) \
double get_ ## MEMBER(){ \
return  0.0; \
}
#endif


#endif
