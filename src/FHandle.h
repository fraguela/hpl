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
/// \file     FHandle.h
/// \author   Moisés Viñas        <moises.vinas@udc.es>
/// \author   Zeki Bozkus         <zeki.bozkus@khas.edu.tr>
/// \author   Basilio B. Fraguela <basilio.fraguela@udc.es>
///

#ifndef FHANDLE_H
#define FHANDLE_H

#include <initializer_list>

#include "GlobalState.h"
#include "HPL_utils.h"
#include "Device.h"
#include "exceptions/HPLException.hpp"

namespace HPL {

  /// Information kept by the library for user-defined functions
  class FHandle {
  public:

    /// Default empty constructor
    /** Only sets up the name. It sets the number of arguments to 0 and the code empty */
    FHandle();
    
    /// Constructor for native kernels
    FHandle(const String_t& name, const String_t& native_code);
    
    /// Fill this object with information for a function with \p num_args arguments
    /** @param[in] num_args  number of input arguments of the function */
    void fillIn(int num_args);
    
    /// Fills in the #code_ field with the code currently stored in the global HPL::Codifier
    /** @param[in] rettypestr string for the function return type, which void by default */
    void endDefinition(const char *rettypestr = 0);
    
    /// Prints the object to std::cout for debugging purposes
    void print();
    
    /// Prints the object to CLbinding::kernelfile 
    void fprint();

    /*
     * @brief Copy formal parameters ArrayInfo from TheGlobalState to CLbinding for optimization.
     * @param[in] argnum Index of this argument in the kernel call.
     * @param[in] arg    Argument represented as BaseArray.
     */
    void copyFormalArrayInfo(const int argnum, const BaseArray *arg);

    void copyFormalArrayInfo(const int argnum, bool isWrite, bool isWriteBeforeRead) {
      //std::cout << argnum << ' ' << isWrite << ' ' << isWriteBeforeRead << "\n";
      fhandleTable[argnum].isWrite = isWrite;
      fhandleTable[argnum].isWriteBeforeRead = isWriteBeforeRead;
    }
    
    void setTable(std::initializer_list<HPL::AccessType> init_list);

    /**
     * @brief Checks if the associated kernel has private arguments and
     * modifies its header to be a device function instead of a __kernel.
     */
    void checkPrivateArgs();
    
    bool isNative() const { return isNative_; }
    
    int numArgs_;         ///< Number of arguments of the function.
    /**< The default constructor sets it to 0. This indicates the FHandle has not been filled in yet. */
    const String_t name_; ///< Function name for the library (not for the user).
    String_t code_;       ///< Code for body of the function.

    struct arg_type {
      bool isWrite;                   ///< Flag that indicates if this array is written or not in the FHandle.
      bool isWriteBeforeRead;         ///< Flag that indicates if this array is written before being read.
    } fhandleTable[MAX_KERNEL_NARG]; ///< Contains the data of the arguments of the kernel, independent of any execution.

  private:
    
    bool is_fprint;        ///< Whether the kernel code has already been written to global program file or string
    const bool isNative_;  ///< Whether the function is built by HPL or native code provided by the user
    
  };
  
};
#endif
