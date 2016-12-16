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
/// \file     HPL_clBLAS_exceptions.hpp
/// \author   Moisés Viñas        <moises.vinas@udc.es>
/// \author   Zeki Bozkus         <zeki.bozkus@khas.edu.tr>
/// \author   Basilio B. Fraguela <basilio.fraguela@udc.es>
/// \author   Diego Nieto         <diego.nieto@bsc.es>
///

#ifndef HPL_CLBLAS_EXCEPTIONS
#define HPL_CLBLAS_EXCEPTIONS

#include <HPL_clBLAS.h>

namespace HPL {

  class HPLclBLASException : public std::exception {
    public:
    HPLclBLASException(cl_int errorcode, const char *msg = 0);
  };

}

#endif
