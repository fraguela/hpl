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
/// \file     HPLException.hpp
/// \author   Moisés Viñas        <moises.vinas@udc.es>
/// \author   Zeki Bozkus         <zeki.bozkus@khas.edu.tr>
/// \author   Basilio B. Fraguela <basilio.fraguela@udc.es>
///

#ifndef HPLEXCEPTION_HPP
#define HPLEXCEPTION_HPP

#include <exception>
/*
 #ifdef __APPLE__
 #include <OpenCL/cl.hpp>
 #else
 #include <CL/cl.hpp>
 #endif
*/

#include "cl.hpp"

/** @file  HPLException.hpp
  * @brief Declaration of HPL::HPLException and default macros to use it
  */

/// Macro to run expr OpenCL call. If it does not return \c CL_SUCCESS, an HPL::HPLException is raised with the message provided
#define HPLSafeMsg(expr, msg) {                         \
  const cl_int HPLException_ErrCode = (expr);           \
  if (HPLException_ErrCode != CL_SUCCESS)               \
    throw HPL::HPLException(HPLException_ErrCode, msg); \
}

/// Default macro invocation to \c HPLSafeMsg with an empty message
#define HPLSafe(expr) HPLSafeMsg(expr, "");

/// Throw an exception if the condition is fulfilled
#define HPLExceptionIf(condition, msg) {           \
  if(condition) throw HPL::HPLException(0, msg);   \
}

/// Throw an exception if the condition is not fulfilled
#define HPLExceptionIfNot(condition, msg) {           \
  if(!(condition)) throw HPL::HPLException(0, msg);   \
}

namespace HPL {

  /// Exception class designed for HPL events
  class HPLException : public std::exception {

    const cl_int errcode_;   ///< Error code returned by an OpenCL call
    const char * const msg_; ///< Error description provided by HPL
    char messageBuffer[256]; ///< Complete Exception description built by HPLException

  public:

    /// Constructor
    HPLException(cl_int errcode, const char *msg = 0);

    /// Overload of \c std::exception::what that returns the full description of the exception
    const char* what() const throw();

    /// Returns the error code returned by the OpenCL call
    cl_int getErrCode() const { return errcode_; }

    /// Returns the error description provided by HPL
    const char * getMsg() const { return msg_; }
  };

};

#endif
