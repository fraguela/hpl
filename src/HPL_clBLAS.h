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
/// \file     HPL_clBLAS.h
/// \author   Moisés Viñas        <moises.vinas@udc.es>
/// \author   Zeki Bozkus         <zeki.bozkus@khas.edu.tr>
/// \author   Basilio B. Fraguela <basilio.fraguela@udc.es>
/// \author   Diego Nieto         <diego.nieto@bsc.es>
///
#ifndef HPL_CLBLAS
#define HPL_CLBLAS

#include <clBLAS.h>
#include <HPL.h>
#include <exceptions/HPL_clBLAS_exceptions.hpp>

clblasStatus HPL_clblasSetup();

void HPL_clblasTeardown();

/**********
 * BLAS 1 *
 **********/

///
/// \brief SWAP with simple interface
///
clblasStatus clBlasSswap(HPL::Array<float, 1>& X,
                         HPL::Array<float, 1>& Y,
                         const HPL::Device& d = HPL::Device());

///
/// \brief SWAP with full interface
///
clblasStatus clBlasSswap(size_t N,
                         HPL::Array<float, 1>& X,
                         size_t offx,
                         int incx,
                         HPL::Array<float, 1>& Y,
                         size_t offy,
                         int incy,
                         const HPL::Device& d = HPL::Device(),
                         cl_uint numEventsInWaitList = 0,
                         const cl_event * eventWaitList = 0,
                         cl_event * events = 0);

///
/// \brief SWAP with simple interface
///
clblasStatus clBlasDswap(HPL::Array<double, 1>& X,
                         HPL::Array<double, 1>& Y,
                         const HPL::Device& d = HPL::Device());

///
/// \brief SWAP with full interface
///
clblasStatus clBlasDswap(size_t N,
                         HPL::Array<double, 1>& X,
                         size_t offx,
                         int incx,
                         HPL::Array<double, 1>& Y,
                         size_t offy,
                         int incy,
                         const HPL::Device& d = HPL::Device(),
                         cl_uint numEventsInWaitList = 0,
                         const cl_event * eventWaitList = 0,
                         cl_event * events = 0);

///
/// \brief SWAP with simple interface
///
clblasStatus clBlasCswap(HPL::Array<HPL::float2, 1>& X,
                         HPL::Array<HPL::float2, 1>& Y,
                         const HPL::Device& d = HPL::Device());

///
/// \brief SWAP with full interface
///
clblasStatus clBlasCswap(size_t N,
                         HPL::Array<HPL::float2, 1>& X,
                         size_t offx,
                         int incx,
                         HPL::Array<HPL::float2, 1>& Y,
                         size_t offy,
                         int incy,
                         const HPL::Device& d = HPL::Device(),
                         cl_uint numEventsInWaitList = 0,
                         const cl_event * eventWaitList = 0,
                         cl_event * events = 0);


///
/// \brief SCAL with simple interface
///
clblasStatus clBlasSscal(cl_float alpha,
                         HPL::Array<float, 1>& X,
                         const HPL::Device& d = HPL::Device());

///
/// \brief SCAL with full interface
///
clblasStatus clBlasSscal(size_t N,
                         cl_float alpha,
                         HPL::Array<float, 1>& X,
                         size_t offx,
                         int incx,
                         const HPL::Device& d = HPL::Device(),
                         cl_uint numEventsInWaitList = 0,
                         const cl_event * eventWaitList = 0,
                         cl_event * events = 0);

///
/// \brief SCAL with simple interface
///
clblasStatus clBlasDscal(cl_double alpha,
                         HPL::Array<double, 1>& X,
                         const HPL::Device& d = HPL::Device());

///
/// \brief SCAL with full interface
///
clblasStatus clBlasDscal(size_t N,
                         cl_double alpha,
                         HPL::Array<double, 1>& X,
                         size_t offx,
                         int incx,
                         const HPL::Device& d = HPL::Device(),
                         cl_uint numEventsInWaitList = 0,
                         const cl_event * eventWaitList = 0,
                         cl_event * events = 0);

///
/// \brief SCAL with simple interface
///
clblasStatus clBlasCscal(cl_float2 alpha,
                         HPL::Array<HPL::float2, 1>& X,
                         const HPL::Device& d = HPL::Device());

///
/// \brief SCAL with full interface
///
clblasStatus clBlasCscal(size_t N,
                         cl_float2 alpha,
                         HPL::Array<HPL::float2, 1>& X,
                         size_t offx,
                         int incx,
                         const HPL::Device& d = HPL::Device(),
                         cl_uint numEventsInWaitList = 0,
                         const cl_event * eventWaitList = 0,
                         cl_event * events = 0);

///
/// \brief SSCAL with simple interface
///
clblasStatus clBlasCsscal(cl_float alpha,
                          HPL::Array<HPL::float2, 1>& X,
                          const HPL::Device& d = HPL::Device());

///
/// \brief SSCAL with full interface
///
clblasStatus clBlasCsscal(size_t N,
                          cl_float alpha,
                          HPL::Array<HPL::float2, 1>& X,
                          size_t offx,
                          int incx,
                          const HPL::Device& d = HPL::Device(),
                          cl_uint numEventsInWaitList = 0,
                          const cl_event * eventWaitList = 0,
                          cl_event * events = 0);

///
/// \brief COPY with simple interface
///
clblasStatus clBlasScopy(const HPL::Array<float, 1>& X,
                         HPL::Array<float, 1>& Y,
                         const HPL::Device& d = HPL::Device());

///
/// \brief COPY with full interface
///
clblasStatus clBlasScopy(size_t N,
                         const HPL::Array<float, 1>& X,
                         size_t offx,
                         int incx,
                         HPL::Array<float, 1>& Y,
                         size_t offy,
                         int incy,
                         const HPL::Device& d = HPL::Device(),
                         cl_uint numEventsInWaitList = 0,
                         const cl_event * eventWaitList = 0,
                         cl_event * events = 0);

///
/// \brief COPY with simple interface
///
clblasStatus clBlasDcopy(const HPL::Array<double, 1>& X,
                         HPL::Array<double, 1>& Y,
                         const HPL::Device& d = HPL::Device());

///
/// \brief COPY with full interface
///
clblasStatus clBlasDcopy(size_t N,
                         const HPL::Array<double, 1>& X,
                         size_t offx,
                         int incx,
                         HPL::Array<double, 1>& Y,
                         size_t offy,
                         int incy,
                         const HPL::Device& d = HPL::Device(),
                         cl_uint numEventsInWaitList = 0,
                         const cl_event * eventWaitList = 0,
                         cl_event * events = 0);

///
/// \brief COPY with simple interface
///
clblasStatus clBlasCcopy(const HPL::Array<HPL::float2, 1>& X,
                         HPL::Array<HPL::float2, 1>& Y,
                         const HPL::Device& d = HPL::Device());

///
/// \brief COPY with full interface
///
clblasStatus clBlasCcopy(size_t N,
                         const HPL::Array<HPL::float2, 1>& X,
                         size_t offx,
                         int incx,
                         HPL::Array<HPL::float2, 1>& Y,
                         size_t offy,
                         int incy,
                         const HPL::Device& d = HPL::Device(),
                         cl_uint numEventsInWaitList = 0,
                         const cl_event * eventWaitList = 0,
                         cl_event * events = 0);


///
/// \brief SAXPY with simple interface
///
clblasStatus clblasSaxpy(cl_float alpha,
                         const HPL::Array<float, 1>& x,
                         HPL::Array<float, 1>& y,
                         const HPL::Device& d = HPL::Device());

///
/// \brief SAXPY with full interface
///
clblasStatus clblasSaxpy(size_t N,
                         cl_float alpha,
                         const HPL::Array<float, 1>& x,
                         size_t offx,
                         int incx,
                         HPL::Array<float, 1>& y,
                         size_t offy,
                         int incy,
                         const HPL::Device& d = HPL::Device(),
                         cl_uint numEventsInWaitList = 0,
                         const cl_event * eventWaitList = 0,
                         cl_event * events = 0);

///
/// \brief SAXPY with simple interface
///
clblasStatus clblasDaxpy(cl_double alpha,
                         const HPL::Array<double, 1>& x,
                         HPL::Array<double, 1>& y,
                         const HPL::Device& d = HPL::Device());

///
/// \briefSAXPY with full interface
///
clblasStatus clblasDaxpy(size_t N,
                         cl_double alpha,
                         const HPL::Array<double, 1>& x,
                         size_t offx,
                         int incx,
                         HPL::Array<double, 1>& y,
                         size_t offy,
                         int incy,
                         const HPL::Device& d = HPL::Device(),
                         cl_uint numEventsInWaitList = 0,
                         const cl_event * eventWaitList = 0,
                         cl_event * events = 0);

///
/// \brief SAXPY with simple interface
///
clblasStatus clblasCaxpy(cl_float2 alpha,
                         const HPL::Array<HPL::float2, 1>& x,
                         HPL::Array<HPL::float2, 1>& y,
                         const HPL::Device& d = HPL::Device());

///
/// \briefSAXPY with full interface
///
clblasStatus clblasCaxpy(size_t N,
                         cl_float2 alpha,
                         const HPL::Array<HPL::float2, 1>& x,
                         size_t offx,
                         int incx,
                         HPL::Array<HPL::float2, 1>& y,
                         size_t offy,
                         int incy,
                         const HPL::Device& d = HPL::Device(),
                         cl_uint numEventsInWaitList = 0,
                         const cl_event * eventWaitList = 0,
                         cl_event * events = 0);

///
/// \brief DOT with simple interface
///
clblasStatus clblasSdot(HPL::Array<float, 1>& dotProduct,
                        const HPL::Array<float, 1>& X,
                        const HPL::Array<float, 1>& Y,
                        const HPL::Device& d = HPL::Device());

///
/// \brief DOT with full interface
///
clblasStatus clblasSdot(size_t N,
                        HPL::Array<float, 1>& dotProduct,
                        size_t offDP,
                        const HPL::Array<float, 1>& X,
                        size_t offx,
                        int incx,
                        const HPL::Array<float, 1>& Y,
                        size_t offy,
                        int incy,
                        HPL::Array<float, 1>& scratchBuff,
                        const HPL::Device& d = HPL::Device(),
                        cl_uint numEventsInWaitList = 0,
                        const cl_event * eventWaitList = 0,
                        cl_event * events = 0);

///
/// \brief DOT with simple interface
///
clblasStatus clblasDdot(HPL::Array<double, 1>& dotProduct,
                        const HPL::Array<double, 1>& X,
                        const HPL::Array<double, 1>& Y,
                        const HPL::Device& d = HPL::Device());

///
/// \brief DOT with full interface
///
clblasStatus clblasDdot(size_t N,
                        HPL::Array<double, 1>& dotProduct,
                        size_t offDP,
                        const HPL::Array<double, 1>& X,
                        size_t offx,
                        int incx,
                        const HPL::Array<double, 1>& Y,
                        size_t offy,
                        int incy,
                        HPL::Array<double, 1>& scratchBuff,
                        const HPL::Device& d = HPL::Device(),
                        cl_uint numEventsInWaitList = 0,
                        const cl_event * eventWaitList = 0,
                        cl_event * events = 0);

///
/// \brief DOT with simple interface
///
clblasStatus clblasCdotu(HPL::Array<HPL::float2, 1>& dotProduct,
                         const HPL::Array<HPL::float2, 1>& X,
                         const HPL::Array<HPL::float2, 1>& Y,
                         const HPL::Device& d = HPL::Device());

///
/// \brief DOT with full interface
///
clblasStatus clblasCdotu(size_t N,
                         HPL::Array<HPL::float2, 1>& dotProduct,
                         size_t offDP,
                         const HPL::Array<HPL::float2, 1>& X,
                         size_t offx,
                         int incx,
                         const HPL::Array<HPL::float2, 1>& Y,
                         size_t offy,
                         int incy,
                         HPL::Array<HPL::float2, 1>& scratchBuff,
                         const HPL::Device& d = HPL::Device(),
                         cl_uint numEventsInWaitList = 0,
                         const cl_event * eventWaitList = 0,
                         cl_event * events = 0);

///
/// \brief DOT with simple interface
///
clblasStatus clblasCdotc(HPL::Array<HPL::float2, 1>& dotProduct,
                         const HPL::Array<HPL::float2, 1>& X,
                         const HPL::Array<HPL::float2, 1>& Y,
                         const HPL::Device& d = HPL::Device());

///
/// \brief DOT with full interface
///
clblasStatus clblasCdotc(size_t N,
                         HPL::Array<HPL::float2, 1>& dotProduct,
                         size_t offDP,
                         const HPL::Array<HPL::float2, 1>& X,
                         size_t offx,
                         int incx,
                         const HPL::Array<HPL::float2, 1>& Y,
                         size_t offy,
                         int incy,
                         HPL::Array<HPL::float2, 1>& scratchBuff,
                         const HPL::Device& d = HPL::Device(),
                         cl_uint numEventsInWaitList = 0,
                         const cl_event * eventWaitList = 0,
                         cl_event * events = 0);

///
/// \brief ROTG with simple interface
///
clblasStatus clblasSrotg(HPL::Array<float, 1>& SA,
                         HPL::Array<float, 1>& SB,
                         HPL::Array<float, 1>& C,
                         HPL::Array<float, 1>& S,
                         const HPL::Device& d = HPL::Device());

///
/// \brief ROTG with full interface
///
clblasStatus clblasSrotg(HPL::Array<float, 1>& SA,
                         size_t offSA,
                         HPL::Array<float, 1>& SB,
                         size_t offSB,
                         HPL::Array<float, 1>& C,
                         size_t offC,
                         HPL::Array<float, 1>& S,
                         size_t offS,
                         const HPL::Device& d = HPL::Device(),
                         cl_uint numEventsInWaitList = 0,
                         const cl_event * eventWaitList = 0,
                         cl_event * events = 0);

///
/// \brief ROTG with simple interface
///
clblasStatus clblasDrotg(HPL::Array<double, 1>& SA,
                         HPL::Array<double, 1>& SB,
                         HPL::Array<double, 1>& C,
                         HPL::Array<double, 1>& S,
                         const HPL::Device& d = HPL::Device());

///
/// \brief ROTG with full interface
///
clblasStatus clblasDrotg(HPL::Array<double, 1>& SA,
                         size_t offSA,
                         HPL::Array<double, 1>& SB,
                         size_t offSB,
                         HPL::Array<double, 1>& C,
                         size_t offC,
                         HPL::Array<double, 1>& S,
                         size_t offS,
                         const HPL::Device& d = HPL::Device(),
                         cl_uint numEventsInWaitList = 0,
                         const cl_event * eventWaitList = 0,
                         cl_event * events = 0);

///
/// \brief ROTG with simple interface
///
clblasStatus clblasCrotg(HPL::Array<HPL::float2, 1>& SA,
                         HPL::Array<HPL::float2, 1>& SB,
                         HPL::Array<float, 1>& C,
                         HPL::Array<HPL::float2, 1>& S,
                         const HPL::Device& d = HPL::Device());

///
/// \brief ROTG with full interface
///
clblasStatus clblasCrotg(HPL::Array<HPL::float2, 1>& SA,
                         size_t offSA,
                         HPL::Array<HPL::float2, 1>& SB,
                         size_t offSB,
                         HPL::Array<float, 1>& C,
                         size_t offC,
                         HPL::Array<HPL::float2, 1>& S,
                         size_t offS,
                         const HPL::Device& d = HPL::Device(),
                         cl_uint numEventsInWaitList = 0,
                         const cl_event * eventWaitList = 0,
                         cl_event * events = 0);

///
/// \brief ROTMG with simple interface
///
clblasStatus clblasSrotmg(HPL::Array<float, 1>& SD1,
                          HPL::Array<float, 1>& SD2,
                          HPL::Array<float, 1>& SX1,
                          const HPL::Array<float, 1>& SY1,
                          HPL::Array<float, 1>& SPARAM,
                          const HPL::Device& d = HPL::Device());

///
/// \brief ROTMG with full interface
///
clblasStatus clblasSrotmg(HPL::Array<float, 1>& SD1,
                          size_t offSD1,
                          HPL::Array<float, 1>& SD2,
                          size_t offSD2,
                          HPL::Array<float, 1>& SX1,
                          size_t offSX1,
                          const HPL::Array<float, 1>& SY1,
                          size_t offSY1,
                          HPL::Array<float, 1>& SPARAM,
                          size_t offSparam,
                          const HPL::Device& d = HPL::Device(),
                          cl_uint numEventsInWaitList = 0,
                          const cl_event * eventWaitList = 0,
                          cl_event * events = 0);

///
/// \brief ROTMG with simple interface
///
clblasStatus clblasDrotmg(HPL::Array<double, 1>& SD1,
                          HPL::Array<double, 1>& SD2,
                          HPL::Array<double, 1>& SX1,
                          const HPL::Array<double, 1>& SY1,
                          HPL::Array<double, 1>& SPARAM,
                          const HPL::Device& d = HPL::Device());

///
/// \brief ROTMG with full interface
///
clblasStatus clblasDrotmg(HPL::Array<double, 1>& SD1,
                          size_t offSD1,
                          HPL::Array<double, 1>& SD2,
                          size_t offSD2,
                          HPL::Array<double, 1>& SX1,
                          size_t offSX1,
                          const HPL::Array<double, 1>& SY1,
                          size_t offSY1,
                          HPL::Array<double, 1>& SPARAM,
                          size_t offSparam,
                          const HPL::Device& d = HPL::Device(),
                          cl_uint numEventsInWaitList = 0,
                          const cl_event * eventWaitList = 0,
                          cl_event * events = 0);

///
/// \brief ROT with simple interface
///
clblasStatus clblasSrot(HPL::Array<float, 1>& X,
                        HPL::Array<float, 1>& Y,
                        cl_float C,
                        cl_float S,
                        const HPL::Device& d = HPL::Device());

///
/// \brief ROT with full interface
///
clblasStatus clblasSrot(size_t N,
                        HPL::Array<float, 1>& X,
                        size_t offx,
                        int incx,
                        HPL::Array<float, 1>& Y,
                        size_t offy,
                        int incy,
                        cl_float C,
                        cl_float S,
                        const HPL::Device& d = HPL::Device(),
                        cl_uint numEventsInWaitList = 0,
                        const cl_event * eventWaitList = 0,
                        cl_event * events = 0);

///
/// \brief ROT with simple interface
///
clblasStatus clblasDrot(HPL::Array<double, 1>& X,
                        HPL::Array<double, 1>& Y,
                        cl_double C,
                        cl_double S,
                        const HPL::Device& d = HPL::Device());

///
/// \brief ROT with full interface
///
clblasStatus clblasDrot(size_t N,
                        HPL::Array<double, 1>& X,
                        size_t offx,
                        int incx,
                        HPL::Array<double, 1>& Y,
                        size_t offy,
                        int incy,
                        cl_double C,
                        cl_double S,
                        const HPL::Device& d = HPL::Device(),
                        cl_uint numEventsInWaitList = 0,
                        const cl_event * eventWaitList = 0,
                        cl_event * events = 0);

///
/// \brief ROT with simple interface
///
clblasStatus clblasCsrot(HPL::Array<HPL::float2, 1>& X,
                         HPL::Array<HPL::float2, 1>& Y,
                         cl_float C,
                         cl_float S,
                         const HPL::Device& d = HPL::Device());

///
/// \brief ROT with full interface
///
clblasStatus clblasCsrot(size_t N,
                         HPL::Array<HPL::float2, 1>& X,
                         size_t offx,
                         int incx,
                         HPL::Array<HPL::float2, 1>& Y,
                         size_t offy,
                         int incy,
                         cl_float C,
                         cl_float S,
                         const HPL::Device& d = HPL::Device(),
                         cl_uint numEventsInWaitList = 0,
                         const cl_event * eventWaitList = 0,
                         cl_event * events = 0);

///
/// \brief ROTM with simple interface
///
clblasStatus clblasSrotm(HPL::Array<float, 1>& X,
                         HPL::Array<float, 1>& Y,
                         const HPL::Array<float, 1>& SPARAM,
                         const HPL::Device& d = HPL::Device());

///
/// \brief ROTM with simple interface
///
clblasStatus clblasSrotm(size_t N,
                         HPL::Array<float, 1>& X,
                         size_t offx,
                         int incx,
                         HPL::Array<float, 1>& Y,
                         size_t offy,
                         int incy,
                         const HPL::Array<float, 1>& SPARAM,
                         size_t offSparam,
                         const HPL::Device& d = HPL::Device(),
                         cl_uint numEventsInWaitList = 0,
                         const cl_event * eventWaitList = 0,
                         cl_event * events = 0);

///
/// \brief ROTM with simple interface
///
clblasStatus clblasDrotm(HPL::Array<double, 1>& X,
                         HPL::Array<double, 1>& Y,
                         const HPL::Array<double, 1>& SPARAM,
                         const HPL::Device& d = HPL::Device());

///
/// \brief ROTM with simple interface
///
clblasStatus clblasDrotm(size_t N,
                         HPL::Array<double, 1>& X,
                         size_t offx,
                         int incx,
                         HPL::Array<double, 1>& Y,
                         size_t offy,
                         int incy,
                         const HPL::Array<double, 1>& SPARAM,
                         size_t offSparam,
                         const HPL::Device& d = HPL::Device(),
                         cl_uint numEventsInWaitList = 0,
                         const cl_event * eventWaitList = 0,
                         cl_event * events = 0);

///
/// \brief NRM2 with simple interface
///
clblasStatus clblasSnrm2(HPL::Array<float, 1>& NRM2,
                         const HPL::Array<float, 1>& X,
                         const HPL::Device& d = HPL::Device());

///
/// \brief NRM2 with full interface
///
clblasStatus clblasSnrm2(size_t N,
                         HPL::Array<float, 1>& NRM2,
                         size_t offNRM2,
                         const HPL::Array<float, 1>& X,
                         size_t offx,
                         int incx,
                         HPL::Array<float, 1>& scratchBuff,
                         const HPL::Device& d = HPL::Device(),
                         cl_uint numEventsInWaitList = 0,
                         const cl_event * eventWaitList = 0,
                         cl_event * events = 0);

///
/// \brief NRM2 with simple interface
///
clblasStatus clblasDnrm2(HPL::Array<double, 1>& NRM2,
                         const HPL::Array<double, 1>& X,
                         const HPL::Device& d = HPL::Device());

///
/// \brief NRM2 with full interface
///
clblasStatus clblasDnrm2(size_t N,
                         HPL::Array<double, 1>& NRM2,
                         size_t offNRM2,
                         const HPL::Array<double, 1>& X,
                         size_t offx,
                         int incx,
                         HPL::Array<double, 1>& scratchBuff,
                         const HPL::Device& d = HPL::Device(),
                         cl_uint numEventsInWaitList = 0,
                         const cl_event * eventWaitList = 0,
                         cl_event * events = 0);

///
/// \brief NRM2 with simple interface
///
clblasStatus clblasScnrm2(HPL::Array<float, 1>& NRM2,
                         const HPL::Array<HPL::float2, 1>& X,
                         const HPL::Device& d = HPL::Device());

///
/// \brief NRM2 with full interface
///
clblasStatus clblasScnrm2(size_t N,
                         HPL::Array<float, 1>& NRM2,
                         size_t offNRM2,
                         const HPL::Array<HPL::float2, 1>& X,
                         size_t offx,
                         int incx,
                         HPL::Array<HPL::float2, 1>& scratchBuff,
                         const HPL::Device& d = HPL::Device(),
                         cl_uint numEventsInWaitList = 0,
                         const cl_event * eventWaitList = 0,
                         cl_event * events = 0);

///
/// \brief iAMAX with simple interface
///
clblasStatus clblasiSamax(HPL::Array<unsigned int, 1>& iMax,
                          const HPL::Array<float, 1>& X,
                          const HPL::Device& d = HPL::Device());

///
/// \brief iAMAX with full interface
///
clblasStatus clblasiSamax(size_t N,
                          HPL::Array<unsigned int, 1>& iMax,
                          size_t offiMax,
                          const HPL::Array<float, 1>& X,
                          size_t offx,
                          int incx,
                          HPL::Array<float, 1>& scratchBuff,
                          const HPL::Device& d = HPL::Device(),
                          cl_uint numEventsInWaitList = 0,
                          const cl_event * eventWaitList = 0,
                          cl_event * events = 0);

///
/// \brief iAMAX with simple interface
///
clblasStatus clblasiDamax(HPL::Array<unsigned int, 1>& iMax,
                          const HPL::Array<double, 1>& X,
                          const HPL::Device& d = HPL::Device());

///
/// \brief iAMAX with full interface
///
clblasStatus clblasiDamax(size_t N,
                          HPL::Array<unsigned int, 1>& iMax,
                          size_t offiMax,
                          const HPL::Array<double, 1>& X,
                          size_t offx,
                          int incx,
                          HPL::Array<double, 1>& scratchBuff,
                          const HPL::Device& d = HPL::Device(),
                          cl_uint numEventsInWaitList = 0,
                          const cl_event * eventWaitList = 0,
                          cl_event * events = 0);

///
/// \brief iAMAX with simple interface
///
clblasStatus clblasiCamax(HPL::Array<unsigned int, 1>& iMax,
                          const HPL::Array<HPL::float2, 1>& X,
                          const HPL::Device& d = HPL::Device());

///
/// \brief iAMAX with full interface
///
clblasStatus clblasiCamax(size_t N,
                          HPL::Array<unsigned int, 1>& iMax,
                          size_t offiMax,
                          const HPL::Array<HPL::float2, 1>& X,
                          size_t offx,
                          int incx,
                          HPL::Array<HPL::float2, 1>& scratchBuff,
                          const HPL::Device& d = HPL::Device(),
                          cl_uint numEventsInWaitList = 0,
                          const cl_event * eventWaitList = 0,
                          cl_event * events = 0);

///
/// \brief ASUM with simple interface
///
clblasStatus clblasSasum(HPL::Array<float, 1>& asum,
                         const HPL::Array<float, 1>& X,
                         const HPL::Device& d = HPL::Device());

///
/// \brief ASUM with full interface
///
clblasStatus clblasSasum(size_t N,
                         HPL::Array<float, 1>& asum,
                         size_t offAsum,
                         const HPL::Array<float, 1>& X,
                         size_t offx,
                         int incx,
                         HPL::Array<float, 1>& scratchBuff,
                         const HPL::Device& d = HPL::Device(),
                         cl_uint numEventsInWaitList = 0,
                         const cl_event * eventWaitList = 0,
                         cl_event * events = 0);

///
/// \brief ASUM with simple interface
///
clblasStatus clblasDasum(HPL::Array<double, 1>& asum,
                         const HPL::Array<double, 1>& X,
                         const HPL::Device& d = HPL::Device());

///
/// \brief ASUM with full interface
///
clblasStatus clblasDasum(size_t N,
                         HPL::Array<double, 1>& asum,
                         size_t offAsum,
                         const HPL::Array<double, 1>& X,
                         size_t offx,
                         int incx,
                         HPL::Array<double, 1>& scratchBuff,
                         const HPL::Device& d = HPL::Device(),
                         cl_uint numEventsInWaitList = 0,
                         const cl_event * eventWaitList = 0,
                         cl_event * events = 0);

///
/// \brief ASUM with simple interface
///
clblasStatus clblasScasum(HPL::Array<HPL::float2, 1>& asum,
                          const HPL::Array<HPL::float2, 1>& X,
                          const HPL::Device& d = HPL::Device());

///
/// \brief ASUM with full interface
///
clblasStatus clblasScasum(size_t N,
                          HPL::Array<HPL::float2, 1>& asum,
                          size_t offAsum,
                          const HPL::Array<HPL::float2, 1>& X,
                          size_t offx,
                          int incx,
                          HPL::Array<HPL::float2, 1>& scratchBuff,
                          const HPL::Device& d = HPL::Device(),
                          cl_uint numEventsInWaitList = 0,
                          const cl_event * eventWaitList = 0,
                          cl_event * events = 0);

/**********
 * BLAS 2 *
 **********/

///
/// \brief GEMV with simple interfaz
///
clblasStatus clblasSgemv(const HPL::Array<float, 2>& A,
                         const HPL::Array<float, 1>& x,
                         HPL::Array<float, 1>& y,
                         const HPL::Device& d = HPL::Device());

///
/// \brief GEMV with full interfaz
///
clblasStatus clblasSgemv(clblasOrder order,
                         clblasTranspose transA,
                         size_t M,
                         size_t N,
                         cl_float alpha,
                         const HPL::Array<float, 2>& A,
                         size_t offA,
                         size_t lda,
                         const HPL::Array<float, 1>& x,
                         size_t offx,
                         int incx,
                         cl_float beta,
                         HPL::Array<float, 1>& y,
                         size_t offy,
                         int incy,
                         const HPL::Device& d = HPL::Device(),
                         cl_uint numEventsInWaitList = 0,
                         const cl_event * eventWaitList = 0,
                         cl_event * events = 0);

///
/// \brief GEMV with simple interfaz
///
clblasStatus clblasDgemv(const HPL::Array<double, 2>& A,
                         const HPL::Array<double, 1>& x,
                         HPL::Array<double, 1>& y,
                         const HPL::Device& d = HPL::Device());

///
/// \brief GEMV with full interfaz
///
clblasStatus clblasDgemv(clblasOrder order,
                         clblasTranspose transA,
                         size_t M,
                         size_t N,
                         cl_double alpha,
                         const HPL::Array<double, 2>& A,
                         size_t offA,
                         size_t lda,
                         const HPL::Array<double, 1>& x,
                         size_t offx,
                         int incx,
                         cl_double beta,
                         HPL::Array<double, 1>& y,
                         size_t offy,
                         int incy,
                         const HPL::Device& d = HPL::Device(),
                         cl_uint numEventsInWaitList = 0,
                         const cl_event * eventWaitList = 0,
                         cl_event * events = 0);

///
/// \brief GEMV with simple interfaz
///
clblasStatus clblasCgemv(const HPL::Array<HPL::float2, 2>& A,
                         const HPL::Array<HPL::float2, 1>& x,
                         HPL::Array<HPL::float2, 1>& y,
                         const HPL::Device& d = HPL::Device());

///
/// \brief GEMV with full interfaz
///
clblasStatus clblasCgemv(clblasOrder order,
                         clblasTranspose transA,
                         size_t M,
                         size_t N,
                         cl_float2 alpha,
                         const HPL::Array<HPL::float2, 2>& A,
                         size_t offA,
                         size_t lda,
                         const HPL::Array<HPL::float2, 1>& x,
                         size_t offx,
                         int incx,
                         cl_float2 beta,
                         HPL::Array<HPL::float2, 1>& y,
                         size_t offy,
                         int incy,
                         const HPL::Device& d = HPL::Device(),
                         cl_uint numEventsInWaitList = 0,
                         const cl_event * eventWaitList = 0,
                         cl_event * events = 0);

///
/// \brief SYMV with simple interfaz
///
clblasStatus clblasSsymv(clblasUplo uplo,
                         const HPL::Array<float, 2>& A,
                         const HPL::Array<float, 1>& X,
                         HPL::Array<float, 1>& Y,
                         const HPL::Device& d = HPL::Device());

///
/// \brief SYMV with full interfaz
///
clblasStatus clblasSsymv(clblasOrder order,
                         clblasUplo uplo,
                         size_t N,
                         cl_float alpha,
                         const HPL::Array<float, 2>& A,
                         size_t offA,
                         size_t lda,
                         const HPL::Array<float, 1>& X,
                         size_t offx,
                         int incx,
                         cl_float beta,
                         HPL::Array<float, 1>& Y,
                         size_t offy,
                         int incy,
                         const HPL::Device& d = HPL::Device(),
                         cl_uint numEventsInWaitList = 0,
                         const cl_event * eventWaitList = 0,
                         cl_event * events = 0);

///
/// \brief SYMV with simple interfaz
///
clblasStatus clblasDsymv(clblasUplo uplo,
                         const HPL::Array<double, 2>& A,
                         const HPL::Array<double, 1>& X,
                         HPL::Array<double, 1>& Y,
                         const HPL::Device& d = HPL::Device());

///
/// \brief SYMV with full interfaz
///
clblasStatus clblasDsymv(clblasOrder order,
                         clblasUplo uplo,
                         size_t N,
                         cl_double alpha,
                         const HPL::Array<double, 2>& A,
                         size_t offA,
                         size_t lda,
                         const HPL::Array<double, 1>& X,
                         size_t offx,
                         int incx,
                         cl_double beta,
                         HPL::Array<double, 1>& Y,
                         size_t offy,
                         int incy,
                         const HPL::Device& d = HPL::Device(),
                         cl_uint numEventsInWaitList = 0,
                         const cl_event * eventWaitList = 0,
                         cl_event * events = 0);

///
/// \brief HEMV with simple interfaz
///
clblasStatus clblasChemv(clblasUplo uplo,
                         const HPL::Array<HPL::float2, 2>& A,
                         const HPL::Array<HPL::float2, 1>& X,
                         HPL::Array<HPL::float2, 1>& Y,
                         const HPL::Device& d = HPL::Device());

///
/// \brief HEMV with full interfaz
///
clblasStatus clblasChemv(clblasOrder order,
                         clblasUplo uplo,
                         size_t N,
                         cl_float2 alpha,
                         const HPL::Array<HPL::float2, 2>& A,
                         size_t offa,
                         size_t lda,
                         const HPL::Array<HPL::float2, 1>& X,
                         size_t offx,
                         int incx,
                         cl_float2 beta,
                         HPL::Array<HPL::float2, 1>& Y,
                         size_t offy,
                         int incy,
                         const HPL::Device& d = HPL::Device(),
                         cl_uint numEventsInWaitList = 0,
                         const cl_event * eventWaitList = 0,
                         cl_event * events = 0);

///
/// \brief TRMV with simple interfaz
///
clblasStatus clblasStrmv(clblasUplo uplo,
                         clblasDiag diag,
                         const HPL::Array<float, 2>& A,
                         HPL::Array<float, 1>& X,
                         const HPL::Device& d = HPL::Device());

///
/// \brief TRMV with full interfaz
///
clblasStatus clblasStrmv(clblasOrder order,
                         clblasUplo uplo,
                         clblasTranspose trans,
                         clblasDiag diag,
                         size_t N,
                         const HPL::Array<float, 2>& A,
                         size_t offa,
                         size_t lda,
                         HPL::Array<float, 1>& X,
                         size_t offx,
                         int incx,
                         HPL::Array<float, 1>& scratchBuff,
                         const HPL::Device& d = HPL::Device(),
                         cl_uint numEventsInWaitList = 0,
                         const cl_event * eventWaitList = 0,
                         cl_event * events = 0);

///
/// \brief TRMV with simple interfaz
///
clblasStatus clblasDtrmv(clblasUplo uplo,
                         clblasDiag diag,
                         const HPL::Array<double, 2>& A,
                         HPL::Array<double, 1>& X,
                         const HPL::Device& d = HPL::Device());

///
/// \brief TRMV with full interfaz
///
clblasStatus clblasDtrmv(clblasOrder order,
                         clblasUplo uplo,
                         clblasTranspose trans,
                         clblasDiag diag,
                         size_t N,
                         const HPL::Array<double, 2>& A,
                         size_t offa,
                         size_t lda,
                         HPL::Array<double, 1>& X,
                         size_t offx,
                         int incx,
                         HPL::Array<double, 1>& scratchBuff,
                         const HPL::Device& d = HPL::Device(),
                         cl_uint numEventsInWaitList = 0,
                         const cl_event * eventWaitList = 0,
                         cl_event * events = 0);

///
/// \brief TRMV with simple interfaz
///
clblasStatus clblasCtrmv(clblasUplo uplo,
                         clblasDiag diag,
                         const HPL::Array<HPL::float2, 2>& A,
                         HPL::Array<HPL::float2, 1>& X,
                         const HPL::Device& d = HPL::Device());

///
/// \brief TRMV with full interfaz
///
clblasStatus clblasCtrmv(clblasOrder order,
                         clblasUplo uplo,
                         clblasTranspose trans,
                         clblasDiag diag,
                         size_t N,
                         const HPL::Array<HPL::float2, 2>& A,
                         size_t offa,
                         size_t lda,
                         HPL::Array<HPL::float2, 1>& X,
                         size_t offx,
                         int incx,
                         HPL::Array<HPL::float2, 1>& scratchBuff,
                         const HPL::Device& d = HPL::Device(),
                         cl_uint numEventsInWaitList = 0,
                         const cl_event * eventWaitList = 0,
                         cl_event * events = 0);

///
/// \brief TRSV with simple interfaz
///
clblasStatus clblasStrsv(clblasUplo uplo,
                         clblasDiag diag,
                         const HPL::Array<float, 2>& A,
                         HPL::Array<float, 1>& X,
                         const HPL::Device& d = HPL::Device());

///
/// \brief TRSV with full interfaz
///
clblasStatus clblasStrsv(clblasOrder order,
                         clblasUplo uplo,
                         clblasTranspose trans,
                         clblasDiag diag,
                         size_t N,
                         const HPL::Array<float, 2>& A,
                         size_t offa,
                         size_t lda,
                         HPL::Array<float, 1>& X,
                         size_t offx,
                         int incx,
                         const HPL::Device& d = HPL::Device(),
                         cl_uint numEventsInWaitList = 0,
                         const cl_event * eventWaitList = 0,
                         cl_event * events = 0);

///
/// \brief TRSV with simple interfaz
///
clblasStatus clblasDtrsv(clblasUplo uplo,
                         clblasDiag diag,
                         const HPL::Array<double, 2>& A,
                         HPL::Array<double, 1>& X,
                         const HPL::Device& d = HPL::Device());

///
/// \brief TRSV with full interfaz
///
clblasStatus clblasDtrsv(clblasOrder order,
                         clblasUplo uplo,
                         clblasTranspose trans,
                         clblasDiag diag,
                         size_t N,
                         const HPL::Array<double, 2>& A,
                         size_t offa,
                         size_t lda,
                         HPL::Array<double, 1>& X,
                         size_t offx,
                         int incx,
                         const HPL::Device& d = HPL::Device(),
                         cl_uint numEventsInWaitList = 0,
                         const cl_event * eventWaitList = 0,
                         cl_event * events = 0);

///
/// \brief TRSV with simple interfaz
///
clblasStatus clblasCtrsv(clblasUplo uplo,
                         clblasDiag diag,
                         const HPL::Array<HPL::float2, 2>& A,
                         HPL::Array<HPL::float2, 1>& X,
                         const HPL::Device& d = HPL::Device());

///
/// \brief TRSV with full interfaz
///
clblasStatus clblasCtrsv(clblasOrder order,
                         clblasUplo uplo,
                         clblasTranspose trans,
                         clblasDiag diag,
                         size_t N,
                         const HPL::Array<HPL::float2, 2>& A,
                         size_t offa,
                         size_t lda,
                         HPL::Array<HPL::float2, 1>& X,
                         size_t offx,
                         int incx,
                         const HPL::Device& d = HPL::Device(),
                         cl_uint numEventsInWaitList = 0,
                         const cl_event * eventWaitList = 0,
                         cl_event * events = 0);

///
/// \brief GER with simple interfaz
///
clblasStatus clblasSger(const HPL::Array<float, 1>& X,
                        const HPL::Array<float, 1>& Y,
                        HPL::Array<float, 2>& A,
                        const HPL::Device& d = HPL::Device());

///
/// \brief GER with full interfaz
///
clblasStatus clblasSger(clblasOrder order,
                        size_t M,
                        size_t N,
                        cl_float alpha,
                        const HPL::Array<float, 1>& X,
                        size_t offx,
                        int incx,
                        const HPL::Array<float, 1>& Y,
                        size_t offy,
                        int incy,
                        HPL::Array<float, 2>& A,
                        size_t offa,
                        size_t lda,
                        const HPL::Device& d = HPL::Device(),
                        cl_uint numEventsInWaitList = 0,
                        const cl_event * eventWaitList = 0,
                        cl_event * events = 0);

///
/// \brief GER with simple interfaz
///
clblasStatus clblasDger(const HPL::Array<double, 1>& X,
                        const HPL::Array<double, 1>& Y,
                        HPL::Array<double, 2>& A,
                        const HPL::Device& d = HPL::Device());

///
/// \brief GER with full interfaz
///
clblasStatus clblasDger(clblasOrder order,
                        size_t M,
                        size_t N,
                        cl_double alpha,
                        const HPL::Array<double, 1>& X,
                        size_t offx,
                        int incx,
                        const HPL::Array<double, 1>& Y,
                        size_t offy,
                        int incy,
                        HPL::Array<double, 2>& A,
                        size_t offa,
                        size_t lda,
                        const HPL::Device& d = HPL::Device(),
                        cl_uint numEventsInWaitList = 0,
                        const cl_event * eventWaitList = 0,
                        cl_event * events = 0);

///
/// \brief GERU with simple interfaz
///
clblasStatus clblasCgeru(const HPL::Array<HPL::float2, 1>& X,
                         const HPL::Array<HPL::float2, 1>& Y,
                         HPL::Array<HPL::float2, 2>& A,
                         const HPL::Device& d = HPL::Device());

///
/// \brief GERU with full interfaz
///
clblasStatus clblasCgeru(clblasOrder order,
                         size_t M,
                         size_t N,
                         cl_float2 alpha,
                         const HPL::Array<HPL::float2, 1>& X,
                         size_t offx,
                         int incx,
                         const HPL::Array<HPL::float2, 1>& Y,
                         size_t offy,
                         int incy,
                         HPL::Array<HPL::float2, 2>& A,
                         size_t offa,
                         size_t lda,
                         const HPL::Device& d = HPL::Device(),
                         cl_uint numEventsInWaitList = 0,
                         const cl_event * eventWaitList = 0,
                         cl_event * events = 0);

///
/// \brief GERC with simple interfaz
///
clblasStatus clblasCgerc(const HPL::Array<HPL::float2, 1>& X,
                         const HPL::Array<HPL::float2, 1>& Y,
                         HPL::Array<HPL::float2, 2>& A,
                         const HPL::Device& d = HPL::Device());

///
/// \brief GERC with full interfaz
///
clblasStatus clblasCgerc(clblasOrder order,
                         size_t M,
                         size_t N,
                         cl_float2 alpha,
                         const HPL::Array<HPL::float2, 1>& X,
                         size_t offx,
                         int incx,
                         const HPL::Array<HPL::float2, 1>& Y,
                         size_t offy,
                         int incy,
                         HPL::Array<HPL::float2, 2>& A,
                         size_t offa,
                         size_t lda,
                         const HPL::Device& d = HPL::Device(),
                         cl_uint numEventsInWaitList = 0,
                         const cl_event * eventWaitList = 0,
                         cl_event * events = 0);

///
/// \brief SYR with simple interfaz
///
clblasStatus clblasSsyr(clblasUplo uplo,
                        const HPL::Array<float, 1>& X,
                        HPL::Array<float, 2>& A,
                        const HPL::Device& d = HPL::Device());

///
/// \brief SYR with full interfaz
///
clblasStatus clblasSsyr(clblasOrder order,
                        clblasUplo uplo,
                        size_t N,
                        cl_float alpha,
                        const HPL::Array<float, 1>& X,
                        size_t offx,
                        int incx,
                        HPL::Array<float, 2>& A,
                        size_t offa,
                        size_t lda,
                        const HPL::Device& d = HPL::Device(),
                        cl_uint numEventsInWaitList = 0,
                        const cl_event * eventWaitList = 0,
                        cl_event * events = 0);

///
/// \brief SYR with simple interfaz
///
clblasStatus clblasDsyr(clblasUplo uplo,
                        const HPL::Array<double, 1>& X,
                        HPL::Array<double, 2>& A,
                        const HPL::Device& d = HPL::Device());

///
/// \brief SYR with full interfaz
///
clblasStatus clblasDsyr(clblasOrder order,
                        clblasUplo uplo,
                        size_t N,
                        cl_double alpha,
                        const HPL::Array<double, 1>& X,
                        size_t offx,
                        int incx,
                        HPL::Array<double, 2>& A,
                        size_t offa,
                        size_t lda,
                        const HPL::Device& d = HPL::Device(),
                        cl_uint numEventsInWaitList = 0,
                        const cl_event * eventWaitList = 0,
                        cl_event * events = 0);

///
/// \brief HER with simple interfaz
///
clblasStatus clblasCher(clblasUplo uplo,
                        const HPL::Array<HPL::float2, 1>& X,
                        HPL::Array<HPL::float2, 2>& A,
                        const HPL::Device& d = HPL::Device());

///
/// \brief HER with full interfaz
///
clblasStatus clblasCher(clblasOrder order,
                        clblasUplo uplo,
                        size_t N,
                        cl_float alpha,
                        const HPL::Array<HPL::float2, 1>& X,
                        size_t offx,
                        int incx,
                        HPL::Array<HPL::float2, 2>& A,
                        size_t offa,
                        size_t lda,
                        const HPL::Device& d = HPL::Device(),
                        cl_uint numEventsInWaitList = 0,
                        const cl_event * eventWaitList = 0,
                        cl_event * events = 0);

///
/// \brief SYR2 with simple interfaz
///
clblasStatus clblasSsyr2(clblasUplo uplo,
                         const HPL::Array<float, 1>& X,
                         const HPL::Array<float, 1>& Y,
                         HPL::Array<float, 2>& A,
                         const HPL::Device& d = HPL::Device());

///
/// \brief SYR2 with full interfaz
///
clblasStatus clblasSsyr2(clblasOrder order,
                         clblasUplo uplo,
                         size_t N,
                         cl_float alpha,
                         const HPL::Array<float, 1>& X,
                         size_t offx,
                         int incx,
                         const HPL::Array<float, 1>& Y,
                         size_t offy,
                         int incy,
                         HPL::Array<float, 2>& A,
                         size_t offa,
                         size_t lda,
                         const HPL::Device& d = HPL::Device(),
                         cl_uint numEventsInWaitList = 0,
                         const cl_event * eventWaitList = 0,
                         cl_event * events = 0);

///
/// \brief SYR2 with simple interfaz
///
clblasStatus clblasDsyr2(clblasUplo uplo,
                         const HPL::Array<double, 1>& X,
                         const HPL::Array<double, 1>& Y,
                         HPL::Array<double, 2>& A,
                         const HPL::Device& d = HPL::Device());

///
/// \brief SYR2 with full interfaz
///
clblasStatus clblasDsyr2(clblasOrder order,
                         clblasUplo uplo,
                         size_t N,
                         cl_double alpha,
                         const HPL::Array<double, 1>& X,
                         size_t offx,
                         int incx,
                         const HPL::Array<double, 1>& Y,
                         size_t offy,
                         int incy,
                         HPL::Array<double, 2>& A,
                         size_t offa,
                         size_t lda,
                         const HPL::Device& d = HPL::Device(),
                         cl_uint numEventsInWaitList = 0,
                         const cl_event * eventWaitList = 0,
                         cl_event * events = 0);

///
/// \brief HER2 with simple interfaz
///
clblasStatus clblasCher2(clblasUplo uplo,
                         const HPL::Array<HPL::float2, 1>& X,
                         const HPL::Array<HPL::float2, 1>& Y,
                         HPL::Array<HPL::float2, 2>& A,
                         const HPL::Device& d = HPL::Device());

///
/// \brief HER2 with full interfaz
///
clblasStatus clblasCher2(clblasOrder order,
                         clblasUplo uplo,
                         size_t N,
                         cl_float2 alpha,
                         const HPL::Array<HPL::float2, 1>& X,
                         size_t offx,
                         int incx,
                         const HPL::Array<HPL::float2, 1>& Y,
                         size_t offy,
                         int incy,
                         HPL::Array<HPL::float2, 2>& A,
                         size_t offa,
                         size_t lda,
                         const HPL::Device& d = HPL::Device(),
                         cl_uint numEventsInWaitList = 0,
                         const cl_event * eventWaitList = 0,
                         cl_event * events = 0);

///
/// \brief TPMV with simple interfaz
///
clblasStatus clblasStpmv(clblasUplo uplo,
                         const HPL::Array<float, 1>& AP,
                         HPL::Array<float, 1>& X,
                         const HPL::Device& d = HPL::Device());

///
/// \brief TPMV with full interfaz
///
clblasStatus clblasStpmv(clblasOrder order,
                         clblasUplo uplo,
                         clblasTranspose trans,
                         clblasDiag diag,
                         size_t N,
                         const HPL::Array<float, 1>& AP,
                         size_t offa,
                         HPL::Array<float, 1>& X,
                         size_t offx,
                         int incx,
                         HPL::Array<float, 1>& scratchBuff,
                         const HPL::Device& d = HPL::Device(),
                         cl_uint numEventsInWaitList = 0,
                         const cl_event * eventWaitList = 0,
                         cl_event * events = 0);

///
/// \brief TPMV with simple interfaz
///
clblasStatus clblasDtpmv(clblasUplo uplo,
                         const HPL::Array<double, 1>& AP,
                         HPL::Array<double, 1>& X,
                         const HPL::Device& d = HPL::Device());

///
/// \brief TPMV with full interfaz
///
clblasStatus clblasDtpmv(clblasOrder order,
                         clblasUplo uplo,
                         clblasTranspose trans,
                         clblasDiag diag,
                         size_t N,
                         const HPL::Array<double, 1>& AP,
                         size_t offa,
                         HPL::Array<double, 1>& X,
                         size_t offx,
                         int incx,
                         HPL::Array<double, 1>& scratchBuff,
                         const HPL::Device& d = HPL::Device(),
                         cl_uint numEventsInWaitList = 0,
                         const cl_event * eventWaitList = 0,
                         cl_event * events = 0);

///
/// \brief TPMV with simple interfaz
///
clblasStatus clblasCtpmv(clblasUplo uplo,
                         const HPL::Array<HPL::float2, 1>& AP,
                         HPL::Array<HPL::float2, 1>& X,
                         const HPL::Device& d = HPL::Device());

///
/// \brief TPMV with full interfaz
///
clblasStatus clblasCtpmv(clblasOrder order,
                         clblasUplo uplo,
                         clblasTranspose trans,
                         clblasDiag diag,
                         size_t N,
                         const HPL::Array<HPL::float2, 1>& AP,
                         size_t offa,
                         HPL::Array<HPL::float2, 1>& X,
                         size_t offx,
                         int incx,
                         HPL::Array<HPL::float2, 1>& scratchBuff,
                         const HPL::Device& d = HPL::Device(),
                         cl_uint numEventsInWaitList = 0,
                         const cl_event * eventWaitList = 0,
                         cl_event * events = 0);

///
/// \brief TPSV with simple interfaz
///
clblasStatus clblasStpsv(clblasUplo uplo,
                         const HPL::Array<float, 1>& A,
                         HPL::Array<float, 1>& X,
                         const HPL::Device& d = HPL::Device());

///
/// \brief TPSV with full interfaz
///
clblasStatus clblasStpsv(clblasOrder order,
                         clblasUplo uplo,
                         clblasTranspose trans,
                         clblasDiag diag,
                         size_t N,
                         const HPL::Array<float, 1>& A,
                         size_t offa,
                         HPL::Array<float, 1>& X,
                         size_t offx,
                         int incx,
                         const HPL::Device& d = HPL::Device(),
                         cl_uint numEventsInWaitList = 0,
                         const cl_event * eventWaitList = 0,
                         cl_event * events = 0);

///
/// \brief TPSV with simple interfaz
///
clblasStatus clblasDtpsv(clblasUplo uplo,
                         const HPL::Array<double, 1>& A,
                         HPL::Array<double, 1>& X,
                         const HPL::Device& d = HPL::Device());

///
/// \brief TPSV with full interfaz
///
clblasStatus clblasDtpsv(clblasOrder order,
                         clblasUplo uplo,
                         clblasTranspose trans,
                         clblasDiag diag,
                         size_t N,
                         const HPL::Array<double, 1>& A,
                         size_t offa,
                         HPL::Array<double, 1>& X,
                         size_t offx,
                         int incx,
                         const HPL::Device& d = HPL::Device(),
                         cl_uint numEventsInWaitList = 0,
                         const cl_event * eventWaitList = 0,
                         cl_event * events = 0);

///
/// \brief TPSV with simple interfaz
///
clblasStatus clblasCtpsv(clblasUplo uplo,
                         const HPL::Array<HPL::float2, 1>& A,
                         HPL::Array<HPL::float2, 1>& X,
                         const HPL::Device& d = HPL::Device());

///
/// \brief TPSV with full interfaz
///
clblasStatus clblasCtpsv(clblasOrder order,
                         clblasUplo uplo,
                         clblasTranspose trans,
                         clblasDiag diag,
                         size_t N,
                         const HPL::Array<HPL::float2, 1>& A,
                         size_t offa,
                         HPL::Array<HPL::float2, 1>& X,
                         size_t offx,
                         int incx,
                         const HPL::Device& d = HPL::Device(),
                         cl_uint numEventsInWaitList = 0,
                         const cl_event * eventWaitList = 0,
                         cl_event * events = 0);

///
/// \brief SPMV with simple interfaz
///
clblasStatus clblasSspmv(clblasUplo uplo,
                         const HPL::Array<float, 1>& AP,
                         const HPL::Array<float, 1>& X,
                         HPL::Array<float, 1>& Y,
                         const HPL::Device& d = HPL::Device());

///
/// \brief SPMV with full interfaz
///
clblasStatus clblasSspmv(clblasOrder order,
                         clblasUplo uplo,
                         size_t N,
                         cl_float alpha,
                         const HPL::Array<float, 1>& AP,
                         size_t offa,
                         const HPL::Array<float, 1>& X,
                         size_t offx,
                         int incx,
                         cl_float beta,
                         HPL::Array<float, 1>& Y,
                         size_t offy,
                         int incy,
                         const HPL::Device& d = HPL::Device(),
                         cl_uint numEventsInWaitList = 0,
                         const cl_event * eventWaitList = 0,
                         cl_event * events = 0);

///
/// \brief SPMV with simple interfaz
///
clblasStatus clblasDspmv(clblasUplo uplo,
                         const HPL::Array<double, 1>& AP,
                         const HPL::Array<double, 1>& X,
                         HPL::Array<double, 1>& Y,
                         const HPL::Device& d = HPL::Device());

///
/// \brief SPMV with full interfaz
///
clblasStatus clblasDspmv(clblasOrder order,
                         clblasUplo uplo,
                         size_t N,
                         cl_double alpha,
                         const HPL::Array<double, 1>& AP,
                         size_t offa,
                         const HPL::Array<double, 1>& X,
                         size_t offx,
                         int incx,
                         cl_double beta,
                         HPL::Array<double, 1>& Y,
                         size_t offy,
                         int incy,
                         const HPL::Device& d = HPL::Device(),
                         cl_uint numEventsInWaitList = 0,
                         const cl_event * eventWaitList = 0,
                         cl_event * events = 0);

///
/// \brief HPMV with simple interfaz
///
clblasStatus clblasChpmv(clblasUplo uplo,
                         const HPL::Array<HPL::float2, 1>& AP,
                         const HPL::Array<HPL::float2, 1>& X,
                         HPL::Array<HPL::float2, 1>& Y,
                         const HPL::Device& d = HPL::Device());

///
/// \brief HPMV with full interfaz
///
clblasStatus clblasChpmv(clblasOrder order,
                         clblasUplo uplo,
                         size_t N,
                         cl_float2 alpha,
                         const HPL::Array<HPL::float2, 1>& AP,
                         size_t offa,
                         const HPL::Array<HPL::float2, 1>& X,
                         size_t offx,
                         int incx,
                         cl_float2 beta,
                         HPL::Array<HPL::float2, 1>& Y,
                         size_t offy,
                         int incy,
                         const HPL::Device& d = HPL::Device(),
                         cl_uint numEventsInWaitList = 0,
                         const cl_event * eventWaitList = 0,
                         cl_event * events = 0);

///
/// \brief SPR with simple interfaz
///
clblasStatus clblasSspr(clblasUplo uplo,
                        const HPL::Array<float, 1>& X,
                        HPL::Array<float, 1>& AP,
                        const HPL::Device& d = HPL::Device());

///
/// \brief SPR with full interfaz
///
clblasStatus clblasSspr(clblasOrder order,
                        clblasUplo uplo,
                        size_t N,
                        cl_float alpha,
                        const HPL::Array<float, 1>& X,
                        size_t offx,
                        int incx,
                        HPL::Array<float, 1>& AP,
                        size_t offa,
                        const HPL::Device& d = HPL::Device(),
                        cl_uint numEventsInWaitList = 0,
                        const cl_event * eventWaitList = 0,
                        cl_event * events = 0);

///
/// \brief SPR with simple interfaz
///
clblasStatus clblasDspr(clblasUplo uplo,
                        const HPL::Array<double, 1>& X,
                        HPL::Array<double, 1>& AP,
                        const HPL::Device& d = HPL::Device());

///
/// \brief SPR with full interfaz
///
clblasStatus clblasDspr(clblasOrder order,
                        clblasUplo uplo,
                        size_t N,
                        cl_double alpha,
                        const HPL::Array<double, 1>& X,
                        size_t offx,
                        int incx,
                        HPL::Array<double, 1>& AP,
                        size_t offa,
                        const HPL::Device& d = HPL::Device(),
                        cl_uint numEventsInWaitList = 0,
                        const cl_event * eventWaitList = 0,
                        cl_event * events = 0);

///
/// \brief HPR with simple interfaz
///
clblasStatus clblasChpr(clblasUplo uplo,
                        const HPL::Array<HPL::float2, 1>& X,
                        HPL::Array<HPL::float2, 1>& AP,
                        const HPL::Device& d = HPL::Device());

///
/// \brief HPR with full interfaz
///
clblasStatus clblasChpr(clblasOrder order,
                        clblasUplo uplo,
                        size_t N,
                        cl_float alpha,
                        const HPL::Array<HPL::float2, 1>& X,
                        size_t offx,
                        int incx,
                        HPL::Array<HPL::float2, 1>& AP,
                        size_t offa,
                        const HPL::Device& d = HPL::Device(),
                        cl_uint numEventsInWaitList = 0,
                        const cl_event * eventWaitList = 0,
                        cl_event * events = 0);

///
/// \brief SPR2 with simple interfaz
///
clblasStatus clblasSspr2(clblasUplo uplo,
                         const HPL::Array<float, 1>& X,
                         const HPL::Array<float, 1>& Y,
                         HPL::Array<float, 1>& AP,
                         const HPL::Device& d = HPL::Device());

///
/// \brief SPR2 with full interfaz
///
clblasStatus clblasSspr2(clblasOrder order,
                         clblasUplo uplo,
                         size_t N,
                         cl_float alpha,
                         const HPL::Array<float, 1>& X,
                         size_t offx,
                         int incx,
                         const HPL::Array<float, 1>& Y,
                         size_t offy,
                         int incy,
                         HPL::Array<float, 1>& AP,
                         size_t offa,
                         const HPL::Device& d = HPL::Device(),
                         cl_uint numEventsInWaitList = 0,
                         const cl_event * eventWaitList = 0,
                         cl_event * events = 0);

///
/// \brief SPR2 with simple interfaz
///
clblasStatus clblasDspr2(clblasUplo uplo,
                         const HPL::Array<double, 1>& X,
                         const HPL::Array<double, 1>& Y,
                         HPL::Array<double, 1>& AP,
                         const HPL::Device& d = HPL::Device());

///
/// \brief SPR2 with full interfaz
///
clblasStatus clblasDspr2(clblasOrder order,
                         clblasUplo uplo,
                         size_t N,
                         cl_double alpha,
                         const HPL::Array<double, 1>& X,
                         size_t offx,
                         int incx,
                         const HPL::Array<double, 1>& Y,
                         size_t offy,
                         int incy,
                         HPL::Array<double, 1>& AP,
                         size_t offa,
                         const HPL::Device& d = HPL::Device(),
                         cl_uint numEventsInWaitList = 0,
                         const cl_event * eventWaitList = 0,
                         cl_event * events = 0);

///
/// \brief HPR2 with simple interfaz
///
clblasStatus clblasChpr2(clblasUplo uplo,
                         const HPL::Array<HPL::float2, 1>& X,
                         const HPL::Array<HPL::float2, 1>& Y,
                         HPL::Array<HPL::float2, 1>& AP,
                         const HPL::Device& d = HPL::Device());

///
/// \brief HPR2 with full interfaz
///
clblasStatus clblasChpr2(clblasOrder order,
                         clblasUplo uplo,
                         size_t N,
                         cl_float2 alpha,
                         const HPL::Array<HPL::float2, 1>& X,
                         size_t offx,
                         int incx,
                         const HPL::Array<HPL::float2, 1>& Y,
                         size_t offy,
                         int incy,
                         HPL::Array<HPL::float2, 1>& AP,
                         size_t offa,
                         const HPL::Device& d = HPL::Device(),
                         cl_uint numEventsInWaitList = 0,
                         const cl_event * eventWaitList = 0,
                         cl_event * events = 0);

///
/// \brief GBMV with simple interfaz
///
clblasStatus clblasSgbmv(size_t KL,
                         size_t KU,
                         const HPL::Array<float, 1>& A,
                         const HPL::Array<float, 1>& X,
                         HPL::Array<float, 1>& Y,
                         const HPL::Device& d = HPL::Device());

///
/// \brief GBMV with full interfaz
///
clblasStatus clblasSgbmv(clblasOrder order,
                         clblasTranspose trans,
                         size_t M,
                         size_t N,
                         size_t KL,
                         size_t KU,
                         cl_float alpha,
                         const HPL::Array<float, 1>& A,
                         size_t offa,
                         size_t lda,
                         const HPL::Array<float, 1>& X,
                         size_t offx,
                         int incx,
                         cl_float beta,
                         HPL::Array<float, 1>& Y,
                         size_t offy,
                         int incy,
                         const HPL::Device& d = HPL::Device(),
                         cl_uint numEventsInWaitList = 0,
                         const cl_event * eventWaitList = 0,
                         cl_event * events = 0);

///
/// \brief GBMV with simple interfaz
///
clblasStatus clblasDgbmv(size_t KL,
                         size_t KU,
                         const HPL::Array<double, 1>& A,
                         const HPL::Array<double, 1>& X,
                         HPL::Array<double, 1>& Y,
                         const HPL::Device& d = HPL::Device());

///
/// \brief GBMV with full interfaz
///
clblasStatus clblasDgbmv(clblasOrder order,
                         clblasTranspose trans,
                         size_t M,
                         size_t N,
                         size_t KL,
                         size_t KU,
                         cl_double alpha,
                         const HPL::Array<double, 1>& A,
                         size_t offa,
                         size_t lda,
                         const HPL::Array<double, 1>& X,
                         size_t offx,
                         int incx,
                         cl_double beta,
                         HPL::Array<double, 1>& Y,
                         size_t offy,
                         int incy,
                         const HPL::Device& d = HPL::Device(),
                         cl_uint numEventsInWaitList = 0,
                         const cl_event * eventWaitList = 0,
                         cl_event * events = 0);

///
/// \brief GBMV with simple interfaz
///
clblasStatus clblasCgbmv(size_t KL,
                         size_t KU,
                         const HPL::Array<HPL::float2, 1>& A,
                         const HPL::Array<HPL::float2, 1>& X,
                         HPL::Array<HPL::float2, 1>& Y,
                         const HPL::Device& d = HPL::Device());

///
/// \brief GBMV with full interfaz
///
clblasStatus clblasCgbmv(clblasOrder order,
                         clblasTranspose trans,
                         size_t M,
                         size_t N,
                         size_t KL,
                         size_t KU,
                         cl_float2 alpha,
                         const HPL::Array<HPL::float2, 1>& A,
                         size_t offa,
                         size_t lda,
                         const HPL::Array<HPL::float2, 1>& X,
                         size_t offx,
                         int incx,
                         cl_float2 beta,
                         HPL::Array<HPL::float2, 1>& Y,
                         size_t offy,
                         int incy,
                         const HPL::Device& d = HPL::Device(),
                         cl_uint numEventsInWaitList = 0,
                         const cl_event * eventWaitList = 0,
                         cl_event * events = 0);

///
/// \brief TBMV with simple interfaz
///
clblasStatus clblasStbmv(clblasUplo uplo,
                         clblasDiag diag,
                         size_t K,
                         const HPL::Array<float, 1>& A,
                         HPL::Array<float, 1>& X,
                         const HPL::Device& d = HPL::Device());

///
/// \brief TBMV with full interfaz
///
clblasStatus clblasStbmv(clblasOrder order,
                         clblasUplo uplo,
                         clblasTranspose trans,
                         clblasDiag diag,
                         size_t N,
                         size_t K,
                         const HPL::Array<float, 1>& A,
                         size_t offa,
                         size_t lda,
                         HPL::Array<float, 1>& X,
                         size_t offx,
                         int incx,
                         HPL::Array<float, 1>& scratchBuff,
                         const HPL::Device& d = HPL::Device(),
                         cl_uint numEventsInWaitList = 0,
                         const cl_event * eventWaitList = 0,
                         cl_event * events = 0);

///
/// \brief TBMV with simple interfaz
///
clblasStatus clblasDtbmv(clblasUplo uplo,
                         clblasDiag diag,
                         size_t K,
                         const HPL::Array<double, 1>& A,
                         HPL::Array<double, 1>& X,
                         const HPL::Device& d = HPL::Device());

///
/// \brief TBMV with full interfaz
///
clblasStatus clblasDtbmv(clblasOrder order,
                         clblasUplo uplo,
                         clblasTranspose trans,
                         clblasDiag diag,
                         size_t N,
                         size_t K,
                         const HPL::Array<double, 1>& A,
                         size_t offa,
                         size_t lda,
                         HPL::Array<double, 1>& X,
                         size_t offx,
                         int incx,
                         HPL::Array<double, 1>& scratchBuff,
                         const HPL::Device& d = HPL::Device(),
                         cl_uint numEventsInWaitList = 0,
                         const cl_event * eventWaitList = 0,
                         cl_event * events = 0);

///
/// \brief TBMV with simple interfaz
///
clblasStatus clblasCtbmv(clblasUplo uplo,
                         clblasDiag diag,
                         size_t K,
                         const HPL::Array<HPL::float2, 1>& A,
                         HPL::Array<HPL::float2, 1>& X,
                         const HPL::Device& d = HPL::Device());

///
/// \brief TBMV with full interfaz
///
clblasStatus clblasCtbmv(clblasOrder order,
                         clblasUplo uplo,
                         clblasTranspose trans,
                         clblasDiag diag,
                         size_t N,
                         size_t K,
                         const HPL::Array<HPL::float2, 1>& A,
                         size_t offa,
                         size_t lda,
                         HPL::Array<HPL::float2, 1>& X,
                         size_t offx,
                         int incx,
                         HPL::Array<HPL::float2, 1>& scratchBuff,
                         const HPL::Device& d = HPL::Device(),
                         cl_uint numEventsInWaitList = 0,
                         const cl_event * eventWaitList = 0,
                         cl_event * events = 0);

///
/// \brief SBMV with simple interfaz
///
clblasStatus clblasSsbmv(clblasUplo uplo,
                         size_t K,
                         const HPL::Array<float, 1>& A,
                         const HPL::Array<float, 1>& X,
                         HPL::Array<float, 1>& Y,
                         const HPL::Device& d = HPL::Device());

///
/// \brief SBMV with full interfaz
///
clblasStatus clblasSsbmv(clblasOrder order,
                         clblasUplo uplo,
                         size_t N,
                         size_t K,
                         cl_float alpha,
                         const HPL::Array<float, 1>& A,
                         size_t offa,
                         size_t lda,
                         const HPL::Array<float, 1>& X,
                         size_t offx,
                         int incx,
                         cl_float beta,
                         HPL::Array<float, 1>& Y,
                         size_t offy,
                         int incy,
                         const HPL::Device& d = HPL::Device(),
                         cl_uint numEventsInWaitList = 0,
                         const cl_event * eventWaitList = 0,
                         cl_event * events = 0);

///
/// \brief SBMV with simple interfaz
///
clblasStatus clblasDsbmv(clblasUplo uplo,
                         size_t K,
                         const HPL::Array<double, 1>& A,
                         const HPL::Array<double, 1>& X,
                         HPL::Array<double, 1>& Y,
                         const HPL::Device& d = HPL::Device());

///
/// \brief SBMV with full interfaz
///
clblasStatus clblasDsbmv(clblasOrder order,
                         clblasUplo uplo,
                         size_t N,
                         size_t K,
                         cl_double alpha,
                         const HPL::Array<double, 1>& A,
                         size_t offa,
                         size_t lda,
                         const HPL::Array<double, 1>& X,
                         size_t offx,
                         int incx,
                         cl_double beta,
                         HPL::Array<double, 1>& Y,
                         size_t offy,
                         int incy,
                         const HPL::Device& d = HPL::Device(),
                         cl_uint numEventsInWaitList = 0,
                         const cl_event * eventWaitList = 0,
                         cl_event * events = 0);

///
/// \brief HBMV with simple intefaz
///
clblasStatus clblasChbmv(clblasUplo uplo,
                         size_t K,
                         const HPL::Array<HPL::float2, 1>& A,
                         const HPL::Array<HPL::float2, 1>& X,
                         HPL::Array<HPL::float2, 1>& Y,
                         const HPL::Device& d = HPL::Device());

///
/// \brief HBMV with full intefaz
///
clblasStatus clblasChbmv(clblasOrder order,
                         clblasUplo uplo,
                         size_t N,
                         size_t K,
                         cl_float2 alpha,
                         const HPL::Array<HPL::float2, 1>& A,
                         size_t offa,
                         size_t lda,
                         const HPL::Array<HPL::float2, 1>& X,
                         size_t offx,
                         int incx,
                         cl_float2 beta,
                         HPL::Array<HPL::float2, 1>& Y,
                         size_t offy,
                         int incy,
                         const HPL::Device& d = HPL::Device(),
                         cl_uint numEventsInWaitList = 0,
                         const cl_event * eventWaitList = 0,
                         cl_event * events = 0);

///
/// \brief TBSV with simple interfaz
///
clblasStatus clblasStbsv(clblasUplo uplo,
                         clblasDiag diag,
                         size_t K,
                         const HPL::Array<float, 1>& A,
                         HPL::Array<float, 1>& X,
                         const HPL::Device& d = HPL::Device());

///
/// \brief TBSV with full interfaz
///
clblasStatus clblasStbsv(clblasOrder order,
                         clblasUplo uplo,
                         clblasTranspose trans,
                         clblasDiag diag,
                         size_t N,
                         size_t K,
                         const HPL::Array<float, 1>& A,
                         size_t offa,
                         size_t lda,
                         HPL::Array<float, 1>& X,
                         size_t offx,
                         int incx,
                         const HPL::Device& d = HPL::Device(),
                         cl_uint numEventsInWaitList = 0,
                         const cl_event * eventWaitList = 0,
                         cl_event * events = 0);

///
/// \brief TBSV with simple interfaz
///
clblasStatus clblasDtbsv(clblasUplo uplo,
                         clblasDiag diag,
                         size_t K,
                         const HPL::Array<double, 1>& A,
                         HPL::Array<double, 1>& X,
                         const HPL::Device& d = HPL::Device());

///
/// \brief TBSV with full interfaz
///
clblasStatus clblasDtbsv(clblasOrder order,
                         clblasUplo uplo,
                         clblasTranspose trans,
                         clblasDiag diag,
                         size_t N,
                         size_t K,
                         const HPL::Array<double, 1>& A,
                         size_t offa,
                         size_t lda,
                         HPL::Array<double, 1>& X,
                         size_t offx,
                         int incx,
                         const HPL::Device& d = HPL::Device(),
                         cl_uint numEventsInWaitList = 0,
                         const cl_event * eventWaitList = 0,
                         cl_event * events = 0);

///
/// \brief TBSV with simple interfaz
///
clblasStatus clblasCtbsv(clblasUplo uplo,
                         clblasDiag diag,
                         size_t K,
                         const HPL::Array<HPL::float2, 1>& A,
                         HPL::Array<HPL::float2, 1>& X,
                         const HPL::Device& d = HPL::Device());

///
/// \brief TBSV with full interfaz
///
clblasStatus clblasCtbsv(clblasOrder order,
                         clblasUplo uplo,
                         clblasTranspose trans,
                         clblasDiag diag,
                         size_t N,
                         size_t K,
                         const HPL::Array<HPL::float2, 1>& A,
                         size_t offa,
                         size_t lda,
                         HPL::Array<HPL::float2, 1>& X,
                         size_t offx,
                         int incx,
                         const HPL::Device& d = HPL::Device(),
                         cl_uint numEventsInWaitList = 0,
                         const cl_event * eventWaitList = 0,
                         cl_event * events = 0);

/**********
 * BLAS 3 *
 **********/

///
/// \brief SGEMM with simple interface
///
clblasStatus clblasSgemm(const HPL::Array<float, 2>& x,
                         const HPL::Array<float, 2>& y,
                         HPL::Array<float, 2>& z,
                         const HPL::Device& d = HPL::Device());

///
/// \brief SGEMM with full interface
///
clblasStatus clblasSgemm(clblasOrder order,
                         clblasTranspose transA,
                         clblasTranspose transB,
                         size_t M,
                         size_t N,
                         size_t K,
                         cl_float alpha,
                         const HPL::Array<float, 2>& A,
                         size_t offA,
                         size_t lda,
                         const HPL::Array<float, 2>& B,
                         size_t offB,
                         size_t ldb,
                         cl_float beta,
                         HPL::Array<float, 2>& C,
                         size_t offC,
                         size_t ldc,
                         const HPL::Device& d = HPL::Device(),
                         cl_uint numEventsInWaitList = 0,
                         const cl_event * eventWaitList = 0,
                         cl_event * events = 0);

///
/// \brief SGEMM with simple interface
///
clblasStatus clblasDgemm(const HPL::Array<double, 2>& x,
                         const HPL::Array<double, 2>& y,
                         HPL::Array<double, 2>& z,
                         const HPL::Device& d = HPL::Device());

///
/// \brief SGEMM with full interface
///
clblasStatus clblasDgemm(clblasOrder order,
                         clblasTranspose transA,
                         clblasTranspose transB,
                         size_t M,
                         size_t N,
                         size_t K,
                         cl_double alpha,
                         const HPL::Array<double, 2>& A,
                         size_t offA,
                         size_t lda,
                         const HPL::Array<double, 2>& B,
                         size_t offB,
                         size_t ldb,
                         cl_double beta,
                         HPL::Array<double, 2>& C,
                         size_t offC,
                         size_t ldc,
                         const HPL::Device& d = HPL::Device(),
                         cl_uint numEventsInWaitList = 0,
                         const cl_event * eventWaitList = 0,
                         cl_event * events = 0);

///
/// \brief SGEMM with simple interface
///
clblasStatus clblasCgemm(const HPL::Array<HPL::float2, 2>& x,
                         const HPL::Array<HPL::float2, 2>& y,
                         HPL::Array<HPL::float2, 2>& z,
                         const HPL::Device& d = HPL::Device());

///
/// \brief SGEMM with full interface
///
clblasStatus clblasCgemm(clblasOrder order,
                         clblasTranspose transA,
                         clblasTranspose transB,
                         size_t M,
                         size_t N,
                         size_t K,
                         cl_float2 alpha,
                         const HPL::Array<HPL::float2, 2>& A,
                         size_t offA,
                         size_t lda,
                         const HPL::Array<HPL::float2, 2>& B,
                         size_t offB,
                         size_t ldb,
                         cl_float2 beta,
                         HPL::Array<HPL::float2, 2>& C,
                         size_t offC,
                         size_t ldc,
                         const HPL::Device& d = HPL::Device(),
                         cl_uint numEventsInWaitList = 0,
                         const cl_event * eventWaitList = 0,
                         cl_event * events = 0);

///
/// \brief TRMM with simple interfaz
///
clblasStatus clblasStrmm(clblasSide side,
                         clblasUplo uplo,
                         clblasDiag diag,
                         const HPL::Array<float, 2>& A,
                         HPL::Array<float, 2>& B,
                         const HPL::Device& d = HPL::Device());

///
/// \brief TRMM with full interfaz
///
clblasStatus clblasStrmm(clblasOrder order,
                         clblasSide side,
                         clblasUplo uplo,
                         clblasTranspose transA,
                         clblasDiag diag,
                         size_t M,
                         size_t N,
                         cl_float alpha,
                         const HPL::Array<float, 2>& A,
                         size_t offA,
                         size_t lda,
                         HPL::Array<float, 2>& B,
                         size_t offB,
                         size_t ldb,
                         const HPL::Device& d = HPL::Device(),
                         cl_uint numEventsInWaitList = 0,
                         const cl_event * eventWaitList = 0,
                         cl_event * events = 0);

///
/// \brief TRMM with simple interfaz
///
clblasStatus clblasDtrmm(clblasSide side,
                         clblasUplo uplo,
                         clblasDiag diag,
                         const HPL::Array<double, 2>& A,
                         HPL::Array<double, 2>& B,
                         const HPL::Device& d = HPL::Device());

///
/// \brief TRMM with full interfaz
///
clblasStatus clblasDtrmm(clblasOrder order,
                         clblasSide side,
                         clblasUplo uplo,
                         clblasTranspose transA,
                         clblasDiag diag,
                         size_t M,
                         size_t N,
                         cl_double alpha,
                         const HPL::Array<double, 2>& A,
                         size_t offA,
                         size_t lda,
                         HPL::Array<double, 2>& B,
                         size_t offB,
                         size_t ldb,
                         const HPL::Device& d = HPL::Device(),
                         cl_uint numEventsInWaitList = 0,
                         const cl_event * eventWaitList = 0,
                         cl_event * events = 0);

///
/// \brief TRMM with simple interfaz
///
clblasStatus clblasCtrmm(clblasSide side,
                         clblasUplo uplo,
                         clblasDiag diag,
                         const HPL::Array<HPL::float2, 2>& A,
                         HPL::Array<HPL::float2, 2>& B,
                         const HPL::Device& d = HPL::Device());

///
/// \brief TRMM with full interfaz
///
clblasStatus clblasCtrmm(clblasOrder order,
                         clblasSide side,
                         clblasUplo uplo,
                         clblasTranspose transA,
                         clblasDiag diag,
                         size_t M,
                         size_t N,
                         cl_float2 alpha,
                         const HPL::Array<HPL::float2, 2>& A,
                         size_t offA,
                         size_t lda,
                         HPL::Array<HPL::float2, 2>& B,
                         size_t offB,
                         size_t ldb,
                         const HPL::Device& d = HPL::Device(),
                         cl_uint numEventsInWaitList = 0,
                         const cl_event * eventWaitList = 0,
                         cl_event * events = 0);

///
/// \brief TRSM with simple interfaz
///
clblasStatus clblasStrsm(clblasSide side,
                         clblasUplo uplo,
                         clblasDiag diag,
                         const HPL::Array<float, 2>& A,
                         HPL::Array<float, 2>& B,
                         const HPL::Device& d = HPL::Device());


///
/// \brief TRSM with full interfaz
///
clblasStatus clblasStrsm(clblasOrder order,
                         clblasSide side,
                         clblasUplo uplo,
                         clblasTranspose transA,
                         clblasDiag diag,
                         size_t M,
                         size_t N,
                         cl_float alpha,
                         const HPL::Array<float, 2>& A,
                         size_t offA,
                         size_t lda,
                         HPL::Array<float, 2>& B,
                         size_t offB,
                         size_t ldb,
                         const HPL::Device& d = HPL::Device(),
                         cl_uint numEventsInWaitList = 0,
                         const cl_event * eventWaitList = 0,
                         cl_event * events = 0);

///
/// \brief TRSM with simple interfaz
///
clblasStatus clblasDtrsm(clblasSide side,
                         clblasUplo uplo,
                         clblasDiag diag,
                         const HPL::Array<double, 2>& A,
                         HPL::Array<double, 2>& B,
                         const HPL::Device& d = HPL::Device());


///
/// \brief TRSM with full interfaz
///
clblasStatus clblasDtrsm(clblasOrder order,
                         clblasSide side,
                         clblasUplo uplo,
                         clblasTranspose transA,
                         clblasDiag diag,
                         size_t M,
                         size_t N,
                         cl_double alpha,
                         const HPL::Array<double, 2>& A,
                         size_t offA,
                         size_t lda,
                         HPL::Array<double, 2>& B,
                         size_t offB,
                         size_t ldb,
                         const HPL::Device& d = HPL::Device(),
                         cl_uint numEventsInWaitList = 0,
                         const cl_event * eventWaitList = 0,
                         cl_event * events = 0);

///
/// \brief TRSM with simple interfaz
///
clblasStatus clblasCtrsm(clblasSide side,
                         clblasUplo uplo,
                         clblasDiag diag,
                         const HPL::Array<HPL::float2, 2>& A,
                         HPL::Array<HPL::float2, 2>& B,
                         const HPL::Device& d = HPL::Device());


///
/// \brief TRSM with full interfaz
///
clblasStatus clblasCtrsm(clblasOrder order,
                         clblasSide side,
                         clblasUplo uplo,
                         clblasTranspose transA,
                         clblasDiag diag,
                         size_t M,
                         size_t N,
                         cl_float2 alpha,
                         const HPL::Array<HPL::float2, 2>& A,
                         size_t offA,
                         size_t lda,
                         HPL::Array<HPL::float2, 2>& B,
                         size_t offB,
                         size_t ldb,
                         const HPL::Device& d = HPL::Device(),
                         cl_uint numEventsInWaitList = 0,
                         const cl_event * eventWaitList = 0,
                         cl_event * events = 0);

///
/// \brief SYRK with simple interfaz
///
clblasStatus clblasSsyrk(clblasUplo uplo,
                         const HPL::Array<float, 2>& A,
                         HPL::Array<float, 2>& C,
                         const HPL::Device& d = HPL::Device());

///
/// \brief SYRK with full interfaz
///
clblasStatus clblasSsyrk(clblasOrder order,
                         clblasUplo uplo,
                         clblasTranspose transA,
                         size_t N,
                         size_t K,
                         cl_float alpha,
                         const HPL::Array<float, 2>& A,
                         size_t offA,
                         size_t lda,
                         cl_float beta,
                         HPL::Array<float, 2>& C,
                         size_t offC,
                         size_t ldc,
                         const HPL::Device& d = HPL::Device(),
                         cl_uint numEventsInWaitList = 0,
                         const cl_event * eventWaitList = 0,
                         cl_event * events = 0);

///
/// \brief SYRK with simple interfaz
///
clblasStatus clblasDsyrk(clblasUplo uplo,
                         const HPL::Array<double, 2>& A,
                         HPL::Array<double, 2>& C,
                         const HPL::Device& d = HPL::Device());

///
/// \brief SYRK with full interfaz
///
clblasStatus clblasDsyrk(clblasOrder order,
                         clblasUplo uplo,
                         clblasTranspose transA,
                         size_t N,
                         size_t K,
                         cl_double alpha,
                         const HPL::Array<double, 2>& A,
                         size_t offA,
                         size_t lda,
                         cl_double beta,
                         HPL::Array<double, 2>& C,
                         size_t offC,
                         size_t ldc,
                         const HPL::Device& d = HPL::Device(),
                         cl_uint numEventsInWaitList = 0,
                         const cl_event * eventWaitList = 0,
                         cl_event * events = 0);

///
/// \brief SYRK with simple interfaz
///
clblasStatus clblasCsyrk(clblasUplo uplo,
                         const HPL::Array<HPL::float2, 2>& A,
                         HPL::Array<HPL::float2, 2>& C,
                         const HPL::Device& d = HPL::Device());

///
/// \brief SYRK with full interfaz
///
clblasStatus clblasCsyrk(clblasOrder order,
                         clblasUplo uplo,
                         clblasTranspose transA,
                         size_t N,
                         size_t K,
                         cl_float2 alpha,
                         const HPL::Array<HPL::float2, 2>& A,
                         size_t offA,
                         size_t lda,
                         cl_float2 beta,
                         HPL::Array<HPL::float2, 2>& C,
                         size_t offC,
                         size_t ldc,
                         const HPL::Device& d = HPL::Device(),
                         cl_uint numEventsInWaitList = 0,
                         const cl_event * eventWaitList = 0,
                         cl_event * events = 0);

///
/// \brief SYRK2 with simple interfaz
///
clblasStatus clblasSsyr2k(clblasUplo uplo,
                          const HPL::Array<float, 2>& A,
                          const HPL::Array<float, 2>& B,
                          HPL::Array<float, 2>& C,
                          const HPL::Device& d = HPL::Device());

///
/// \brief SYRK2 with full interfaz
///
clblasStatus clblasSsyr2k(clblasOrder order,
                          clblasUplo uplo,
                          clblasTranspose transAB,
                          size_t N,
                          size_t K,
                          cl_float alpha,
                          const HPL::Array<float, 2>& A,
                          size_t offA,
                          size_t lda,
                          const HPL::Array<float, 2>& B,
                          size_t offB,
                          size_t ldb,
                          cl_float beta,
                          HPL::Array<float, 2>& C,
                          size_t offC,
                          size_t ldc,
                          const HPL::Device& d = HPL::Device(),
                          cl_uint numEventsInWaitList = 0,
                          const cl_event * eventWaitList = 0,
                          cl_event * events = 0);

///
/// \brief SYRK2 with simple interfaz
///
clblasStatus clblasDsyr2k(clblasUplo uplo,
                          const HPL::Array<double, 2>& A,
                          const HPL::Array<double, 2>& B,
                          HPL::Array<double, 2>& C,
                          const HPL::Device& d = HPL::Device());

///
/// \brief SYRK2 with full interfaz
///
clblasStatus clblasDsyr2k(clblasOrder order,
                          clblasUplo uplo,
                          clblasTranspose transAB,
                          size_t N,
                          size_t K,
                          cl_double alpha,
                          const HPL::Array<double, 2>& A,
                          size_t offA,
                          size_t lda,
                          const HPL::Array<double, 2>& B,
                          size_t offB,
                          size_t ldb,
                          cl_double beta,
                          HPL::Array<double, 2>& C,
                          size_t offC,
                          size_t ldc,
                          const HPL::Device& d = HPL::Device(),
                          cl_uint numEventsInWaitList = 0,
                          const cl_event * eventWaitList = 0,
                          cl_event * events = 0);

///
/// \brief SYRK2 with simple interfaz
///
clblasStatus clblasCsyr2k(clblasUplo uplo,
                          const HPL::Array<HPL::float2, 2>& A,
                          const HPL::Array<HPL::float2, 2>& B,
                          HPL::Array<HPL::float2, 2>& C,
                          const HPL::Device& d = HPL::Device());

///
/// \brief SYRK2 with full interfaz
///
clblasStatus clblasCsyr2k(clblasOrder order,
                          clblasUplo uplo,
                          clblasTranspose transAB,
                          size_t N,
                          size_t K,
                          cl_float2 alpha,
                          const HPL::Array<HPL::float2, 2>& A,
                          size_t offA,
                          size_t lda,
                          const HPL::Array<HPL::float2, 2>& B,
                          size_t offB,
                          size_t ldb,
                          cl_float2 beta,
                          HPL::Array<HPL::float2, 2>& C,
                          size_t offC,
                          size_t ldc,
                          const HPL::Device& d = HPL::Device(),
                          cl_uint numEventsInWaitList = 0,
                          const cl_event * eventWaitList = 0,
                          cl_event * events = 0);

///
/// \brief SYMM with simple interfaz
///
clblasStatus clblasSsymm(clblasUplo uplo,
                         clblasSide side,
                         const HPL::Array<float, 2>& A,
                         const HPL::Array<float, 2>& B,
                         HPL::Array<float, 2>& C,
                         const HPL::Device& d = HPL::Device());

///
/// \brief SYMM with full interfaz
///
clblasStatus clblasSsymm(clblasOrder order,
                         clblasSide side,
                         clblasUplo uplo,
                         size_t M,
                         size_t N,
                         cl_float alpha,
                         const HPL::Array<float, 2>& A,
                         size_t offa,
                         size_t lda,
                         const HPL::Array<float, 2>& B,
                         size_t offb,
                         size_t ldb,
                         cl_float beta,
                         HPL::Array<float, 2>& C,
                         size_t offc,
                         size_t ldc,
                         const HPL::Device& d = HPL::Device(),
                         cl_uint numEventsInWaitList = 0,
                         const cl_event * eventWaitList = 0,
                         cl_event * events = 0);

///
/// \brief SYMM with simple interfaz
///
clblasStatus clblasDsymm(clblasUplo uplo,
                         clblasSide side,
                         const HPL::Array<double, 2>& A,
                         const HPL::Array<double, 2>& B,
                         HPL::Array<double, 2>& C,
                         const HPL::Device& d = HPL::Device());

///
/// \brief SYMM with full interfaz
///
clblasStatus clblasDsymm(clblasOrder order,
                         clblasSide side,
                         clblasUplo uplo,
                         size_t M,
                         size_t N,
                         cl_double alpha,
                         const HPL::Array<double, 2>& A,
                         size_t offa,
                         size_t lda,
                         const HPL::Array<double, 2>& B,
                         size_t offb,
                         size_t ldb,
                         cl_double beta,
                         HPL::Array<double, 2>& C,
                         size_t offc,
                         size_t ldc,
                         const HPL::Device& d = HPL::Device(),
                         cl_uint numEventsInWaitList = 0,
                         const cl_event * eventWaitList = 0,
                         cl_event * events = 0);

///
/// \brief SYMM with simple interfaz
///
clblasStatus clblasCsymm(clblasUplo uplo,
                         clblasSide side,
                         const HPL::Array<HPL::float2, 2>& A,
                         const HPL::Array<HPL::float2, 2>& B,
                         HPL::Array<HPL::float2, 2>& C,
                         const HPL::Device& d = HPL::Device());

///
/// \brief SYMM with full interfaz
///
clblasStatus clblasCsymm(clblasOrder order,
                         clblasSide side,
                         clblasUplo uplo,
                         size_t M,
                         size_t N,
                         cl_float2 alpha,
                         const HPL::Array<HPL::float2, 2>& A,
                         size_t offa,
                         size_t lda,
                         const HPL::Array<HPL::float2, 2>& B,
                         size_t offb,
                         size_t ldb,
                         cl_float2 beta,
                         HPL::Array<HPL::float2, 2>& C,
                         size_t offc,
                         size_t ldc,
                         const HPL::Device& d = HPL::Device(),
                         cl_uint numEventsInWaitList = 0,
                         const cl_event * eventWaitList = 0,
                         cl_event * events = 0);

///
/// \brief HEMM with simple interfaz
///
clblasStatus clblasChemm(clblasSide side,
                         clblasUplo uplo,
                         const HPL::Array<HPL::float2, 2>& A,
                         const HPL::Array<HPL::float2, 2>& B,
                         HPL::Array<HPL::float2, 2>& C,
                         const HPL::Device& d = HPL::Device());

///
/// \brief HEMM with full interfaz
///
clblasStatus clblasChemm(clblasOrder order,
                         clblasSide side,
                         clblasUplo uplo,
                         size_t M,
                         size_t N,
                         cl_float2 alpha,
                         const HPL::Array<HPL::float2, 2>& A,
                         size_t offa,
                         size_t lda,
                         const HPL::Array<HPL::float2, 2>& B,
                         size_t offb,
                         size_t ldb,
                         cl_float2 beta,
                         HPL::Array<HPL::float2, 2>& C,
                         size_t offc,
                         size_t ldc,
                         const HPL::Device& d = HPL::Device(),
                         cl_uint numEventsInWaitList = 0,
                         const cl_event * eventWaitList = 0,
                         cl_event * events = 0);

///
/// \brief HERK with simple interfaz
///
clblasStatus clblasCherk(clblasUplo uplo,
                         const HPL::Array<HPL::float2, 2>& A,
                         HPL::Array<HPL::float2, 2>& C,
                         const HPL::Device& d = HPL::Device());

///
/// \brief HERK with full interfaz
///
clblasStatus clblasCherk(clblasOrder order,
                         clblasUplo uplo,
                         clblasTranspose transA,
                         size_t N,
                         size_t K,
                         float alpha,
                         const HPL::Array<HPL::float2, 2>& A,
                         size_t offa,
                         size_t lda,
                         float beta,
                         HPL::Array<HPL::float2, 2>& C,
                         size_t offc,
                         size_t ldc,
                         const HPL::Device& d = HPL::Device(),
                         cl_uint numEventsInWaitList = 0,
                         const cl_event * eventWaitList = 0,
                         cl_event * events = 0);

///
/// \brief HER2K with simple interfaz
///
clblasStatus clblasCher2k(clblasUplo uplo,
                          const HPL::Array<HPL::float2, 2>& A,
                          const HPL::Array<HPL::float2, 2>& B,
                          HPL::Array<HPL::float2, 2>& C,
                          const HPL::Device& d = HPL::Device());

///
/// \brief HER2K with full interfaz
///
clblasStatus clblasCher2k(clblasOrder order,
                          clblasUplo uplo,
                          clblasTranspose trans,
                          size_t N,
                          size_t K,
                          cl_float2 alpha,
                          const HPL::Array<HPL::float2, 2>& A,
                          size_t offa,
                          size_t lda,
                          const HPL::Array<HPL::float2, 2>& B,
                          size_t offb,
                          size_t ldb,
                          cl_float beta,
                          HPL::Array<HPL::float2, 2>& C,
                          size_t offc,
                          size_t ldc,
                          const HPL::Device& d = HPL::Device(),
                          cl_uint numEventsInWaitList = 0,
                          const cl_event * eventWaitList = 0,
                          cl_event * events = 0);

#endif
