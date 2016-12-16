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
/// \file     GlobalState.cpp
/// \author   Moisés Viñas        <moises.vinas@udc.es>
/// \author   Zeki Bozkus         <zeki.bozkus@khas.edu.tr>
/// \author   Basilio B. Fraguela <basilio.fraguela@udc.es>
///

#include <iostream>
#include "FHandle.h"
#include "GlobalState.h"
#include "Array.h"

namespace {

#ifdef HPL_WINDOWS
  char *strdup (const char *s) {
    char *d = (char*)malloc (strlen (s) + 1);   // Space for length plus nul
    if (d == NULL) return NULL;           // No memory
    strcpy (d, s);                        // Copy the characters
    return d;                             // Return the new string
  }
#endif

  void fill_codifier_PreVars()
  {
    HPL::Codifier::PreVars[0] = HPL::idx.name().c_str();
    HPL::Codifier::PreVars[1] = HPL::idy.name().c_str();
    HPL::Codifier::PreVars[2] = HPL::idz.name().c_str();
    HPL::Codifier::PreVars[3] = HPL::lidx.name().c_str();
    HPL::Codifier::PreVars[4] = HPL::lidy.name().c_str();
    HPL::Codifier::PreVars[5] = HPL::lidz.name().c_str();
    HPL::Codifier::PreVars[6] = HPL::gidx.name().c_str();
    HPL::Codifier::PreVars[7] = HPL::gidy.name().c_str();
    HPL::Codifier::PreVars[8] = HPL::gidz.name().c_str();
    HPL::Codifier::PreVars[9] = HPL::szx.name().c_str();
    HPL::Codifier::PreVars[10] = HPL::szy.name().c_str();
    HPL::Codifier::PreVars[11] = HPL::szz.name().c_str();
    HPL::Codifier::PreVars[12] = HPL::lszx.name().c_str();
    HPL::Codifier::PreVars[13] = HPL::lszy.name().c_str();
    HPL::Codifier::PreVars[14] = HPL::lszz.name().c_str();
    HPL::Codifier::PreVars[15] = HPL::ngroupsx.name().c_str();
    HPL::Codifier::PreVars[16] = HPL::ngroupsy.name().c_str();
    HPL::Codifier::PreVars[17] = HPL::ngroupsz.name().c_str();
  }
  
  bool can_be_code(const std::string& native_code)
  { static const std::string whitespaces (" \t\f\v\n\r");

    const size_t found = native_code.find_last_not_of(whitespaces);
    
    if (found == std::string::npos) {
      return true; //We assume the actual kernel was put in another string...
    }
    
    //HPLExceptionIf (found == std::string::npos, 
    //		    "Exception thrown in GlobalState::can_be_code : String full of whitespaces provided as code");
    
    const char c = native_code[found];
    return  (c == ';') || (c == '}');
  }
  
  std::string read_string_from_file(const std::string& filename)
  {
    std::ifstream sourceFile(filename);
    HPLExceptionIf ( sourceFile.fail(), 
		    "Exception thrown in GlobalState::read_string_from_file : Source interpreted as file name that cannot be opened");
    
    std::string str((std::istreambuf_iterator<char>(sourceFile)), std::istreambuf_iterator<char>());
    
    return str;
  }
  
}

namespace HPL {
  
  bool AutoManageExceptions = true;

  int GlobalState::nest_ = 0;
  
  const char *GlobalState::pragmas_text[] = {
    "cl_khr_fp64", 
    "cl_khr_select_fprounding_mode", 
    "cl_khr_global_int32_base_atomics",
    "cl_khr_global_int32_extended_atomics", 
    "cl_khr_local_int32_base_atomics", 
    "cl_khr_local_int32_extended_atomics",
    "cl_khr_int64_base_atomics", 
    "cl_khr_int64_extended_atomics", 
    "cl_khr_3d_image_writes",
    "cl_khr_byte_addressable_store", 
    "cl_khr_fp16", 
    "CL_APPLE_gl_sharing", 
    "CL_KHR_gl_sharing"
  };
  
  GlobalState::GlobalState() :
  env(),
  nameGenerator_("_a"),
  clbinding_(TheCLbinding()),
  codifierArray_(1)
  {
    /*
    keywords_.insert("do");
    keywords_.insert("if");
    keywords_.insert("or");
    keywords_.insert("pp"); //Apple OpenCL on 10.7.5
    keywords_.insert("sp"); //Apple OpenCL on 10.7.5

    keywords_.insert("and");
    keywords_.insert("any");
    keywords_.insert("asm");
    keywords_.insert("for");
    keywords_.insert("int");
    keywords_.insert("nan");
    keywords_.insert("new");
    keywords_.insert("not");
    keywords_.insert("pow");
    keywords_.insert("try");
    keywords_.insert("xor");

    keywords_.insert("acos");
    keywords_.insert("atan");
    keywords_.insert("auto");
    keywords_.insert("bool");
    keywords_.insert("case");
    keywords_.insert("char");
    keywords_.insert("else");
    keywords_.insert("enum");
    keywords_.insert("goto");
    keywords_.insert("long");
    keywords_.insert("this");
    keywords_.insert("true");
    keywords_.insert("void");
    */
    
    for(int i = 0; i <  PRAGMAS_NUMBER; ++i)
      pragmas[i] = 0;
    
    HPL_PROFILEACTION(
		      secs_kernel_creation = 0.0;
		      secs_kernel_compilation = 0.0;
		      secs_data_CPU_to_GPU = 0.0;
		      secs_data_GPU_to_CPU = 0.0;
		      secs_device_execution = 0.0;
		      total_secs_kernel_creation = 0.0;
		      total_secs_kernel_compilation = 0.0;
		      total_secs_data_CPU_to_GPU = 0.0;
		      total_secs_data_GPU_to_CPU = 0.0;
		      total_secs_device_execution = 0.0;
    );
  }
  
  /** the first call returns 'a', then 'b', 'c', ..., 'z', 'aa', 'ab', etc.
   
    @todo If not building, just return "_" (unless in DEBUG mode)
  */
  String_t GlobalState::getNewID()
  {
  
    String_t ret = nameGenerator_;
    
    for(String_t::size_type sz = nameGenerator_.size(); sz > 1; sz--) {
      char c = nameGenerator_[sz - 1];
      if(c < 'z') {
	nameGenerator_[sz - 1] = c + 1;
	break;
      } else {
	nameGenerator_[sz - 1] = 'a';
	if(sz == 2) nameGenerator_ += 'a';
      }
    }
    
    /* We detect if the name generated is a C++ keyword
    if(keywords_.find(nameGenerator_) != keywords_.end()){
      getNewID();
    }
    */
    
    return ret;
  }
  
  FHandle& GlobalState::getFHandle(void *f)
  {
    if(!nest_ && (Codifier::PreVars[0] == 0))
      fill_codifier_PreVars();
    
    FHandle *ret = fStorage_[f];
    
    if (!ret) {
      ret = new FHandle();
      fStorage_[f] = ret;
    }
    
    return *ret;
  }

  FHandle& GlobalState::getNativeFHandle(void *f, const String_t& name, const String_t& native_code)
  {
    if(!nest_ && (Codifier::PreVars[0] == 0))
      fill_codifier_PreVars();
    
    FHandle *ret = fStorage_[f];
    
    if (!ret) {
      ret = new FHandle(name, can_be_code(native_code) ? native_code : read_string_from_file(native_code));
      fStorage_[f] = ret;
    } else {
      throw HPLException(0, "Exception thrown in GlobalState::getNativeFHandle : Attempt to redefine a native handle");
    }
    
    return *ret;
  }
  
  void GlobalState::deleteFHandle(void *f)
  {
    // The erase method does not call to delete if the element stored in the map is an pointer to object
    FHandleStorage_t::iterator it = fStorage_.find(f);
    
    if(it != fStorage_.end()) {
      FHandle *tmp_fhandle = (*it).second;
      assert( tmp_fhandle != 0 );
      delete tmp_fhandle;
      fStorage_.erase(it);
    }
  }

  ArrayInfo *GlobalState::getArrayInfo(const BaseArray *bArr)
  {
    ArrayInfo *arrayinfo = arrayInfoMap_[bArr->name()];

    if (!arrayinfo) {
      arrayinfo = new ArrayInfo();
      arrayInfoMap_[bArr->name()] = arrayinfo;
    }
    
    return arrayinfo;
  }

  Codifier& GlobalState::getCodifier()
  {
    return codifierArray_[nest_];
  }
  
  void GlobalState::addCodifierNest()
  {
    nest_++;
    Codifier* c = new Codifier();
    codifierArray_.push_back(*c);
  }

  void GlobalState::subCodifierNest()
  {
    nest_--;
    codifierArray_.pop_back();
  }

  int GlobalState::getNumberOfFHandles()
  {
    return static_cast<int>(fStorage_.size());
  }

  void GlobalState::dumpCode(const String_t& code)
  { String_t aux;
    char conc[100];
    
    for(int i = 0; i < PRAGMAS_NUMBER; i++) {
      if(pragmas[i]==1) {
	sprintf(conc, "#pragma OPENCL EXTENSION %s : enable\n", pragmas_text[i]);
	aux += conc;
      }
    }
    aux += code;
    
    if( env.hpl_kernel_file_enable )
      clbinding_.kernelfile <<  aux << std::endl;
    else
      clbinding_.allCode += aux; // there may be more than inter function calls
  }
  
  void GlobalState::resetCode()
  {
    FHandleStorage_t::const_iterator const it_end = fStorage_.end();
    for(FHandleStorage_t::const_iterator it = fStorage_.begin(); it != it_end; ++it) {
      FHandle *tmp_fhandle = (*it).second;
      assert(tmp_fhandle != 0);
      delete tmp_fhandle;
    }
    fStorage_.clear();
    
    clbinding_.resetCode();
  }
  
  void GlobalState::registerStructInfo(StructInfo *sip)
  { static const String_t str("typedef struct ");
    char tmp[128];
    
    const char * const struct_name = sip->name();
    
    StructName2InfoMap_t::const_iterator cit = structInfoMap_.find(struct_name);
    
    if(cit == structInfoMap_.end()) {
      structInfoMap_.insert(std::pair<const char *, StructInfo *>(struct_name, sip));
      sprintf(tmp, " %s;\n", struct_name);
      if(utils::findWord((str + sip->body()) + tmp, "double"))
         addPragma(GlobalState::CL_KHR_FP64);
      dumpCode((str + sip->body()) + tmp);
    }
  }
  
  void GlobalState::checkStructField(const char *struct_name, const char *field_name)
  {
    StructName2InfoMap_t::const_iterator cit = structInfoMap_.find(struct_name);
    assert(cit != structInfoMap_.end());
    assert((*cit).second->existsField(field_name));
  }
  
  ProfilingData GlobalState::getProfile() const 
  { 
    HPL_PROFILEACTION(return ProfilingData(secs_kernel_creation, 
					   secs_kernel_compilation, 
					   secs_data_CPU_to_GPU,
					   secs_data_GPU_to_CPU,
					   secs_device_execution));
    return ProfilingData();
  }
  
  ProfilingData GlobalState::getTotalProfile() const 
  { 
    HPL_PROFILEACTION(return ProfilingData(total_secs_kernel_creation, 
					   total_secs_kernel_compilation, 
					   total_secs_data_CPU_to_GPU,
					   total_secs_data_GPU_to_CPU,
					   total_secs_device_execution));
    return ProfilingData();
  }
  
  GlobalState& TheGlobalState()
  { static GlobalState GS;
    
    return GS;
  }

  GlobalState::StructInfo::StructInfo(std::size_t sz, const char *name, const char *body)
  : sz_(sz), name_(name), body_(body)
  {
    char * working = strdup(body_);
    assert(working);
    
    char * wp = working;
    
    do {
      
      char * p = strchr(wp, ';');
      wp = p + 1;
      
      if(!p) break;
      
      do { --p; } while (utils::notInWord(*p));
      *(p + 1) = 0;
      
      do { --p; } while (!utils::notInWord(*p));
      const char * const field_name = strdup(p + 1);
      
      do { --p; } while (utils::notInWord(*p));
      *(p + 1) = 0;
      
      do { --p; } while (!utils::notInWord(*p));
      const char *  const type_name = strdup(p + 1);
      
      //std::cout<< '[' << type_name << "][" << field_name << "]\n";
      
      keywords_.insert(std::pair<const char *, const char *>(field_name, type_name));
      
    } while(1);
    
    delete working;
    
    TheGlobalState().registerStructInfo(this);
  }
  
  bool GlobalState::StructInfo::existsField(const char *f) const
  {
    Field2TypeMapper_t::const_iterator cit = keywords_.find(f);
    return (cit != keywords_.end());
  }

}
