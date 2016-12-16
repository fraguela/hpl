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
/// \file     CLbinding.cpp
/// \author   Moisés Viñas        <moises.vinas@udc.es>
/// \author   Zeki Bozkus         <zeki.bozkus@khas.edu.tr>
/// \author   Basilio B. Fraguela <basilio.fraguela@udc.es>
///

#include <cassert>
#include <cstring>
#include <string>
#include "CLbinding.h"
#include "Array.h"
#include "FHandle.h"
#include "FRunner.h"
#include "exceptions/HPLException.hpp"
#include "HPL.h"
#include <list>


#ifdef __APPLE__
#define memalign(x,y) valloc(y)
#else
#include <malloc.h>
#endif

namespace {
  
  /// Finds the Platform_t from the name of the platform
  HPL::Platform_t string_to_platform_id(const HPL::String_t& platform_string)
  {
    for(int i = 0; i < HPL::MAX_PLATFORMS; i++) {
      std::size_t found = platform_string.find(HPL::PlatformName[i]);
      if(found != HPL::String_t::npos)
	return (HPL::Platform_t)i;
    }
    throw HPL::HPLException(0, "Exception thrown in string_to_platform_id: Unknown platform name!\n");
  }
  
  
  bool match_platform_device(HPL::String_t platform_name, HPL::String_t device_vendor)
  { static const char * const aux2 = "Advanced";  //THE DEVICE VENDOR FOR AMD IS 'Advanced Micro Devices, Inc.'

    // Notice that for Apple this loop will never match
    for(int i = 0; i < HPL::MAX_PLATFORMS; i++) {
      std::size_t match_p = platform_name.find(HPL::PlatformName[i]);
      std::size_t match_d = device_vendor.find((i != HPL::AMD) ? HPL::PlatformName[i] : aux2);
      if((match_p!= HPL::String_t::npos) && (match_d!= HPL::String_t::npos))
	return true;
    }    
    return false;
  }
  
  
  /// Fillds the HPL::DeviceProperties associated to a cl::Device
  void fillDeviceProperties(HPL::DeviceProperties* dP, const cl::Device& d)
  {
    dP->maxDimensions          = d.getInfo<CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS>();
    d.getInfo(CL_DEVICE_MAX_WORK_ITEM_SIZES, &(dP->maxLocalDomainSizes));
    dP->maxLocalDomainSize     = d.getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>();
    
    dP->globalMemSize          = d.getInfo<CL_DEVICE_GLOBAL_MEM_SIZE>();
    dP->localMemSize           = d.getInfo<CL_DEVICE_LOCAL_MEM_SIZE>();
    dP->dedicatedLocalMem      = (d.getInfo<CL_DEVICE_LOCAL_MEM_TYPE>() == CL_LOCAL);
    dP->unifiedMemory          = d.getInfo<CL_DEVICE_HOST_UNIFIED_MEMORY>();
    dP->maxConstantBufferSize  = d.getInfo<CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE>();

    dP->maxMemAllocSize        = d.getInfo<CL_DEVICE_MAX_MEM_ALLOC_SIZE>();
    dP->globalMemCacheSize     = d.getInfo<CL_DEVICE_GLOBAL_MEM_CACHE_SIZE>();
    
    switch(d.getInfo<CL_DEVICE_GLOBAL_MEM_CACHE_TYPE>()) {
      case CL_NONE : dP->globalMemCacheType = HPL::HPL_NONE; break;
      case CL_READ_ONLY_CACHE: dP->globalMemCacheType = HPL::HPL_RD; break;
      case CL_READ_WRITE_CACHE: dP->globalMemCacheType = HPL::HPL_RDWR; break;
      default:
	throw HPL::HPLException(0, "Exception thrown in fillDeviceProperties()");
    }
    
    dP->computeUnits           = d.getInfo<CL_DEVICE_MAX_COMPUTE_UNITS>();
    
    d.getInfo<cl::STRING_CLASS>(CL_DEVICE_NAME, &(dP->deviceName));
    d.getInfo<cl::STRING_CLASS>(CL_DEVICE_VENDOR, &(dP->deviceVendor));
    d.getInfo<cl::STRING_CLASS>(CL_DEVICE_EXTENSIONS, &(dP->extensions));  
  }

#ifdef HPL_PROFILE
  double waitForEvent(cl::Event* event)
  {
    cl_int err_aux = event->wait(); //Waits until the event is completed.
    if (err_aux != CL_SUCCESS) {
      throw HPL::HPLException(err_aux, "Exception thrown in CLbinding::waitForEvent: wait()");
    }
    cl_ulong time_begin, time_end;
    err_aux  = event->getProfilingInfo<cl_ulong>(CL_PROFILING_COMMAND_START, &time_begin);
    err_aux |= event->getProfilingInfo<cl_ulong>(CL_PROFILING_COMMAND_END, &time_end);
    
    if (err_aux != CL_SUCCESS) {
      throw HPL::HPLException(err_aux, "Exception thrown in CLbinding::waitForEvent: clGetEventProfilingInfo()");
    }
    
    return ((double)(time_end - time_begin))/1000000000.0;
  }
#endif

  /// Default platform for each kind of supported device. MAX_PLATFORMS for no platform.
  HPL::Platform_t DefaultPlatform[3] = { HPL::MAX_PLATFORMS, HPL::MAX_PLATFORMS, HPL::MAX_PLATFORMS };

  std::map<void *, cl::Buffer *> PinnedBuffers;
  
} //end namespace ""


namespace HPL {

namespace internal {

  typedef std::map<String_t, cl::Kernel*> KernelStorage_t;
  
  struct DeviceImplementation;
  
  
  /// Holds data for all the devices of the same type of a given platform
  struct DeviceTypeData {
    
    cl::Context context_;
    cl::Program program_;
    KernelStorage_t kernels_;
    std::vector<cl::Device> devices_;
    bool copyFaster;
    bool subbuff;

    unsigned int nDevices_;
    DeviceImplementation *dv_;
    
    DeviceTypeData() :
    copyFaster(false), subbuff(false), nDevices_(0), dv_(0)
    { }
    
    void fill(const std::vector<cl::Device>& vector, cl_context_properties *cps, Platform_t platform_id, Device_t devt, const String_t& platform_name);
    
    cl::Kernel* getKernel(const FHandle *fh) const { return getKernel(fh->name_); }
    
    cl::Kernel* getKernel(const String_t& func_name) const;
    
    void checkPragmasSupport() const;
    
    void buildProgram(const std::string& sources);
    
    cl::Kernel* tryNativeKernelBuild(const FHandle *fh);
    
    ~DeviceTypeData() { std::cerr << "End\n";}
  };
  
  
  /// Holds the internal data of a device
  struct DeviceImplementation : public DeviceDescriptor {

    DeviceTypeData *dtd_;
    cl::Device device_;
    cl::CommandQueue cq_;
    DeviceProperties dp_;
    std::list<InternalMultiDimArray*> arrays;
    std::set<InternalMultiDimArray*> curArrays;
    
    void fill(const cl::Device& device, DeviceTypeData *dtd, Platform_t platform_id, Device_t devt, int devn);
    void removeLRU();
    void clearCurArrays();
    bool sizesAreTheSame();
    void reloadArray(InternalMultiDimArray* ba);

  };

  
  void DeviceImplementation::fill(const cl::Device& device, DeviceTypeData *dtd, Platform_t platform_id, Device_t devt, int devn)
  { cl_int err;
    
    dtd_ = dtd;
    platform_id_ = platform_id;
    devt_ = devt;
    devn_ = devn;
    
    device_ = device;

    fillDeviceProperties(&dp_, device);
    
#ifdef HPL_PROFILE
    cq_ = cl::CommandQueue(dtd_->context_, device, CL_QUEUE_PROFILING_ENABLE, &err);
#else
    cq_ = cl::CommandQueue(dtd_->context_, device, 0, &err);
#endif
    
    if (err != CL_SUCCESS)
      throw HPLException(err, "Exception thrown in DeviceImplementation::fill: CommandQueue() failed");
  }
  
  void DeviceImplementation::removeLRU()
  {
    std::set<InternalMultiDimArray*>::iterator it_c;
    std::list<InternalMultiDimArray*>:: iterator it_a;
    if(arrays.size() == curArrays.size()) return;
    InternalMultiDimArray* ba = arrays.front();
    if((it_c = curArrays.find(ba)) == curArrays.end())
    {
      ba->getData(HPL_RDWR);

      for(it_a = arrays.begin(); it_a!=arrays.end(); ++it_a)
        if((*it_a)->getFather()==ba) {delete((*it_a)->hdm.get_dbuffer(platform_id_, devt_, devn_)); (*it_a)->hdm.add_dbuffer(platform_id_, devt_, devn_, NULL);}

      delete((ba)->hdm.get_dbuffer(platform_id_, devt_, devn_));
      (ba)->hdm.add_dbuffer(platform_id_, devt_, devn_, NULL);
      arrays.pop_front();

    }
    else throw HPL::HPLException(0, "Exception thrown in CLbinding::removeLRU: arrays and curArrays should have the same buffers at least!");
  }
  
  void DeviceImplementation::reloadArray(InternalMultiDimArray* ba)
  {
    arrays.remove(ba);
    arrays.push_back(ba);
  }

  bool DeviceImplementation::sizesAreTheSame()
  {
    return (arrays.size() == curArrays.size());
  }

  void DeviceImplementation::clearCurArrays()
  {
    curArrays.clear();
  }


  cl::Kernel* DeviceTypeData::getKernel(const String_t& func_name) const
  {
    KernelStorage_t::const_iterator it = kernels_.find(func_name);
    
    if(it == kernels_.end())
       return NULL;
       
    cl::Kernel * kernel = (*it).second;
    
    assert(kernel != 0);
    //if (!kernel)
    //  return NULL;
    
    // cl kernel should be exist too, this can happen if eval(foo) is used to get frunner without calling the kernel.
    if( *(cl_kernel *)kernel == NULL ){
      return NULL;
    }
    
    return kernel;
  }
  
  
  void DeviceTypeData::checkPragmasSupport() const
  {
    //BBF:  assumes all devices support the same extensions
    cl::STRING_CLASS extensions = devices_[0].getInfo<CL_DEVICE_EXTENSIONS>();
    
    for(int i = 0 ; i < GlobalState::PRAGMAS_NUMBER; i++) {
      if( TheGlobalState().getPragma(i) && (!strstr(extensions.c_str(), GlobalState::pragmas_text[i]))) {
	String_t aux("Exception thrown in DeviceTypeData::check_pragmas_support: Device does not support the pragma ");
	aux += GlobalState::pragmas_text[i];
	throw HPLException(0, aux.c_str());
      }
    }
  }
  
  void DeviceTypeData::buildProgram(const std::string& sources)
  { cl_int err;
    
    // first deallocate all existing kernels
    KernelStorage_t::const_iterator const it_end = kernels_.end();
    for(KernelStorage_t::const_iterator it = kernels_.begin(); it != it_end; ++it) {
      assert((*it).second != 0);
      delete [] (*it).second;
    }
    kernels_.clear();
    
    HPL_PROFILEACTION(TheGlobalState().timer.start());
    
    checkPragmasSupport();

    cl::Program::Sources tmp_sources(1, std::make_pair((const char *) sources.c_str(), sources.length()+1));
				     
    program_ = cl::Program(context_, tmp_sources, &err);
    if (err != CL_SUCCESS)
      throw HPLException(err,"Exception thrown in DeviceTypeData::build_program: Program::Program() failed");

    err = program_.build(devices_);

    HPL_PROFILEACTION(TheGlobalState().secs_kernel_compilation = TheGlobalState().timer.stop());
    HPL_PROFILEACTION(TheGlobalState().total_secs_kernel_compilation += TheGlobalState().secs_kernel_compilation);
    
    if (err != CL_SUCCESS) {
      //THE BUILD CAN RETURN A BUILD ERROR CL_INVALID_BINARY
      // BUY ONLY MEANS THAT ONE DEVICE OF DEVICES DOES NOT
      // SUPPORT DOUBLE PRECISION
      if((err == CL_BUILD_PROGRAM_FAILURE) || (err == CL_INVALID_BINARY)) {
	cl::STRING_CLASS str = program_.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices_[0]); //BBF: assumes all devices equal
	std::cout << " \n\t\t\tBUILD LOG\n";
	std::cout << " ************************************************\n";
	std::cout << str.c_str() << std::endl;
	std::cout << " ************************************************\n";
	throw HPLException(err,"Exception thrown in DeviceTypeData::build_program: Program::build() failed");
      }
      if(err != CL_INVALID_BINARY)
      {
	throw HPLException(err,"Exception thrown in DeviceTypeData::build_program: Program::build() failed");
      }
    }
    
    //The latest program has as many kernels as the fhandlestorage table entries has.
    //We will create a kernel object for each kernel present in the program.
    std::map<void *, FHandle*>::iterator it;
    it = TheGlobalState().getFStorage()->begin();
    for(int i = 0; i < TheGlobalState().getNumberOfFHandles(); i++) {
      FHandle* func = (*it).second;
      if(func->code_[0] == '_' || func->isNative()) { //identifies a kernel
	cl::Kernel * const tmp = new cl::Kernel[nDevices_];
	for (unsigned int j = 0; j < nDevices_; j++) {
	  tmp[j] = cl::Kernel(program_, func->name_.c_str(), &err); //TODO: Check this err?
	}
	kernels_[func->name_] = tmp;
      }
      it++;
    }
    
  }
  
  ///Native kernels may have put and compiled all at once in the program but they may not be
  ///declared at the compilation point, so we try to get them here just in case
  cl::Kernel* DeviceTypeData::tryNativeKernelBuild(const FHandle *fh)
  { cl_int err;
    
    cl::Kernel kern(program_, fh->name_.c_str(), &err);

    if (err == CL_SUCCESS) {
      cl::Kernel * const tmp = new cl::Kernel[nDevices_];
      tmp[0] = kern;
      for (unsigned int j = 1; j < nDevices_; j++) {
	tmp[j] = cl::Kernel(program_, fh->name_.c_str(), &err); //TODO: Check this err?
      }
      kernels_[fh->name_] = tmp;
      return tmp;
    } else {
      return NULL;
    }
    
  }

  void DeviceTypeData::fill(const std::vector<cl::Device>& vector,
                            cl_context_properties *cps,
                            Platform_t platform_id,
                            Device_t devt,
                            const String_t& platform_name)
  { cl_int err;

    devices_ = vector;
    nDevices_ = static_cast<unsigned int>(vector.size());

    if(nDevices_) {
      
      context_ = cl::Context(vector, cps, NULL, NULL, &err);
      if (err != CL_SUCCESS)
        throw HPLException(err, "Exception thrown in DeviceTypeData::fill: Context() failed");
      
      dv_ =  new DeviceImplementation[nDevices_];
      for(int j = 0; j < nDevices_; j++)
	dv_[j].fill(vector[j], this, platform_id, devt, j);
      
      if(DefaultPlatform[devt] == HPL::MAX_PLATFORMS)
	DefaultPlatform[devt] = platform_id;
      else {
	cl::STRING_CLASS device_vendor;
	vector[0].getInfo<cl::STRING_CLASS>(CL_DEVICE_VENDOR, &device_vendor);
	if(match_platform_device(platform_name, device_vendor))
	  DefaultPlatform[devt] = platform_id;
      }
    }
    
  }

  /**
   * @brief Checks whether local size reached the limit for
   * a concrete device.
   * @param[in] device Device to checking.
   */
  void check_local_size(FRunner* frunner, cl::Kernel* const kernel)
  { cl_int err;
    
    /* Get Device specific Information */
    const DeviceProperties& dp = frunner->getDevice()->dp_;
    cl::Device* const device_aux = &frunner->getDevice()->device_;
    
    cl_uint localSize = frunner->getNdLocalX() * frunner->getNdLocalY() * frunner->getNdLocalZ();
    if(localSize > dp.maxLocalDomainSize) {
      throw HPLException(0,"Exception thrown in CLbinding::check_local_size: Device does not support requested number of work items.");
    }
    
    /* Check group size against group size returned by kernel */
    
    std::size_t kernelWorkGroupSize = kernel->getWorkGroupInfo<CL_KERNEL_WORK_GROUP_SIZE>(*device_aux, &err);
    if (err != CL_SUCCESS) {
      throw HPLException(err,"Exception thrown in CLbinding::check_local_size: getInfo(CL_DEVICE_MAX_WORK_GROUP_SIZE) failed.");
    }
    if(localSize > kernelWorkGroupSize) {
      throw HPLException(0,"Exception thrown in CLbinding::check_local_size: Kernel does not support requested number of work items.");
    }
    
    unsigned long long usedLocalMemory;
    usedLocalMemory = kernel->getWorkGroupInfo<CL_KERNEL_LOCAL_MEM_SIZE>(*device_aux, &err);
    if(usedLocalMemory > dp.localMemSize) {
      throw HPLException(err,"Exception thrown in CLbinding::check_local_size: Used local memory is larger than total local memory.");
    }
    
  }

  /**
   * @brief Creates and compiles the program stored in \p allCode.
   * Furthermore, for each program created, this method also
   * creates all the kernel objects that exists in that kernel.
   */
  void build_program(Device& device, const String_t& allCode)
  {
    if( TheGlobalState().env.hpl_kernel_file_enable ) {
      // Read source file
      std::ifstream sourceFile;
      //sourceFile.open("cc.cl");
      sourceFile.open("HPL_kernels.cl");
      if( sourceFile.is_open() == false ) {
	throw HPLException(0,"Exception thrown in CLbinding::build_program: The CL source code could not be load");
      }
      
      std::string sourceCode(std::istreambuf_iterator<char>(sourceFile),
			     (std::istreambuf_iterator<char>()));
      device->dtd_->buildProgram(sourceCode);
    } else {  //use internal allCode
      device->dtd_->buildProgram(allCode);
    }
    
  }
  
  /**
   * @brief Creates the cl::Program with the kernel code already created.
   */
  cl::Kernel *program_init( FRunner * const frunner, const String_t& allCode)
  {
    HPL_PROFILEACTION(TheGlobalState().secs_kernel_compilation = 0.0);
    
    const FHandle * const fh = frunner->getFhandle();
    Device& dev = frunner->getDevice();
    DeviceTypeData* dtd = dev->dtd_;
    
    cl::Kernel * ret = dtd->getKernel(fh);

    if( ret == NULL ) {
      if(fh->isNative()) {
	ret = dtd->tryNativeKernelBuild(fh);
	if (ret != NULL) return ret + dev->devn_;
      }
      build_program(dev, allCode);
      ret = dtd->getKernel(fh);
      assert(ret != NULL);
    }
    
    return ret + dev->devn_;
  }
  
  /**
   * @brief Links the cl::Buffers created with kernel arguments.
   * @param[in] kernel Kernel that will be set.
   */
  void set_kernel_args(FRunner * const frunner, cl::Kernel * const kernel)
  {
    FHandle* const fhandle = frunner->getFhandle();
    const Platform_t platform_id = frunner->getDevice()->platform_id_;
    const Device_t    devt        = frunner->getDevice()->devt_;
    const int         devn        = frunner->getDevice()->devn_;

    cl_int err;
    // Set arguments to kernel
    int count = 0;
    for(int i = 0; i < fhandle->numArgs_; i++ ) {
    	if( frunner->frunnerTable[i].base_array == 0) { //scalars
    		err = kernel->setArg(count, frunner->frunnerTable[i].size, (void *)(frunner->frunnerTable[i].dbuffer));
    		count++;
    	}
    	else if( frunner->frunnerTable[i].base_array->flags() == Local ) {
    		err = kernel->setArg(count, frunner->frunnerTable[i].size * frunner->frunnerTable[i].base_array->getDataItemSize(), NULL);
    		count++;
    	}
    	else {
    		if( frunner->frunnerTable[i].base_array->dbuffer_ == NULL )
    		{
    			err = kernel->setArg(count, *frunner->frunnerTable[i].dbuffer);

    		}
    		else
    		{
    			err = kernel->setArg(count, *(frunner->frunnerTable[i].base_array->dbuffer_));
    		}
    		count++;
    	}
      if (err != CL_SUCCESS) {
	throw HPLException(err,"Exception thrown in CLbinding::set_kernel_args: setArg() failed");
      }
      
      if(frunner->frunnerTable[i].base_array == 0 ) continue;  // No size needed for scalars
      if(fhandle->isNative()) continue; // No size expected in native kernels
      if(frunner->frunnerTable[i].base_array->getNumberOfDimensions() == 1 ) continue;  // No size needed
      
      if(frunner->frunnerTable[i].base_array->getNumberOfDimensions() == 2 ) {
    	  //////////////////////////////////////////CHILDREN_BEGIN_CODE////////////////////////////////////////////
    	  /////////////////////////////////////////IF IT IS A SON, THE WIDTH IS THE SAME OF FATHER/////////////////
    	  if(frunner->frunnerTable[i].base_array->getFather()!=NULL && frunner->frunnerTable[i].base_array->getFather()->hdm.get_dbuffer(platform_id, devt, devn) !=NULL)
//    	  if(TheGlobalState().clbinding().allow_subbuffer(platform_id,devt) && frunner->frunnerTable[i].base_array->getFather()!=NULL && frunner->frunnerTable[i].base_array->getFather()->hdm.get_dbuffer(platform_id, devt, devn) !=NULL)
    	  {
    		  err = kernel->setArg(count, frunner->frunnerTable[i].base_array->getTX().end);  count++;
    	  }
    	  else
    	  {
    		  err = kernel->setArg(count, frunner->frunnerTable[i].base_array->getDimension(1));  count++;
    	  }
    	  //////////////////////////////////////////CHILDREN_END_CODE//////////////////////////////////////////////

      }
      else if(frunner->frunnerTable[i].base_array->getNumberOfDimensions() == 3 ) {
    	  if(( frunner->frunnerTable[i].base_array->getTX().end != 0) || (frunner->frunnerTable[i].base_array->getTY().end != 0))
    	  {
    		  //err = kernel->setArg(count, frunner->frunnerTable[i].base_array->getTX().end * frunner->frunnerTable[i].base_array->getTY().end);  count++;
    		  err = kernel->setArg(count, frunner->frunnerTable[i].base_array->getXYZDim(0) * frunner->frunnerTable[i].base_array->getXYZDim(1)); count++;
    	  }
    	  else
    	  {
    		  err = kernel->setArg(count, frunner->frunnerTable[i].base_array->getDimension(1)*frunner->frunnerTable[i].base_array->getDimension(2));  count++;
    	  }
    	  //err = kernel->setArg(count, frunner->frunnerTable[i].base_array->getDimension(1));  count++;
    	  if(( frunner->frunnerTable[i].base_array->getTX().end != 0) || (frunner->frunnerTable[i].base_array->getTY().end != 0))
    	  {
    		  err = kernel->setArg(count, frunner->frunnerTable[i].base_array->getXYZDim(0));  count++;
    	  }
    	  else
    	  {
    		  err = kernel->setArg(count, frunner->frunnerTable[i].base_array->getDimension(2));  count++;
    	  }
    	  //err = kernel->setArg(count, frunner->frunnerTable[i].base_array->getDimension(2));  count++;
      }
      else
	throw HPLException(err,"Exception thrown in CLbinding::set_kernel_args: setArg(): Can not handle more than 3 dimensional Array YET!");
      
      if (err != CL_SUCCESS) {
	throw HPLException(err, "Exception thrown in CLbinding::set_kernel_args: setArg() failed");
      }
      
    }
  }
  
  void set_deallocate_info(FRunner * const frunner, cl::Kernel * const kernel)
  {
    FHandle* const fhandle = frunner->getFhandle();
    const Platform_t platform_id = frunner->getDevice()->platform_id_;
    const Device_t    devt        = frunner->getDevice()->devt_;
    const int         devn        = frunner->getDevice()->devn_;

    // Set arguments to kernel
    int count = 0;
    for(int i = 0; i < fhandle->numArgs_; i++ ) {
     if(frunner->frunnerTable[i].base_array)
      if( frunner->frunnerTable[i].base_array->flags() == Global ) {
       	frunner->getDevice()->curArrays.insert(frunner->frunnerTable[i].base_array);
        frunner->getDevice()->reloadArray(frunner->frunnerTable[i].base_array);
        if(frunner->frunnerTable[i].base_array->getFather()!=NULL && frunner->frunnerTable[i].base_array->getFather()->hdm.get_dbuffer(platform_id, devt, devn) !=NULL)
        {
                frunner->getDevice()->curArrays.insert(frunner->frunnerTable[i].base_array->getFather());
                frunner->getDevice()->reloadArray(frunner->frunnerTable[i].base_array->getFather());
        }
      }
    }
  }


  /**
   * @brief Enqueue the kernel to executing on the selected device.
   * @param[in] frunner  Execution environment.
   * @param[in] kernel   Opencl kernel to run
   * @param[in] dochecks Whether legality checks should be performed
   */
  void run_kernel(FRunner* const frunner, cl::Kernel* const kernel, bool dochecks)
  { HPL_PROFILEDEFINITION(cl::Event event); // TODO: Can this be static and reused?
    cl::Event *p_event = NULL;
    HPL_PROFILEACTION(p_event = &event);
    
    cl::CommandQueue* const queue_aux = &frunner->getDevice()->cq_;
    
    if (dochecks) {
      internal::check_local_size(frunner, kernel);
    }
    
    cl::NDRange global(frunner->getNdGlobalX(), frunner->getNdGlobalY(), frunner->getNdGlobalZ());
    cl::NDRange local = frunner->getNdLocalX() ? cl::NDRange(frunner->getNdLocalX(), frunner->getNdLocalY(), frunner->getNdLocalZ()) : cl::NullRange;

    cl_int err = queue_aux->enqueueNDRangeKernel(*kernel, cl::NullRange, global, local, NULL, p_event);
    
    if (err != CL_SUCCESS) {
      if(err == CL_MEM_OBJECT_ALLOCATION_FAILURE)
      {
        while(1) {
          frunner->getDevice()->removeLRU();
          err = queue_aux->enqueueNDRangeKernel(*kernel, cl::NullRange, global, local, NULL, p_event);
          if (err != CL_SUCCESS) {
            if(frunner->getDevice()->sizesAreTheSame())
              throw HPLException(err, "Exception thrown in CLbinding::run_kernel: enqueueNDRangeKernel() failed (memory allocation problem)");
          }
          else break;
        }
        frunner->getDevice()->clearCurArrays();
      }
      else
        throw HPLException(err, "Exception thrown in CLbinding::run_kernel: enqueueNDRangeKernel() failed (parameter mismatch)");
    }
    else
      frunner->getDevice()->clearCurArrays();


    HPL_PROFILEACTION(TheGlobalState().secs_device_execution = waitForEvent(&event));
    HPL_PROFILEACTION(TheGlobalState().total_secs_device_execution += TheGlobalState().secs_device_execution);
  }
  
} //end namespace internal
} //end namespace HPL


namespace {
  
  /** @brief    Holder of all the internal data for all the devices
      @internal This was a "HPL::internal::DeviceTypeData Devices[HPL::MAX_PLATFORMS][HPL::MAX_DEVICE_TYPES];"
		but it turned out that the array was initialized AFTER CLbinding::platform_init() was invoked,
                therefore erasing all the devices. Thus I had to change to a delayed initialization scheme 
                based on a pointer */
  HPL::internal::DeviceTypeData (*Devices)[HPL::MAX_DEVICE_TYPES] = nullptr;
  
} //end namespace ""

namespace HPL {
  
CLbinding::CLbinding()
{
	// This is to open text file
	if( getenv("HPL_KERNEL_FILE_ENABLE") != NULL ) {
		kernelfile.open("HPL_kernels.cl");
		if(kernelfile.is_open() == false ) {
			throw HPLException(0,"Exception thrown in CLbinding::CLbinding(): Unable to open for kernelfile writing");
		}
	}

	allCode = "";
	// Here we create de OpenCL platform.
	try {
		platform_init();
	} catch(HPLException o){
		std::cout << o.what() << std::endl;
		exit(1);
	}

}

CLbinding::~CLbinding()
{
  if( !PinnedBuffers.empty() ) {
    internal::DeviceImplementation *di = getDevice(DefaultPlatform[static_cast<int>(GPU)], GPU, 0);
    for (auto it = PinnedBuffers.begin(); it != PinnedBuffers.end(); ++it) {
      cl_int err = di->cq_.enqueueUnmapMemObject(*((*it).second), (*it).first);
      assert(err == CL_SUCCESS);
      
      delete (*it).second;
    }
    PinnedBuffers.clear();
  }
}
  
internal::DeviceImplementation *CLbinding::defaultDevice(Device_t devt, int device_n) const
{
  HPLExceptionIf(DefaultPlatform[devt] == HPL::MAX_PLATFORMS, "There are no devices of the requested type");
  return getDevice(DefaultPlatform[devt], devt, device_n);
}

internal::DeviceImplementation *CLbinding::defaultDevice() const
{ Device_t t;

  if( DefaultPlatform[GPU] != HPL::MAX_PLATFORMS ) t = GPU;
  else if( DefaultPlatform[ACCELERATOR] != HPL::MAX_PLATFORMS ) t = ACCELERATOR;
  else t = CPU;

  return defaultDevice(t, 0);
}

internal::DeviceImplementation *CLbinding::getDevice(Platform_t platform_id, Device_t devt, int device_n) const
{
  HPLExceptionIf(Devices[platform_id][devt].nDevices_ <= device_n, "The requested device is not available");
  return Devices[platform_id][devt].dv_ + device_n;
}

void CLbinding::getDeviceProperties(const internal::DeviceImplementation *p, DeviceProperties& dp) const
{
  dp = p->dp_;
}

cl::CommandQueue *CLbinding::getCommandQueue(Platform_t platform_id, Device_t devt, int device_n) const
{
        return &Devices[platform_id][devt].dv_[device_n].cq_;
}

bool CLbinding::get_copy_faster(Platform_t platform_id, Device_t device_type)
{
	return Devices[platform_id][device_type].copyFaster;
}

bool CLbinding::allow_subbuffer(Platform_t platform_id, Device_t device_type)
{
    return Devices[platform_id][device_type].subbuff;
}

void CLbinding::platform_init()
{
  cl_int err, err_aux, err_aux_2;
  
  if( Devices ) return;
  
  Devices = (internal::DeviceTypeData (*) [HPL::MAX_DEVICE_TYPES]) new internal::DeviceTypeData [HPL::MAX_PLATFORMS * HPL::MAX_DEVICE_TYPES];
  
  // Get available platforms
  std::vector<cl::Platform> platforms;
  
  HPLSafeMsg(cl::Platform::get(&platforms),
	     "Exception thrown in CLbinding::platform_init: Platform::get() failed");
  
  std::vector<cl::Device> CPU_devices_aux, GPU_devices_aux, ACCELERATOR_devices_aux;
  
  cl::STRING_CLASS platform_name;
  
  for(int i = 0; i < platforms.size(); i++) {

    // Select the default platform and create a context using this platform and the GPU
    CPU_devices_aux.clear();
    GPU_devices_aux.clear();
    ACCELERATOR_devices_aux.clear();
    
    cl_context_properties cps[3] = {
      CL_CONTEXT_PLATFORM,
      (cl_context_properties)(platforms[i])(),
      0
    };
    
    platforms[i].getInfo(CL_PLATFORM_NAME, &platform_name);
    Platform_t platform_id = string_to_platform_id(platform_name);
    
    err = platforms[i].getDevices(CL_DEVICE_TYPE_CPU, &CPU_devices_aux);
    err_aux = platforms[i].getDevices(CL_DEVICE_TYPE_GPU, &GPU_devices_aux);
    err_aux_2 = platforms[i].getDevices(CL_DEVICE_TYPE_ACCELERATOR, &ACCELERATOR_devices_aux);
    
    if((err != CL_SUCCESS) && (err_aux != CL_SUCCESS) && (err_aux_2 != CL_SUCCESS))
    {
      throw HPLException(err, "Exception thrown in CLbinding::platform_init: Platform::getDevices() No device available");
    }
    
    Devices[platform_id][CPU].fill(CPU_devices_aux, cps, platform_id, CPU, platform_name);
    Devices[platform_id][GPU].fill(GPU_devices_aux, cps, platform_id, GPU, platform_name);
    Devices[platform_id][ACCELERATOR].fill(ACCELERATOR_devices_aux, cps, platform_id, ACCELERATOR, platform_name);
  }
  
  const char *hpl_config_file_name = getenv("HPL_CONFIG_FILE");
  if (hpl_config_file_name != NULL) {
    std::ifstream file(hpl_config_file_name);
    int dt, pt;
    char c;
    
    while (file >> dt >> pt >> c) {
      LOG("Read config: " << dt << ' ' << pt << c);
      Devices[pt][dt].copyFaster = (c == 'C');
      Devices[pt][dt].subbuff = (c == 'S');
    }
  }
  
}

  

// This routine goes throug argument list, if the same basearray is used multiple times, marked them as alias
void CLbinding::handle_alias(FRunner * const frunner)
{
    FHandle* fhandle = frunner->getFhandle();
    
    for(int i = 0; i < fhandle->numArgs_; i++ ) 
      frunner->frunnerTable[i].isAlias = false;
    
    for(int i = 0; i < fhandle->numArgs_; i++ ) 
      if( frunner->frunnerTable[i].base_array ) { //skip scalars
	for(int j = i + 1;  j < fhandle->numArgs_; j++ ) 
	  if( frunner->frunnerTable[i].base_array == frunner->frunnerTable[j].base_array )  {
	    frunner->frunnerTable[i].isAlias = true;
	    frunner->frunnerTable[j].isAlias = true;
	  }		 
      }
  
}

void CLbinding::create_buffers(FRunner * const frunner)
{
	const int numargs =  frunner->getFhandle()->numArgs_;

	const Platform_t platform_id = frunner->getDevice()->platform_id_;
	const Device_t    devt        = frunner->getDevice()->devt_;
	const int         devn        = frunner->getDevice()->devn_;

	for(int i = 0; i < numargs; i++ ) {
		if( frunner->frunnerTable[i].base_array ) { //skip scalars
			if( frunner->frunnerTable[i].base_array->dbuffer_ != NULL ) continue; // Manually hadled copy_in/copy_out
			if( frunner->frunnerTable[i].base_array->hdm.get_dbuffer(platform_id, devt, devn) !=NULL ) continue;  // reuse the buffer
			/////////////////////////////////////////////////////////////////////////////////////CHILDREN_BEGIN_CODE//////////////////////////////////////////////////////////////////////////////////
			/////////////////////////////////////IF IT HAS FATHER AND THE FATHER HAS BUFFER IN THIS DEVICE WE CREATE A SUBBUFFER//////////////////////////////////////////////////////////////////////
			if( frunner->frunnerTable[i].base_array->getFather()!=NULL && frunner->frunnerTable[i].base_array->getFather()->hdm.get_dbuffer(platform_id, devt, devn) !=NULL )
//			if(allow_subbuffer(platform_id,devt) && frunner->frunnerTable[i].base_array->getFather()!=NULL && frunner->frunnerTable[i].base_array->getFather()->hdm.get_dbuffer(platform_id, devt, devn) !=NULL )
			{
				frunner->frunnerTable[i].dbuffer = allocate_dsubbuffer(platform_id, devt, devn, frunner->frunnerTable[i].base_array);
			}
			else
			{
				if(frunner->frunnerTable[i].base_array->getFather()!=NULL && frunner->frunnerTable[i].base_array->getFather()->hdm.get_dbuffer(platform_id, devt, devn) !=NULL)
				{
					frunner->frunnerTable[i].base_array->getFather()->getData(HPL_RDWR);
					frunner->frunnerTable[i].base_array->getFather()->hdm.invalidate(platform_id, devt, devn);
				}

				frunner->frunnerTable[i].dbuffer = allocate_dbuffer(platform_id, devt, devn, frunner->frunnerTable[i].base_array);
			}
			////////////////////////////////////////////////////////////////////////////////////CHILDREN_END_CODE/////////////////////////////////////////////////////////////////////////////////////
//			frunner->frunnerTable[i].base_array->invalidSon(platform_id, devt, devn);

			frunner->frunnerTable[i].base_array->hdm.add_dbuffer(platform_id, devt, devn, frunner->frunnerTable[i].dbuffer);
		}
	}

}
  
//void CLbinding::store_arglistfordevice(FRunner *frunner)
//{
//	// Save the last frunner for device.sync();
////	if( is_async == TRUE ) {
//		KeyProgramMap kp = KeyProgramMap(frunner->getPlatformId(),frunner->getDeviceType(),frunner->getDeviceNumber());
//		if(arglistfordevice[kp] != NULL) {
//			throw HPLException(0, "Exception thrown in CLbinding::copy_to_buffers: two kernels are called without device.synch() this feature not yet implemented");
//		}
//		ArgListForDevice *arglist = new ArgListForDevice();
//		for(int i = 0; i < nargs; i++ ) {
//			arglist->fhandleTable[i].base_array = fhandleTable[i].base_array;
//		}
//		arglist->nargs = nargs;
//		arglistfordevice[kp] = arglist;
////	}
//}
  
void CLbinding::copy_to_buffers(FRunner * const frunner)
{

	handle_alias(frunner);  // this will mark aliased for argument list
	cl::CommandQueue* const queue_aux = &frunner->getDevice()->cq_;
	FHandle* fhandle = frunner->getFhandle();

	const Platform_t platform_id = frunner->getDevice()->platform_id_;
	const Device_t    devt        = frunner->getDevice()->devt_;
	const int         devn        = frunner->getDevice()->devn_;

	for(int i = 0; i < fhandle->numArgs_; i++ ) {
		if( frunner->frunnerTable[i].base_array == 0) continue; //skip scalars
		if( frunner->frunnerTable[i].base_array->dbuffer_ != NULL ) continue; // Manual copy_in is used
		if( frunner->frunnerTable[i].base_array->flags() == Local ) continue;    // Local is a just place holder for actual arg

		frunner->frunnerTable[i].base_array->hdm.store_argIntend(fhandle->fhandleTable[i].isWrite, fhandle->fhandleTable[i].isWriteBeforeRead);  // store Intend info for arg to HDM

		if(frunner->frunnerTable[i].isAlias) frunner->frunnerTable[i].base_array->hdm.set_argIntend(HPL_ARG_INTEND_INOUT);  // alias basearray indent is always INOUT for HDM
		frunner->frunnerTable[i].base_array->hdm.set_device(platform_id, devt, devn);  // store frunner to HDM

		frunner->frunnerTable[i].dbuffer = frunner->frunnerTable[i].base_array->hdm.get_dbuffer(platform_id, devt, devn);
		if(frunner->frunnerTable[i].base_array->hdm.get_argIntend() == HPL_ARG_INTEND_OUT)
			frunner->frunnerTable[i].base_array->onlyWrite(platform_id, devt, devn, queue_aux);
		if(frunner->frunnerTable[i].base_array->hdm.get_argIntend() == HPL_ARG_INTEND_INOUT)
			frunner->frunnerTable[i].base_array->readWrite(platform_id, devt, devn, queue_aux);
		if(frunner->frunnerTable[i].base_array->hdm.get_argIntend() == HPL_ARG_INTEND_IN)
			frunner->frunnerTable[i].base_array->onlyRead(platform_id, devt, devn, queue_aux);
	}
}

void CLbinding::readWrite(InternalMultiDimArray* array, Platform_t platform_id, Device_t device_type, int device_n)
{
 array->readWrite(platform_id, device_type, device_n, &Devices[platform_id][device_type].dv_[device_n].cq_);
}

void CLbinding::enqueueMap(Platform_t platform_id, Device_t device_type, int device_n, InternalMultiDimArray* base_array)
{
  if(TheGlobalState().env.hpl_unifiedmemory_enable && Device(platform_id,device_type,device_n)->dp_.unifiedMemory==1)
  {
    cl::Buffer *dbuffer = base_array->hdm.get_dbuffer(platform_id,device_type, device_n);
    cl::CommandQueue* queue_aux = &Devices[platform_id][device_type].dv_[device_n].cq_;

    base_array->getData(HPL_NONE);

    int length = base_array->getSize();

    cl_int error;
    if(dbuffer == NULL)
	return;
    else 
    {
      queue_aux->enqueueMapBuffer( *dbuffer, CL_TRUE, CL_MAP_READ | CL_MAP_WRITE, 0, length * base_array->getDataItemSize(), NULL, NULL, &error);
      if (error != CL_SUCCESS) {
        throw HPLException(error,"Exception thrown in CLbinding::enqueueMap failed");
      }

    }
  }
  return;
}


//  void CLbinding::set_kernel_args_lite(FRunner *frunner)
//  {
//
//	  FHandle* fhandle = frunner->getFhandle();
//	  Kernel* kernel = fhandle->get_kernel(frunner->getPlatformId(),frunner->getDeviceType(), frunner->getDeviceNumber());
//	  cl_int err = CL_SUCCESS;
//	  // Set arguments to kernel
//	  int count = 0;
//	  for(int i = 0; i < fhandle->numArgs_; i++ ) {
//		  if( frunner->frunnerTable[i].base_array == 0 ) { //scalar
//			  err = kernel->setArg(count, frunner->frunnerTable[i].size, (void *)(frunner->frunnerTable[i].dbuffer));
//			  count++;
//		  }
//		  else {
//			  count++;
//			  if(frunner->frunnerTable[i].base_array->getNumberOfDimensions() == 2 ) {
//				  err = kernel->setArg(count, frunner->frunnerTable[i].base_array->getDimension(1));
//				  count++;
//			  }
//			  else if(frunner->frunnerTable[i].base_array->getNumberOfDimensions() == 3 ) {
//				  err = kernel->setArg(count, frunner->frunnerTable[i].base_array->getDimension(1));
//				  count++;
//				  err = kernel->setArg(count, frunner->frunnerTable[i].base_array->getDimension(2));
//				  count++;
//			  }
//		  }
//		  if (err != CL_SUCCESS) {
//			  throw HPLException(err,"Exception thrown in CLbinding::set_kernel_args: setArg() failed");
//		  }
//
//	  }
//
//  };

void CLbinding::sync(internal::DeviceImplementation *device)
{
  HPLSafeMsg(clFinish((device->cq_)()), "Exception thrown in CLbinding::sync: clFinish() failed");
}

void CLbinding::copy_from_buffer_to_basearray(Platform_t platform_id, Device_t device_type, int device, InternalMultiDimArray *base_array )
{

	cl::Buffer *dbuffer = base_array->hdm.get_dbuffer(platform_id,device_type, device);
	cl::CommandQueue* queue_aux = &Devices[platform_id][device_type].dv_[device].cq_;

	///////////////////////////////////////////////////////////////CHILDREN_BEGIN_CODE//////////////////////////////////////////////
	///////////////////////////////////////////////////////////////IF THE FATHER HAS BUFFER, COPY TO IT/////////////////////////////
	if(base_array->getFather()!=NULL && base_array->getFather()->hdm.get_dbuffer(platform_id, device_type, device) !=NULL)
//	if(allow_subbuffer(platform_id,device_type) && base_array->getFather()!=NULL && base_array->getFather()->hdm.get_dbuffer(platform_id, device_type, device) !=NULL)
	{
		copy_out_s( queue_aux, base_array, base_array->getFather()->hdm.get_dbuffer(platform_id, device_type, device));
	}
	else
	{
		copy_out(queue_aux, base_array, dbuffer,platform_id, device_type,device);
	}

}

void CLbinding::copy_from_buffer_to_basearray_range_offset(Platform_t platform_id, Device_t device_type, int device,
                  const InternalMultiDimArray *base_array,
                  int ox, int oy, int oz, int lx, int ly, int lz,
                  int host_row_bytes, int host_slice_bytes, int row_bytes, int slice_bytes, void* rango)
{
          cl::Buffer *dbuffer;
          cl::size_t<3> origin;
  HPL_PROFILEDEFINITION(cl::Event event);
  cl::Event *p_event = NULL;
  HPL_PROFILEACTION(p_event = &event);

          if((base_array->getFather()!= NULL) && (base_array->getFather()->hdm.get_dbuffer(platform_id, device_type, device) !=NULL))
          //if(allow_subbuffer(platform_id,device_type) && (base_array->getFather()!= NULL) && (base_array->getFather()->hdm.get_dbuffer(platform_id, device_type, device) !=NULL))
          {
                  origin[0] = (base_array->getTX().origin + ox)*base_array->getDataItemSize();
                  origin[1] = (base_array->getTY().origin + oy);
                  origin[2] = (base_array->getTZ().origin + oz);
                  dbuffer = base_array->getFather()->hdm.get_dbuffer(platform_id, device_type, device);
          }
          else
          {
                  origin[0] = ox*base_array->getDataItemSize() ;
                  origin[1] = oy;
                  origin[2] = oz;
                  dbuffer = base_array->hdm.get_dbuffer(platform_id, device_type, device);
				  //MVB : THE WIDTH OF A ONE DIMENSIONAL SON IS NOT EQUAL TO THE
				  // FATHER'S ONE
                  if(base_array->getNumberOfDimensions() == 1)
                  {
                	  row_bytes = host_row_bytes;
                	  slice_bytes = host_slice_bytes;
                  }
          }


	  cl::CommandQueue* queue_aux = &Devices[platform_id][device_type].dv_[device].cq_;

	  cl::size_t<3> length;
	  length[0] = lx*base_array->getDataItemSize() ;	//Width in bytes
	  length[1] = ly; 			                //Height in rows
	  length[2] = lz;					//Depth in slices

	  HPL_PROFILEDEFINITION(double GPU_to_CPU_counter);
	  cl_int err = CL_SUCCESS;

//	  err = queue_aux->enqueueReadBufferRect(*dbuffer, CL_TRUE, origin, origin, length, row_bytes,
//			  slice_bytes, host_row_bytes, /*host_slice_bytes*/((base_array->getNumberOfDimensions() != 3)?host_slice_bytes : length[1]*host_row_bytes), rango, NULL, &event);

  const size_t buffer_row_bytes   = base_array->getXYZDim(0)*base_array->getDataItemSize();
  const size_t buffer_slice_bytes = base_array->getXYZDim(0) * base_array->getXYZDim(1) * base_array->getDataItemSize();
  /*
  std::cout << "RDBUF Offset:["<< origin[0] << ' ' << origin[1] << ' ' << origin[2] << "] BF+HOST " << rango << std::endl;
  std::cout << "      Region:["<< length[0] << ' ' << length[1] << ' ' << length[2] << "]\n";
  std::cout << " Buf_row_bytes =" << buffer_row_bytes; //<< " Buf_slice_bytes =" << buffer_slice_bytes << std::endl;
  std::cout << " Host_row_bytes=" << host_row_bytes    //<< " Host_slice_bytes=" << host_slice_bytes
                                 << std::endl;
  */
	  err = queue_aux->enqueueReadBufferRect(*dbuffer, CL_TRUE, origin, origin, length,
                                                 buffer_row_bytes, buffer_slice_bytes,
                                                 host_row_bytes, host_slice_bytes, rango, NULL, p_event);

	  HPL_PROFILEACTION(GPU_to_CPU_counter = waitForEvent(&event));
	  HPL_PROFILEACTION(TheGlobalState().secs_data_GPU_to_CPU += GPU_to_CPU_counter);
	  HPL_PROFILEACTION(TheGlobalState().total_secs_data_GPU_to_CPU += GPU_to_CPU_counter);

	  if (err != CL_SUCCESS) {
		  throw HPLException(err, "Exception thrown in CLbinding::copy_from_buffer_to_basearray_range_offset: failed");

	  }
}

void copy_src2(Array<unsigned int,1> xin,
                Array<unsigned int,1> xout,
                Int inoffset, Int outoffset)
{
        xout[idx + outoffset] = xin[idx + inoffset];
}

void CLbinding::enqueue_copy(Platform_t dst_platform_id, Device_t dst_device_type, int dst_device,
                               	Platform_t src_platform_id, Device_t src_device_type, int src_device, const InternalMultiDimArray *dst,
                               	const InternalMultiDimArray *src, int dst_offset, int src_offset, int bytes)
{
	if(dst_device_type==src_device_type && dst_device == src_device)
	{
		//////////////////TODO (getFather())
	        cl::Buffer *dbuffer_dst = dst->getFather()->hdm.get_dbuffer(dst_platform_id, dst_device_type, dst_device);
	        cl::Buffer *dbuffer_src = src->getFather()->hdm.get_dbuffer(src_platform_id, src_device_type, src_device);

	        FRunner fr = eval(copy_src2).device(Device(dst_platform_id, dst_device_type, dst_device));

	        cl::Kernel * kernel1 = setup_and_notrun(&fr);

	        cl::NDRange global(bytes/sizeof(unsigned int), 1, 1);

	        cl_int err = kernel1->setArg(0, sizeof(cl::Buffer), (void *)(dbuffer_src));
	        err |= kernel1->setArg(1, sizeof(cl::Buffer), (void *)(dbuffer_dst));
	        int offset_indouble =  static_cast<int>(src_offset *(src->getDataItemSize()/sizeof(unsigned int)));
	        int offset_outdouble =  static_cast<int>(dst_offset *(dst->getDataItemSize()/sizeof(unsigned int)));

	        err |= kernel1->setArg(2, sizeof(unsigned int), &offset_indouble);
	        err |= kernel1->setArg(3, sizeof(unsigned int), &offset_outdouble);

	        if (err != CL_SUCCESS) {
	        	throw HPLException(err, "Exception thrown in CLbinding::enqueue_copy: failed");

	        }

	        err =  Devices[dst_platform_id][dst_device_type].dv_[dst_device].cq_.enqueueNDRangeKernel(*kernel1, cl::NullRange, global, cl::NullRange);
	        if (err != CL_SUCCESS)
	        	throw HPLException(err, "Exception thrown in CLbinding::enqueue_copy: failed");
	}
	else
	{
		//////////////////TODO (getFather())
	        cl::CommandQueue* queue_aux = &Devices[dst_platform_id][dst_device_type].dv_[dst_device].cq_;
	        cl::Buffer *dbuffer_dst = dst/*->getFather()*/->hdm.get_dbuffer(dst_platform_id, dst_device_type, dst_device);
	        cl::Buffer *dbuffer_src = src/*->getFather()*/->hdm.get_dbuffer(src_platform_id, src_device_type, src_device);

	        //Devices[src_platform_id][src_device_type].dv_[src_device].cq_.enqueueMapBuffer(*dbuffer_src, CL_TRUE, CL_MAP_READ, 0, 4);
	        Devices[src_platform_id][src_device_type].dv_[src_device].cq_.finish();

	        cl_int err = queue_aux->enqueueCopyBuffer(*dbuffer_src, *dbuffer_dst, src_offset*src->getDataItemSize(), dst_offset*src->getDataItemSize(), bytes);

	        if (err != CL_SUCCESS)
	        	throw HPLException(err, "Exception thrown in CLbinding::enqueue_copy: failed");

	}
}


void CLbinding::copy_from_buffer_to_basearray_range(Platform_t platform_id, Device_t device_type,
		  int device, const InternalMultiDimArray *base_array, int ox, int oy, int oz, int lx, int ly, int lz, int row_bytes,
		  int slice_bytes, void* rango)
{
          cl::Buffer *dbuffer;
          cl::size_t<3> origin;
          HPL_PROFILEDEFINITION(cl::Event event);
          cl::Event *p_event = NULL;
          HPL_PROFILEACTION(p_event = &event);
  
          if((base_array->getFather()!=NULL) && (base_array->getFather()->hdm.get_dbuffer(platform_id, device_type, device) !=NULL))
          //if(allow_subbuffer(platform_id,device_type) && (base_array->getFather()!=NULL) && (base_array->getFather()->hdm.get_dbuffer(platform_id, device_type, device) !=NULL))
          {
                  origin[0] = ox*base_array->getDataItemSize() ;
                  origin[1] = oy;
                  origin[2] = oz;
                  dbuffer = base_array->getFather()->hdm.get_dbuffer(platform_id, device_type, device);
          }
          else
          {
                  origin[0] = 0;
                  origin[1] = 0;
                  origin[2] = 0;
                  dbuffer = base_array->hdm.get_dbuffer(platform_id,device_type, device);
          }


	  cl::CommandQueue* queue_aux = &Devices[platform_id][device_type].dv_[device].cq_;

	  cl::size_t<3> length;
	  length[0] = lx*base_array->getDataItemSize() ;	//Width in bytes
	  length[1] = ly; 			                //Height in rows
	  length[2] = lz;					//Depth in slices

	  cl_int err = CL_SUCCESS;
	  HPL_PROFILEDEFINITION(double GPU_to_CPU_counter);
  
      if((base_array->getFather()!=NULL) && (base_array->getFather()->hdm.get_dbuffer(platform_id, device_type, device) !=NULL))
              err = queue_aux->enqueueReadBufferRect(*dbuffer, CL_TRUE, origin, origin, length, row_bytes, slice_bytes,
            		  row_bytes, slice_bytes, rango, NULL, p_event);
      else
              err = queue_aux->enqueueReadBufferRect(*dbuffer, CL_TRUE, origin, origin, length, length[0],
                      length[0]*length[1], row_bytes, slice_bytes, rango, NULL, p_event);
  
	  HPL_PROFILEACTION(GPU_to_CPU_counter = waitForEvent(&event));
	  HPL_PROFILEACTION(TheGlobalState().secs_data_GPU_to_CPU += GPU_to_CPU_counter);
	  HPL_PROFILEACTION(TheGlobalState().total_secs_data_GPU_to_CPU += GPU_to_CPU_counter);

	  if (err != CL_SUCCESS) {
		  throw HPLException(err, "Exception thrown in CLbinding::copy_from_buffer_to_basearray_range: failed");

	  }
}

void CLbinding::copy_to_buffer_from_basearray_range_offset(Platform_t platform_id, Device_t device_type,
                  int device, InternalMultiDimArray *base_array,
                  int ox, int oy, int oz, int lx, int ly, int lz,
                  int row_bytes, int slice_bytes, void* rango)
{
	  HPL_PROFILEDEFINITION(cl::Event event);
          cl::Event *p_event = NULL;
          HPL_PROFILEACTION(p_event = &event);
  
	  cl::Buffer *dbuffer = base_array->hdm.get_dbuffer(platform_id,device_type, device);

          cl::CommandQueue* queue_aux = &Devices[platform_id][device_type].dv_[device].cq_;

          cl::size_t<3> origin;
          if(base_array->getFather()->hdm.get_dbuffer(platform_id, device_type, device) !=NULL)
          //if(allow_subbuffer(platform_id,device_type) && base_array->getFather()->hdm.get_dbuffer(platform_id, device_type, device) !=NULL)
          {
        	  origin[0] = (base_array->getTX().origin + ox)*base_array->getDataItemSize();
        	  origin[1] = (base_array->getTY().origin + oy);
        	  origin[2] = (base_array->getTZ().origin + oz);
        	  dbuffer = base_array->getFather()->hdm.get_dbuffer(platform_id,device_type, device);
          }
          else
          {
        	  origin[0] = ox*base_array->getDataItemSize();
        	  origin[1] = oy;
        	  origin[2] = oz;
			  //MVB : THE WIDTH OF A ONE DIMENSIONAL SON IS NOT EQUAL TO THE
			  // FATHER'S ONE
        	  if(base_array->getNumberOfDimensions() == 1)
        		  row_bytes = slice_bytes = 0;
          }
  
          assert( dbuffer != nullptr );
  
          cl::size_t<3> length;
          length[0] = lx*base_array->getDataItemSize() ;        //Width in bytes
          length[1] = ly;                                       //Height in rows
          length[2] = lz;          //Depth in slices
    	  cl::size_t<3> cero;
    	  cero[0] = 0;
    	  cero[1] = 0;
    	  cero[2] = 0;

          HPL_PROFILEDEFINITION(double GPU_to_CPU_counter);
  const size_t buffer_row_bytes   = base_array->getXYZDim(0)*base_array->getDataItemSize();
  const size_t buffer_slice_bytes = base_array->getXYZDim(0) * base_array->getXYZDim(1) * base_array->getDataItemSize();
  
  /*
  std::cout << "WRBUF Offset:["<< origin[0] << ' ' << origin[1] << ' ' << origin[2] << "] BF. HOST=0 " << rango <<  ' ' << base_array << ' ' << dbuffer << std::endl;
  std::cout << "      Region:["<< length[0] << ' ' << length[1] << ' ' << length[2] << "]\n";
  std::cout << " Buf_row_bytes =" << buffer_row_bytes; // << " Buf_slice_bytes =" << buffer_slice_bytes << std::endl;
  std::cout << " Host_row_bytes=" << length[0] //<< " Host_slice_bytes=" << length[1]*length[0]
                                 << std::endl;
  */
  
          cl_int err = queue_aux->enqueueWriteBufferRect(*dbuffer, CL_TRUE, origin, cero, length,
                                                         buffer_row_bytes, buffer_slice_bytes,
                                                         length[0], length[1]*length[0], rango,
                                                         NULL, p_event);

  /*** DEBUGGING CODE ****
  if (origin[1]) {
    if (err != CL_SUCCESS) {
      throw HPLException(err, "Exception thrown in CLbinding::copy_to_buffer_to_basearray_range_offset: failed");
    }
    //origin[1]--;
    origin[1] = 0;
    std::cout << "DBRD Offset:["<< origin[0] << ' ' << origin[1] << ' ' << origin[2] << "] BF. HOST=0 " << rango <<  ' ' << base_array << ' ' << dbuffer << std::endl;
    std::cout << "      Region:["<< length[0] << ' ' << length[1] << ' ' << length[2] << "]\n";
    std::cout << " Buf_row_bytes =" << buffer_row_bytes; // << " Buf_slice_bytes =" << buffer_slice_bytes << std::endl;
    std::cout << " Host_row_bytes=" << length[0] //<< " Host_slice_bytes=" << length[1]*length[0]
    << std::endl;
    err = queue_aux->enqueueReadBufferRect(*dbuffer, CL_TRUE, origin, cero, length, buffer_row_bytes, buffer_slice_bytes,
                                           length[0], length[1]*length[0], rango);

    if (err != CL_SUCCESS) {
      throw HPLException(err, "Exception thrown in CLbinding::copy_to_buffer_to_basearray_range_offset: failed");
    }
    float *tmp = (float*)rango; //(float*)calloc(128 * 128, sizeof(float)); 
    printf("%f %f || %f %f || %f %f", tmp[0], tmp[1], tmp[128], tmp[129], tmp[256], tmp[257]);
  }
  *** END DEBUGGING CODE ****/
  
  
          HPL_PROFILEACTION(GPU_to_CPU_counter = waitForEvent(&event));
          HPL_PROFILEACTION(TheGlobalState().secs_data_GPU_to_CPU += GPU_to_CPU_counter);
          HPL_PROFILEACTION(TheGlobalState().total_secs_data_GPU_to_CPU += GPU_to_CPU_counter);

          if (err != CL_SUCCESS) {
                  throw HPLException(err, "Exception thrown in CLbinding::copy_to_buffer_to_basearray_range_offset: failed");

          }
}

void CLbinding::copy_to_buffer_from_basearray_range(Platform_t platform_id, Device_t device_type, int device, const InternalMultiDimArray *base_array,
		  int ox, int oy, int oz, int lx, int ly, int lz, int row_bytes, int slice_bytes, int host_row_bytes, int host_slice_bytes, void* rango)
{
          cl::Buffer *dbuffer;
          cl::size_t<3> origin;
          HPL_PROFILEDEFINITION(cl::Event event);
          cl::Event *p_event = NULL;
          HPL_PROFILEACTION(p_event = &event);
  
          if((base_array->getFather()!=NULL) && (base_array->getFather()->hdm.get_dbuffer(platform_id, device_type, device) !=NULL))
          //if(allow_subbuffer(platform_id,device_type) && (base_array->getFather()!=NULL) && (base_array->getFather()->hdm.get_dbuffer(platform_id, device_type, device) !=NULL))
          {
                  origin[0] = ox*base_array->getDataItemSize() ;
                  origin[1] = oy;
                  origin[2] = oz;
                  dbuffer = base_array->getFather()->hdm.get_dbuffer(platform_id, device_type, device);
          }
          else
          {
                  origin[0] = 0;
                  origin[1] = 0;
                  origin[2] = 0;
                  dbuffer = base_array->hdm.get_dbuffer(platform_id,device_type, device);
          }

	  cl::CommandQueue* queue_aux = &Devices[platform_id][device_type].dv_[device].cq_;

	  cl::size_t<3> cero;
	  cero[0] = 0;
	  cero[1] = 0;
	  cero[2] = 0;
	  cl::size_t<3> length;
	  length[0] = lx*base_array->getDataItemSize();	//Width in bytes
	  length[1] = ly; 								//Height in rows
	  length[2] = lz;									//Depth in slices

	  HPL_PROFILEDEFINITION(double CPU_to_GPU_counter);
	  cl_int err = queue_aux->enqueueWriteBufferRect(*dbuffer, CL_TRUE, origin, cero, length, row_bytes,
			  slice_bytes, host_row_bytes, host_slice_bytes, rango, NULL, p_event);
  
	  HPL_PROFILEACTION(CPU_to_GPU_counter = waitForEvent(&event));
	  HPL_PROFILEACTION(TheGlobalState().secs_data_CPU_to_GPU += CPU_to_GPU_counter);
	  HPL_PROFILEACTION(TheGlobalState().total_secs_data_CPU_to_GPU += CPU_to_GPU_counter);

	  if (err != CL_SUCCESS) {
		  throw HPLException(err, "Exception thrown in CLbinding::copy_to_buffer_from_basearray_range: failed");

	  }
}



void CLbinding::copy_from_buffers(FRunner *frunner, bool enable)
{
	  if( enable == false ) return;
	  FHandle* fhandle = frunner->getFhandle();
    
          const Platform_t platform_id = frunner->getDevice()->platform_id_;
          const Device_t    devt       = frunner->getDevice()->devt_;
          const int         devn       = frunner->getDevice()->devn_;
    
	  for(int i = 0; i < fhandle->numArgs_; i++ ) {
		  if( frunner->frunnerTable[i].base_array->flags() == Local ) continue;    // Local is a just place holder for actual arg
		  if(fhandle->fhandleTable[i].isWrite == false ) continue;
		  if( frunner->frunnerTable[i].base_array->dbuffer_ != NULL) continue; // manual copy_out
		  frunner->frunnerTable[i].base_array->onlyRead((Platform_t)MAX_PLATFORMS, (Device_t)0, 0, &Devices[platform_id][devt].dv_[devn].cq_);
		  copy_from_buffer_to_basearray(platform_id, devt, devn, frunner->frunnerTable[i].base_array );
	  }
}

// This will allocate dbuffer for the BaseaArray
cl::Buffer *CLbinding::allocate_dbuffer(Platform_t platform_id, Device_t device_type, int device, InternalMultiDimArray *base_array )
  {

    if((base_array->getFather()!=NULL))
    {
      if(base_array->getNumberOfDimensions() == 1)
      {
    	  if(TheGlobalState().env.hpl_unifiedmemory_enable && Device(platform_id,device_type,device)->dp_.unifiedMemory==1)
	{
		  cl::Buffer *ret = new cl::Buffer(Devices[platform_id][device_type].context_, CL_MEM_READ_WRITE|CL_MEM_USE_HOST_PTR, base_array->getXYZDim(0) * base_array->getXYZDim(1)  * base_array->getDataItemSize(), base_array->getData());
		  Devices[platform_id][device_type].dv_[device].cq_.enqueueMapBuffer( *ret, CL_TRUE, CL_MAP_READ | CL_MAP_WRITE, 0, base_array->getXYZDim(0) * base_array->getXYZDim(1)  * base_array->getDataItemSize());
    		  return ret;

	}
    	  return new cl::Buffer(Devices[platform_id][device_type].context_, CL_MEM_READ_WRITE, base_array->getXYZDim(0) * base_array->getXYZDim(1)  * base_array->getDataItemSize());
      }
      else
      {
        if(base_array->getNumberOfDimensions() == 2)
	{
	  if(TheGlobalState().env.hpl_unifiedmemory_enable && Device(platform_id,device_type,device)->dp_.unifiedMemory==1)
	  {
		cl::Buffer *ret = new cl::Buffer(Devices[platform_id][device_type].context_, CL_MEM_READ_WRITE|CL_MEM_USE_HOST_PTR, base_array->getXYZDim(0) * base_array->getXYZDim(1)  * base_array->getDataItemSize(), base_array->getData());
                Devices[platform_id][device_type].dv_[device].cq_.enqueueMapBuffer( *ret, CL_TRUE, CL_MAP_READ | CL_MAP_WRITE, 0, base_array->getXYZDim(0) * base_array->getXYZDim(1)  * base_array->getDataItemSize());
	      	return ret;
	   }
	
          return new cl::Buffer(Devices[platform_id][device_type].context_, CL_MEM_READ_WRITE, base_array->getXYZDim(0) * base_array->getXYZDim(1)  * base_array->getDataItemSize());
	}
        else
          if(base_array->getNumberOfDimensions() == 3)
            return new cl::Buffer(Devices[platform_id][device_type].context_, CL_MEM_READ_WRITE, base_array->getXYZDim(0) * base_array->getXYZDim(1) * base_array->getXYZDim(2) *base_array->getDataItemSize());
        else
          throw HPLException(0, "Exception thrown in CLbinding::allocate_dbuffer >3 dims unsupported.");
      }
    }
    else
    {
      if( TheGlobalState().env.hpl_unifiedmemory_enable && Device(platform_id,device_type,device)->dp_.unifiedMemory==1)
      {
                  cl::Buffer *ret = new cl::Buffer(Devices[platform_id][device_type].context_, CL_MEM_READ_WRITE|CL_MEM_USE_HOST_PTR, base_array->getSize()  * base_array->getDataItemSize(), base_array->getData());
                  Devices[platform_id][device_type].dv_[device].cq_.enqueueMapBuffer( *ret, CL_TRUE, CL_MAP_READ | CL_MAP_WRITE, 0, base_array->getSize()  * base_array->getDataItemSize());
                  return ret;
      }
      else
        return new cl::Buffer(Devices[platform_id][device_type].context_, CL_MEM_READ_WRITE, base_array->getSize() * base_array->getDataItemSize());
      
    }
    
  }

cl::Buffer *CLbinding::allocate_dsubbuffer( Platform_t platform_id, Device_t  device_type, int device, InternalMultiDimArray *base_array )
{
	cl::Buffer *dbuffer ;
	cl_buffer_region region;

	if(base_array->getNumberOfDimensions()<=1)
	{
		region.origin = base_array->getTX().origin * base_array->getDataItemSize();
		region.size = base_array->getXYZDim(0) * base_array->getDataItemSize();
	}
	else
	{
		region.origin = (base_array->getTY().origin * base_array->getTX().end + base_array->getTX().origin) * base_array->getDataItemSize();
		region.size = (base_array->getXYZDim(2)*base_array->getXYZDim(1) * base_array->getTX().end - base_array->getTX().origin) * base_array->getDataItemSize();
	}
	//if( region.origin % (Devices[platform_id][device_type].dv_[device].device_.getInfo<CL_DEVICE_MEM_BASE_ADDR_ALIGN>()/8) != 0 ) throw HPLException(0, "Exception thrown in CLbinding::allocate_dsubbuffer: The size of the subregions has to be multiple of CL_DEVICE_MEM_BASE_ADD_ALIGN");
	cl_int error;

	dbuffer = new cl::Buffer((base_array->getFather()->hdm.get_dbuffer(platform_id, device_type, device)->createSubBuffer(CL_MEM_READ_WRITE, CL_BUFFER_CREATE_TYPE_REGION, &region, &error)));
    if(error != CL_SUCCESS) {
        if(error == CL_MISALIGNED_SUB_BUFFER_OFFSET) {
            throw HPLException(0, "Exception thrown in CLbinding::allocate_dsubbuffer: The origin of the subregions has to be multiple of CL_DEVICE_MEM_BASE_ADD_ALIGN");
        } else {
            throw HPLException(error, "Exception thrown in CLbinding::allocate_dsubbuffer: failed");
        }
    }
	return dbuffer;
};
/////////////////////////////////////////////COPY IN THE FATHER BUFFER///////////////////////////////////////////////////

  void CLbinding::copy_in_s(cl::CommandQueue* queue_aux, InternalMultiDimArray *base_array, cl::Buffer *dbuffer, int start, int length)
  {
      cl_int err;
      void* ptr;
      HPL_PROFILEDEFINITION(cl::Event event);
      cl::Event *p_event = NULL;
      HPL_PROFILEACTION(p_event = &event);

      // Check inputs for not nulls
      if( dbuffer == NULL ) throw HPLException(0, "Exception thrown in CLbinding::copy_in: dbuffer is null, not allocated for the device");
      if( base_array == NULL ) throw HPLException(0, "Exception thrown in CLbinding::copy_in: base_array is null, not allocated for the host");
      base_array->hdm.numberof_copy_in++;      // Keeps the total number of copy_in for performance benchmarks.

      if(queue_aux == NULL)
      {
       queue_aux = &Devices[base_array->hdm.get_device_platform()][base_array->hdm.get_device_type()].dv_[base_array->hdm.get_device_number()].cq_;
      }

      ptr = base_array->getData(HPL_NONE);
      ptr = (void*)(((char*)ptr) + start * base_array->getDataItemSize());

      if(length == -1)
          length = base_array->getSize();

        int dims[3] = {0};
        for(int i = 0; i < base_array->getNumberOfDimensions(); i++)
                dims[i] = base_array->getXYZDim(i);

          cl::size_t<3> zeroes;
          zeroes[0] = 0;
          zeroes[1] = 0;
          zeroes[2] = 0;
          cl::size_t<3> length_r;
          length_r[0] = base_array->getXYZDim(0)*base_array->getDataItemSize();        //Width in bytes
          length_r[1] = (dims[1]==0? 1: dims[1]);                                                                 //Height in rows
          length_r[2] = (dims[2]==0? 1: dims[2]);                                                                 //Depth in slices
          cl::size_t<3> origin_device;
          origin_device[0] = base_array->getTX().origin *base_array->getDataItemSize();
          origin_device[1] = base_array->getTY().origin;
          origin_device[2] = 0;

          HPL_PROFILEDEFINITION(double CPU_to_GPU_counter);

        err = queue_aux->enqueueWriteBufferRect(*dbuffer, CL_TRUE, origin_device, zeroes, length_r, base_array->getTX().end*base_array->getDataItemSize(), base_array->getTX().end*base_array->getTY().end*base_array->getDataItemSize(),
          base_array->getTX().end*base_array->getDataItemSize(), base_array->getTX().end*base_array->getTY().end*base_array->getDataItemSize()/*0,0*/, ptr, NULL, p_event);

          HPL_PROFILEACTION(CPU_to_GPU_counter = waitForEvent(&event));
          HPL_PROFILEACTION(TheGlobalState().secs_data_CPU_to_GPU += CPU_to_GPU_counter);
          HPL_PROFILEACTION(TheGlobalState().total_secs_data_CPU_to_GPU += CPU_to_GPU_counter);

          if (err != CL_SUCCESS) {
                  throw HPLException(err, "Exception thrown in CLbinding::copy_to_buffer_from_basearray_range: failed");

          }

}

void CLbinding::copy_in(cl::CommandQueue* queue_aux, InternalMultiDimArray *base_array, cl::Buffer *dbuffer, Platform_t p_id, Device_t d_type, int d_n, int start, int length)
{
      cl_int err;
      void* ptr;
      HPL_PROFILEDEFINITION(cl::Event event);
      cl::Event *p_event = NULL;
      HPL_PROFILEACTION(p_event = &event);

      // Check inputs for not nulls
      if( dbuffer == NULL ) throw HPLException(0, "Exception thrown in CLbinding::copy_in: dbuffer is null, not allocated for the device");
      if( base_array == NULL ) throw HPLException(0, "Exception thrown in CLbinding::copy_in: base_array is null, not allocated for the host");
      base_array->hdm.numberof_copy_in++;      // Keeps the total number of copy_in for performance benchmarks.
//      if( TheGlobalState().env.hpl_unifiedmemory_enable ) return;   // No need to cop in for unified memory

      ptr = base_array->getData(HPL_NONE);
      ptr = (void*)(((char*)ptr) + start * base_array->getDataItemSize());

      if(length == -1)
	length = base_array->getSize();

	  {
    	  /////////////////////////CHILDREN_CODE_BEGIN/////////////////////////////////////////////
		  //MVB : THE WIDTH OF A ONE DIMENSIONAL SON IS NOT EQUAL TO THE
		  // FATHER'S ONE
    	  if((base_array->getNumberOfDimensions() != 1) && ((base_array->getTX().end != 0) || (base_array->getTY().end != 0)))
    	  {

    	          int dims[3] = {0};
    	          for(int i = 0; i < base_array->getNumberOfDimensions(); i++)
    	                 	dims[i] = base_array->getXYZDim(i);

    	            cl::size_t<3> zeroes;
    	            zeroes[0] = 0;
    	            zeroes[1] = 0;
    	            zeroes[2] = 0;
    	            cl::size_t<3> length;
    	            length[0] = base_array->getXYZDim(0)*base_array->getDataItemSize();        //Width in bytes
    	            length[1] = (dims[1]==0? 1: dims[1]);                                                                 //Height in rows
    	            length[2] = (dims[2]==0? 1: dims[2]);                                                                 //Depth in slices

    	            HPL_PROFILEDEFINITION(double CPU_to_GPU_counter);

    	            err = queue_aux->enqueueWriteBufferRect(*dbuffer, CL_TRUE, zeroes, zeroes, length, base_array->getXYZDim(0)*base_array->getDataItemSize(),base_array->getXYZDim(0) * base_array->getXYZDim(1) *base_array->getDataItemSize(),
    	                	            base_array->getTX().end*base_array->getDataItemSize(), base_array->getTX().end* base_array->getTY().end *base_array->getDataItemSize(), ptr, NULL, p_event);

    	            HPL_PROFILEACTION(CPU_to_GPU_counter = waitForEvent(&event));
    	            HPL_PROFILEACTION(TheGlobalState().secs_data_CPU_to_GPU += CPU_to_GPU_counter);
    	            HPL_PROFILEACTION(TheGlobalState().total_secs_data_CPU_to_GPU += CPU_to_GPU_counter);

    	            if (err != CL_SUCCESS) {
    	                    throw HPLException(err, "Exception thrown in CLbinding::copy_to_buffer_from_basearray_range: failed");

    	            }

    	  }
    	  else
    	  ///////////////////////CHILDREN_CODE_END////////////////////////////////////////////////////////////////////////////////
    	  {
    		  HPL_PROFILEDEFINITION(double CPU_to_GPU_counter);
    		  if( TheGlobalState().env.hpl_unifiedmemory_enable && Device(p_id,d_type,d_n)->dp_.unifiedMemory==1) {
    			  err = queue_aux->enqueueUnmapMemObject(*dbuffer, ptr, NULL, p_event);

    			  if(err==CL_SUCCESS){
    				  HPL_PROFILEACTION(CPU_to_GPU_counter = waitForEvent(&event));
    				  HPL_PROFILEACTION(TheGlobalState().secs_data_CPU_to_GPU += CPU_to_GPU_counter);
    				  HPL_PROFILEACTION(TheGlobalState().total_secs_data_CPU_to_GPU += CPU_to_GPU_counter);
    			  }
    		  }
    		  else
    		  {
    			  err = queue_aux->enqueueWriteBuffer(*dbuffer, CL_TRUE, start *base_array->getDataItemSize(), length * base_array->getDataItemSize(), ptr, NULL, p_event);

                  if (err != CL_SUCCESS) {
                    if(err == CL_MEM_OBJECT_ALLOCATION_FAILURE)
                    {
                      while(1) {
                        Device(p_id,d_type,d_n)->removeLRU();
                        err = queue_aux->enqueueWriteBuffer(*dbuffer, CL_TRUE, start *base_array->getDataItemSize(), length * base_array->getDataItemSize(), ptr, NULL, p_event);
                        if (err != CL_SUCCESS) {
                          if(Device(p_id,d_type,d_n)->sizesAreTheSame())
                            throw HPLException(err, "Exception thrown in CLbinding::run_kernel: enqueueNDRangeKernel() failed (memory allocation problem)");
                        }
                        else
                          break;
                      }
                    }
                    else
                      throw HPLException(err, "Exception thrown in CLbinding::copy_in: failed");
                  }


    			  HPL_PROFILEACTION(CPU_to_GPU_counter = waitForEvent(&event));
    			  HPL_PROFILEACTION(TheGlobalState().secs_data_CPU_to_GPU += CPU_to_GPU_counter);
    			  HPL_PROFILEACTION(TheGlobalState().total_secs_data_CPU_to_GPU += CPU_to_GPU_counter);
//    			  if (err != CL_SUCCESS) {
//    				  throw HPLException(err, "Exception thrown in CLbinding::copy_in: failed");
//    			  }
    		  }
    	  }
	  }
	  if( TheGlobalState().env.hpl_print_copy ) std::cout << "copy_in BaseArray = " << base_array << std::endl;

}

//////////////////////////////////////////////////CHILDREN_CODE_BEGIN/////////////////////////////////////////////////////////
//////////////////COPY OUT FROM THE FATHER BUFFER
  void CLbinding::copy_out_s(cl::CommandQueue* queue_aux, InternalMultiDimArray *base_array, cl::Buffer *dbuffer, int start, int length )
  {
    cl_int err;
    void* ptr;
    HPL_PROFILEDEFINITION(cl::Event event);
    cl::Event *p_event = NULL;
    HPL_PROFILEACTION(p_event = &event);

      if( dbuffer == NULL ) throw HPLException(0, "Exception thrown in CLbinding::copy_out: dbuffer is null, not allocated for the device");
      if( base_array == NULL ) throw HPLException(0, "Exception thrown in CLbinding::copy_out: base_array is null, not allocated for the host");
      base_array->hdm.numberof_copy_out++;      // Keeps the total number of copy_out for performance benchmarks.

      ptr = base_array->getData(HPL_NONE);
      ptr = (void*)(((char*)ptr) + start * base_array->getDataItemSize());
      if(length == -1)
                  length = base_array->getSize();

        int dims[3] = {0};
        for(int i = 0; i < base_array->getNumberOfDimensions(); i++)
                dims[i] = base_array->getXYZDim(i);

          cl::size_t<3> length_r;
          length_r[0] = base_array->getXYZDim(0)*base_array->getDataItemSize();        //Width in bytes
          length_r[1] = dims[1]==0? 1: dims[1];                                        //Height in rows
          length_r[2] = dims[2]==0? 1: dims[2];

          cl::size_t<3> zeroes;
          zeroes[0] = 0;
          zeroes[1] = 0;
          zeroes[2] = 0;
          cl::size_t<3> origin_device;
          origin_device[0] = base_array->getTX().origin * base_array->getDataItemSize();
          origin_device[1] = base_array->getTY().origin;
          origin_device[2] = 0;

          HPL_PROFILEDEFINITION(double GPU_to_CPU_counter);
    
          ///MVB: in case of unifiedmemory enabled, we will use the Rect versions.
          err = queue_aux->enqueueReadBufferRect(*dbuffer, CL_TRUE, origin_device, zeroes, length_r, base_array->getTX().end*base_array->getDataItemSize(), base_array->getTX().end*base_array->getTY().end*base_array->getDataItemSize(),
                  base_array->getTX().end * base_array->getDataItemSize(), base_array->getTX().end * base_array->getTY().end*base_array->getDataItemSize(), ptr, NULL, p_event);

    
//        err = queue_aux->enqueueReadBufferRect(*dbuffer, CL_TRUE, origin_device, zeroes, length_r, base_array->getTX().end*base_array->getDataItemSize(), base_array->getTX().end*base_array->getTY().end*base_array->getDataItemSize(),
//        base_array->getTX().end * base_array->getDataItemSize(), base_array->getTX().end * base_array->getTY().end*base_array->getDataItemSize(), ptr, NULL, &event);

          HPL_PROFILEACTION(GPU_to_CPU_counter = waitForEvent(&event));
          HPL_PROFILEACTION(TheGlobalState().secs_data_GPU_to_CPU += GPU_to_CPU_counter);
          HPL_PROFILEACTION(TheGlobalState().total_secs_data_GPU_to_CPU += GPU_to_CPU_counter);
          if (err != CL_SUCCESS) {
                  throw HPLException(err, "Exception thrown in CLbinding::copy_out_s: failed");

          }

}

  // This will copy out dbuffer to base_array
void CLbinding::copy_out(cl::CommandQueue* queue_aux, InternalMultiDimArray *base_array, cl::Buffer *dbuffer, Platform_t p_id, Device_t d_type, int d_n, int start, int length )
{
    cl_int err;
    void* ptr;
    HPL_PROFILEDEFINITION(cl::Event event);
    cl::Event *p_event = NULL;
    HPL_PROFILEACTION(p_event = &event);
  
      if( dbuffer == NULL ) throw HPLException(0, "Exception thrown in CLbinding::copy_out: dbuffer is null, not allocated for the device");
      if( base_array == NULL ) throw HPLException(0, "Exception thrown in CLbinding::copy_out: base_array is null, not allocated for the host");
      base_array->hdm.numberof_copy_out++;      // Keeps the total number of copy_out for performance benchmarks.

      ptr = base_array->getData(HPL_NONE);
      ptr = (void*)(((char*)ptr) + start * base_array->getDataItemSize());

      if(length == -1)
	length = base_array->getSize();

	  {
    	  //////////////////////////////////////////////////////////////CHILDREN_BEGIN_CODE////////////////////////////////////////////////////////////
		  //MVB : THE WIDTH OF A ONE DIMENSIONAL SON IS NOT EQUAL TO THE
		  // FATHER'S ONE
	  if((base_array->getNumberOfDimensions() != 1) && (base_array->getFather()!=NULL))
    	  {
    	          int dims[3] = {0};
    	          for(int i = 0; i < base_array->getNumberOfDimensions(); i++)
    	                  dims[i] = base_array->getXYZDim(i);

    	            cl::size_t<3> length;
    	            length[0] = base_array->getXYZDim(0)*base_array->getDataItemSize();        //Width in bytes
    	            length[1] = dims[1]==0? 1: dims[1];                                                           //Height in rows
    	            length[2] = dims[2]==0? 1: dims[2];

    	            cl::size_t<3> zeroes;
    	            zeroes[0] = 0;
    	            zeroes[1] = 0;
    	            zeroes[2] = 0;

    	            HPL_PROFILEDEFINITION(double GPU_to_CPU_counter);
            
            const size_t buffer_row_bytes   = base_array->getXYZDim(0)*base_array->getDataItemSize();
            const size_t buffer_slice_bytes = base_array->getXYZDim(0) * base_array->getXYZDim(1) * base_array->getDataItemSize();
            
            /*
            std::cout << "CPOUTF Offset=0 BF=HOST=0 Region:["<< length[0] << ' ' << length[1] << ' ' << length[2] << "] "<< base_array << ' ' << dbuffer << "\n";
            std::cout << " Buf_row_bytes =" << buffer_row_bytes; // << " Buf_slice_bytes =" << buffer_slice_bytes << std::endl;
            std::cout << " Host_row_bytes=" << (base_array->getTX().end * base_array->getDataItemSize()) //<< " Host_slice_bytes=" << length[1]*length[0]
            << std::endl;
            */
            
    	            ///MVB: in case of unifiedmemory enabled, we will use the Rect versions.
      	          err = queue_aux->enqueueReadBufferRect(*dbuffer, CL_TRUE, zeroes, zeroes, length,  buffer_row_bytes, buffer_slice_bytes, base_array->getTX().end * base_array->getDataItemSize(), base_array->getTX().end *  base_array->getTY().end *base_array->getDataItemSize(), ptr, NULL, p_event);



    	            HPL_PROFILEACTION(GPU_to_CPU_counter = waitForEvent(&event));
    	            HPL_PROFILEACTION(TheGlobalState().secs_data_GPU_to_CPU += GPU_to_CPU_counter);
    	            HPL_PROFILEACTION(TheGlobalState().total_secs_data_GPU_to_CPU += GPU_to_CPU_counter);
    	            if (err != CL_SUCCESS) {
    	                    throw HPLException(err, "Exception thrown in CLbinding::copy_out: failed");

    	            }
    	  }
    	  else
    	  {
    	  ///////////////////////////////////////////////////////////CHILDREN_END_CODE/////////////////////////////////////////////////////////////////////

                  //printf("reading %d bytes from buffer %p to %p\n", length * base_array->getDataItemSize(), dbuffer, ptr);
		  HPL_PROFILEDEFINITION(double GPU_to_CPU_counter);
		  if( TheGlobalState().env.hpl_unifiedmemory_enable && Device(p_id,d_type,d_n)->dp_.unifiedMemory==1) {
			ptr = queue_aux->enqueueMapBuffer( *dbuffer, CL_TRUE, CL_MAP_READ | CL_MAP_WRITE, 0, length * base_array->getDataItemSize(), NULL, p_event, &err);
		  } else
			  err = queue_aux->enqueueReadBuffer(*dbuffer, CL_TRUE, start*base_array->getDataItemSize(), length * base_array->getDataItemSize(), ptr, NULL, p_event);

		  HPL_PROFILEACTION(GPU_to_CPU_counter = waitForEvent(&event));
		  HPL_PROFILEACTION(TheGlobalState().secs_data_GPU_to_CPU += GPU_to_CPU_counter);
		  HPL_PROFILEACTION(TheGlobalState().total_secs_data_GPU_to_CPU += GPU_to_CPU_counter);
		  if (err != CL_SUCCESS) {
			  throw HPLException(err, "Exception thrown in CLbinding::copy_out: failed");
		  }
    	  }

	  }

	  if( TheGlobalState().env.hpl_print_copy ) std::cout << "copy_out BaseArray = " << base_array << std::endl;

}

void CLbinding::free_resources()
{
	  // Close text file
	  if( TheGlobalState().env.hpl_kernel_file_enable )
		  kernelfile.close();

}

void *CLbinding::allocMem(size_t buf_size, bool fast_mem)
{ void *ret;
  
  const HPL::Platform_t pt = DefaultPlatform[static_cast<int>(GPU)];
  
  if (!fast_mem || (pt == MAX_PLATFORMS)) {
    ret = memalign(4096, buf_size);
  } else {
    cl_int err;
    
    internal::DeviceImplementation *di = getDevice(pt, GPU, 0);
    
    cl::Buffer *pinned_buffer = new cl::Buffer(di->dtd_->context_, CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR, buf_size, NULL, &err);
    assert(pinned_buffer != 0);
    assert(err == CL_SUCCESS);
    
    ret = di->cq_.enqueueMapBuffer(*pinned_buffer, CL_TRUE, CL_MAP_READ | CL_MAP_WRITE, 0, buf_size, NULL, NULL, &err);
    assert(err == CL_SUCCESS);
    
    PinnedBuffers[ret] = pinned_buffer;
  }
  
  return ret;
}

void CLbinding::freeMem(void *p)
{
  auto it = PinnedBuffers.find(p);
  
  //Notice that even if the Array is marked as fastMem, CLbinding::allocMem could have decided not to use pinned memory
  if (it != PinnedBuffers.end()) {
    cl::Buffer *pinned_buffer = it->second;
  
    PinnedBuffers.erase(it);
  
    internal::DeviceImplementation *di = getDevice(DefaultPlatform[static_cast<int>(GPU)], GPU, 0);
    cl_int err = di->cq_.enqueueUnmapMemObject(*pinned_buffer, p);
    assert(err == CL_SUCCESS);
  
    delete pinned_buffer;
  } else {
    free(p);
  }
}
  
/*
  void CLbinding::calculate_args_sizes(FHandle* fhandle)
  {
	  if (fhandle->numArgs_ >= MAX_KERNEL_NARG) {
		  char aux_message[64];
		  sprintf(aux_message, "Exception thrown in  CLbinding::calculate_args_sizes: Can not handle kernel number of args more than %d", MAX_KERNEL_NARG);
		  throw HPLException(0, aux_message);
	  }
	  for(int i = 0; i < fhandle->numArgs_; i++ ) {
		  frunner->frunnerTable[i].size = calculate_BaseArray_size(frunner->frunnerTable[i].base_array);
	  }
  }


  int CLbinding::calculate_BaseArray_size(BaseArray *base_array)
  {
	  int ndim = base_array->getNumberOfDimensions();
	  int totalSize = 1;
	  for(int k = 0; k < ndim; k++ ) {
		  totalSize *= base_array->getDimension(k);
	  }
	  return totalSize;
  }
*/

cl::Kernel* CLbinding::setup_and_notrun(FRunner * const frunner)
{
        return internal::program_init(frunner, allCode);
}
  
void CLbinding::setup_and_run(FRunner * const frunner)
{
  cl::Kernel * const kernel = internal::program_init(frunner, allCode);
  create_buffers(frunner);
  internal::set_deallocate_info(frunner, kernel);
  copy_to_buffers(frunner);
  internal::set_kernel_args(frunner, kernel);
  internal::run_kernel(frunner, kernel, true);
}
  
void CLbinding::setup_and_run_lite(FRunner * const frunner)
{
  Device& dev = frunner->getDevice();
  cl::Kernel * const kernel = dev->dtd_->getKernel(frunner->getFhandle()) + dev->devn_;
  internal::set_deallocate_info(frunner, kernel);
  copy_to_buffers(frunner);
  internal::set_kernel_args(frunner, kernel);
  internal::run_kernel(frunner, kernel, false);
}

cl::CommandQueue& CLbinding::external_setup(FRunner * const frunner)
{
  create_buffers(frunner);
  copy_to_buffers(frunner);
  return frunner->getDevice()->cq_;
}
  
unsigned int CLbinding::getDeviceNumber(Device_t type_n) const
{
  unsigned int result = 0;
    
  for(int i = 0; i < HPL::MAX_PLATFORMS; ++i)
    if (Devices[i][type_n].nDevices_ > result) {
      result = Devices[i][type_n].nDevices_;
    }
    
  return result;
}
  
unsigned int CLbinding::getDeviceNumber(Platform_t platform_id, Device_t type_n) const
{
    return Devices[platform_id][type_n].nDevices_;
}
 
void CLbinding::resetCode()
{
  allCode = "";
  
  if( TheGlobalState().env.hpl_kernel_file_enable ) {
    kernelfile.close();
    kernelfile.open("HPL_kernels.cl", std::ofstream::out | std::ofstream::trunc);
    HPLExceptionIf(!kernelfile.is_open(), "Exception thrown in CLbinding::CLbinding(): Unable to open for kernelfile writing");
  }
}
  
CLbinding& TheCLbinding()
{ static CLbinding CLB;
    
  return CLB;
}
  
} //end namespace HPL
