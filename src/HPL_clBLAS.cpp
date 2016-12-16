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
/// \file     HPL_clBLAS.cpp
/// \author   Moisés Viñas        <moises.vinas@udc.es>
/// \author   Zeki Bozkus         <zeki.bozkus@khas.edu.tr>
/// \author   Basilio B. Fraguela <basilio.fraguela@udc.es>
/// \author   Diego Nieto         <diego.nieto@bsc.es>
///

#include "HPL_clBLAS.h"


namespace {
  
  enum BLASFuncIndex { 
    AXPY  = 0,
    GEMM  = 1,
    SWAP  = 2,
    SCAL  = 3,
    SSCAL = 4,
    COPY  = 5,
    DOT   = 6,
    ROTG  = 7,
    ROTMG = 8,
    ROT   = 9,
    ROTM  = 10,
    NRM2  = 11,
    iAMAX = 12,
    ASUM  = 13,
    GEMV  = 14,
    SYMV  = 15,
    HEMV  = 16,
    TRMV  = 17,
    TRSV  = 18,
    GER   = 19,
    GERU  = 20,
    GERC  = 21,
    SYR   = 22,
    HER   = 23,
    SYR2  = 24,
    HER2  = 25,
    TPMV  = 26,
    TPSV  = 27,
    SPMV  = 28,
    HPMV  = 29,
    SPR   = 30,
    HPR   = 31,
    SPR2  = 32,
    HPR2  = 33,
    GBMV  = 34,
    TBMV  = 35,
    SBMV  = 36,
    HBMV  = 37,
    TBSV  = 38,
    TRMM  = 39,
    TRSM  = 40,
    SYRK  = 41,
    SYR2K = 42,
    SYMM  = 43,
    HEMM  = 44,
    HERK  = 45,
    HER2K = 46,
    LAST  = 47
  };

  ///Array of BLASFuncIndex::LAST patterns
  HPL::FHandle FHandle_v[BLASFuncIndex::LAST];

#define cl_mem_FRunner(f,i) ((*((f.frunnerTable[i].base_array->dbuffer_ == NULL ) ? f.frunnerTable[i].dbuffer : f.frunnerTable[i].base_array->dbuffer_))())
/*#define is_subb(f,i,d) (f.frunnerTable[i].base_array->getFather()!=NULL && f.frunnerTable[i].base_array->getFather()->hdm.get_dbuffer(d.getPlatformId(), d.getDeviceType(), d.getDeviceNumber()) !=NULL)*/
#define is_subb(x,d) (x.getFather()!=NULL && x.getFather()->hdm.get_dbuffer(d.getPlatformId(), d.getDeviceType(), d.getDeviceNumber()) !=NULL)
#define getLowLevelDim1(x,d) (is_subb(x,d) ? x.getTX().end : x.getDimension(1))
#define getLowLevelDim0(x,d) (is_subb(x,d) ? x.getTY().end : x.getDimension(0))
}

clblasStatus HPL_clblasSetup()
{
  clblasStatus 	err = clblasSetup();
  
  if (err != CL_SUCCESS)
    throw HPL::HPLclBLASException(err, "Exception thrown in HPL_clblasSetup");
  
  // Set memory transfers operations
  FHandle_v[BLASFuncIndex::GEMM].setTable({HPL::HPL_RD, HPL::HPL_RD, HPL::HPL_RDWR});
  FHandle_v[BLASFuncIndex::SWAP].setTable({HPL::HPL_RDWR, HPL::HPL_RDWR});
  FHandle_v[BLASFuncIndex::SCAL].setTable({HPL::HPL_RDWR});
  FHandle_v[BLASFuncIndex::SSCAL].setTable({HPL::HPL_RDWR});
  FHandle_v[BLASFuncIndex::COPY].setTable({HPL::HPL_RD, HPL::HPL_WR});
  FHandle_v[BLASFuncIndex::AXPY].setTable({HPL::HPL_RD, HPL::HPL_RDWR});
  FHandle_v[BLASFuncIndex::DOT].setTable({HPL::HPL_WR, HPL::HPL_RD, HPL::HPL_RD, HPL::HPL_RDWR});
  FHandle_v[BLASFuncIndex::ROTG].setTable({HPL::HPL_RDWR, HPL::HPL_RDWR, HPL::HPL_RDWR, HPL::HPL_RDWR});
  FHandle_v[BLASFuncIndex::ROTMG].setTable({HPL::HPL_RDWR, HPL::HPL_RDWR, HPL::HPL_RDWR, HPL::HPL_RD,  HPL::HPL_RDWR});
  FHandle_v[BLASFuncIndex::ROT].setTable({HPL::HPL_RDWR, HPL::HPL_RDWR});
  FHandle_v[BLASFuncIndex::ROTM].setTable({HPL::HPL_RDWR, HPL::HPL_RDWR, HPL::HPL_RD});
  FHandle_v[BLASFuncIndex::NRM2].setTable({HPL::HPL_WR, HPL::HPL_RD, HPL::HPL_RDWR});
  FHandle_v[BLASFuncIndex::iAMAX].setTable({HPL::HPL_WR, HPL::HPL_RD, HPL::HPL_RDWR});
  FHandle_v[BLASFuncIndex::ASUM].setTable({HPL::HPL_WR, HPL::HPL_RD, HPL::HPL_RDWR});
  FHandle_v[BLASFuncIndex::GEMV].setTable({HPL::HPL_RD, HPL::HPL_RD, HPL::HPL_RDWR});
  FHandle_v[BLASFuncIndex::SYMV].setTable({HPL::HPL_RD, HPL::HPL_RD, HPL::HPL_RDWR});
  FHandle_v[BLASFuncIndex::HEMV].setTable({HPL::HPL_RD, HPL::HPL_RD, HPL::HPL_RDWR});
  FHandle_v[BLASFuncIndex::TRMV].setTable({HPL::HPL_RD, HPL::HPL_RDWR, HPL::HPL_RDWR});
  FHandle_v[BLASFuncIndex::TRSV].setTable({HPL::HPL_RD, HPL::HPL_RDWR});
  FHandle_v[BLASFuncIndex::GER].setTable({HPL::HPL_RD, HPL::HPL_RD, HPL::HPL_RDWR});
  FHandle_v[BLASFuncIndex::GERU].setTable({HPL::HPL_RD, HPL::HPL_RD, HPL::HPL_RDWR});
  FHandle_v[BLASFuncIndex::GERC].setTable({HPL::HPL_RD, HPL::HPL_RD, HPL::HPL_RDWR});
  FHandle_v[BLASFuncIndex::SYR].setTable({HPL::HPL_RD, HPL::HPL_RDWR});
  FHandle_v[BLASFuncIndex::HER].setTable({HPL::HPL_RD, HPL::HPL_RDWR});
  FHandle_v[BLASFuncIndex::SYR2].setTable({HPL::HPL_RD, HPL::HPL_RD, HPL::HPL_RDWR});
  FHandle_v[BLASFuncIndex::HER2].setTable({HPL::HPL_RD, HPL::HPL_RD, HPL::HPL_RDWR});
  FHandle_v[BLASFuncIndex::TPMV].setTable({HPL::HPL_RD, HPL::HPL_RDWR, HPL::HPL_RDWR});
  FHandle_v[BLASFuncIndex::TPSV].setTable({HPL::HPL_RD, HPL::HPL_RDWR});
  FHandle_v[BLASFuncIndex::SPMV].setTable({HPL::HPL_RD, HPL::HPL_RD, HPL::HPL_RDWR});
  FHandle_v[BLASFuncIndex::HPMV].setTable({HPL::HPL_RD, HPL::HPL_RD, HPL::HPL_RDWR});
  FHandle_v[BLASFuncIndex::SPR].setTable({HPL::HPL_RD, HPL::HPL_RDWR});
  FHandle_v[BLASFuncIndex::HPR].setTable({HPL::HPL_RD, HPL::HPL_RDWR});
  FHandle_v[BLASFuncIndex::SPR2].setTable({HPL::HPL_RD, HPL::HPL_RD, HPL::HPL_RDWR});
  FHandle_v[BLASFuncIndex::HPR2].setTable({HPL::HPL_RD, HPL::HPL_RD, HPL::HPL_RDWR});
  FHandle_v[BLASFuncIndex::GBMV].setTable({HPL::HPL_RD, HPL::HPL_RD, HPL::HPL_RDWR});
  FHandle_v[BLASFuncIndex::TBMV].setTable({HPL::HPL_RD, HPL::HPL_RDWR, HPL::HPL_RDWR});
  FHandle_v[BLASFuncIndex::SBMV].setTable({HPL::HPL_RD, HPL::HPL_RD, HPL::HPL_RDWR});
  FHandle_v[BLASFuncIndex::HBMV].setTable({HPL::HPL_RD, HPL::HPL_RD, HPL::HPL_RDWR});
  FHandle_v[BLASFuncIndex::TBSV].setTable({HPL::HPL_RD, HPL::HPL_RDWR});
  FHandle_v[BLASFuncIndex::TRMM].setTable({HPL::HPL_RD, HPL::HPL_RDWR});
  FHandle_v[BLASFuncIndex::TRSM].setTable({HPL::HPL_RD, HPL::HPL_RDWR});
  FHandle_v[BLASFuncIndex::SYRK].setTable({HPL::HPL_RD, HPL::HPL_RDWR});
  FHandle_v[BLASFuncIndex::SYR2K].setTable({HPL::HPL_RD, HPL::HPL_RD, HPL::HPL_RDWR});
  FHandle_v[BLASFuncIndex::SYMM].setTable({HPL::HPL_RD, HPL::HPL_RD, HPL::HPL_RDWR});
  FHandle_v[BLASFuncIndex::HEMM].setTable({HPL::HPL_RD, HPL::HPL_RD, HPL::HPL_RDWR});
  FHandle_v[BLASFuncIndex::HERK].setTable({HPL::HPL_RD, HPL::HPL_RDWR});
  FHandle_v[BLASFuncIndex::HER2K].setTable({HPL::HPL_RD, HPL::HPL_RD, HPL::HPL_RDWR});

  return err;
}

void HPL_clblasTeardown()
{
  clblasTeardown();
}

clblasStatus clBlasSswap(size_t N,
                         HPL::Array<float, 1>& X,
                         size_t offx,
                         int incx,
                         HPL::Array<float, 1>& Y,
                         size_t offy,
                         int incy,
                         const HPL::Device& d,
                         cl_uint numEventsInWaitList,
                         const cl_event * eventWaitList,
                         cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::SWAP);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0,0), X, Y);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasSswap(N,
            cl_mem_FRunner(f, 0),
            offx,
            incx,
            cl_mem_FRunner(f, 1),
            offy,
            incy,
            1,
            &cq,
            numEventsInWaitList,
            eventWaitList,
            events);
    if (err != CL_SUCCESS)
    throw HPL::HPLclBLASException(err, "Exception thrown in clblasSswap");

    return err;
}

clblasStatus clBlasSswap(HPL::Array<float, 1>& X,
                         HPL::Array<float, 1>& Y,
                         const HPL::Device& d)
{
    return clBlasSswap(X.getDimension(0),
            X,
            0,
            1,
            Y,
            0,
            1,
            d);
}


clblasStatus clBlasDswap(size_t N,
                         HPL::Array<double, 1>& X,
                         size_t offx,
                         int incx,
                         HPL::Array<double, 1>& Y,
                         size_t offy,
                         int incy,
                         const HPL::Device& d,
                         cl_uint numEventsInWaitList,
                         const cl_event * eventWaitList,
                         cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::SWAP);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0,0), X, Y);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasDswap(N,
            cl_mem_FRunner(f, 0),
            offx,
            incx,
            cl_mem_FRunner(f, 1),
            offy,
            incy,
            1,
            &cq,
            numEventsInWaitList,
            eventWaitList,
            events);
    if (err != CL_SUCCESS)
    throw HPL::HPLclBLASException(err, "Exception thrown in clblasDswap");

    return err;
}

clblasStatus clBlasDswap(HPL::Array<double, 1>& X,
                         HPL::Array<double, 1>& Y,
                         const HPL::Device& d)
{
    return clBlasDswap(X.getDimension(0),
            X,
            0,
            1,
            Y,
            0,
            1,
            d);
}

/// SWAP with full interface
clblasStatus clBlasCswap(size_t N,
                         HPL::Array<HPL::float2, 1>& X,
                         size_t offx,
                         int incx,
                         HPL::Array<HPL::float2, 1>& Y,
                         size_t offy,
                         int incy,
                         const HPL::Device& d,
                         cl_uint numEventsInWaitList,
                         const cl_event * eventWaitList,
                         cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::SWAP);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0,0), X, Y);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasCswap(N,
                                   cl_mem_FRunner(f, 0),
                                   offx,
                                   incx,
                                   cl_mem_FRunner(f, 1),
                                   offy,
                                   incy,
                                   1,
                                   &cq,
                                   numEventsInWaitList,
                                   eventWaitList,
                                   events);
    if (err != CL_SUCCESS)
    throw HPL::HPLclBLASException(err, "Exception thrown in clblasCswap");

    return err;
}

/// SWAP with simple interface
clblasStatus clBlasCswap(HPL::Array<HPL::float2, 1>& X,
                         HPL::Array<HPL::float2, 1>& Y,
                         const HPL::Device& d)
{
    return clBlasCswap(X.getDimension(0),
            X,
            0,
            1,
            Y,
            0,
            1,
            d);
}

clblasStatus clBlasSscal(size_t N,
                         cl_float alpha,
                         HPL::Array<float, 1>& X,
                         size_t offx,
                         int incx,
                         const HPL::Device& d,
                         cl_uint numEventsInWaitList,
                         const cl_event * eventWaitList,
                         cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::SCAL);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0,0), X);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasSscal(N,
            alpha,
            cl_mem_FRunner(f, 0),
            offx,
            incx,
            1,
            &cq,
            numEventsInWaitList,
            eventWaitList,
            events);
    if (err != CL_SUCCESS)
    throw HPL::HPLclBLASException(err, "Exception thrown in clblasSscal");

    return err;
}


clblasStatus clBlasSscal(cl_float alpha,
                         HPL::Array<float, 1>& X,
                         const HPL::Device& d) {
    return clBlasSscal(X.getDimension(0),
            alpha,
            X,
            0,
            1,
            d);
}

clblasStatus clBlasDscal(size_t N,
                         cl_double alpha,
                         HPL::Array<double, 1>& X,
                         size_t offx,
                         int incx,
                         const HPL::Device& d,
                         cl_uint numEventsInWaitList,
                         const cl_event * eventWaitList,
                         cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::SCAL);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0,0), X);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasDscal(N,
            alpha,
            cl_mem_FRunner(f, 0),
            offx,
            incx,
            1,
            &cq,
            numEventsInWaitList,
            eventWaitList,
            events);
    if (err != CL_SUCCESS)
    throw HPL::HPLclBLASException(err, "Exception thrown in clblasDscal");

    return err;
}

clblasStatus clBlasDscal(cl_double alpha,
                         HPL::Array<double, 1>& X,
                         const HPL::Device& d) {
    return clBlasDscal(X.getDimension(0),
            alpha,
            X,
            0,
            1,
            d);
}

clblasStatus clBlasCscal(size_t N,
                         cl_float2 alpha,
                         HPL::Array<HPL::float2, 1>& X,
                         size_t offx,
                         int incx,
                         const HPL::Device& d,
                         cl_uint numEventsInWaitList,
                         const cl_event * eventWaitList,
                         cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::SCAL);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0,0), X);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasCscal(N,
            alpha,
            cl_mem_FRunner(f, 0),
            offx,
            incx,
            1,
            &cq,
            numEventsInWaitList,
            eventWaitList,
            events);
    if (err != CL_SUCCESS)
    throw HPL::HPLclBLASException(err, "Exception thrown in clblasCscal");

    return err;
}

clblasStatus clBlasCscal(cl_float2 alpha,
                         HPL::Array<HPL::float2, 1>& X,
                         const HPL::Device& d)
{
    return clBlasCscal(X.getDimension(0),
            alpha,
            X,
            0,
            1,
            d);
}

clblasStatus clBlasCsscal(size_t N,
                          cl_float alpha,
                          HPL::Array<HPL::float2, 1>& X,
                          size_t offx,
                          int incx,
                          const HPL::Device& d,
                          cl_uint numEventsInWaitList,
                          const cl_event * eventWaitList,
                          cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::SSCAL);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0,0), X);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasCsscal(N,
            alpha,
            cl_mem_FRunner(f, 0),
            offx,
            incx,
            1,
            &cq,
            numEventsInWaitList,
            eventWaitList,
            events);
    if (err != CL_SUCCESS)
    throw HPL::HPLclBLASException(err, "Exception thrown in clblasCsscal");

    return err;
}

clblasStatus clBlasCsscal(cl_float alpha,
                          HPL::Array<HPL::float2, 1>& X,
                          const HPL::Device& d)
{
    return clBlasCsscal(X.getDimension(0),
            alpha,
            X,
            0,
            1,
            d);
}

clblasStatus clBlasScopy(size_t N,
                         const HPL::Array<float, 1>& X,
                         size_t offx,
                         int incx,
                         HPL::Array<float, 1>& Y,
                         size_t offy,
                         int incy,
                         const HPL::Device& d,
                         cl_uint numEventsInWaitList,
                         const cl_event * eventWaitList,
                         cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::COPY);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0,0), const_cast<HPL::Array<float, 1>&>(X), Y);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasScopy(N,
            cl_mem_FRunner(f, 0),
            offx,
            incx,
            cl_mem_FRunner(f, 1),
            offy,
            incy,
            1,
            &cq,
            numEventsInWaitList,
            eventWaitList,
            events);
    if (err != CL_SUCCESS)
    throw HPL::HPLclBLASException(err, "Exception thrown in clblasScopy");

    return err;
}

clblasStatus clBlasScopy(const HPL::Array<float, 1>& X,
                         HPL::Array<float, 1>& Y,
                         const HPL::Device& d)
{
    return clBlasScopy(X.getDimension(0),
            X,
            0,
            1,
            Y,
            0,
            1,
            d);
}

clblasStatus clBlasDcopy(size_t N,
                         const HPL::Array<double, 1>& X,
                         size_t offx,
                         int incx,
                         HPL::Array<double, 1>& Y,
                         size_t offy,
                         int incy,
                         const HPL::Device& d,
                         cl_uint numEventsInWaitList,
                         const cl_event * eventWaitList,
                         cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::COPY);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0,0), const_cast<HPL::Array<double, 1>&>(X), Y);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasDcopy(N,
            cl_mem_FRunner(f, 0),
            offx,
            incx,
            cl_mem_FRunner(f, 1),
            offy,
            incy,
            1,
            &cq,
            numEventsInWaitList,
            eventWaitList,
            events);
    if (err != CL_SUCCESS)
    throw HPL::HPLclBLASException(err, "Exception thrown in clblasDcopy");

    return err;
}

clblasStatus clBlasDcopy(const HPL::Array<double, 1>& X,
                         HPL::Array<double, 1>& Y,
                         const HPL::Device& d)
{
    return clBlasDcopy(X.getDimension(0),
            X,
            0,
            1,
            Y,
            0,
            1,
            d);
}

clblasStatus clBlasCcopy(size_t N,
                         const HPL::Array<HPL::float2, 1>& X,
                         size_t offx,
                         int incx,
                         HPL::Array<HPL::float2, 1>& Y,
                         size_t offy,
                         int incy,
                         const HPL::Device& d,
                         cl_uint numEventsInWaitList,
                         const cl_event * eventWaitList,
                         cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::COPY);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0,0), const_cast<HPL::Array<HPL::float2, 1>&>(X), Y);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasCcopy(N,
            cl_mem_FRunner(f, 0),
            offx,
            incx,
            cl_mem_FRunner(f, 1),
            offy,
            incy,
            1,
            &cq,
            numEventsInWaitList,
            eventWaitList,
            events);
    if (err != CL_SUCCESS)
    throw HPL::HPLclBLASException(err, "Exception thrown in clblasCcopy");

    return err;
}

clblasStatus clBlasCcopy(const HPL::Array<HPL::float2, 1>& X,
                         HPL::Array<HPL::float2, 1>& Y,
                         const HPL::Device& d)
{
    return clBlasCcopy(X.getDimension(0),
            X,
            0,
            1,
            Y,
            0,
            1,
            d);
}

clblasStatus clblasSaxpy(size_t     N,
             cl_float   alpha,
             const HPL::Array<float, 1>& x,
             size_t     offx,
             int    incx,
             HPL::Array<float, 1>& y,
             size_t     offy,
             int    incy,
             const HPL::Device& d,
             cl_uint      numEventsInWaitList,
             const cl_event * eventWaitList,
             cl_event *       events)
{
  HPL::FRunner f(FHandle_v + BLASFuncIndex::AXPY);

  f.device(d).fillInFRunnerTable(0, HPL::Tuple(0,0), const_cast<HPL::Array<float, 1>&>(x), y);

  cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

  clblasStatus err = clblasSaxpy(N,
                 alpha,
                 cl_mem_FRunner(f, 0),
                 offx,
                 incx,
                 cl_mem_FRunner(f, 1),
                 offy,
                 incy,
                 1,
                 &cq,
                 numEventsInWaitList,
                 eventWaitList,
                 events);
  if (err != CL_SUCCESS)
    throw HPL::HPLclBLASException(err, "Exception thrown in clblasSaxpy");

  return err;
}

clblasStatus clblasSaxpy(cl_float alpha,
             const HPL::Array<float, 1>& x,
             HPL::Array<float, 1>& y,
             const HPL::Device& d)
{
  return clblasSaxpy(x.getDimension(0),
                 alpha,
                 x,
                 0,
                 1,
                 y,
                 0,
                 1,
                 d);
}

clblasStatus clblasDaxpy(size_t     N,
             cl_double   alpha,
             const HPL::Array<double, 1>& x,
             size_t     offx,
             int    incx,
             HPL::Array<double, 1>& y,
             size_t     offy,
             int    incy,
             const HPL::Device& d,
             cl_uint      numEventsInWaitList,
             const cl_event * eventWaitList,
             cl_event *       events)
{
  HPL::FRunner f(FHandle_v + BLASFuncIndex::AXPY);

  f.device(d).fillInFRunnerTable(0, HPL::Tuple(0,0), const_cast<HPL::Array<double, 1>&>(x), y);

  cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

  clblasStatus err = clblasDaxpy(N,
                 alpha,
                 cl_mem_FRunner(f, 0),
                 offx,
                 incx,
                 cl_mem_FRunner(f, 1),
                 offy,
                 incy,
                 1,
                 &cq,
                 numEventsInWaitList,
                 eventWaitList,
                 events);
  if (err != CL_SUCCESS)
    throw HPL::HPLclBLASException(err, "Exception thrown in clblasDaxpy");

  return err;
}

clblasStatus clblasDaxpy(cl_double alpha,
             const HPL::Array<double, 1>& x,
             HPL::Array<double, 1>& y,
             const HPL::Device& d)
{
  return clblasDaxpy(x.getDimension(0),
                 alpha,
                 x,
                 0,
                 1,
                 y,
                 0,
                 1,
                 d);
}

clblasStatus clblasCaxpy(size_t     N,
             cl_float2   alpha,
             const HPL::Array<HPL::float2, 1>& x,
             size_t     offx,
             int    incx,
             HPL::Array<HPL::float2, 1>& y,
             size_t     offy,
             int    incy,
             const HPL::Device& d,
             cl_uint      numEventsInWaitList,
             const cl_event * eventWaitList,
             cl_event *       events)
{
  HPL::FRunner f(FHandle_v + BLASFuncIndex::AXPY);

  f.device(d).fillInFRunnerTable(0, HPL::Tuple(0,0), const_cast<HPL::Array<HPL::float2, 1>&>(x), y);

  cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

  clblasStatus err = clblasCaxpy(N,
                 alpha,
                 cl_mem_FRunner(f, 0),
                 offx,
                 incx,
                 cl_mem_FRunner(f, 1),
                 offy,
                 incy,
                 1,
                 &cq,
                 numEventsInWaitList,
                 eventWaitList,
                 events);
  if (err != CL_SUCCESS)
    throw HPL::HPLclBLASException(err, "Exception thrown in clblasCaxpy");

  return err;
}

clblasStatus clblasCaxpy(cl_float2 alpha,
             const HPL::Array<HPL::float2, 1>& x,
             HPL::Array<HPL::float2, 1>& y,
             const HPL::Device& d)
{
  return clblasCaxpy(x.getDimension(0),
                 alpha,
                 x,
                 0,
                 1,
                 y,
                 0,
                 1,
                 d);
}

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
                        const HPL::Device& d,
                        cl_uint numEventsInWaitList,
                        const cl_event * eventWaitList,
                        cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::DOT);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0,0), dotProduct, const_cast<HPL::Array<float, 1>&>(X), const_cast<HPL::Array<float, 1>&>(Y), scratchBuff);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasSdot(N,
            cl_mem_FRunner(f, 0),
            offDP,
            cl_mem_FRunner(f, 1),
            offx,
            incx,
            cl_mem_FRunner(f, 2),
            offy,
            incy,
            cl_mem_FRunner(f, 3),
            1,
            &cq,
            numEventsInWaitList,
            eventWaitList,
            events);
    if (err != CL_SUCCESS)
    throw HPL::HPLclBLASException(err, "Exception thrown in clblasSdot");

    return err;
}

clblasStatus clblasSdot(HPL::Array<float, 1>& dotProduct,
                        const HPL::Array<float, 1>& X,
                        const HPL::Array<float, 1>& Y,
                        const HPL::Device& d)
{
    HPL::Array<float, 1> scratchBuff(X.getDimension(0));
    return clblasSdot(X.getDimension(0),
            dotProduct,
            0,
            X,
            0,
            1,
            Y,
            0,
            1,
            scratchBuff,
            d);
}

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
                        const HPL::Device& d,
                        cl_uint numEventsInWaitList,
                        const cl_event * eventWaitList,
                        cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::DOT);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0,0), dotProduct, const_cast<HPL::Array<double, 1>&>(X), const_cast<HPL::Array<double, 1>&>(Y), scratchBuff);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasDdot(N,
            cl_mem_FRunner(f, 0),
            offDP,
            cl_mem_FRunner(f, 1),
            offx,
            incx,
            cl_mem_FRunner(f, 2),
            offy,
            incy,
            cl_mem_FRunner(f, 3),
            1,
            &cq,
            numEventsInWaitList,
            eventWaitList,
            events);
    if (err != CL_SUCCESS)
    throw HPL::HPLclBLASException(err, "Exception thrown in clblasDdot");

    return err;
}

clblasStatus clblasDdot(HPL::Array<double, 1>& dotProduct,
                        const HPL::Array<double, 1>& X,
                        const HPL::Array<double, 1>& Y,
                        const HPL::Device& d)
{
    HPL::Array<double, 1> scratchBuff(X.getDimension(0));
    return clblasDdot(X.getDimension(0),
            dotProduct,
            0,
            X,
            0,
            1,
            Y,
            0,
            1,
            scratchBuff,
            d);
}

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
                         const HPL::Device& d,
                         cl_uint numEventsInWaitList,
                         const cl_event * eventWaitList,
                         cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::DOT);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0,0), dotProduct, const_cast<HPL::Array<HPL::float2, 1>&>(X), const_cast<HPL::Array<HPL::float2, 1>&>(Y), scratchBuff);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasCdotu(N,
            cl_mem_FRunner(f, 0),
            offDP,
            cl_mem_FRunner(f, 1),
            offx,
            incx,
            cl_mem_FRunner(f, 2),
            offy,
            incy,
            cl_mem_FRunner(f, 3),
            1,
            &cq,
            numEventsInWaitList,
            eventWaitList,
            events);
    if (err != CL_SUCCESS)
    throw HPL::HPLclBLASException(err, "Exception thrown in clblasCdotu");

    return err;
}

clblasStatus clblasCdotu(HPL::Array<HPL::float2, 1>& dotProduct,
                         const HPL::Array<HPL::float2, 1>& X,
                         const HPL::Array<HPL::float2, 1>& Y,
                         const HPL::Device& d)
{
    HPL::Array<HPL::float2, 1> scratchBuff(X.getDimension(0));
    return clblasCdotu(X.getDimension(0),
            dotProduct,
            0,
            X,
            0,
            1,
            Y,
            0,
            1,
            scratchBuff,
            d);
}

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
                         const HPL::Device& d,
                         cl_uint numEventsInWaitList,
                         const cl_event * eventWaitList,
                         cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::DOT);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0,0), dotProduct, const_cast<HPL::Array<HPL::float2, 1>&>(X), const_cast<HPL::Array<HPL::float2, 1>&>(Y), scratchBuff);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasCdotc(N,
            cl_mem_FRunner(f, 0),
            offDP,
            cl_mem_FRunner(f, 1),
            offx,
            incx,
            cl_mem_FRunner(f, 2),
            offy,
            incy,
            cl_mem_FRunner(f, 3),
            1,
            &cq,
            numEventsInWaitList,
            eventWaitList,
            events);
    if (err != CL_SUCCESS)
    throw HPL::HPLclBLASException(err, "Exception thrown in clblasCdotc");

    return err;
}

clblasStatus clblasCdotc(HPL::Array<HPL::float2, 1>& dotProduct,
                         const HPL::Array<HPL::float2, 1>& X,
                         const HPL::Array<HPL::float2, 1>& Y,
                         const HPL::Device& d)
{
    HPL::Array<HPL::float2, 1> scratchBuff(X.getDimension(0));
    return clblasCdotc(X.getDimension(0),
            dotProduct,
            0,
            X,
            0,
            1,
            Y,
            0,
            1,
            scratchBuff,
            d);
}

clblasStatus clblasSrotg(HPL::Array<float, 1>& SA,
                         size_t offSA,
                         HPL::Array<float, 1>& SB,
                         size_t offSB,
                         HPL::Array<float, 1>& C,
                         size_t offC,
                         HPL::Array<float, 1>& S,
                         size_t offS,
                         const HPL::Device& d,
                         cl_uint numEventsInWaitList,
                         const cl_event * eventWaitList,
                         cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::ROTG);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0,0),  SA, SB, C, S);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasSrotg(cl_mem_FRunner(f, 0),
            offSA,
            cl_mem_FRunner(f, 1),
            offSB,
            cl_mem_FRunner(f, 2),
            offC,
            cl_mem_FRunner(f, 3),
            offS,
            1,
            &cq,
            numEventsInWaitList,
            eventWaitList,
            events);
    if (err != CL_SUCCESS)
    throw HPL::HPLclBLASException(err, "Exception thrown in clblasSrotg");

    return err;
}

clblasStatus clblasSrotg(HPL::Array<float, 1>& SA,
                         HPL::Array<float, 1>& SB,
                         HPL::Array<float, 1>& C,
                         HPL::Array<float, 1>& S,
                         const HPL::Device& d)
{
    return clblasSrotg(SA,
            0,
            SB,
            0,
            C,
            0,
            S,
            0,
            d);
}

clblasStatus clblasDrotg(HPL::Array<double, 1>& SA,
                         size_t offSA,
                         HPL::Array<double, 1>& SB,
                         size_t offSB,
                         HPL::Array<double, 1>& C,
                         size_t offC,
                         HPL::Array<double, 1>& S,
                         size_t offS,
                         const HPL::Device& d,
                         cl_uint numEventsInWaitList,
                         const cl_event * eventWaitList,
                         cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::ROTG);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0,0),  SA, SB, C, S);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasDrotg(cl_mem_FRunner(f, 0),
            offSA,
            cl_mem_FRunner(f, 1),
            offSB,
            cl_mem_FRunner(f, 2),
            offC,
            cl_mem_FRunner(f, 3),
            offS,
            1,
            &cq,
            numEventsInWaitList,
            eventWaitList,
            events);
    if (err != CL_SUCCESS)
    throw HPL::HPLclBLASException(err, "Exception thrown in clblasDrotg");

    return err;
}

clblasStatus clblasDrotg(HPL::Array<double, 1>& SA,
                         HPL::Array<double, 1>& SB,
                         HPL::Array<double, 1>& C,
                         HPL::Array<double, 1>& S,
                         const HPL::Device& d)
{
    return clblasDrotg(SA,
            0,
            SB,
            0,
            C,
            0,
            S,
            0,
            d);
}

clblasStatus clblasCrotg(HPL::Array<HPL::float2, 1>& SA,
                         size_t offSA,
                         HPL::Array<HPL::float2, 1>& SB,
                         size_t offSB,
                         HPL::Array<float, 1>& C,
                         size_t offC,
                         HPL::Array<HPL::float2, 1>& S,
                         size_t offS,
                         const HPL::Device& d,
                         cl_uint numEventsInWaitList,
                         const cl_event * eventWaitList,
                         cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::ROTG);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0,0),  SA, SB, C, S);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasCrotg(cl_mem_FRunner(f, 0),
            offSA,
            cl_mem_FRunner(f, 1),
            offSB,
            cl_mem_FRunner(f, 2),
            offC,
            cl_mem_FRunner(f, 3),
            offS,
            1,
            &cq,
            numEventsInWaitList,
            eventWaitList,
            events);
    if (err != CL_SUCCESS)
    throw HPL::HPLclBLASException(err, "Exception thrown in clblasCrotg");

    return err;
}

clblasStatus clblasCrotg(HPL::Array<HPL::float2, 1>& SA,
                         HPL::Array<HPL::float2, 1>& SB,
                         HPL::Array<float, 1>& C,
                         HPL::Array<HPL::float2, 1>& S,
                         const HPL::Device& d)
{
    return clblasCrotg(SA,
            0,
            SB,
            0,
            C,
            0,
            S,
            0,
            d);
}

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
                          const HPL::Device& d,
                          cl_uint numEventsInWaitList,
                          const cl_event * eventWaitList,
                          cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::ROTMG);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0,0),  SD1, SD2, SX1, const_cast<HPL::Array<float, 1>&>(SY1), SPARAM);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasSrotmg(cl_mem_FRunner(f, 0),
            offSD1,
            cl_mem_FRunner(f, 1),
            offSD2,
            cl_mem_FRunner(f, 2),
            offSX1,
            cl_mem_FRunner(f, 3),
            offSY1,
            cl_mem_FRunner(f, 4),
            offSparam,
            1,
            &cq,
            numEventsInWaitList,
            eventWaitList,
            events);
    if (err != CL_SUCCESS)
    throw HPL::HPLclBLASException(err, "Exception thrown in clblasSrotmg");

    return err;
}

clblasStatus clblasSrotmg(HPL::Array<float, 1>& SD1,
                          HPL::Array<float, 1>& SD2,
                          HPL::Array<float, 1>& SX1,
                          const HPL::Array<float, 1>& SY1,
                          HPL::Array<float, 1>& SPARAM,
                          const HPL::Device& d)
{
    return clblasSrotmg(SD1,
            0,
            SD2,
            0,
            SX1,
            0,
            SY1,
            0,
            SPARAM,
            0,
            d);
}

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
                          const HPL::Device& d,
                          cl_uint numEventsInWaitList,
                          const cl_event * eventWaitList,
                          cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::ROTMG);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0,0),  SD1, SD2, SX1, const_cast<HPL::Array<double, 1>&>(SY1), SPARAM);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasDrotmg(cl_mem_FRunner(f, 0),
            offSD1,
            cl_mem_FRunner(f, 1),
            offSD2,
            cl_mem_FRunner(f, 2),
            offSX1,
            cl_mem_FRunner(f, 3),
            offSY1,
            cl_mem_FRunner(f, 4),
            offSparam,
            1,
            &cq,
            numEventsInWaitList,
            eventWaitList,
            events);
    if (err != CL_SUCCESS)
    throw HPL::HPLclBLASException(err, "Exception thrown in clblasDrotmg");

    return err;
}

clblasStatus clblasDrotmg(HPL::Array<double, 1>& SD1,
                          HPL::Array<double, 1>& SD2,
                          HPL::Array<double, 1>& SX1,
                          const HPL::Array<double, 1>& SY1,
                          HPL::Array<double, 1>& SPARAM,
                          const HPL::Device& d)
{
    return clblasDrotmg(SD1,
            0,
            SD2,
            0,
            SX1,
            0,
            SY1,
            0,
            SPARAM,
            0,
            d);
}

clblasStatus clblasSrot(size_t N,
                        HPL::Array<float, 1>& X,
                        size_t offx,
                        int incx,
                        HPL::Array<float, 1>& Y,
                        size_t offy,
                        int incy,
                        cl_float C,
                        cl_float S,
                        const HPL::Device& d,
                        cl_uint numEventsInWaitList,
                        const cl_event * eventWaitList,
                        cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::ROT);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0,0),  X, Y);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasSrot(N,
            cl_mem_FRunner(f, 0),
            offx,
            incx,
            cl_mem_FRunner(f, 1),
            offy,
            incy,
            C,
            S,
            1,
            &cq,
            numEventsInWaitList,
            eventWaitList,
            events);
    if (err != CL_SUCCESS)
    throw HPL::HPLclBLASException(err, "Exception thrown in clblasSrot");

    return err;
}

clblasStatus clblasSrot(HPL::Array<float, 1>& X,
                        HPL::Array<float, 1>& Y,
                        cl_float C,
                        cl_float S,
                        const HPL::Device& d)
{
    return clblasSrot(X.getDimension(0),
            X,
            0,
            1,
            Y,
            0,
            1,
            C,
            S,
            d);
}

clblasStatus clblasDrot(size_t N,
                        HPL::Array<double, 1>& X,
                        size_t offx,
                        int incx,
                        HPL::Array<double, 1>& Y,
                        size_t offy,
                        int incy,
                        cl_double C,
                        cl_double S,
                        const HPL::Device& d,
                        cl_uint numEventsInWaitList,
                        const cl_event * eventWaitList,
                        cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::ROT);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0,0),  X, Y);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasDrot(N,
            cl_mem_FRunner(f, 0),
            offx,
            incx,
            cl_mem_FRunner(f, 1),
            offy,
            incy,
            C,
            S,
            1,
            &cq,
            numEventsInWaitList,
            eventWaitList,
            events);
    if (err != CL_SUCCESS)
    throw HPL::HPLclBLASException(err, "Exception thrown in clblasDrot");

    return err;
}

clblasStatus clblasDrot(HPL::Array<double, 1>& X,
                        HPL::Array<double, 1>& Y,
                        cl_double C,
                        cl_double S,
                        const HPL::Device& d)
{
    return clblasDrot(X.getDimension(0),
            X,
            0,
            1,
            Y,
            0,
            1,
            C,
            S,
            d);
}

clblasStatus clblasCsrot(size_t N,
                        HPL::Array<HPL::float2, 1>& X,
                        size_t offx,
                        int incx,
                        HPL::Array<HPL::float2, 1>& Y,
                        size_t offy,
                        int incy,
                        cl_float C,
                        cl_float S,
                        const HPL::Device& d,
                        cl_uint numEventsInWaitList,
                        const cl_event * eventWaitList,
                        cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::ROT);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0,0),  X, Y);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasCsrot(N,
            cl_mem_FRunner(f, 0),
            offx,
            incx,
            cl_mem_FRunner(f, 1),
            offy,
            incy,
            C,
            S,
            1,
            &cq,
            numEventsInWaitList,
            eventWaitList,
            events);
    if (err != CL_SUCCESS)
    throw HPL::HPLclBLASException(err, "Exception thrown in clblasCsrot");

    return err;
}

clblasStatus clblasCsrot(HPL::Array<HPL::float2, 1>& X,
                        HPL::Array<HPL::float2, 1>& Y,
                        cl_float C,
                        cl_float S,
                        const HPL::Device& d)
{
    return clblasCsrot(X.getDimension(0),
            X,
            0,
            1,
            Y,
            0,
            1,
            C,
            S,
            d);
}

clblasStatus clblasSrotm(size_t N,
                         HPL::Array<float, 1>& X,
                         size_t offx,
                         int incx,
                         HPL::Array<float, 1>& Y,
                         size_t offy,
                         int incy,
                         const HPL::Array<float, 1>& SPARAM,
                         size_t offSparam,
                         const HPL::Device& d,
                         cl_uint numEventsInWaitList,
                         const cl_event * eventWaitList,
                         cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::ROTM);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0,0),  X, Y, const_cast<HPL::Array<float, 1>&>(SPARAM));

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasSrotm(N,
            cl_mem_FRunner(f, 0),
            offx,
            incx,
            cl_mem_FRunner(f, 1),
            offy,
            incy,
            cl_mem_FRunner(f, 2),
            offSparam,
            1,
            &cq,
            numEventsInWaitList,
            eventWaitList,
            events);
    if (err != CL_SUCCESS)
    throw HPL::HPLclBLASException(err, "Exception thrown in clblasSrotm");

    return err;
}

clblasStatus clblasSrotm(HPL::Array<float, 1>& X,
                         HPL::Array<float, 1>& Y,
                         const HPL::Array<float, 1>& SPARAM,
                         const HPL::Device& d)
{
    return clblasSrotm(X.getDimension(0),
            X,
            0,
            1,
            Y,
            0,
            1,
            SPARAM,
            0,
            d);
}

clblasStatus clblasDrotm(size_t N,
                         HPL::Array<double, 1>& X,
                         size_t offx,
                         int incx,
                         HPL::Array<double, 1>& Y,
                         size_t offy,
                         int incy,
                         const HPL::Array<double, 1>& SPARAM,
                         size_t offSparam,
                         const HPL::Device& d,
                         cl_uint numEventsInWaitList,
                         const cl_event * eventWaitList,
                         cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::ROTM);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0,0),  X, Y, const_cast<HPL::Array<double, 1>&>(SPARAM));

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasDrotm(N,
            cl_mem_FRunner(f, 0),
            offx,
            incx,
            cl_mem_FRunner(f, 1),
            offy,
            incy,
            cl_mem_FRunner(f, 2),
            offSparam,
            1,
            &cq,
            numEventsInWaitList,
            eventWaitList,
            events);
    if (err != CL_SUCCESS)
    throw HPL::HPLclBLASException(err, "Exception thrown in clblasDrotm");

    return err;
}

clblasStatus clblasDrotm(HPL::Array<double, 1>& X,
                         HPL::Array<double, 1>& Y,
                         const HPL::Array<double, 1>& SPARAM,
                         const HPL::Device& d)
{
    return clblasDrotm(X.getDimension(0),
            X,
            0,
            1,
            Y,
            0,
            1,
            SPARAM,
            0,
            d);
}

clblasStatus clblasSnrm2(size_t N,
                         HPL::Array<float, 1>& NRM2,
                         size_t offNRM2,
                         const HPL::Array<float, 1>& X,
                         size_t offx,
                         int incx,
                         HPL::Array<float, 1>& scratchBuff,
                         const HPL::Device& d,
                         cl_uint numEventsInWaitList,
                         const cl_event * eventWaitList,
                         cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::NRM2);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0,0),  NRM2, const_cast<HPL::Array<float, 1>&>(X), scratchBuff);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasSnrm2(N,
            cl_mem_FRunner(f, 0),
            offNRM2,
            cl_mem_FRunner(f, 1),
            offx,
            incx,
            cl_mem_FRunner(f, 2),
            1,
            &cq,
            numEventsInWaitList,
            eventWaitList,
            events);
    if (err != CL_SUCCESS)
    throw HPL::HPLclBLASException(err, "Exception thrown in clblasSnrm2");

    return err;
}

clblasStatus clblasSnrm2(HPL::Array<float, 1>& NRM2,
                         const HPL::Array<float, 1>& X,
                         const HPL::Device& d)
{
    HPL::Array<float, 1> scratchBuff(X.getDimension(0)*2);
    return clblasSnrm2(X.getDimension(0),
            NRM2,
            0,
            X,
            0,
            1,
            scratchBuff,
            d);
}

clblasStatus clblasDnrm2(size_t N,
                         HPL::Array<double, 1>& NRM2,
                         size_t offNRM2,
                         const HPL::Array<double, 1>& X,
                         size_t offx,
                         int incx,
                         HPL::Array<double, 1>& scratchBuff,
                         const HPL::Device& d,
                         cl_uint numEventsInWaitList,
                         const cl_event * eventWaitList,
                         cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::NRM2);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0,0),  NRM2, const_cast<HPL::Array<double, 1>&>(X), scratchBuff);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasDnrm2(N,
            cl_mem_FRunner(f, 0),
            offNRM2,
            cl_mem_FRunner(f, 1),
            offx,
            incx,
            cl_mem_FRunner(f, 2),
            1,
            &cq,
            numEventsInWaitList,
            eventWaitList,
            events);
    if (err != CL_SUCCESS)
    throw HPL::HPLclBLASException(err, "Exception thrown in clblasDnrm2");

    return err;
}

clblasStatus clblasDnrm2(HPL::Array<double, 1>& NRM2,
                         const HPL::Array<double, 1>& X,
                         const HPL::Device& d)
{
    HPL::Array<double, 1> scratchBuff(X.getDimension(0)*2);
    return clblasDnrm2(X.getDimension(0),
            NRM2,
            0,
            X,
            0,
            1,
            scratchBuff,
            d);
}

clblasStatus clblasScnrm2(size_t N,
                         HPL::Array<float, 1>& NRM2,
                         size_t offNRM2,
                         const HPL::Array<HPL::float2, 1>& X,
                         size_t offx,
                         int incx,
                         HPL::Array<HPL::float2, 1>& scratchBuff,
                         const HPL::Device& d,
                         cl_uint numEventsInWaitList,
                         const cl_event * eventWaitList,
                         cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::NRM2);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0,0),  NRM2, const_cast<HPL::Array<HPL::float2, 1>&>(X), scratchBuff);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasScnrm2(N,
            cl_mem_FRunner(f, 0),
            offNRM2,
            cl_mem_FRunner(f, 1),
            offx,
            incx,
            cl_mem_FRunner(f, 2),
            1,
            &cq,
            numEventsInWaitList,
            eventWaitList,
            events);
    if (err != CL_SUCCESS)
    throw HPL::HPLclBLASException(err, "Exception thrown in clblasScnrm2");

    return err;
}

clblasStatus clblasScnrm2(HPL::Array<float, 1>& NRM2,
                         const HPL::Array<HPL::float2, 1>& X,
                         const HPL::Device& d)
{
    HPL::Array<HPL::float2, 1> scratchBuff(X.getDimension(0)*2);
    return clblasScnrm2(X.getDimension(0),
            NRM2,
            0,
            X,
            0,
            1,
            scratchBuff,
            d);
}

clblasStatus clblasiSamax(size_t N,
                          HPL::Array<unsigned int, 1>& iMax,
                          size_t offiMax,
                          const HPL::Array<float, 1>& X,
                          size_t offx,
                          int incx,
                          HPL::Array<float, 1>& scratchBuff,
                          const HPL::Device& d,
                          cl_uint numEventsInWaitList,
                          const cl_event * eventWaitList,
                          cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::iAMAX);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0,0),  iMax, const_cast<HPL::Array<float, 1>&>(X), scratchBuff);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasiSamax(N,
            cl_mem_FRunner(f, 0),
            offiMax,
            cl_mem_FRunner(f, 1),
            offx,
            incx,
            cl_mem_FRunner(f, 2),
            1,
            &cq,
            numEventsInWaitList,
            eventWaitList,
            events);
    if (err != CL_SUCCESS)
    throw HPL::HPLclBLASException(err, "Exception thrown in clblasiSamax");

    return err;
}

clblasStatus clblasiSamax(HPL::Array<unsigned int, 1>& iMax,
                          const HPL::Array<float, 1>& X,
                          const HPL::Device& d)
{
    HPL::Array<float, 1> scratchBuff(X.getDimension(0)*2);
    return clblasiSamax(X.getDimension(0),
            iMax,
            0,
            X,
            0,
            1,
            scratchBuff,
            d);
}

clblasStatus clblasiDamax(size_t N,
                          HPL::Array<unsigned int, 1>& iMax,
                          size_t offiMax,
                          const HPL::Array<double, 1>& X,
                          size_t offx,
                          int incx,
                          HPL::Array<double, 1>& scratchBuff,
                          const HPL::Device& d,
                          cl_uint numEventsInWaitList,
                          const cl_event * eventWaitList,
                          cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::iAMAX);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0,0),  iMax, const_cast<HPL::Array<double, 1>&>(X), scratchBuff);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasiDamax(N,
            cl_mem_FRunner(f, 0),
            offiMax,
            cl_mem_FRunner(f, 1),
            offx,
            incx,
            cl_mem_FRunner(f, 2),
            1,
            &cq,
            numEventsInWaitList,
            eventWaitList,
            events);
    if (err != CL_SUCCESS)
    throw HPL::HPLclBLASException(err, "Exception thrown in clblasiDamax");

    return err;
}

clblasStatus clblasiDamax(HPL::Array<unsigned int, 1>& iMax,
                          const HPL::Array<double, 1>& X,
                          const HPL::Device& d)
{
    HPL::Array<double, 1> scratchBuff(X.getDimension(0)*2);
    return clblasiDamax(X.getDimension(0),
            iMax,
            0,
            X,
            0,
            1,
            scratchBuff,
            d);
}

clblasStatus clblasiCamax(size_t N,
                          HPL::Array<unsigned int, 1>& iMax,
                          size_t offiMax,
                          const HPL::Array<HPL::float2, 1>& X,
                          size_t offx,
                          int incx,
                          HPL::Array<HPL::float2, 1>& scratchBuff,
                          const HPL::Device& d,
                          cl_uint numEventsInWaitList,
                          const cl_event * eventWaitList,
                          cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::iAMAX);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0,0),  iMax, const_cast<HPL::Array<HPL::float2, 1>&>(X), scratchBuff);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasiCamax(N,
            cl_mem_FRunner(f, 0),
            offiMax,
            cl_mem_FRunner(f, 1),
            offx,
            incx,
            cl_mem_FRunner(f, 2),
            1,
            &cq,
            numEventsInWaitList,
            eventWaitList,
            events);
    if (err != CL_SUCCESS)
    throw HPL::HPLclBLASException(err, "Exception thrown in clblasiCamax");

    return err;
}

clblasStatus clblasiCamax(HPL::Array<unsigned int, 1>& iMax,
                          const HPL::Array<HPL::float2, 1>& X,
                          const HPL::Device& d)
{
    HPL::Array<HPL::float2, 1> scratchBuff(X.getDimension(0)*2);
    return clblasiCamax(X.getDimension(0),
            iMax,
            0,
            X,
            0,
            1,
            scratchBuff,
            d);
}

clblasStatus clblasSasum(size_t N,
                         HPL::Array<float, 1>& asum,
                         size_t offAsum,
                         const HPL::Array<float, 1>& X,
                         size_t offx,
                         int incx,
                         HPL::Array<float, 1>& scratchBuff,
                         const HPL::Device& d,
                         cl_uint numEventsInWaitList,
                         const cl_event * eventWaitList,
                         cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::iAMAX);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0,0),  asum, const_cast<HPL::Array<float, 1>&>(X), scratchBuff);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasSasum(N,
            cl_mem_FRunner(f, 0),
            offAsum,
            cl_mem_FRunner(f, 1),
            offx,
            incx,
            cl_mem_FRunner(f, 2),
            1,
            &cq,
            numEventsInWaitList,
            eventWaitList,
            events);
    if (err != CL_SUCCESS)
    throw HPL::HPLclBLASException(err, "Exception thrown in clblasSasum");

    return err;
}

clblasStatus clblasSasum(HPL::Array<float, 1>& asum,
                         const HPL::Array<float, 1>& X,
                         const HPL::Device& d)
{
    HPL::Array<float, 1> scratchBuff(X.getDimension(0));
    return clblasSasum(X.getDimension(0),
            asum,
            0,
            X,
            0,
            1,
            scratchBuff,
            d);
}

clblasStatus clblasDasum(size_t N,
                         HPL::Array<double, 1>& asum,
                         size_t offAsum,
                         const HPL::Array<double, 1>& X,
                         size_t offx,
                         int incx,
                         HPL::Array<double, 1>& scratchBuff,
                         const HPL::Device& d,
                         cl_uint numEventsInWaitList,
                         const cl_event * eventWaitList,
                         cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::iAMAX);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0,0),  asum, const_cast<HPL::Array<double, 1>&>(X), scratchBuff);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasDasum(N,
            cl_mem_FRunner(f, 0),
            offAsum,
            cl_mem_FRunner(f, 1),
            offx,
            incx,
            cl_mem_FRunner(f, 2),
            1,
            &cq,
            numEventsInWaitList,
            eventWaitList,
            events);
    if (err != CL_SUCCESS)
    throw HPL::HPLclBLASException(err, "Exception thrown in clblasDasum");

    return err;
}

clblasStatus clblasDasum(HPL::Array<double, 1>& asum,
                         const HPL::Array<double, 1>& X,
                         const HPL::Device& d)
{
    HPL::Array<double, 1> scratchBuff(X.getDimension(0));
    return clblasDasum(X.getDimension(0),
            asum,
            0,
            X,
            0,
            1,
            scratchBuff,
            d);
}

clblasStatus clblasScasum(size_t N,
                         HPL::Array<HPL::float2, 1>& asum,
                         size_t offAsum,
                         const HPL::Array<HPL::float2, 1>& X,
                         size_t offx,
                         int incx,
                         HPL::Array<HPL::float2, 1>& scratchBuff,
                         const HPL::Device& d,
                         cl_uint numEventsInWaitList,
                         const cl_event * eventWaitList,
                         cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::iAMAX);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0,0),  asum, const_cast<HPL::Array<HPL::float2, 1>&>(X), scratchBuff);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasScasum(N,
            cl_mem_FRunner(f, 0),
            offAsum,
            cl_mem_FRunner(f, 1),
            offx,
            incx,
            cl_mem_FRunner(f, 2),
            1,
            &cq,
            numEventsInWaitList,
            eventWaitList,
            events);
    if (err != CL_SUCCESS)
    throw HPL::HPLclBLASException(err, "Exception thrown in clblasCasum");

    return err;
}

clblasStatus clblasScasum(HPL::Array<HPL::float2, 1>& asum,
                         const HPL::Array<HPL::float2, 1>& X,
                         const HPL::Device& d)
{
    HPL::Array<HPL::float2, 1> scratchBuff(X.getDimension(0));
    return clblasScasum(X.getDimension(0),
            asum,
            0,
            X,
            0,
            1,
            scratchBuff,
            d);
}

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
                         const HPL::Device& d,
                         cl_uint numEventsInWaitList,
                         const cl_event * eventWaitList,
                         cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::GEMV);

    f.device(d).fillInFRunnerTable(0,
                                   HPL::Tuple(0, 0),
                                   const_cast<HPL::Array<float, 2>&>(A),
                                   const_cast<HPL::Array<float, 1>&>(x),
                                   y);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasSgemv(order,
                                   transA,
                                   M,
                                   N,
                                   alpha,
                                   cl_mem_FRunner(f, 0),
                                   offA,
                                   lda,
                                   cl_mem_FRunner(f, 1),
                                   offx,
                                   incx,
                                   beta,
                                   cl_mem_FRunner(f, 2),
                                   offy,
                                   incy,
                                   1,
                                   &cq,
                                   numEventsInWaitList,
                                   eventWaitList,
                                   events);

    if (err != CL_SUCCESS)
        throw HPL::HPLclBLASException(err, "Exception thrown in clblasSgemv");

    return err;
}

clblasStatus clblasSgemv(const HPL::Array<float, 2>& A,
                         const HPL::Array<float, 1>& x,
                         HPL::Array<float, 1>& y,
                         const HPL::Device& d)
{
    return clblasSgemv(clblasRowMajor,
                       clblasNoTrans,
                       A.getDimension(0),
                       A.getDimension(1),
                       1,
                       A,
                       0,
                       getLowLevelDim1(A, d),
                       x,
                       0,
                       1,
                       0,
                       y,
                       0,
                       1,
                       d);
}

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
                         const HPL::Device& d,
                         cl_uint numEventsInWaitList,
                         const cl_event * eventWaitList,
                         cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::GEMV);

    f.device(d).fillInFRunnerTable(0,
                                   HPL::Tuple(0, 0),
                                   const_cast<HPL::Array<double, 2>&>(A),
                                   const_cast<HPL::Array<double, 1>&>(x),
                                   y);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasDgemv(order,
                                   transA,
                                   M,
                                   N,
                                   alpha,
                                   cl_mem_FRunner(f, 0),
                                   offA,
                                   lda,
                                   cl_mem_FRunner(f, 1),
                                   offx,
                                   incx,
                                   beta,
                                   cl_mem_FRunner(f, 2),
                                   offy,
                                   incy,
                                   1,
                                   &cq,
                                   numEventsInWaitList,
                                   eventWaitList,
                                   events);

    if (err != CL_SUCCESS)
        throw HPL::HPLclBLASException(err, "Exception thrown in clblasDgemv");

    return err;
}

clblasStatus clblasDgemv(const HPL::Array<double, 2>& A,
                         const HPL::Array<double, 1>& x,
                         HPL::Array<double, 1>& y,
                         const HPL::Device& d)
{
    return clblasDgemv(clblasRowMajor,
                       clblasNoTrans,
                       A.getDimension(0),
                       A.getDimension(1),
                       1,
                       A,
                       0,
                       getLowLevelDim1(A, d),
                       x,
                       0,
                       1,
                       0,
                       y,
                       0,
                       1,
                       d);
}

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
                         const HPL::Device& d,
                         cl_uint numEventsInWaitList,
                         const cl_event * eventWaitList,
                         cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::GEMV);

    f.device(d).fillInFRunnerTable(0,
                                   HPL::Tuple(0, 0),
                                   const_cast<HPL::Array<HPL::float2, 2>&>(A),
                                   const_cast<HPL::Array<HPL::float2, 1>&>(x),
                                   y);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasCgemv(order,
                                   transA,
                                   M,
                                   N,
                                   alpha,
                                   cl_mem_FRunner(f, 0),
                                   offA,
                                   lda,
                                   cl_mem_FRunner(f, 1),
                                   offx,
                                   incx,
                                   beta,
                                   cl_mem_FRunner(f, 2),
                                   offy,
                                   incy,
                                   1,
                                   &cq,
                                   numEventsInWaitList,
                                   eventWaitList,
                                   events);

    if (err != CL_SUCCESS)
        throw HPL::HPLclBLASException(err, "Exception thrown in clblasCgemv");

    return err;
}

clblasStatus clblasCgemv(const HPL::Array<HPL::float2, 2>& A,
                         const HPL::Array<HPL::float2, 1>& x,
                         HPL::Array<HPL::float2, 1>& y,
                         const HPL::Device& d)
{
    cl_float2 alpha = {1, 0};
    cl_float2 beta  = {0, 0};
    return clblasCgemv(clblasRowMajor,
                       clblasNoTrans,
                       A.getDimension(0),
                       A.getDimension(1),
                       alpha,
                       A,
                       0,
                       getLowLevelDim1(A, d),
                       x,
                       0,
                       1,
                       beta,
                       y,
                       0,
                       1,
                       d);
}

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
                         const HPL::Device& d,
                         cl_uint numEventsInWaitList,
                         const cl_event * eventWaitList,
                         cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::SYMV);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0, 0), const_cast<HPL::Array<float, 2>&>(A),
                                   const_cast<HPL::Array<float, 1>&>(X), Y);

    cl_command_queue cq =
    HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasSsymv(order,
                                   uplo,
                                   N,
                                   alpha,
                                   cl_mem_FRunner(f, 0),
                                   offA,
                                   lda,
                                   cl_mem_FRunner(f, 1),
                                   offx,
                                   incx,
                                   beta,
                                   cl_mem_FRunner(f, 2),
                                   offy,
                                   incy,
                                   1,
                                   &cq,
                                   numEventsInWaitList,
                                   eventWaitList,
                                   events);

    if (err != CL_SUCCESS)
        throw HPL::HPLclBLASException(err, "Exception thrown in clblasSsymv");

    return err;
}

clblasStatus clblasSsymv(clblasUplo uplo,
                         const HPL::Array<float, 2>& A,
                         const HPL::Array<float, 1>& X,
                         HPL::Array<float, 1>& Y,
                         const HPL::Device& d)
{
    return clblasSsymv(clblasRowMajor,
                       uplo,
                       A.getDimension(0),
                       1,
                       A,
                       0,
                       getLowLevelDim1(A, d),
                       X,
                       0,
                       1,
                       1,
                       Y,
                       0,
                       1,
                       d);
}

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
                         const HPL::Device& d,
                         cl_uint numEventsInWaitList,
                         const cl_event * eventWaitList,
                         cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::SYMV);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0, 0), const_cast<HPL::Array<double, 2>&>(A),
                                   const_cast<HPL::Array<double, 1>&>(X), Y);

    cl_command_queue cq =
    HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasDsymv(order,
                                   uplo,
                                   N,
                                   alpha,
                                   cl_mem_FRunner(f, 0),
                                   offA,
                                   lda,
                                   cl_mem_FRunner(f, 1),
                                   offx,
                                   incx,
                                   beta,
                                   cl_mem_FRunner(f, 2),
                                   offy,
                                   incy,
                                   1,
                                   &cq,
                                   numEventsInWaitList,
                                   eventWaitList,
                                   events);

    if (err != CL_SUCCESS)
        throw HPL::HPLclBLASException(err, "Exception thrown in clblasDsymv");

    return err;
}

clblasStatus clblasDsymv(clblasUplo uplo,
                         const HPL::Array<double, 2>& A,
                         const HPL::Array<double, 1>& X,
                         HPL::Array<double, 1>& Y,
                         const HPL::Device& d)
{
    return clblasDsymv(clblasRowMajor,
                       uplo,
                       A.getDimension(0),
                       1,
                       A,
                       0,
                       getLowLevelDim1(A, d),
                       X,
                       0,
                       1,
                       1,
                       Y,
                       0,
                       1,
                       d);
}

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
                         const HPL::Device& d,
                         cl_uint numEventsInWaitList,
                         const cl_event * eventWaitList,
                         cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::HEMV);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0, 0), const_cast<HPL::Array<HPL::float2, 2>&>(A),
                                   const_cast<HPL::Array<HPL::float2, 1>&>(X), Y);

    cl_command_queue cq =
    HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasChemv(order,
                                   uplo,
                                   N,
                                   alpha,
                                   cl_mem_FRunner(f, 0),
                                   offa,
                                   lda,
                                   cl_mem_FRunner(f, 1),
                                   offx,
                                   incx,
                                   beta,
                                   cl_mem_FRunner(f, 2),
                                   offy,
                                   incy,
                                   1,
                                   &cq,
                                   numEventsInWaitList,
                                   eventWaitList,
                                   events);

    if (err != CL_SUCCESS)
        throw HPL::HPLclBLASException(err, "Exception thrown in clblasChemv");

    return err;
}

clblasStatus clblasChemv(clblasUplo uplo,
                         const HPL::Array<HPL::float2, 2>& A,
                         const HPL::Array<HPL::float2, 1>& X,
                         HPL::Array<HPL::float2, 1>& Y,
                         const HPL::Device& d)
{
    cl_float2 alpha = {1.0f, 0.f};
    cl_float2 beta = {0.0f, 0.f};
    return clblasChemv(clblasRowMajor,
                       uplo,
                       A.getDimension(0),
                       alpha,
                       A,
                       0,
                       getLowLevelDim1(A, d),
                       X,
                       0,
                       1,
                       beta,
                       Y,
                       0,
                       1,
                       d);
}

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
                         const HPL::Device& d,
                         cl_uint numEventsInWaitList,
                         const cl_event * eventWaitList,
                         cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::TRMV);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0, 0), const_cast<HPL::Array<float, 2>&>(A),
                                   X, scratchBuff);

    cl_command_queue cq =
    HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasStrmv(order,
                                   uplo,
                                   trans,
                                   diag,
                                   N,
                                   cl_mem_FRunner(f, 0),
                                   offa,
                                   lda,
                                   cl_mem_FRunner(f, 1),
                                   offx,
                                   incx,
                                   cl_mem_FRunner(f, 2),
                                   1,
                                   &cq,
                                   numEventsInWaitList,
                                   eventWaitList,
                                   events);

    if (err != CL_SUCCESS)
        throw HPL::HPLclBLASException(err, "Exception thrown in clblasStrmv");

    return err;
}


clblasStatus clblasStrmv(clblasUplo uplo,
                         clblasDiag diag,
                         const HPL::Array<float, 2>& A,
                         HPL::Array<float, 1>& X,
                         const HPL::Device& d)
{
    HPL::Array<float, 1> scratchBuff(X.getDimension(0));
    return clblasStrmv(clblasRowMajor,
                           uplo,
                           clblasNoTrans,
                           diag,
                           A.getDimension(0),
                           A,
                           0,
                           getLowLevelDim1(A, d),
                           X,
                           0,
                           1,
                           scratchBuff,
                           d);
}

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
                         const HPL::Device& d,
                         cl_uint numEventsInWaitList,
                         const cl_event * eventWaitList,
                         cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::TRMV);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0, 0), const_cast<HPL::Array<double, 2>&>(A),
                                   X, scratchBuff);

    cl_command_queue cq =
    HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasDtrmv(order,
                                   uplo,
                                   trans,
                                   diag,
                                   N,
                                   cl_mem_FRunner(f, 0),
                                   offa,
                                   lda,
                                   cl_mem_FRunner(f, 1),
                                   offx,
                                   incx,
                                   cl_mem_FRunner(f, 2),
                                   1,
                                   &cq,
                                   numEventsInWaitList,
                                   eventWaitList,
                                   events);

    if (err != CL_SUCCESS)
        throw HPL::HPLclBLASException(err, "Exception thrown in clblasDtrmv");

    return err;
}


clblasStatus clblasDtrmv(clblasUplo uplo,
                         clblasDiag diag,
                         const HPL::Array<double, 2>& A,
                         HPL::Array<double, 1>& X,
                         const HPL::Device& d)
{
    HPL::Array<double, 1> scratchBuff(X.getDimension(0));
    return clblasDtrmv(clblasRowMajor,
                           uplo,
                           clblasNoTrans,
                           diag,
                           A.getDimension(0),
                           A,
                           0,
                           getLowLevelDim1(A, d),
                           X,
                           0,
                           1,
                           scratchBuff,
                           d);
}

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
                         const HPL::Device& d,
                         cl_uint numEventsInWaitList,
                         const cl_event * eventWaitList,
                         cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::TRMV);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0, 0), const_cast<HPL::Array<HPL::float2, 2>&>(A),
                                   X, scratchBuff);

    cl_command_queue cq =
    HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasCtrmv(order,
                                   uplo,
                                   trans,
                                   diag,
                                   N,
                                   cl_mem_FRunner(f, 0),
                                   offa,
                                   lda,
                                   cl_mem_FRunner(f, 1),
                                   offx,
                                   incx,
                                   cl_mem_FRunner(f, 2),
                                   1,
                                   &cq,
                                   numEventsInWaitList,
                                   eventWaitList,
                                   events);

    if (err != CL_SUCCESS)
        throw HPL::HPLclBLASException(err, "Exception thrown in clblasCtrmv");

    return err;
}


clblasStatus clblasCtrmv(clblasUplo uplo,
                         clblasDiag diag,
                         const HPL::Array<HPL::float2, 2>& A,
                         HPL::Array<HPL::float2, 1>& X,
                         const HPL::Device& d)
{
    HPL::Array<HPL::float2, 1> scratchBuff(X.getDimension(0));
    return clblasCtrmv(clblasRowMajor,
                           uplo,
                           clblasNoTrans,
                           diag,
                           A.getDimension(0),
                           A,
                           0,
                           getLowLevelDim1(A, d),
                           X,
                           0,
                           1,
                           scratchBuff,
                           d);
}

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
                         const HPL::Device& d,
                         cl_uint numEventsInWaitList,
                         const cl_event * eventWaitList,
                         cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::TRSV);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0, 0), const_cast<HPL::Array<float, 2>&>(A), X);

    cl_command_queue cq =
    HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasStrsv(order,
                                   uplo,
                                   trans,
                                   diag,
                                   N,
                                   cl_mem_FRunner(f, 0),
                                   offa,
                                   lda,
                                   cl_mem_FRunner(f, 1),
                                   offx,
                                   incx,
                                   1,
                                   &cq,
                                   numEventsInWaitList,
                                   eventWaitList,
                                   events);

    if (err != CL_SUCCESS)
        throw HPL::HPLclBLASException(err, "Exception thrown in clblasStrsv");

    return err;
}

clblasStatus clblasStrsv(clblasUplo uplo,
                         clblasDiag diag,
                         const HPL::Array<float, 2>& A,
                         HPL::Array<float, 1>& X,
                         const HPL::Device& d)
{
    return clblasStrsv(clblasRowMajor,
                               uplo,
                               clblasNoTrans,
                               diag,
                               A.getDimension(0),
                               A,
                               0,
                               getLowLevelDim1(A, d),
                               X,
                               0,
                               1,
                               d);
}

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
                         const HPL::Device& d,
                         cl_uint numEventsInWaitList,
                         const cl_event * eventWaitList,
                         cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::TRSV);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0, 0), const_cast<HPL::Array<double, 2>&>(A), X);

    cl_command_queue cq =
    HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasDtrsv(order,
                                   uplo,
                                   trans,
                                   diag,
                                   N,
                                   cl_mem_FRunner(f, 0),
                                   offa,
                                   lda,
                                   cl_mem_FRunner(f, 1),
                                   offx,
                                   incx,
                                   1,
                                   &cq,
                                   numEventsInWaitList,
                                   eventWaitList,
                                   events);

    if (err != CL_SUCCESS)
        throw HPL::HPLclBLASException(err, "Exception thrown in clblasDtrsv");

    return err;
}

clblasStatus clblasDtrsv(clblasUplo uplo,
                         clblasDiag diag,
                         const HPL::Array<double, 2>& A,
                         HPL::Array<double, 1>& X,
                         const HPL::Device& d)
{
    return clblasDtrsv(clblasRowMajor,
                               uplo,
                               clblasNoTrans,
                               diag,
                               A.getDimension(0),
                               A,
                               0,
                               getLowLevelDim1(A, d),
                               X,
                               0,
                               1,
                               d);
}

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
                         const HPL::Device& d,
                         cl_uint numEventsInWaitList,
                         const cl_event * eventWaitList,
                         cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::TRSV);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0, 0), const_cast<HPL::Array<HPL::float2, 2>&>(A), X);

    cl_command_queue cq =
    HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasCtrsv(order,
                                   uplo,
                                   trans,
                                   diag,
                                   N,
                                   cl_mem_FRunner(f, 0),
                                   offa,
                                   lda,
                                   cl_mem_FRunner(f, 1),
                                   offx,
                                   incx,
                                   1,
                                   &cq,
                                   numEventsInWaitList,
                                   eventWaitList,
                                   events);

    if (err != CL_SUCCESS)
        throw HPL::HPLclBLASException(err, "Exception thrown in clblasCtrsv");

    return err;
}

clblasStatus clblasCtrsv(clblasUplo uplo,
                         clblasDiag diag,
                         const HPL::Array<HPL::float2, 2>& A,
                         HPL::Array<HPL::float2, 1>& X,
                         const HPL::Device& d)
{
    return clblasCtrsv(clblasRowMajor,
                               uplo,
                               clblasNoTrans,
                               diag,
                               A.getDimension(0),
                               A,
                               0,
                               getLowLevelDim1(A, d),
                               X,
                               0,
                               1,
                               d);
}

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
                        const HPL::Device& d,
                        cl_uint numEventsInWaitList,
                        const cl_event * eventWaitList,
                        cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::GER);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0, 0), const_cast<HPL::Array<float, 1>&>(X),
                                   const_cast<HPL::Array<float, 1>&>(Y), A);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasSger(order,
                                   M,
                                   N,
                                   alpha,
                                   cl_mem_FRunner(f, 0),
                                   offx,
                                   incx,
                                   cl_mem_FRunner(f, 1),
                                   offy,
                                   incy,
                                   cl_mem_FRunner(f, 2),
                                   offa,
                                   lda,
                                   1,
                                   &cq,
                                   numEventsInWaitList,
                                   eventWaitList,
                                   events);

    if (err != CL_SUCCESS)
        throw HPL::HPLclBLASException(err, "Exception thrown in clblasSger");

    return err;
}

clblasStatus clblasSger(const HPL::Array<float, 1>& X,
                        const HPL::Array<float, 1>& Y,
                        HPL::Array<float, 2>& A,
                        const HPL::Device& d)
{
    return clblasSger(clblasRowMajor,
                               A.getDimension(0),
                               A.getDimension(1),
                               1,
                               X,
                               0,
                               1,
                               Y,
                               0,
                               1,
                               A,
                               0,
                               getLowLevelDim1(A, d),
                               d);
}

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
                        const HPL::Device& d,
                        cl_uint numEventsInWaitList,
                        const cl_event * eventWaitList,
                        cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::GER);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0, 0), const_cast<HPL::Array<double, 1>&>(X),
                                   const_cast<HPL::Array<double, 1>&>(Y), A);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasDger(order,
                                   M,
                                   N,
                                   alpha,
                                   cl_mem_FRunner(f, 0),
                                   offx,
                                   incx,
                                   cl_mem_FRunner(f, 1),
                                   offy,
                                   incy,
                                   cl_mem_FRunner(f, 2),
                                   offa,
                                   lda,
                                   1,
                                   &cq,
                                   numEventsInWaitList,
                                   eventWaitList,
                                   events);

    if (err != CL_SUCCESS)
        throw HPL::HPLclBLASException(err, "Exception thrown in clblasDger");

    return err;
}

clblasStatus clblasDger(const HPL::Array<double, 1>& X,
                        const HPL::Array<double, 1>& Y,
                        HPL::Array<double, 2>& A,
                        const HPL::Device& d)
{
    return clblasDger(clblasRowMajor,
                               A.getDimension(0),
                               A.getDimension(1),
                               1,
                               X,
                               0,
                               1,
                               Y,
                               0,
                               1,
                               A,
                               0,
                               getLowLevelDim1(A, d),
                               d);
}

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
                         const HPL::Device& d,
                         cl_uint numEventsInWaitList,
                         const cl_event * eventWaitList,
                         cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::GERU);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0, 0), const_cast<HPL::Array<HPL::float2, 1>&>(X),
                                   const_cast<HPL::Array<HPL::float2, 1>&>(Y), A);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasCgeru(order,
                                   M,
                                   N,
                                   alpha,
                                   cl_mem_FRunner(f, 0),
                                   offx,
                                   incx,
                                   cl_mem_FRunner(f, 1),
                                   offy,
                                   incy,
                                   cl_mem_FRunner(f, 2),
                                   offa,
                                   lda,
                                   1,
                                   &cq,
                                   numEventsInWaitList,
                                   eventWaitList,
                                   events);

    if (err != CL_SUCCESS)
        throw HPL::HPLclBLASException(err, "Exception thrown in clblasCgeru");

    return err;
}

clblasStatus clblasCgeru(const HPL::Array<HPL::float2, 1>& X,
                         const HPL::Array<HPL::float2, 1>& Y,
                         HPL::Array<HPL::float2, 2>& A,
                         const HPL::Device& d)
{
    cl_float2 alpha = {1, 0};
    return clblasCgeru(clblasRowMajor,
                       A.getDimension(0),
                       A.getDimension(1),
                       alpha,
                       X,
                       0,
                       1,
                       Y,
                       0,
                       1,
                       A,
                       0,
                       getLowLevelDim1(A, d),
                       d);
}

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
                         const HPL::Device& d,
                         cl_uint numEventsInWaitList,
                         const cl_event * eventWaitList,
                         cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::GERC);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0, 0), const_cast<HPL::Array<HPL::float2, 1>&>(X),
                                   const_cast<HPL::Array<HPL::float2, 1>&>(Y), A);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasCgerc(order,
                                   M,
                                   N,
                                   alpha,
                                   cl_mem_FRunner(f, 0),
                                   offx,
                                   incx,
                                   cl_mem_FRunner(f, 1),
                                   offy,
                                   incy,
                                   cl_mem_FRunner(f, 2),
                                   offa,
                                   lda,
                                   1,
                                   &cq,
                                   numEventsInWaitList,
                                   eventWaitList,
                                   events);

    if (err != CL_SUCCESS)
        throw HPL::HPLclBLASException(err, "Exception thrown in clblasCgerc");

    return err;
}

clblasStatus clblasCgerc(const HPL::Array<HPL::float2, 1>& X,
                         const HPL::Array<HPL::float2, 1>& Y,
                         HPL::Array<HPL::float2, 2>& A,
                         const HPL::Device& d)
{
    cl_float2 alpha = {1, 0};
    return clblasCgerc(clblasRowMajor,
                       A.getDimension(0),
                       A.getDimension(1),
                       alpha,
                       X,
                       0,
                       1,
                       Y,
                       0,
                       1,
                       A,
                       0,
                       getLowLevelDim1(A, d),
                       d);
}

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
                        const HPL::Device& d,
                        cl_uint numEventsInWaitList,
                        const cl_event * eventWaitList,
                        cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::SYR);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0, 0), const_cast<HPL::Array<float, 1>&>(X), A);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasSsyr(order,
                                  uplo,
                                  N,
                                  alpha,
                                  cl_mem_FRunner(f, 0),
                                  offx,
                                  incx,
                                  cl_mem_FRunner(f, 1),
                                  offa,
                                  lda,
                                  1,
                                  &cq,
                                  numEventsInWaitList,
                                  eventWaitList,
                                  events);

    if (err != CL_SUCCESS)
        throw HPL::HPLclBLASException(err, "Exception thrown in clblasSsyr");

    return err;
}

clblasStatus clblasSsyr(clblasUplo uplo,
                        const HPL::Array<float, 1>& X,
                        HPL::Array<float, 2>& A,
                        const HPL::Device& d)
{
    return clblasSsyr(clblasRowMajor,
                       uplo,
                       A.getDimension(1),
                       1,
                       X,
                       0,
                       1,
                       A,
                       0,
                       getLowLevelDim1(A, d),
                       d);
}

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
                        const HPL::Device& d,
                        cl_uint numEventsInWaitList,
                        const cl_event * eventWaitList,
                        cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::SYR);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0, 0), const_cast<HPL::Array<double, 1>&>(X), A);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasDsyr(order,
                                  uplo,
                                  N,
                                  alpha,
                                  cl_mem_FRunner(f, 0),
                                  offx,
                                  incx,
                                  cl_mem_FRunner(f, 1),
                                  offa,
                                  lda,
                                  1,
                                  &cq,
                                  numEventsInWaitList,
                                  eventWaitList,
                                  events);

    if (err != CL_SUCCESS)
        throw HPL::HPLclBLASException(err, "Exception thrown in clblasDsyr");

    return err;
}

clblasStatus clblasDsyr(clblasUplo uplo,
                        const HPL::Array<double, 1>& X,
                        HPL::Array<double, 2>& A,
                        const HPL::Device& d)
{
    return clblasDsyr(clblasRowMajor,
                       uplo,
                       A.getDimension(1),
                       1,
                       X,
                       0,
                       1,
                       A,
                       0,
                       getLowLevelDim1(A, d),
                       d);
}

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
                        const HPL::Device& d,
                        cl_uint numEventsInWaitList,
                        const cl_event * eventWaitList,
                        cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::HER);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0, 0), const_cast<HPL::Array<HPL::float2, 1>&>(X), A);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasCher(order,
                                  uplo,
                                  N,
                                  alpha,
                                  cl_mem_FRunner(f, 0),
                                  offx,
                                  incx,
                                  cl_mem_FRunner(f, 1),
                                  offa,
                                  lda,
                                  1,
                                  &cq,
                                  numEventsInWaitList,
                                  eventWaitList,
                                  events);

    if (err != CL_SUCCESS)
        throw HPL::HPLclBLASException(err, "Exception thrown in clblasCher");

    return err;
}

clblasStatus clblasCher(clblasUplo uplo,
                        const HPL::Array<HPL::float2, 1>& X,
                        HPL::Array<HPL::float2, 2>& A,
                        const HPL::Device& d)
{
    return clblasCher(clblasRowMajor,
                      uplo,
                      A.getDimension(1),
                      1,
                      X,
                      0,
                      1,
                      A,
                      0,
                      getLowLevelDim1(A, d),
                      d);
}

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
                         const HPL::Device& d,
                         cl_uint numEventsInWaitList,
                         const cl_event * eventWaitList,
                         cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::SYR2);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0, 0), const_cast<HPL::Array<float, 1>&>(X),
                                   const_cast<HPL::Array<float, 1>&>(Y), A);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasSsyr2(order,
                                   uplo,
                                   N,
                                   alpha,
                                   cl_mem_FRunner(f, 0),
                                   offx,
                                   incx,
                                   cl_mem_FRunner(f, 1),
                                   offy,
                                   incy,
                                   cl_mem_FRunner(f, 2),
                                   offa,
                                   lda,
                                   1,
                                   &cq,
                                   numEventsInWaitList,
                                   eventWaitList,
                                   events);

    if (err != CL_SUCCESS)
        throw HPL::HPLclBLASException(err, "Exception thrown in clblasSsyr2");

    return err;
}

clblasStatus clblasSsyr2(clblasUplo uplo,
                         const HPL::Array<float, 1>& X,
                         const HPL::Array<float, 1>& Y,
                         HPL::Array<float, 2>& A,
                         const HPL::Device& d)
{
    return clblasSsyr2(clblasRowMajor,
                       uplo,
                       A.getDimension(1),
                       1,
                       X,
                       0,
                       1,
                       Y,
                       0,
                       1,
                       A,
                       0,
                       getLowLevelDim1(A, d),
                       d);
}

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
                         const HPL::Device& d,
                         cl_uint numEventsInWaitList,
                         const cl_event * eventWaitList,
                         cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::SYR2);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0, 0), const_cast<HPL::Array<double, 1>&>(X),
                                   const_cast<HPL::Array<double, 1>&>(Y), A);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasDsyr2(order,
                                   uplo,
                                   N,
                                   alpha,
                                   cl_mem_FRunner(f, 0),
                                   offx,
                                   incx,
                                   cl_mem_FRunner(f, 1),
                                   offy,
                                   incy,
                                   cl_mem_FRunner(f, 2),
                                   offa,
                                   lda,
                                   1,
                                   &cq,
                                   numEventsInWaitList,
                                   eventWaitList,
                                   events);

    if (err != CL_SUCCESS)
        throw HPL::HPLclBLASException(err, "Exception thrown in clblasDsyr2");

    return err;
}

clblasStatus clblasDsyr2(clblasUplo uplo,
                         const HPL::Array<double, 1>& X,
                         const HPL::Array<double, 1>& Y,
                         HPL::Array<double, 2>& A,
                         const HPL::Device& d)
{
    return clblasDsyr2(clblasRowMajor,
                       uplo,
                       A.getDimension(1),
                       1,
                       X,
                       0,
                       1,
                       Y,
                       0,
                       1,
                       A,
                       0,
                       getLowLevelDim1(A, d),
                       d);
}

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
                         const HPL::Device& d,
                         cl_uint numEventsInWaitList,
                         const cl_event * eventWaitList,
                         cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::HER2);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0, 0), const_cast<HPL::Array<HPL::float2, 1>&>(X),
                                   const_cast<HPL::Array<HPL::float2, 1>&>(Y), A);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasCher2(order,
                                   uplo,
                                   N,
                                   alpha,
                                   cl_mem_FRunner(f, 0),
                                   offx,
                                   incx,
                                   cl_mem_FRunner(f, 1),
                                   offy,
                                   incy,
                                   cl_mem_FRunner(f, 2),
                                   offa,
                                   lda,
                                   1,
                                   &cq,
                                   numEventsInWaitList,
                                   eventWaitList,
                                   events);

    if (err != CL_SUCCESS)
        throw HPL::HPLclBLASException(err, "Exception thrown in clblasCher2");

    return err;
}

clblasStatus clblasCher2(clblasUplo uplo,
                         const HPL::Array<HPL::float2, 1>& X,
                         const HPL::Array<HPL::float2, 1>& Y,
                         HPL::Array<HPL::float2, 2>& A,
                         const HPL::Device& d)
{
    cl_float2 alpha = {1, 0};
    return clblasCher2(clblasRowMajor,
                       uplo,
                       A.getDimension(1),
                       alpha,
                       X,
                       0,
                       1,
                       Y,
                       0,
                       1,
                       A,
                       0,
                       getLowLevelDim1(A, d),
                       d);
}

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
                         const HPL::Device& d,
                         cl_uint numEventsInWaitList,
                         const cl_event * eventWaitList,
                         cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::TPMV);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0, 0), const_cast<HPL::Array<float, 1>&>(AP), X, scratchBuff);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasStpmv(order,
                                   uplo,
                                   trans,
                                   diag,
                                   N,
                                   cl_mem_FRunner(f, 0),
                                   offa,
                                   cl_mem_FRunner(f, 1),
                                   offx,
                                   incx,
                                   cl_mem_FRunner(f, 2),
                                   1,
                                   &cq,
                                   numEventsInWaitList,
                                   eventWaitList,
                                   events);

    if (err != CL_SUCCESS)
        throw HPL::HPLclBLASException(err, "Exception thrown in clblasStpmv");

    return err;
}

clblasStatus clblasStpmv(clblasUplo uplo,
                         const HPL::Array<float, 1>& AP,
                         HPL::Array<float, 1>& X,
                         const HPL::Device& d)
{
    HPL::Array<float, 1> scratchBuff(X.getDimension(0));
    return clblasStpmv(clblasRowMajor,
                       uplo,
                       clblasNoTrans,
                       clblasNonUnit,
                       X.getDimension(0),
                       AP,
                       0,
                       X,
                       0,
                       1,
                       scratchBuff,
                       d);
}

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
                         const HPL::Device& d,
                         cl_uint numEventsInWaitList,
                         const cl_event * eventWaitList,
                         cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::TPMV);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0, 0), const_cast<HPL::Array<double, 1>&>(AP), X, scratchBuff);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasDtpmv(order,
                                   uplo,
                                   trans,
                                   diag,
                                   N,
                                   cl_mem_FRunner(f, 0),
                                   offa,
                                   cl_mem_FRunner(f, 1),
                                   offx,
                                   incx,
                                   cl_mem_FRunner(f, 2),
                                   1,
                                   &cq,
                                   numEventsInWaitList,
                                   eventWaitList,
                                   events);

    if (err != CL_SUCCESS)
        throw HPL::HPLclBLASException(err, "Exception thrown in clblasDtpmv");

    return err;
}

clblasStatus clblasDtpmv(clblasUplo uplo,
                         const HPL::Array<double, 1>& AP,
                         HPL::Array<double, 1>& X,
                         const HPL::Device& d)
{
    HPL::Array<double, 1> scratchBuff(X.getDimension(0));
    return clblasDtpmv(clblasRowMajor,
                       uplo,
                       clblasNoTrans,
                       clblasNonUnit,
                       X.getDimension(0),
                       AP,
                       0,
                       X,
                       0,
                       1,
                       scratchBuff,
                       d);
}

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
                         const HPL::Device& d,
                         cl_uint numEventsInWaitList,
                         const cl_event * eventWaitList,
                         cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::TPMV);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0, 0), const_cast<HPL::Array<HPL::float2, 1>&>(AP), X, scratchBuff);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasCtpmv(order,
                                   uplo,
                                   trans,
                                   diag,
                                   N,
                                   cl_mem_FRunner(f, 0),
                                   offa,
                                   cl_mem_FRunner(f, 1),
                                   offx,
                                   incx,
                                   cl_mem_FRunner(f, 2),
                                   1,
                                   &cq,
                                   numEventsInWaitList,
                                   eventWaitList,
                                   events);

    if (err != CL_SUCCESS)
        throw HPL::HPLclBLASException(err, "Exception thrown in clblasCtpmv");

    return err;
}

clblasStatus clblasCtpmv(clblasUplo uplo,
                         const HPL::Array<HPL::float2, 1>& AP,
                         HPL::Array<HPL::float2, 1>& X,
                         const HPL::Device& d)
{
    HPL::Array<HPL::float2, 1> scratchBuff(X.getDimension(0));
    return clblasCtpmv(clblasRowMajor,
                       uplo,
                       clblasNoTrans,
                       clblasNonUnit,
                       X.getDimension(0),
                       AP,
                       0,
                       X,
                       0,
                       1,
                       scratchBuff,
                       d);
}

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
                         const HPL::Device& d,
                         cl_uint numEventsInWaitList,
                         const cl_event * eventWaitList,
                         cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::TPSV);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0, 0), const_cast<HPL::Array<float, 1>&>(A), X);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasStpsv(order,
                                   uplo,
                                   trans,
                                   diag,
                                   N,
                                   cl_mem_FRunner(f, 0),
                                   offa,
                                   cl_mem_FRunner(f, 1),
                                   offx,
                                   incx,
                                   1,
                                   &cq,
                                   numEventsInWaitList,
                                   eventWaitList,
                                   events);

    if (err != CL_SUCCESS)
        throw HPL::HPLclBLASException(err, "Exception thrown in clblasStpsv");

    return err;
}

clblasStatus clblasStpsv(clblasUplo uplo,
                         const HPL::Array<float, 1>& A,
                         HPL::Array<float, 1>& X,
                         const HPL::Device& d)
{
    return clblasStpsv(clblasRowMajor,
                       uplo,
                       clblasNoTrans,
                       clblasNonUnit,
                       X.getDimension(0),
                       A,
                       0,
                       X,
                       0,
                       1,
                       d);
}

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
                         const HPL::Device& d,
                         cl_uint numEventsInWaitList,
                         const cl_event * eventWaitList,
                         cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::TPSV);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0, 0), const_cast<HPL::Array<double, 1>&>(A), X);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasDtpsv(order,
                                   uplo,
                                   trans,
                                   diag,
                                   N,
                                   cl_mem_FRunner(f, 0),
                                   offa,
                                   cl_mem_FRunner(f, 1),
                                   offx,
                                   incx,
                                   1,
                                   &cq,
                                   numEventsInWaitList,
                                   eventWaitList,
                                   events);

    if (err != CL_SUCCESS)
        throw HPL::HPLclBLASException(err, "Exception thrown in clblasDtpsv");

    return err;
}

clblasStatus clblasDtpsv(clblasUplo uplo,
                         const HPL::Array<double, 1>& A,
                         HPL::Array<double, 1>& X,
                         const HPL::Device& d)
{
    return clblasDtpsv(clblasRowMajor,
                       uplo,
                       clblasNoTrans,
                       clblasNonUnit,
                       X.getDimension(0),
                       A,
                       0,
                       X,
                       0,
                       1,
                       d);
}

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
                         const HPL::Device& d,
                         cl_uint numEventsInWaitList,
                         const cl_event * eventWaitList,
                         cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::TPSV);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0, 0), const_cast<HPL::Array<HPL::float2, 1>&>(A), X);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasCtpsv(order,
                                   uplo,
                                   trans,
                                   diag,
                                   N,
                                   cl_mem_FRunner(f, 0),
                                   offa,
                                   cl_mem_FRunner(f, 1),
                                   offx,
                                   incx,
                                   1,
                                   &cq,
                                   numEventsInWaitList,
                                   eventWaitList,
                                   events);

    if (err != CL_SUCCESS)
        throw HPL::HPLclBLASException(err, "Exception thrown in clblasCtpsv");

    return err;
}

clblasStatus clblasCtpsv(clblasUplo uplo,
                         const HPL::Array<HPL::float2, 1>& A,
                         HPL::Array<HPL::float2, 1>& X,
                         const HPL::Device& d)
{
    return clblasCtpsv(clblasRowMajor,
                       uplo,
                       clblasNoTrans,
                       clblasNonUnit,
                       X.getDimension(0),
                       A,
                       0,
                       X,
                       0,
                       1,
                       d);
}

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
                         const HPL::Device& d,
                         cl_uint numEventsInWaitList,
                         const cl_event * eventWaitList,
                         cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::SPMV);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0, 0), const_cast<HPL::Array<float, 1>&>(AP),
                                   const_cast<HPL::Array<float, 1>&>(X), Y);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasSspmv(order,
                                   uplo,
                                   N,
                                   alpha,
                                   cl_mem_FRunner(f, 0),
                                   offa,
                                   cl_mem_FRunner(f, 1),
                                   offx,
                                   incx,
                                   beta,
                                   cl_mem_FRunner(f, 2),
                                   offy,
                                   incy,
                                   1,
                                   &cq,
                                   numEventsInWaitList,
                                   eventWaitList,
                                   events);

    if (err != CL_SUCCESS)
        throw HPL::HPLclBLASException(err, "Exception thrown in clblasSspmv");

    return err;
}

clblasStatus clblasSspmv(clblasUplo uplo,
                         const HPL::Array<float, 1>& AP,
                         const HPL::Array<float, 1>& X,
                         HPL::Array<float, 1>& Y,
                         const HPL::Device& d)
{
    return clblasSspmv(clblasRowMajor,
                       uplo,
                       X.getDimension(0),
                       1,
                       AP,
                       0,
                       X,
                       0,
                       1,
                       0,
                       Y,
                       0,
                       1,
                       d);
}

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
                         const HPL::Device& d,
                         cl_uint numEventsInWaitList,
                         const cl_event * eventWaitList,
                         cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::SPMV);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0, 0), const_cast<HPL::Array<double, 1>&>(AP),
                                   const_cast<HPL::Array<double, 1>&>(X), Y);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasDspmv(order,
                                   uplo,
                                   N,
                                   alpha,
                                   cl_mem_FRunner(f, 0),
                                   offa,
                                   cl_mem_FRunner(f, 1),
                                   offx,
                                   incx,
                                   beta,
                                   cl_mem_FRunner(f, 2),
                                   offy,
                                   incy,
                                   1,
                                   &cq,
                                   numEventsInWaitList,
                                   eventWaitList,
                                   events);

    if (err != CL_SUCCESS)
        throw HPL::HPLclBLASException(err, "Exception thrown in clblasDspmv");

    return err;
}

clblasStatus clblasDspmv(clblasUplo uplo,
                         const HPL::Array<double, 1>& AP,
                         const HPL::Array<double, 1>& X,
                         HPL::Array<double, 1>& Y,
                         const HPL::Device& d)
{
    return clblasDspmv(clblasRowMajor,
                       uplo,
                       X.getDimension(0),
                       1,
                       AP,
                       0,
                       X,
                       0,
                       1,
                       0,
                       Y,
                       0,
                       1,
                       d);
}

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
                         const HPL::Device& d,
                         cl_uint numEventsInWaitList,
                         const cl_event * eventWaitList,
                         cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::HPMV);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0, 0), const_cast<HPL::Array<HPL::float2, 1>&>(AP),
                                   const_cast<HPL::Array<HPL::float2, 1>&>(X), Y);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasChpmv(order,
                                   uplo,
                                   N,
                                   alpha,
                                   cl_mem_FRunner(f, 0),
                                   offa,
                                   cl_mem_FRunner(f, 1),
                                   offx,
                                   incx,
                                   beta,
                                   cl_mem_FRunner(f, 2),
                                   0,
                                   1,
                                   1,
                                   &cq,
                                   numEventsInWaitList,
                                   eventWaitList,
                                   events);

    if (err != CL_SUCCESS)
        throw HPL::HPLclBLASException(err, "Exception thrown in clblasChpmv");

    return err;
}

clblasStatus clblasChpmv(clblasUplo uplo,
                         const HPL::Array<HPL::float2, 1>& AP,
                         const HPL::Array<HPL::float2, 1>& X,
                         HPL::Array<HPL::float2, 1>& Y,
                         const HPL::Device& d)
{
    cl_float2 alpha = {1, 0};
    cl_float2 beta = {0, 0};
    return clblasChpmv(clblasRowMajor,
                       uplo,
                       X.getDimension(0),
                       alpha,
                       AP,
                       0,
                       X,
                       0,
                       1,
                       beta,
                       Y,
                       0,
                       1,
                       d);
}

clblasStatus clblasSspr(clblasOrder order,
                        clblasUplo uplo,
                        size_t N,
                        cl_float alpha,
                        const HPL::Array<float, 1>& X,
                        size_t offx,
                        int incx,
                        HPL::Array<float, 1>& AP,
                        size_t offa,
                        const HPL::Device& d,
                        cl_uint numEventsInWaitList,
                        const cl_event * eventWaitList,
                        cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::SPR);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0, 0), const_cast<HPL::Array<float, 1>&>(X),
                                   AP);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasSspr(order,
                                  uplo,
                                  N,
                                  alpha,
                                  cl_mem_FRunner(f, 0),
                                  offx,
                                  incx,
                                  cl_mem_FRunner(f, 1),
                                  offa,
                                  1,
                                  &cq,
                                  numEventsInWaitList,
                                  eventWaitList,
                                  events);

    if (err != CL_SUCCESS)
        throw HPL::HPLclBLASException(err, "Exception thrown in clblasSspr");

    return err;
}

clblasStatus clblasSspr(clblasUplo uplo,
                        const HPL::Array<float, 1>& X,
                        HPL::Array<float, 1>& AP,
                        const HPL::Device& d)
{
    return clblasSspr(clblasRowMajor,
                       uplo,
                       X.getDimension(0),
                       1,
                       X,
                       0,
                       1,
                       AP,
                       0,
                       d);
}

clblasStatus clblasDspr(clblasOrder order,
                        clblasUplo uplo,
                        size_t N,
                        cl_double alpha,
                        const HPL::Array<double, 1>& X,
                        size_t offx,
                        int incx,
                        HPL::Array<double, 1>& AP,
                        size_t offa,
                        const HPL::Device& d,
                        cl_uint numEventsInWaitList,
                        const cl_event * eventWaitList,
                        cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::SPR);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0, 0), const_cast<HPL::Array<double, 1>&>(X),
                                   AP);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasDspr(order,
                                  uplo,
                                  N,
                                  alpha,
                                  cl_mem_FRunner(f, 0),
                                  offx,
                                  incx,
                                  cl_mem_FRunner(f, 1),
                                  offa,
                                  1,
                                  &cq,
                                  numEventsInWaitList,
                                  eventWaitList,
                                  events);

    if (err != CL_SUCCESS)
        throw HPL::HPLclBLASException(err, "Exception thrown in clblasDspr");

    return err;
}

clblasStatus clblasDspr(clblasUplo uplo,
                        const HPL::Array<double, 1>& X,
                        HPL::Array<double, 1>& AP,
                        const HPL::Device& d)
{
    return clblasDspr(clblasRowMajor,
                       uplo,
                       X.getDimension(0),
                       1,
                       X,
                       0,
                       1,
                       AP,
                       0,
                       d);
}

clblasStatus clblasChpr(clblasOrder order,
                        clblasUplo uplo,
                        size_t N,
                        cl_float alpha,
                        const HPL::Array<HPL::float2, 1>& X,
                        size_t offx,
                        int incx,
                        HPL::Array<HPL::float2, 1>& AP,
                        size_t offa,
                        const HPL::Device& d,
                        cl_uint numEventsInWaitList,
                        const cl_event * eventWaitList,
                        cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::HPR);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0, 0), const_cast<HPL::Array<HPL::float2, 1>&>(X),
                                   AP);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasChpr(order,
                                  uplo,
                                  N,
                                  alpha,
                                  cl_mem_FRunner(f, 0),
                                  offx,
                                  incx,
                                  cl_mem_FRunner(f, 1),
                                  offa,
                                  1,
                                  &cq,
                                  numEventsInWaitList,
                                  eventWaitList,
                                  events);

    if (err != CL_SUCCESS)
        throw HPL::HPLclBLASException(err, "Exception thrown in clblasChpr");

    return err;
}

clblasStatus clblasChpr(clblasUplo uplo,
                        const HPL::Array<HPL::float2, 1>& X,
                        HPL::Array<HPL::float2, 1>& AP,
                        const HPL::Device& d)
{
    return clblasChpr(clblasRowMajor,
                      uplo,
                      X.getDimension(0),
                      1,
                      X,
                      0,
                      1,
                      AP,
                      0,
                      d);
}

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
                         const HPL::Device& d,
                         cl_uint numEventsInWaitList,
                         const cl_event * eventWaitList,
                         cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::SPR2);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0, 0), const_cast<HPL::Array<float, 1>&>(X),
                                   const_cast<HPL::Array<float, 1>&>(Y), AP);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasSspr2(order,
                                   uplo,
                                   N,
                                   alpha,
                                   cl_mem_FRunner(f, 0),
                                   offx,
                                   incx,
                                   cl_mem_FRunner(f, 1),
                                   offy,
                                   incy,
                                   cl_mem_FRunner(f, 2),
                                   offa,
                                   1,
                                   &cq,
                                   numEventsInWaitList,
                                   eventWaitList,
                                   events);

    if (err != CL_SUCCESS)
        throw HPL::HPLclBLASException(err, "Exception thrown in clblasSspr2");

    return err;
}

clblasStatus clblasSspr2(clblasUplo uplo,
                         const HPL::Array<float, 1>& X,
                         const HPL::Array<float, 1>& Y,
                         HPL::Array<float, 1>& AP,
                         const HPL::Device& d)
{
    return clblasSspr2(clblasRowMajor,
                       uplo,
                       X.getDimension(0),
                       1,
                       X,
                       0,
                       1,
                       Y,
                       0,
                       1,
                       AP,
                       0,
                       d);
}

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
                         const HPL::Device& d,
                         cl_uint numEventsInWaitList,
                         const cl_event * eventWaitList,
                         cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::SPR2);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0, 0), const_cast<HPL::Array<double, 1>&>(X),
                                   const_cast<HPL::Array<double, 1>&>(Y), AP);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasDspr2(order,
                                   uplo,
                                   N,
                                   alpha,
                                   cl_mem_FRunner(f, 0),
                                   offx,
                                   incx,
                                   cl_mem_FRunner(f, 1),
                                   offy,
                                   incy,
                                   cl_mem_FRunner(f, 2),
                                   offa,
                                   1,
                                   &cq,
                                   numEventsInWaitList,
                                   eventWaitList,
                                   events);

    if (err != CL_SUCCESS)
        throw HPL::HPLclBLASException(err, "Exception thrown in clblasDspr2");

    return err;
}

clblasStatus clblasDspr2(clblasUplo uplo,
                         const HPL::Array<double, 1>& X,
                         const HPL::Array<double, 1>& Y,
                         HPL::Array<double, 1>& AP,
                         const HPL::Device& d)
{
    return clblasDspr2(clblasRowMajor,
                       uplo,
                       X.getDimension(0),
                       1,
                       X,
                       0,
                       1,
                       Y,
                       0,
                       1,
                       AP,
                       0,
                       d);
}

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
                         const HPL::Device& d,
                         cl_uint numEventsInWaitList,
                         const cl_event * eventWaitList,
                         cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::SPR2);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0, 0), const_cast<HPL::Array<HPL::float2, 1>&>(X),
                                   const_cast<HPL::Array<HPL::float2, 1>&>(Y), AP);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasChpr2(order,
                                   uplo,
                                   N,
                                   alpha,
                                   cl_mem_FRunner(f, 0),
                                   offx,
                                   incx,
                                   cl_mem_FRunner(f, 1),
                                   offy,
                                   incy,
                                   cl_mem_FRunner(f, 2),
                                   offa,
                                   1,
                                   &cq,
                                   numEventsInWaitList,
                                   eventWaitList,
                                   events);

    if (err != CL_SUCCESS)
        throw HPL::HPLclBLASException(err, "Exception thrown in clblasChpr2");

    return err;
}

clblasStatus clblasChpr2(clblasUplo uplo,
                         const HPL::Array<HPL::float2, 1>& X,
                         const HPL::Array<HPL::float2, 1>& Y,
                         HPL::Array<HPL::float2, 1>& AP,
                         const HPL::Device& d)
{
    cl_float2 alpha = {1, 0};
    return clblasChpr2(clblasRowMajor,
                       uplo,
                       X.getDimension(0),
                       alpha,
                       X,
                       0,
                       1,
                       Y,
                       0,
                       1,
                       AP,
                       0,
                       d);
}

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
                         const HPL::Device& d,
                         cl_uint numEventsInWaitList,
                         const cl_event * eventWaitList,
                         cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::GBMV);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0, 0), const_cast<HPL::Array<float, 1>&>(A),
                                   const_cast<HPL::Array<float, 1>&>(X), Y);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasSgbmv(order,
                                   trans,
                                   M,
                                   N,
                                   KL,
                                   KU,
                                   alpha,
                                   cl_mem_FRunner(f, 0),
                                   offa,
                                   lda,
                                   cl_mem_FRunner(f, 1),
                                   offx,
                                   incx,
                                   beta,
                                   cl_mem_FRunner(f, 2),
                                   offy,
                                   incy,
                                   1,
                                   &cq,
                                   numEventsInWaitList,
                                   eventWaitList,
                                   events);

    if (err != CL_SUCCESS)
        throw HPL::HPLclBLASException(err, "Exception thrown in clblasSgbmv");

    return err;
}

clblasStatus clblasSgbmv(size_t KL,
                         size_t KU,
                         const HPL::Array<float, 1>& A,
                         const HPL::Array<float, 1>& X,
                         HPL::Array<float, 1>& Y,
                         const HPL::Device& d)
{
    return clblasSgbmv(clblasRowMajor,
                       clblasNoTrans,
                       Y.getDimension(0),
                       X.getDimension(0),
                       KL,
                       KU,
                       1,
                       A,
                       0,
                       KL+KU+1,
                       X,
                       0,
                       1,
                       0,
                       Y,
                       0,
                       1,
                       d);
}

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
                         const HPL::Device& d,
                         cl_uint numEventsInWaitList,
                         const cl_event * eventWaitList,
                         cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::GBMV);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0, 0), const_cast<HPL::Array<double, 1>&>(A),
                                   const_cast<HPL::Array<double, 1>&>(X), Y);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasDgbmv(order,
                                   trans,
                                   M,
                                   N,
                                   KL,
                                   KU,
                                   alpha,
                                   cl_mem_FRunner(f, 0),
                                   offa,
                                   lda,
                                   cl_mem_FRunner(f, 1),
                                   offx,
                                   incx,
                                   beta,
                                   cl_mem_FRunner(f, 2),
                                   offy,
                                   incy,
                                   1,
                                   &cq,
                                   numEventsInWaitList,
                                   eventWaitList,
                                   events);

    if (err != CL_SUCCESS)
        throw HPL::HPLclBLASException(err, "Exception thrown in clblasDgbmv");

    return err;
}

clblasStatus clblasDgbmv(size_t KL,
                         size_t KU,
                         const HPL::Array<double, 1>& A,
                         const HPL::Array<double, 1>& X,
                         HPL::Array<double, 1>& Y,
                         const HPL::Device& d)
{
    return clblasDgbmv(clblasRowMajor,
                       clblasNoTrans,
                       Y.getDimension(0),
                       X.getDimension(0),
                       KL,
                       KU,
                       1,
                       A,
                       0,
                       KL+KU+1,
                       X,
                       0,
                       1,
                       0,
                       Y,
                       0,
                       1,
                       d);
}

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
                         const HPL::Device& d,
                         cl_uint numEventsInWaitList,
                         const cl_event * eventWaitList,
                         cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::GBMV);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0, 0), const_cast<HPL::Array<HPL::float2, 1>&>(A),
                                   const_cast<HPL::Array<HPL::float2, 1>&>(X), Y);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasCgbmv(order,
                                   trans,
                                   M,
                                   N,
                                   KL,
                                   KU,
                                   alpha,
                                   cl_mem_FRunner(f, 0),
                                   offa,
                                   lda,
                                   cl_mem_FRunner(f, 1),
                                   offx,
                                   incx,
                                   beta,
                                   cl_mem_FRunner(f, 2),
                                   offy,
                                   incy,
                                   1,
                                   &cq,
                                   numEventsInWaitList,
                                   eventWaitList,
                                   events);

    if (err != CL_SUCCESS)
        throw HPL::HPLclBLASException(err, "Exception thrown in clblasCgbmv");

    return err;
}

clblasStatus clblasCgbmv(size_t KL,
                         size_t KU,
                         const HPL::Array<HPL::float2, 1>& A,
                         const HPL::Array<HPL::float2, 1>& X,
                         HPL::Array<HPL::float2, 1>& Y,
                         const HPL::Device& d)
{
    cl_float2 alpha = {1, 0};
    cl_float2 beta  = {0, 0};
    return clblasCgbmv(clblasRowMajor,
                       clblasNoTrans,
                       Y.getDimension(0),
                       X.getDimension(0),
                       KL,
                       KU,
                       alpha,
                       A,
                       0,
                       KL+KU+1,
                       X,
                       0,
                       1,
                       beta,
                       Y,
                       0,
                       1,
                       d);
}

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
                         const HPL::Device& d,
                         cl_uint numEventsInWaitList,
                         const cl_event * eventWaitList,
                         cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::TBMV);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0, 0), const_cast<HPL::Array<float, 1>&>(A),
                                   X, scratchBuff);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasStbmv(order,
                                   uplo,
                                   trans,
                                   diag,
                                   N,
                                   K,
                                   cl_mem_FRunner(f, 0),
                                   offa,
                                   lda,
                                   cl_mem_FRunner(f, 1),
                                   offx,
                                   incx,
                                   cl_mem_FRunner(f, 2),
                                   1,
                                   &cq,
                                   numEventsInWaitList,
                                   eventWaitList,
                                   events);

    if (err != CL_SUCCESS)
        throw HPL::HPLclBLASException(err, "Exception thrown in clblasStbmv");

    return err;
}

clblasStatus clblasStbmv(clblasUplo uplo,
                         clblasDiag diag,
                         size_t K,
                         const HPL::Array<float, 1>& A,
                         HPL::Array<float, 1>& X,
                         const HPL::Device& d)
{
    HPL::Array<float, 1> scratchBuff(1 + (X.getDimension(0)-1));
    return clblasStbmv(clblasRowMajor,
                       uplo,
                       clblasNoTrans,
                       diag,
                       X.getDimension(0),
                       K,
                       A,
                       0,
                       K+1,
                       X,
                       0,
                       1,
                       scratchBuff,
                       d);
}

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
                         const HPL::Device& d,
                         cl_uint numEventsInWaitList,
                         const cl_event * eventWaitList,
                         cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::TBMV);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0, 0), const_cast<HPL::Array<double, 1>&>(A),
                                   X, scratchBuff);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasDtbmv(order,
                                   uplo,
                                   trans,
                                   diag,
                                   N,
                                   K,
                                   cl_mem_FRunner(f, 0),
                                   offa,
                                   lda,
                                   cl_mem_FRunner(f, 1),
                                   offx,
                                   incx,
                                   cl_mem_FRunner(f, 2),
                                   1,
                                   &cq,
                                   numEventsInWaitList,
                                   eventWaitList,
                                   events);

    if (err != CL_SUCCESS)
        throw HPL::HPLclBLASException(err, "Exception thrown in clblasDtbmv");

    return err;
}

clblasStatus clblasDtbmv(clblasUplo uplo,
                         clblasDiag diag,
                         size_t K,
                         const HPL::Array<double, 1>& A,
                         HPL::Array<double, 1>& X,
                         const HPL::Device& d)
{
    HPL::Array<double, 1> scratchBuff(1 + (X.getDimension(0)-1));
    return clblasDtbmv(clblasRowMajor,
                       uplo,
                       clblasNoTrans,
                       diag,
                       X.getDimension(0),
                       K,
                       A,
                       0,
                       K+1,
                       X,
                       0,
                       1,
                       scratchBuff,
                       d);
}

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
                         const HPL::Device& d,
                         cl_uint numEventsInWaitList,
                         const cl_event * eventWaitList,
                         cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::TBMV);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0, 0), const_cast<HPL::Array<HPL::float2, 1>&>(A),
                                   X, scratchBuff);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasCtbmv(order,
                                   uplo,
                                   trans,
                                   diag,
                                   N,
                                   K,
                                   cl_mem_FRunner(f, 0),
                                   offa,
                                   lda,
                                   cl_mem_FRunner(f, 1),
                                   offx,
                                   incx,
                                   cl_mem_FRunner(f, 2),
                                   1,
                                   &cq,
                                   numEventsInWaitList,
                                   eventWaitList,
                                   events);

    if (err != CL_SUCCESS)
        throw HPL::HPLclBLASException(err, "Exception thrown in clblasCtbmv");

    return err;
}

clblasStatus clblasCtbmv(clblasUplo uplo,
                         clblasDiag diag,
                         size_t K,
                         const HPL::Array<HPL::float2, 1>& A,
                         HPL::Array<HPL::float2, 1>& X,
                         const HPL::Device& d)
{
    HPL::Array<HPL::float2, 1> scratchBuff(1 + (X.getDimension(0)-1));
    return clblasCtbmv(clblasRowMajor,
                       uplo,
                       clblasNoTrans,
                       diag,
                       X.getDimension(0),
                       K,
                       A,
                       0,
                       K+1,
                       X,
                       0,
                       1,
                       scratchBuff,
                       d);
}

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
                         const HPL::Device& d,
                         cl_uint numEventsInWaitList,
                         const cl_event * eventWaitList,
                         cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::SBMV);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0, 0), const_cast<HPL::Array<float, 1>&>(A),
                                   const_cast<HPL::Array<float, 1>&>(X), Y);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasSsbmv(order,
                                   uplo,
                                   N,
                                   K,
                                   alpha,
                                   cl_mem_FRunner(f, 0),
                                   offa,
                                   lda,
                                   cl_mem_FRunner(f, 1),
                                   offx,
                                   incx,
                                   beta,
                                   cl_mem_FRunner(f, 2),
                                   0,
                                   1,
                                   1,
                                   &cq,
                                   numEventsInWaitList,
                                   eventWaitList,
                                   events);

    if (err != CL_SUCCESS)
        throw HPL::HPLclBLASException(err, "Exception thrown in clblasSsbmv");

    return err;
}

clblasStatus clblasSsbmv(clblasUplo uplo,
                         size_t K,
                         const HPL::Array<float, 1>& A,
                         const HPL::Array<float, 1>& X,
                         HPL::Array<float, 1>& Y,
                         const HPL::Device& d)
{
    return clblasSsbmv(clblasRowMajor,
                       uplo,
                       X.getDimension(0),
                       K,
                       1,
                       A,
                       0,
                       K+1,
                       X,
                       0,
                       1,
                       0,
                       Y,
                       0,
                       1,
                       d);
}

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
                         const HPL::Device& d,
                         cl_uint numEventsInWaitList,
                         const cl_event * eventWaitList,
                         cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::SBMV);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0, 0), const_cast<HPL::Array<double, 1>&>(A),
                                   const_cast<HPL::Array<double, 1>&>(X), Y);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasDsbmv(order,
                                   uplo,
                                   N,
                                   K,
                                   alpha,
                                   cl_mem_FRunner(f, 0),
                                   offa,
                                   lda,
                                   cl_mem_FRunner(f, 1),
                                   offx,
                                   incx,
                                   beta,
                                   cl_mem_FRunner(f, 2),
                                   0,
                                   1,
                                   1,
                                   &cq,
                                   numEventsInWaitList,
                                   eventWaitList,
                                   events);

    if (err != CL_SUCCESS)
        throw HPL::HPLclBLASException(err, "Exception thrown in clblasDsbmv");

    return err;
}

clblasStatus clblasDsbmv(clblasUplo uplo,
                         size_t K,
                         const HPL::Array<double, 1>& A,
                         const HPL::Array<double, 1>& X,
                         HPL::Array<double, 1>& Y,
                         const HPL::Device& d)
{
    return clblasDsbmv(clblasRowMajor,
                       uplo,
                       X.getDimension(0),
                       K,
                       1,
                       A,
                       0,
                       K+1,
                       X,
                       0,
                       1,
                       0,
                       Y,
                       0,
                       1,
                       d);
}

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
                         const HPL::Device& d,
                         cl_uint numEventsInWaitList,
                         const cl_event * eventWaitList,
                         cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::HBMV);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0, 0), const_cast<HPL::Array<HPL::float2, 1>&>(A),
                                   const_cast<HPL::Array<HPL::float2, 1>&>(X), Y);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasChbmv(order,
                                   uplo,
                                   N,
                                   K,
                                   alpha,
                                   cl_mem_FRunner(f, 0),
                                   offa,
                                   lda,
                                   cl_mem_FRunner(f, 1),
                                   offx,
                                   incx,
                                   beta,
                                   cl_mem_FRunner(f, 2),
                                   0,
                                   1,
                                   1,
                                   &cq,
                                   numEventsInWaitList,
                                   eventWaitList,
                                   events);

    if (err != CL_SUCCESS)
        throw HPL::HPLclBLASException(err, "Exception thrown in clblasChbmv");

    return err;
}

clblasStatus clblasChbmv(clblasUplo uplo,
                         size_t K,
                         const HPL::Array<HPL::float2, 1>& A,
                         const HPL::Array<HPL::float2, 1>& X,
                         HPL::Array<HPL::float2, 1>& Y,
                         const HPL::Device& d)
{
    cl_float2 alpha = {1, 0};
    cl_float2 beta = {0, 0};
    return clblasChbmv(clblasRowMajor,
                       uplo,
                       X.getDimension(0),
                       K,
                       alpha,
                       A,
                       0,
                       K+1,
                       X,
                       0,
                       1,
                       beta,
                       Y,
                       0,
                       1,
                       d);
}

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
                         const HPL::Device& d,
                         cl_uint numEventsInWaitList,
                         const cl_event * eventWaitList,
                         cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::TBSV);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0, 0), const_cast<HPL::Array<float, 1>&>(A), X);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasStbsv(order,
                                   uplo,
                                   trans,
                                   diag,
                                   N,
                                   K,
                                   cl_mem_FRunner(f, 0),
                                   offa,
                                   lda,
                                   cl_mem_FRunner(f, 1),
                                   offx,
                                   incx,
                                   1,
                                   &cq,
                                   numEventsInWaitList,
                                   eventWaitList,
                                   events);

    if (err != CL_SUCCESS)
        throw HPL::HPLclBLASException(err, "Exception thrown in clblasStbsv");

    return err;
}

clblasStatus clblasStbsv(clblasUplo uplo,
                         clblasDiag diag,
                         size_t K,
                         const HPL::Array<float, 1>& A,
                         HPL::Array<float, 1>& X,
                         const HPL::Device& d)
{
    return clblasStbsv(clblasRowMajor,
                       uplo,
                       clblasNoTrans,
                       diag,
                       X.getDimension(0),
                       K,
                       A,
                       0,
                       K+1,
                       X,
                       0,
                       1,
                       d);
}

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
                         const HPL::Device& d,
                         cl_uint numEventsInWaitList,
                         const cl_event * eventWaitList,
                         cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::TBSV);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0, 0), const_cast<HPL::Array<double, 1>&>(A), X);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasDtbsv(order,
                                   uplo,
                                   trans,
                                   diag,
                                   N,
                                   K,
                                   cl_mem_FRunner(f, 0),
                                   offa,
                                   lda,
                                   cl_mem_FRunner(f, 1),
                                   offx,
                                   incx,
                                   1,
                                   &cq,
                                   numEventsInWaitList,
                                   eventWaitList,
                                   events);

    if (err != CL_SUCCESS)
        throw HPL::HPLclBLASException(err, "Exception thrown in clblasDtbsv");

    return err;
}

clblasStatus clblasDtbsv(clblasUplo uplo,
                         clblasDiag diag,
                         size_t K,
                         const HPL::Array<double, 1>& A,
                         HPL::Array<double, 1>& X,
                         const HPL::Device& d)
{
    return clblasDtbsv(clblasRowMajor,
                       uplo,
                       clblasNoTrans,
                       diag,
                       X.getDimension(0),
                       K,
                       A,
                       0,
                       K+1,
                       X,
                       0,
                       1,
                       d);
}

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
                         const HPL::Device& d,
                         cl_uint numEventsInWaitList,
                         const cl_event * eventWaitList,
                         cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::TBSV);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0, 0), const_cast<HPL::Array<HPL::float2, 1>&>(A), X);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasCtbsv(order,
                                   uplo,
                                   trans,
                                   diag,
                                   N,
                                   K,
                                   cl_mem_FRunner(f, 0),
                                   offa,
                                   lda,
                                   cl_mem_FRunner(f, 1),
                                   offx,
                                   incx,
                                   1,
                                   &cq,
                                   numEventsInWaitList,
                                   eventWaitList,
                                   events);

    if (err != CL_SUCCESS)
        throw HPL::HPLclBLASException(err, "Exception thrown in clblasCtbsv");

    return err;
}

clblasStatus clblasCtbsv(clblasUplo uplo,
                         clblasDiag diag,
                         size_t K,
                         const HPL::Array<HPL::float2, 1>& A,
                         HPL::Array<HPL::float2, 1>& X,
                         const HPL::Device& d)
{
    return clblasCtbsv(clblasRowMajor,
                       uplo,
                       clblasNoTrans,
                       diag,
                       X.getDimension(0),
                       K,
                       A,
                       0,
                       K+1,
                       X,
                       0,
                       1,
                       d);
}

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
                         const HPL::Device& d,
                         cl_uint numEventsInWaitList,
                         const cl_event * eventWaitList,
                         cl_event * events)
{
  HPL::FRunner f(FHandle_v + BLASFuncIndex::GEMM);

  f.device(d).fillInFRunnerTable(0, HPL::Tuple(0,0), const_cast<HPL::Array<float, 2>&>(A), const_cast<HPL::Array<float, 2>&>(B), C);

  cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasSgemm(order,
                                   transA,
                                   transB,
                                   M,
                                   N,
                                   K,
                                   alpha,
                                   cl_mem_FRunner(f, 0),
                                   offA,
                                   lda,
                                   cl_mem_FRunner(f, 1),
                                   offB,
                                   ldb,
                                   beta,
                                   cl_mem_FRunner(f, 2),
                                   offC,
                                   ldc,
                                   1,
                                   &cq,
                                   0,
                                   NULL,
                                   NULL);

  if (err != CL_SUCCESS)
    throw HPL::HPLclBLASException(err, "Exception thrown in clblasSgemm");

  return err;
}

clblasStatus clblasSgemm(const HPL::Array<float, 2>& x,
                         const HPL::Array<float, 2>& y,
                         HPL::Array<float, 2>& z,
                         const HPL::Device& d) {
    return clblasSgemm(clblasRowMajor,
                       clblasNoTrans,
                       clblasNoTrans,
                       x.getDimension(0),
                       y.getDimension(1),
                       y.getDimension(0),
                       1,
                       x,
                       0,
                       getLowLevelDim1(x, d),
                       y,
                       0,
                       getLowLevelDim1(y, d),
                       0,
                       z,
                       0,
                       getLowLevelDim1(z, d),
                       d);
}

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
                         const HPL::Device& d,
                         cl_uint numEventsInWaitList,
                         const cl_event * eventWaitList,
                         cl_event * events)
{
  HPL::FRunner f(FHandle_v + BLASFuncIndex::GEMM);

  f.device(d).fillInFRunnerTable(0, HPL::Tuple(0,0), const_cast<HPL::Array<double, 2>&>(A), const_cast<HPL::Array<double, 2>&>(B), C);

  cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasDgemm(order,
                                   transA,
                                   transB,
                                   M,
                                   N,
                                   K,
                                   alpha,
                                   cl_mem_FRunner(f, 0),
                                   offA,
                                   lda,
                                   cl_mem_FRunner(f, 1),
                                   offB,
                                   ldb,
                                   beta,
                                   cl_mem_FRunner(f, 2),
                                   offC,
                                   ldc,
                                   1,
                                   &cq,
                                   0,
                                   NULL,
                                   NULL);

  if (err != CL_SUCCESS)
    throw HPL::HPLclBLASException(err, "Exception thrown in clblasDgemm");

  return err;
}

clblasStatus clblasDgemm(const HPL::Array<double, 2>& x,
                         const HPL::Array<double, 2>& y,
                         HPL::Array<double, 2>& z,
                         const HPL::Device& d) {
    return clblasDgemm(clblasRowMajor,
                       clblasNoTrans,
                       clblasNoTrans,
                       x.getDimension(0),
                       y.getDimension(1),
                       y.getDimension(0),
                       1,
                       x,
                       0,
                       getLowLevelDim1(x, d),
                       y,
                       0,
                       getLowLevelDim1(y, d),
                       0,
                       z,
                       0,
                       getLowLevelDim1(z, d),
                       d);
}

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
                         const HPL::Device& d,
                         cl_uint numEventsInWaitList,
                         const cl_event * eventWaitList,
                         cl_event * events)
{
  HPL::FRunner f(FHandle_v + BLASFuncIndex::GEMM);

  f.device(d).fillInFRunnerTable(0, HPL::Tuple(0,0), const_cast<HPL::Array<HPL::float2, 2>&>(A), const_cast<HPL::Array<HPL::float2, 2>&>(B), C);

  cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasCgemm(order,
                                   transA,
                                   transB,
                                   M,
                                   N,
                                   K,
                                   alpha,
                                   cl_mem_FRunner(f, 0),
                                   offA,
                                   lda,
                                   cl_mem_FRunner(f, 1),
                                   offB,
                                   ldb,
                                   beta,
                                   cl_mem_FRunner(f, 2),
                                   offC,
                                   ldc,
                                   1,
                                   &cq,
                                   0,
                                   NULL,
                                   NULL);

  if (err != CL_SUCCESS)
    throw HPL::HPLclBLASException(err, "Exception thrown in clblasCgemm");

  return err;
}

clblasStatus clblasCgemm(const HPL::Array<HPL::float2, 2>& x,
                         const HPL::Array<HPL::float2, 2>& y,
                         HPL::Array<HPL::float2, 2>& z,
                         const HPL::Device& d) {
    cl_float2 alpha = {1, 0};
    cl_float2 beta  = {0, 0};
    return clblasCgemm(clblasRowMajor,
                       clblasNoTrans,
                       clblasNoTrans,
                       x.getDimension(0),
                       y.getDimension(1),
                       y.getDimension(0),
                       alpha,
                       x,
                       0,
                       getLowLevelDim1(x, d),
                       y,
                       0,
                       getLowLevelDim1(y, d),
                       beta,
                       z,
                       0,
                       getLowLevelDim1(z, d),
                       d);
}

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
                         const HPL::Device& d,
                         cl_uint numEventsInWaitList,
                         const cl_event * eventWaitList,
                         cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::TRMM);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0, 0), const_cast<HPL::Array<float, 2>&>(A), B);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasStrmm(order,
                                   side,	
                                   uplo,
                                   transA,
                                   diag,
                                   M,
                                   N,
                                   alpha,
                                   cl_mem_FRunner(f, 0),
                                   offA,
                                   lda,
                                   cl_mem_FRunner(f, 1),
                                   offB,
                                   ldb,
                                   1,
                                   &cq,
                                   numEventsInWaitList,
                                   eventWaitList,
                                   events);

    if (err != CL_SUCCESS)
        throw HPL::HPLclBLASException(err, "Exception thrown in clblasStrmm");

    return err;
}

clblasStatus clblasStrmm(clblasSide side,
                         clblasUplo uplo,
                         clblasDiag diag,
                         const HPL::Array<float, 2>& A,
                         HPL::Array<float, 2>& B,
                         const HPL::Device& d)
{
    return clblasStrmm(clblasRowMajor,
                       side,
                       uplo,
                       clblasNoTrans,
                       diag,
                       B.getDimension(0),
                       B.getDimension(1),
                       1,
                       A,
                       0,
                       side == clblasLeft ? B.getDimension(0) : B.getDimension(1),
                       B,
                       0,
                       B.getDimension(1),
                       d);
}

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
                         const HPL::Device& d,
                         cl_uint numEventsInWaitList,
                         const cl_event * eventWaitList,
                         cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::TRMM);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0, 0), const_cast<HPL::Array<double, 2>&>(A), B);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasDtrmm(order,
                                   side,
                                   uplo,
                                   transA,
                                   diag,
                                   M,
                                   N,
                                   alpha,
                                   cl_mem_FRunner(f, 0),
                                   offA,
                                   lda,
                                   cl_mem_FRunner(f, 1),
                                   offB,
                                   ldb,
                                   1,
                                   &cq,
                                   numEventsInWaitList,
                                   eventWaitList,
                                   events);

    if (err != CL_SUCCESS)
        throw HPL::HPLclBLASException(err, "Exception thrown in clblasDtrmm");

    return err;
}

clblasStatus clblasDtrmm(clblasSide side,
                         clblasUplo uplo,
                         clblasDiag diag,
                         const HPL::Array<double, 2>& A,
                         HPL::Array<double, 2>& B,
                         const HPL::Device& d)
{
    return clblasDtrmm(clblasRowMajor,
                       side,
                       uplo,
                       clblasNoTrans,
                       diag,
                       B.getDimension(0),
                       B.getDimension(1),
                       1,
                       A,
                       0,
                       side == clblasLeft ? B.getDimension(0) : B.getDimension(1),
                       B,
                       0,
                       B.getDimension(1),
                       d);
}

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
                         const HPL::Device& d,
                         cl_uint numEventsInWaitList,
                         const cl_event * eventWaitList,
                         cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::TRMM);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0, 0), const_cast<HPL::Array<HPL::float2, 2>&>(A), B);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasCtrmm(order,
                                   side,
                                   uplo,
                                   transA,
                                   diag,
                                   M,
                                   N,
                                   alpha,
                                   cl_mem_FRunner(f, 0),
                                   offA,
                                   lda,
                                   cl_mem_FRunner(f, 1),
                                   offB,
                                   ldb,
                                   1,
                                   &cq,
                                   numEventsInWaitList,
                                   eventWaitList,
                                   events);

    if (err != CL_SUCCESS)
        throw HPL::HPLclBLASException(err, "Exception thrown in clblasCtrmm");

    return err;
}

clblasStatus clblasCtrmm(clblasSide side,
                         clblasUplo uplo,
                         clblasDiag diag,
                         const HPL::Array<HPL::float2, 2>& A,
                         HPL::Array<HPL::float2, 2>& B,
                         const HPL::Device& d)
{
    cl_float2 alpha = {1, 0};
    return clblasCtrmm(clblasRowMajor,
                       side,
                       uplo,
                       clblasNoTrans,
                       diag,
                       B.getDimension(0),
                       B.getDimension(1),
                       alpha,
                       A,
                       0,
                       side == clblasLeft ? B.getDimension(0) : B.getDimension(1),
                       B,
                       0,
                       B.getDimension(1),
                       d);
}

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
                         const HPL::Device& d,
                         cl_uint numEventsInWaitList,
                         const cl_event * eventWaitList,
                         cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::TRSM);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0, 0), const_cast<HPL::Array<float, 2>&>(A), B);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasStrsm(order,
                                   side,	
                                   uplo,
                                   transA,
                                   diag,
                                   M,
                                   N,
                                   alpha,
                                   cl_mem_FRunner(f, 0),
                                   offA,
                                   lda,
                                   cl_mem_FRunner(f, 1),
                                   offB,
                                   ldb,
                                   1,
                                   &cq,
                                   numEventsInWaitList,
                                   eventWaitList,
                                   events);

    if (err != CL_SUCCESS)
        throw HPL::HPLclBLASException(err, "Exception thrown in clblasStrsm");

    return err;
}

clblasStatus clblasStrsm(clblasSide side,
                         clblasUplo uplo,
                         clblasDiag diag,
                         const HPL::Array<float, 2>& A,
                         HPL::Array<float, 2>& B,
                         const HPL::Device& d)
{
    return clblasStrsm(clblasRowMajor,
                       side,
                       uplo,
                       clblasNoTrans,
                       diag,
                       B.getDimension(0),
                       B.getDimension(1),
                       1,
                       A,
                       0,
                       side == clblasLeft ? B.getDimension(0) : B.getDimension(1),
                       B,
                       0,
                       B.getDimension(1),
                       d);
}

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
                         const HPL::Device& d,
                         cl_uint numEventsInWaitList,
                         const cl_event * eventWaitList,
                         cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::TRSM);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0, 0), const_cast<HPL::Array<double, 2>&>(A), B);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasDtrsm(order,
                                   side,
                                   uplo,
                                   transA,
                                   diag,
                                   M,
                                   N,
                                   alpha,
                                   cl_mem_FRunner(f, 0),
                                   offA,
                                   lda,
                                   cl_mem_FRunner(f, 1),
                                   offB,
                                   ldb,
                                   1,
                                   &cq,
                                   numEventsInWaitList,
                                   eventWaitList,
                                   events);

    if (err != CL_SUCCESS)
        throw HPL::HPLclBLASException(err, "Exception thrown in clblasDtrsm");

    return err;
}

clblasStatus clblasDtrsm(clblasSide side,
                         clblasUplo uplo,
                         clblasDiag diag,
                         const HPL::Array<double, 2>& A,
                         HPL::Array<double, 2>& B,
                         const HPL::Device& d)
{
    return clblasDtrsm(clblasRowMajor,
                       side,
                       uplo,
                       clblasNoTrans,
                       diag,
                       B.getDimension(0),
                       B.getDimension(1),
                       1,
                       A,
                       0,
                       side == clblasLeft ? B.getDimension(0) : B.getDimension(1),
                       B,
                       0,
                       B.getDimension(1),
                       d);
}

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
                         const HPL::Device& d,
                         cl_uint numEventsInWaitList,
                         const cl_event * eventWaitList,
                         cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::TRSM);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0, 0), const_cast<HPL::Array<HPL::float2, 2>&>(A), B);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasCtrsm(order,
                                   side,
                                   uplo,
                                   transA,
                                   diag,
                                   M,
                                   N,
                                   alpha,
                                   cl_mem_FRunner(f, 0),
                                   offA,
                                   lda,
                                   cl_mem_FRunner(f, 1),
                                   offB,
                                   ldb,
                                   1,
                                   &cq,
                                   numEventsInWaitList,
                                   eventWaitList,
                                   events);

    if (err != CL_SUCCESS)
        throw HPL::HPLclBLASException(err, "Exception thrown in clblasCtrsm");

    return err;
}

clblasStatus clblasCtrsm(clblasSide side,
                         clblasUplo uplo,
                         clblasDiag diag,
                         const HPL::Array<HPL::float2, 2>& A,
                         HPL::Array<HPL::float2, 2>& B,
                         const HPL::Device& d)
{
    cl_float2 alpha = {1, 0};
    return clblasCtrsm(clblasRowMajor,
                       side,
                       uplo,
                       clblasNoTrans,
                       diag,
                       B.getDimension(0),
                       B.getDimension(1),
                       alpha,
                       A,
                       0,
                       side == clblasLeft ? B.getDimension(0) : B.getDimension(1),
                       B,
                       0,
                       B.getDimension(1),
                       d);
}

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
                         const HPL::Device& d,
                         cl_uint numEventsInWaitList,
                         const cl_event * eventWaitList,
                         cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::SYRK);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0, 0), const_cast<HPL::Array<float, 2>&>(A), C);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasSsyrk(order,
                                   uplo,
                                   transA,
                                   N,
                                   K,
                                   alpha,
                                   cl_mem_FRunner(f, 0),
                                   offA,
                                   lda,
                                   beta,
                                   cl_mem_FRunner(f, 1),
                                   offC,
                                   ldc,
                                   1,
                                   &cq,
                                   numEventsInWaitList,
                                   eventWaitList,
                                   events);

    if (err != CL_SUCCESS)
        throw HPL::HPLclBLASException(err, "Exception thrown in clblasSsyrk");

    return err;
}

clblasStatus clblasSsyrk(clblasUplo uplo,
                         const HPL::Array<float, 2>& A,
                         HPL::Array<float, 2>& C,
                         const HPL::Device& d)
{
    return clblasSsyrk(clblasRowMajor,
                       uplo,
                       clblasNoTrans,
                       C.getDimension(0),
                       A.getDimension(1),
                       1,
                       A,
                       0,
                       A.getDimension(1),
                       1,
                       C,
                       0,
                       C.getDimension(1),
                       d);
}

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
                         const HPL::Device& d,
                         cl_uint numEventsInWaitList,
                         const cl_event * eventWaitList,
                         cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::SYRK);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0, 0), const_cast<HPL::Array<double, 2>&>(A), C);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasDsyrk(order,
                                   uplo,
                                   transA,
                                   N,
                                   K,
                                   alpha,
                                   cl_mem_FRunner(f, 0),
                                   offA,
                                   lda,
                                   beta,
                                   cl_mem_FRunner(f, 1),
                                   offC,
                                   ldc,
                                   1,
                                   &cq,
                                   numEventsInWaitList,
                                   eventWaitList,
                                   events);

    if (err != CL_SUCCESS)
        throw HPL::HPLclBLASException(err, "Exception thrown in clblasDsyrk");

    return err;
}

clblasStatus clblasDsyrk(clblasUplo uplo,
                         const HPL::Array<double, 2>& A,
                         HPL::Array<double, 2>& C,
                         const HPL::Device& d)
{
    return clblasDsyrk(clblasRowMajor,
                       uplo,
                       clblasNoTrans,
                       C.getDimension(0),
                       A.getDimension(1),
                       1,
                       A,
                       0,
                       A.getDimension(1),
                       1,
                       C,
                       0,
                       C.getDimension(1),
                       d);
}

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
                         const HPL::Device& d,
                         cl_uint numEventsInWaitList,
                         const cl_event * eventWaitList,
                         cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::SYRK);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0, 0), const_cast<HPL::Array<HPL::float2, 2>&>(A), C);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasCsyrk(order,
                                   uplo,
                                   transA,
                                   N,
                                   K,
                                   alpha,
                                   cl_mem_FRunner(f, 0),
                                   offA,
                                   lda,
                                   beta,
                                   cl_mem_FRunner(f, 1),
                                   offC,
                                   ldc,
                                   1,
                                   &cq,
                                   numEventsInWaitList,
                                   eventWaitList,
                                   events);

    if (err != CL_SUCCESS)
        throw HPL::HPLclBLASException(err, "Exception thrown in clblasCsyrk");

    return err;
}

clblasStatus clblasCsyrk(clblasUplo uplo,
                         const HPL::Array<HPL::float2, 2>& A,
                         HPL::Array<HPL::float2, 2>& C,
                         const HPL::Device& d)
{
    cl_float2 alpha = {1, 0};
    cl_float2 beta  = {0, 0};
    return clblasCsyrk(clblasRowMajor,
                       uplo,
                       clblasNoTrans,
                       C.getDimension(0),
                       A.getDimension(1),
                       alpha,
                       A,
                       0,
                       A.getDimension(1),
                       beta,
                       C,
                       0,
                       C.getDimension(1),
                       d);
}

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
                          const HPL::Device& d,
                          cl_uint numEventsInWaitList,
                          const cl_event * eventWaitList,
                          cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::SYR2K);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0, 0), const_cast<HPL::Array<float, 2>&>(A), 
                                   const_cast<HPL::Array<float, 2>&>(B), C);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasSsyr2k(order,
                                   uplo,
                                   transAB,
                                   N,
                                   K,
                                   alpha,
                                   cl_mem_FRunner(f, 0),
                                   offA,
                                   lda,
                                   cl_mem_FRunner(f, 1),
                                   offB,
                                   ldb,
                                   beta,
                                   cl_mem_FRunner(f, 2),
                                   offC,
                                   ldc,
                                   1,
                                   &cq,
                                   numEventsInWaitList,
                                   eventWaitList,
                                   events);

    if (err != CL_SUCCESS)
        throw HPL::HPLclBLASException(err, "Exception thrown in clblasSsyr2k");

    return err;
}

clblasStatus clblasSsyr2k(clblasUplo uplo,
                          const HPL::Array<float, 2>& A,
                          const HPL::Array<float, 2>& B,
                          HPL::Array<float, 2>& C,
                          const HPL::Device& d)
{
    return clblasSsyr2k(clblasRowMajor,
                       uplo,
                       clblasNoTrans,
                       C.getDimension(0),
                       A.getDimension(1),
                       1,
                       A,
                       0,
                       A.getDimension(1),
                       B,
                       0,
                       B.getDimension(1),
                       1,
                       C,
                       0,
                       C.getDimension(0),
                       d);
}

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
                          const HPL::Device& d,
                          cl_uint numEventsInWaitList,
                          const cl_event * eventWaitList,
                          cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::SYR2K);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0, 0), const_cast<HPL::Array<double, 2>&>(A),
                                   const_cast<HPL::Array<double, 2>&>(B), C);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasDsyr2k(order,
                                   uplo,
                                   transAB,
                                   N,
                                   K,
                                   alpha,
                                   cl_mem_FRunner(f, 0),
                                   offA,
                                   lda,
                                   cl_mem_FRunner(f, 1),
                                   offB,
                                   ldb,
                                   beta,
                                   cl_mem_FRunner(f, 2),
                                   offC,
                                   ldc,
                                   1,
                                   &cq,
                                   numEventsInWaitList,
                                   eventWaitList,
                                   events);

    if (err != CL_SUCCESS)
        throw HPL::HPLclBLASException(err, "Exception thrown in clblasDsyr2k");

    return err;
}

clblasStatus clblasDsyr2k(clblasUplo uplo,
                          const HPL::Array<double, 2>& A,
                          const HPL::Array<double, 2>& B,
                          HPL::Array<double, 2>& C,
                          const HPL::Device& d)
{
    return clblasDsyr2k(clblasRowMajor,
                       uplo,
                       clblasNoTrans,
                       C.getDimension(0),
                       A.getDimension(1),
                       1,
                       A,
                       0,
                       A.getDimension(1),
                       B,
                       0,
                       B.getDimension(1),
                       1,
                       C,
                       0,
                       C.getDimension(0),
                       d);
}

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
                          const HPL::Device& d,
                          cl_uint numEventsInWaitList,
                          const cl_event * eventWaitList,
                          cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::SYR2K);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0, 0), const_cast<HPL::Array<HPL::float2, 2>&>(A),
                                   const_cast<HPL::Array<HPL::float2, 2>&>(B), C);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasCsyr2k(order,
                                   uplo,
                                   transAB,
                                   N,
                                   K,
                                   alpha,
                                   cl_mem_FRunner(f, 0),
                                   offA,
                                   lda,
                                   cl_mem_FRunner(f, 1),
                                   offB,
                                   ldb,
                                   beta,
                                   cl_mem_FRunner(f, 2),
                                   offC,
                                   ldc,
                                   1,
                                   &cq,
                                   numEventsInWaitList,
                                   eventWaitList,
                                   events);

    if (err != CL_SUCCESS)
        throw HPL::HPLclBLASException(err, "Exception thrown in clblasCsyr2k");

    return err;
}

clblasStatus clblasCsyr2k(clblasUplo uplo,
                          const HPL::Array<HPL::float2, 2>& A,
                          const HPL::Array<HPL::float2, 2>& B,
                          HPL::Array<HPL::float2, 2>& C,
                          const HPL::Device& d)
{
    cl_float2 alpha = {1, 0};
    cl_float2 beta  = {0, 0};
    return clblasCsyr2k(clblasRowMajor,
                        uplo,
                        clblasNoTrans,
                        C.getDimension(0),
                        A.getDimension(1),
                        alpha,
                        A,
                        0,
                        A.getDimension(1),
                        B,
                        0,
                        B.getDimension(1),
                        beta,
                        C,
                        0,
                        C.getDimension(0),
                        d);
}

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
                         const HPL::Device& d,
                         cl_uint numEventsInWaitList,
                         const cl_event * eventWaitList,
                         cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::SYMM);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0, 0), const_cast<HPL::Array<float, 2>&>(A), 
                                   const_cast<HPL::Array<float, 2>&>(B), C);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasSsymm(order,
                                   side,
                                   uplo,
                                   M,
                                   N,
                                   alpha,
                                   cl_mem_FRunner(f, 0),
                                   offa,
                                   lda,
                                   cl_mem_FRunner(f, 1),
                                   offb,
                                   ldb,
                                   beta,
                                   cl_mem_FRunner(f, 2),
                                   offc,
                                   ldc,
                                   1,
                                   &cq,
                                   numEventsInWaitList,
                                   eventWaitList,
                                   events);

    if (err != CL_SUCCESS)
        throw HPL::HPLclBLASException(err, "Exception thrown in clblasSsymm");

    return err;
}

clblasStatus clblasSsymm(clblasUplo uplo,
                         clblasSide side,
                         const HPL::Array<float, 2>& A,
                         const HPL::Array<float, 2>& B,
                         HPL::Array<float, 2>& C,
                         const HPL::Device& d)
{
    return clblasSsymm(clblasRowMajor,
                       side,
                       uplo,
                       B.getDimension(0),
                       B.getDimension(1),
                       1,
                       A,
                       0,
                       side == clblasLeft ? B.getDimension(0) : B.getDimension(1),
                       B,
                       0,
                       B.getDimension(1),
                       0,
                       C,
                       0,
                       C.getDimension(1),
                       d);
}

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
                         const HPL::Device& d,
                         cl_uint numEventsInWaitList,
                         const cl_event * eventWaitList,
                         cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::SYMM);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0, 0), const_cast<HPL::Array<double, 2>&>(A),
                                   const_cast<HPL::Array<double, 2>&>(B), C);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasDsymm(order,
                                   side,
                                   uplo,
                                   M,
                                   N,
                                   alpha,
                                   cl_mem_FRunner(f, 0),
                                   offa,
                                   lda,
                                   cl_mem_FRunner(f, 1),
                                   offb,
                                   ldb,
                                   beta,
                                   cl_mem_FRunner(f, 2),
                                   offc,
                                   ldc,
                                   1,
                                   &cq,
                                   numEventsInWaitList,
                                   eventWaitList,
                                   events);

    if (err != CL_SUCCESS)
        throw HPL::HPLclBLASException(err, "Exception thrown in clblasDsymm");

    return err;
}

clblasStatus clblasDsymm(clblasUplo uplo,
                         clblasSide side,
                         const HPL::Array<double, 2>& A,
                         const HPL::Array<double, 2>& B,
                         HPL::Array<double, 2>& C,
                         const HPL::Device& d)
{
    return clblasDsymm(clblasRowMajor,
                       side,
                       uplo,
                       B.getDimension(0),
                       B.getDimension(1),
                       1,
                       A,
                       0,
                       side == clblasLeft ? B.getDimension(0) : B.getDimension(1),
                       B,
                       0,
                       B.getDimension(1),
                       0,
                       C,
                       0,
                       C.getDimension(1),
                       d);
}

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
                         const HPL::Device& d,
                         cl_uint numEventsInWaitList,
                         const cl_event * eventWaitList,
                         cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::SYMM);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0, 0), const_cast<HPL::Array<HPL::float2, 2>&>(A),
                                   const_cast<HPL::Array<HPL::float2, 2>&>(B), C);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasCsymm(order,
                                   side,
                                   uplo,
                                   M,
                                   N,
                                   alpha,
                                   cl_mem_FRunner(f, 0),
                                   offa,
                                   lda,
                                   cl_mem_FRunner(f, 1),
                                   offb,
                                   ldb,
                                   beta,
                                   cl_mem_FRunner(f, 2),
                                   offc,
                                   ldc,
                                   1,
                                   &cq,
                                   numEventsInWaitList,
                                   eventWaitList,
                                   events);

    if (err != CL_SUCCESS)
        throw HPL::HPLclBLASException(err, "Exception thrown in clblasCsymm");

    return err;
}

clblasStatus clblasCsymm(clblasUplo uplo,
                         clblasSide side,
                         const HPL::Array<HPL::float2, 2>& A,
                         const HPL::Array<HPL::float2, 2>& B,
                         HPL::Array<HPL::float2, 2>& C,
                         const HPL::Device& d)
{
    cl_float2 alpha = {1, 0};
    cl_float2 beta  = {0, 0};
    return clblasCsymm(clblasRowMajor,
                       side,
                       uplo,
                       B.getDimension(0),
                       B.getDimension(1),
                       alpha,
                       A,
                       0,
                       side == clblasLeft ? B.getDimension(0) : B.getDimension(1),
                       B,
                       0,
                       B.getDimension(1),
                       beta,
                       C,
                       0,
                       C.getDimension(1),
                       d);
}

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
                         const HPL::Device& d,
                         cl_uint numEventsInWaitList,
                         const cl_event * eventWaitList,
                         cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::HEMM);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0, 0), const_cast<HPL::Array<HPL::float2, 2>&>(A), 
                                   const_cast<HPL::Array<HPL::float2, 2>&>(B), C);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasChemm(order,
                                   side,
                                   uplo,
                                   M,
                                   N,
                                   alpha,
                                   cl_mem_FRunner(f, 0),
                                   offa,
                                   lda,
                                   cl_mem_FRunner(f, 1),
                                   offb,
                                   ldb,
                                   beta,
                                   cl_mem_FRunner(f, 2),
                                   offc,
                                   ldc,
                                   1,
                                   &cq,
                                   numEventsInWaitList,
                                   eventWaitList,
                                   events);

    if (err != CL_SUCCESS)
        throw HPL::HPLclBLASException(err, "Exception thrown in clblasChemm");

    return err;
}

clblasStatus clblasChemm(clblasSide side,
                         clblasUplo uplo,
                         const HPL::Array<HPL::float2, 2>& A,
                         const HPL::Array<HPL::float2, 2>& B,
                         HPL::Array<HPL::float2, 2>& C,
                         const HPL::Device& d)
{
    cl_float2 alpha = {1, 0};
    cl_float2 beta  = {0, 0};
    return clblasChemm(clblasRowMajor,
                       side,
                       uplo,
                       B.getDimension(0),
                       B.getDimension(1),
                       alpha,
                       A,
                       0,
                       side == clblasLeft ? B.getDimension(0) : B.getDimension(1),
                       B,
                       0,
                       B.getDimension(1),
                       beta,
                       C,
                       0,
                       C.getDimension(1),
                       d);
}

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
                         const HPL::Device& d,
                         cl_uint numEventsInWaitList,
                         const cl_event * eventWaitList,
                         cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::HERK);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0, 0), const_cast<HPL::Array<HPL::float2, 2>&>(A), C);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasCherk(order,
                                   uplo,
                                   transA,
                                   N,
                                   K,
                                   alpha,
                                   cl_mem_FRunner(f, 0),
                                   offa,
                                   lda,
                                   beta,
                                   cl_mem_FRunner(f, 1),
                                   offc,
                                   ldc,
                                   1,
                                   &cq,
                                   numEventsInWaitList,
                                   eventWaitList,
                                   events);

    if (err != CL_SUCCESS)
        throw HPL::HPLclBLASException(err, "Exception thrown in clblasCherk");

    return err;
}

clblasStatus clblasCherk(clblasUplo uplo,
                         const HPL::Array<HPL::float2, 2>& A,
                         HPL::Array<HPL::float2, 2>& C,
                         const HPL::Device& d)
{
    return clblasCherk(clblasRowMajor,
                       uplo,
                       clblasNoTrans,
                       C.getDimension(0),
                       A.getDimension(1),
                       1,
                       A,
                       0,
                       A.getDimension(1),
                       1,
                       C,
                       0,
                       C.getDimension(0),
                       d);
}

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
                          const HPL::Device& d,
                          cl_uint numEventsInWaitList,
                          const cl_event * eventWaitList,
                          cl_event * events)
{
    HPL::FRunner f(FHandle_v + BLASFuncIndex::HER2K);

    f.device(d).fillInFRunnerTable(0, HPL::Tuple(0, 0), const_cast<HPL::Array<HPL::float2, 2>&>(A), 
                                   const_cast<HPL::Array<HPL::float2, 2>&>(B), C);

    cl_command_queue cq = HPL::TheGlobalState().clbinding().external_setup(&f)();

    clblasStatus err = clblasCher2k(order,
                                   uplo,
                                   trans,
                                   N,
                                   K,
                                   alpha,
                                   cl_mem_FRunner(f, 0),
                                   offa,
                                   lda,
                                   cl_mem_FRunner(f, 1),
                                   offb,
                                   ldb,
                                   beta,
                                   cl_mem_FRunner(f, 2),
                                   offc,
                                   ldc,
                                   1,
                                   &cq,
                                   numEventsInWaitList,
                                   eventWaitList,
                                   events);

    if (err != CL_SUCCESS)
        throw HPL::HPLclBLASException(err, "Exception thrown in clblasCher2k");

    return err;
}

clblasStatus clblasCher2k(clblasUplo uplo,
                          const HPL::Array<HPL::float2, 2>& A,
                          const HPL::Array<HPL::float2, 2>& B,
                          HPL::Array<HPL::float2, 2>& C,
                          const HPL::Device& d)
{
    cl_float2 alpha = {1, 0};
    return clblasCher2k(clblasRowMajor,
                       uplo,
                       clblasNoTrans,
                       C.getDimension(0),
                       A.getDimension(1),
                       alpha,
                       A,
                       0,
                       A.getDimension(1),
                       B,
                       0,
                       A.getDimension(1),
                       1,
                       C,
                       0,
                       C.getDimension(0),
                       d);
}
