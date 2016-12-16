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
/// \file     GlobalState.h
/// \author   Moisés Viñas        <moises.vinas@udc.es>
/// \author   Zeki Bozkus         <zeki.bozkus@khas.edu.tr>
/// \author   Basilio B. Fraguela <basilio.fraguela@udc.es>
///

#ifndef GLOBALSTATE_H
#define GLOBALSTATE_H

#include <map>
#include <set>
#include "Codifier.h"
#include "CLbinding.h"
#include "ArrayInfo.h"
#include "Env.h"

namespace HPL {

  //Forward declaration
  class FHandle;
  

  /// Keeps the profiling information
  struct ProfilingData {
    double secsKernelCreation;
    double secsKernelCompilation;
    double secsDataHostToDevice;
    double secsDataDeviceToHost;
    double secsKernelExecution;
    
    ProfilingData()
    : secsKernelCreation(0.0),
    secsKernelCompilation(0.0),
    secsDataHostToDevice(0.0),
    secsDataDeviceToHost(0.0),
    secsKernelExecution(0.0)
    {}
    
    ProfilingData(double isecsKernelCreation, 
		  double isecsKernelCompilation, 
		  double isecsDataHostToDevice, 
		  double isecsDataDeviceToHost,
		  double isecsKernelExecution)
    : secsKernelCreation(isecsKernelCreation),
    secsKernelCompilation(isecsKernelCompilation),
    secsDataHostToDevice(isecsDataHostToDevice),
    secsDataDeviceToHost(isecsDataDeviceToHost),
    secsKernelExecution(isecsKernelExecution)
    {}
    
  };


  /// Whether HPL should try to catch its exceptions or send them to the user
  extern bool AutoManageExceptions;
  
  /// Global State of the HPL library
  /** Must be a singleton */
  class GlobalState {
    
    static int nest_;
    
  public:
    
    enum Pragmas { 
      CL_KHR_FP64 = 0, 
      CL_KHR_SELECT_FPROUNDING_MODE = 1, 
      CL_KHR_GLOBAL_INT32_BASE_ATOMICS = 2,
      CL_KHR_GLOBAL_INT32_EXTENDED_ATOMICS = 3, 
      CL_KHR_LOCAL_INT32_BASE_ATOMICS = 4,
      CL_KHR_LOCAL_INT32_EXTENDED_ATOMICS = 5, 
      CL_KHR_INT64_BASE_ATOMICS = 6,
      CL_KHR_INT64_EXTENDED_ATOMICS = 7, 
      CL_KHR_3D_IMAGE_WRITES = 8, 
      CL_KHR_BYTE_ADDRESSABLE_STORE = 9,
      CL_KHR_FP16 = 10, 
      CL_APPLE_GL_SHARING = 11, 
      CL_KHR_GL_SHARING = 12,
      PRAGMAS_NUMBER = 13
    };
    
    static const char *pragmas_text[];
    
    /// Represents the knowledge of HPL on a user struct
    /** @todo support for array fields, e.h. int v[5]; */
    class StructInfo {
      typedef std::map<const char *, const char *, utils::ltstr> Field2TypeMapper_t;
      
      const std::size_t sz_;        ///< Size of the struct (currently unused)
      const char * const name_;     ///< Name of the struct
      const char * const body_;     ///< Body of the struct
      Field2TypeMapper_t keywords_; ///< Maps field names to their type as string

    public:
      
      StructInfo(std::size_t sz, const char *name, const char *body);
      
      const char *name() const { return name_; }
      
      const char *body() const { return body_; }
      
      bool existsField(const char *f) const;
    };

    /// Returns the singleton global HPL::CLbinding object
    CLbinding& clbinding() { return clbinding_; }
    
    /// Each invokation returns a new string identifier (for a new variable)
    String_t getNewID();
    
    /// Returns the function handler for the function \p f
    FHandle& getFHandle(void *f);

    /// Returns the function handler for a natively defined function
    FHandle& getNativeFHandle(void *f, const String_t& name, const String_t& native_code);
    
    /// Removes the function handler of the function \p f
    void deleteFHandle(void *f);

    /**
     * @name These function manage the several codifiers that can exist in the
     * application. Their number is related with the nesting level
     * of the device function calls.
     */
    ///@{
    
    /**
     * @brief This function obtains the codifier for the current nesting level.
     */
    Codifier& getCodifier();
    
    /**
     * @brief This function increments a nesting level of the current codifier.
     */
    void addCodifierNest();
    
    /**
     * @brief This function decrements a nesting level of the current codifier.
     */
    void subCodifierNest();
    ///@}

    /// Returns the ArrayInfo for the BaseArray \p bArr
    ArrayInfo *getArrayInfo(const BaseArray *bArr);
    
    /// Returns the number of available kernels in the system
    int getNumberOfFHandles();

    static int getCodifierNestLevel() { return nest_; }

        friend GlobalState& TheGlobalState();
    
    /// Keeps all the env variables 
    Env env;

    /// Type of the container for user-defined functions.
    typedef std::map<void *, FHandle* > FHandleStorage_t;

    FHandleStorage_t* getFStorage() {return &fStorage_;}

    void dumpCode(const String_t& code);
    
    void registerStructInfo(StructInfo *sip);
    
    void checkStructField(const char *struct_name, const char *field_name);
    
    void addPragma(int pragma) { pragmas[pragma]++; }

    int getPragma(int pragma) const { return pragmas[pragma]; }

    /// Erases internal IR code storage (allCode) and \c fStorage_
    void resetCode();
    
    /**
     * These variables contain the information of profiling.
     * secs_kernel_creation contains the seconds of creating the kernel code.
     * secs_kernel_compilation contains the seconds of compile the kernel.
     * secs_data_CPU_to_GPU contains the seconds needed to fill the GPU data.
     * secs_data_GPU_to_CPU contains the seconds needed to update the host data.
     * secs_device_execution contains the seconds needed to execute the kernel.
     * total versions store the total time for each task for the whole application.
     */
    ///@{
    HPL_PROFILEDEFINITION(double secs_kernel_creation);
    HPL_PROFILEDEFINITION(double secs_kernel_compilation);
    HPL_PROFILEDEFINITION(double secs_data_CPU_to_GPU);
    HPL_PROFILEDEFINITION(double secs_data_GPU_to_CPU);
    HPL_PROFILEDEFINITION(double secs_device_execution);
    HPL_PROFILEDEFINITION(double total_secs_kernel_creation);
    HPL_PROFILEDEFINITION(double total_secs_kernel_compilation);
    HPL_PROFILEDEFINITION(double total_secs_data_CPU_to_GPU);
    HPL_PROFILEDEFINITION(double total_secs_data_GPU_to_CPU);
    HPL_PROFILEDEFINITION(double total_secs_device_execution);
    HPL_PROFILEDEFINITION(Timer timer);
    
    /// Return profiling data for most recent kernel run
    ProfilingData getProfile() const;
    
    /// Return profiling data for all kernel runs
    ProfilingData getTotalProfile() const;
    
    /// @deprecated 
    ADD_PROFILER_GETTER(secs_kernel_creation);
    ADD_PROFILER_GETTER(secs_kernel_compilation);
    ADD_PROFILER_GETTER(secs_data_CPU_to_GPU);
    ADD_PROFILER_GETTER(secs_data_GPU_to_CPU);
    ADD_PROFILER_GETTER(secs_device_execution);
    ADD_PROFILER_GETTER(total_secs_kernel_creation);
    ADD_PROFILER_GETTER(total_secs_kernel_compilation);
    ADD_PROFILER_GETTER(total_secs_data_CPU_to_GPU);
    ADD_PROFILER_GETTER(total_secs_data_GPU_to_CPU);
    ADD_PROFILER_GETTER(total_secs_device_execution);
    
    ///@}

  protected:
    
    /// Type of the container for arrays in the user-defined functions.
    typedef std::map<String_t, ArrayInfo * > ArrayInfoMap_t;
    //std::set<String_t> keywords_; ///< Used to store the C++ keywords to avoid them in the name generation
    
    /// Type of the container for the information on registered structs
    typedef std::map<const char *, StructInfo *, utils::ltstr> StructName2InfoMap_t;
    
    GlobalState();

    ~GlobalState()
    {
    	//We store the profiling information of whole application in a file (HPL_PROFILER_OUTPUT.txt)
    	HPL_PROFILEDEFINITION(utils::showTotalProfilerStatistics());
    }
  
    bool forced_global_vars_;
    String_t nameGenerator_;             ///<  Used to generate new names for variables.
    FHandleStorage_t fStorage_;          ///<  Function container (indexable by its address).
    StructName2InfoMap_t structInfoMap_; ///< storage of information on structs
    CLbinding& clbinding_;                ///<  Global HPL::CLbinding object.
    ArrayInfoMap_t arrayInfoMap_;        ///< Array container keeping info about arrays (indexable by its address).
    std::vector<Codifier> codifierArray_; ///<  Number of codifiers, the index 0 means there is not nesting, index 1 means there is nesting of level 1, ...
    int pragmas[PRAGMAS_NUMBER];          ///< Marks which pragmas the code needs

  };
  
  /// Returns the singleton GlobalState object
  GlobalState& TheGlobalState();

  
/// Facilitates checking the state of the global HPL::Codifier against value \a x.
#define HPL_STATE(x) (HPL::TheGlobalState().getCodifier().state() == (x))

/// Defines a struct and makes it available and known to HPL
/** It must be used outside any function in a single unit of compilation */
#define HPL_DEFINE_STRUCT(name, body...)                                    \
HPL_DECLARE_STRUCT(name, body)                                              \
template<> const char* HPL::TypeInfo<name>::String = #name ;                \
HPL::GlobalState::StructInfo _HPLhandle_##name(sizeof(name), #name, #body); 

///Only declares a struct, but does not register it in HPL
#define HPL_DECLARE_STRUCT(name, body...)             \
  struct name body;                                   \
  template<> const char* HPL::TypeInfo<name>::String;  
  
}

#endif
