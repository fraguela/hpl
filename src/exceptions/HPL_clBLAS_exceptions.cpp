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

///file     HPL_clBLAS_exceptions.cpp
///author   Moisés Viñas        <moises.vinas@udc.es>
///author   Zeki Bozkus         <zeki.bozkus@khas.edu.tr>
///author   Basilio B. Fraguela <basilio.fraguela@udc.es>
///author   Diego Nieto         <diego.nieto@bsc.es>

#include "HPL_clBLAS_exceptions.hpp"

namespace HPL {

  HPLclBLASException::HPLclBLASException(cl_int errorcode, const char *msg) {

     std::string clBLASerrorMsg;
     switch(errorcode) {
        case clblasNotImplemented:
                clBLASerrorMsg = "Not implemented";
                break;
        case clblasNotInitialized:
                clBLASerrorMsg = "clblas library is not initialized yet";
                break;
        case clblasInvalidMatA:
                clBLASerrorMsg = "Matrix A is not a valid memory object";
                break;
        case clblasInvalidMatB:
                clBLASerrorMsg = "Matrix B is not a valid memory object";
                break;
        case clblasInvalidMatC:
                clBLASerrorMsg = "Matrix C is not a valid memory object";
                break;
        case clblasInvalidVecX:
                clBLASerrorMsg = "Vector X is not a valid memory object";
                break;
        case clblasInvalidVecY:
                clBLASerrorMsg = "Vector Y is not a valid memory object";
                break;
        case clblasInvalidDim:
                clBLASerrorMsg = "An input dimension (M,N,K) is invalid";
                break;
        case clblasInvalidLeadDimA:
                clBLASerrorMsg = "Leading dimension A must not be less than the size of the first dimension";
                break;
        case clblasInvalidLeadDimB:
                clBLASerrorMsg = "Leading dimension B must not be less than the size of the second dimension";
                break;
        case clblasInvalidLeadDimC:
                clBLASerrorMsg = "Leading dimension C must not be less than the size of the third dimension";
                break;
        case clblasInvalidIncX:
                clBLASerrorMsg = "The increment for a vector X must not be 0";
                break;
        case clblasInvalidIncY:
                clBLASerrorMsg = "The increment for a vector Y must not be 0";
                break;
        case clblasInsufficientMemMatA:
                clBLASerrorMsg = "The memory object for Matrix A is too small";
                break;
        case clblasInsufficientMemMatB:
                clBLASerrorMsg = "The memory object for Matrix B is too small";
                break;
        case clblasInsufficientMemMatC:
                clBLASerrorMsg = "The memory object for Matrix C is too small";
                break;
        case clblasInsufficientMemVecX:
                clBLASerrorMsg = "The memory object for Vector X is too small";
                break;
        case clblasInsufficientMemVecY:
                clBLASerrorMsg = "The memory object for Vector Y is too small";
                break;
        default:
                clBLASerrorMsg = "";
                break;
     }

     std::string msgOrig = std::string(msg);
     std::string fullMessage = msgOrig + " : " + clBLASerrorMsg;
     throw HPL::HPLException(errorcode, fullMessage.c_str()); 
  }

}
