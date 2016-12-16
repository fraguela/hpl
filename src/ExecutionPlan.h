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
/// \file     ExecutionPlan.h
/// \author   Moisés Viñas        <moises.vinas@udc.es>
/// \author   Zeki Bozkus         <zeki.bozkus@khas.edu.tr>
/// \author   Basilio B. Fraguela <basilio.fraguela@udc.es>
///

#ifndef EXECUTIONPLAN_H
#define EXECUTIONPLAN_H

#include <cmath>
#include <numeric>

namespace HPL {
  
  enum TypeProf {
    FULL = 0,
    PARTIAL = 1
  };
  
  enum SearchAlg {
    NOSEARCH = 0,
    EXHAUS = 1,
    ADAPT = 2 ,
    ADAPT2 = 3
  };
  
  // Forwarded from HPL.h
  unsigned int getDeviceNumber(Device_t type_n);
  
  class ExecutionPlan {

    std::vector<Device> devices_initial; ///< Set of devices initially provided by the user
    std::vector<Device> devices;         ///< Set of devices to use for the executions
    std::vector<int> weights;            ///< Percentage of work for each device in ::devices
    
    void (*f_)();                        ///< Partitioner
    SearchAlg sa_;                       ///< Search algorithm
    TypeProf tp_;                        ///< Kind of profiling for search algorithms based on measurements
    
  public:
    
    template<typename... Args>
    ExecutionPlan(void (&f)(FRunner& fr, Tuple* range, Args&... args), SearchAlg sa = NOSEARCH, TypeProf tp = FULL) : f_((void (*)())f), sa_(sa), tp_(tp)
    { }
    
    ExecutionPlan(SearchAlg sa = NOSEARCH, TypeProf tp = FULL): f_(nullptr), sa_(sa), tp_(tp)
    { }
    
    template<typename... Args>
    ExecutionPlan(void (&f)(FRunner& fr, Tuple* range, Args&... args),
                  const std::vector<Device>& device_vector,
                  SearchAlg sa = NOSEARCH,
                  TypeProf tp = FULL) :
    devices_initial(device_vector),
    f_((void (*)())f),
    sa_(sa),
    tp_(tp)
    {
      equal_weights();
    }
    
    /// Adds a single device with a specified weight
    void add(Device d, int weight)
    {
      devices.push_back(d);
      weights.push_back(weight);
      devices_initial.push_back(d);
    }
    
    /// Set as list of devices the provided vector, defaulting to a uniform distribution of work
    void setDevices(const std::vector<Device>& device_vector)
    {
      devices_initial = device_vector;
      equal_weights();
    }
    
    /// Set as list of devices the ones of the provided type, defaulting to a uniform distribution of work
    void setDevices(const Device_t devt)
    {
      const unsigned int ndevices = getDeviceNumber(devt);
      devices_initial.clear();
      for(unsigned int i = 0; i < ndevices; i++) {
        devices_initial.push_back(Device(devt, i));
      }
      equal_weights();
    }
    
    /// Copy the devices and distribution of work of another ExecutionPlan
    void copyDistribution(const ExecutionPlan& otherExecutionPlan) {
      devices_initial = otherExecutionPlan.devices_initial;
      devices = otherExecutionPlan.devices;
      weights = otherExecutionPlan.weights;
    }
    
    /// How many devices will be used in the execution
    int getNumDevicesInUse() const {
      return devices.size();
    }
    
    template<typename... Args>
    void setPlanner(void (&f)(FRunner& fr, Tuple* range, Args&... args))
    {
      f_ = (void (*)())f;
    }
    
    template<typename... Args>
    void applyTo(Args&... args)
    {
      typedef void (*f_type)(Args&... args);
      
      f_type f = (f_type)f_;
      (*f)(args...);
    }
    
    const std::vector<int>& getWeights() const { return weights; }
    
    friend class FRunner;
    
  private:
    
    /// Distributes equally the work among the devices_initial
    void equal_weights()
    {
      const int num_devices = devices_initial.size();
      std::vector<int> v(num_devices);
      int accum_work = 0;
      for(int i = 0; i < num_devices; i++)
      {
        if(i != (num_devices - 1))
        {
          int work = 100/num_devices;
          v[i] = work;
          accum_work += work;
        }
        else
        {
          int work = 100 - accum_work;
          v[i] = work;
        }
      }
      
      devices.clear();
      weights.clear();
      for(int i = 0; i < num_devices; i++)
      {
        if(v[i] != 0){ weights.push_back(v[i]); devices.push_back(devices_initial[i]); }
      }
      
      
    }
    
    /// Each time it is called it returns a new distribution for the exhaustive search in v.
    /// It returns false when it has generated the last combination
    bool series(std::vector<int>& v, int step = 5)
    { static bool init = true;
      
      
      if (init) { // Generate 0 0 ... 0 100
        int i = v.size() - 1 ;
        v[i] = 100;
        while( i ) {
          v[--i] = 0;
        }
        init = false;
      } else
        if (v[0] == 100) {
          init = true;
          return false;
        } else {
          do {
            int i = v.size() - 1 ;
            while (v[i] == 100) {
              v[i] = 0;
              i--;
            }
            v[i] += step;
          } while(std::accumulate(v.begin(), v.end(), 0) != 100);
        }
      
      ////set devices_initial
      devices.clear();
      weights.clear();
      for(unsigned int i = 0; i < v.size(); i++)
      {
        if(v[i] != 0){ weights.push_back(v[i]); devices.push_back(devices_initial[i]); }
      }
      return true;
    }
    
    void setFinal(std::vector<int>& v)
    {
      weights.clear();
      devices.clear();
      for(unsigned int i = 0; i < v.size(); i++)
      {
        if(v[i]>0){ devices.push_back(devices_initial[i]); weights.push_back(v[i]);}
      }
      
      sa_ = NOSEARCH;
    }
    
    void setFinal()
    {
      sa_ = NOSEARCH;
    }
    
    void setFinal(std::vector<int>& v, std::vector<Device>& dev)
    {
      weights.clear();
      devices.clear();
      for(unsigned int i = 0; i < v.size(); i++)
      {
        if(v[i]>0){ devices.push_back(dev[i]); weights.push_back(v[i]);}
      }
      
      sa_ = NOSEARCH;
    }
    
    void divideExecPlan(Tuple *regionout, int *nd_global_, int* nd_local_)
    { int /*tiles[3],*/ lastpoint[3], dividerdim = 0;
      float tiles[3];
      Tuple region[3];
      
      nd_local_[0] = (nd_local_[0]==0 ? 1 : nd_local_[0]);
      nd_local_[1] = (nd_local_[1]==0 ? 1 : nd_local_[1]);
      nd_local_[2] = (nd_local_[2]==0 ? 1 : nd_local_[2]);
      
      for(int i = 0; i < 3; i++) {
        tiles[i] = (nd_global_[0]+0.0f) / (nd_local_[0]+0.0f);
        if(tiles[i] > 1 && !dividerdim) {
          dividerdim = i + 1;
          lastpoint[i] = -1;
        } else
          lastpoint[i] = nd_global_[i] - 1;
        
        region[i].origin = 0;
        region[i].end = lastpoint[i];
        
      }
      for(unsigned int i = 0; i < devices.size(); i++)
      {
        regionout[i*3+0] = region[0];
        regionout[i*3+1] = region[1];
        regionout[i*3+2] = region[2];
      }
      if(!dividerdim)
      {
        //   return;
        //   process(region);
        //std::cout << "procesa primera region\n";
      }
      else {
        float tweight = 0.f;
        
        for(unsigned int i = 0; i < devices.size(); i++)
          tweight += weights[i];
        dividerdim--;
        for(unsigned int i = 0; (i < devices.size()) && (lastpoint[dividerdim] < nd_global_[dividerdim] - 1); i++) {
          float rweight = weights[i] / tweight;
          int endregion = lastpoint[dividerdim] + 1 + nd_local_[dividerdim] * ceilf(tiles[dividerdim] * rweight) - 1;
          if(endregion >= nd_global_[dividerdim])
            endregion = nd_global_[dividerdim] - 1;
          
          regionout[i*3+dividerdim].origin = lastpoint[dividerdim]+1;
          regionout[i*3+dividerdim].end = endregion;// = region[dividerdim];
          
          lastpoint[dividerdim] = endregion;
        }
      }
    }
    
  };
  
}

#endif

