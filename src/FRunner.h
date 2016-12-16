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
/// \file     FRunner.h
/// \author   Moisés Viñas        <moises.vinas@udc.es>
/// \author   Zeki Bozkus         <zeki.bozkus@khas.edu.tr>
/// \author   Basilio B. Fraguela <basilio.fraguela@udc.es>
///

#ifndef FRUNNER_H_
#define FRUNNER_H_

#include "GlobalState.h"
#include "FHandle.h"
#include "Tuple.h"
#include <type_traits>
#include "ExecutionPlan.h"
#include <algorithm>

//#define FST(arr) arr.setDistrib(D_FST)
//#define SND(arr) arr.setDistrib(D_SND)
//#define TRD(arr) arr.setDistrib(D_TRD)

#define FST(arr, ...)  arr.setDistrib(D_FST, ##__VA_ARGS__)
#define SND(arr, ...)  arr.setDistrib(D_SND, ##__VA_ARGS__)
#define TRD(arr, ...)  arr.setDistrib(D_TRD, ##__VA_ARGS__)

namespace HPL {


/// Represents a domain for kernel execution, akin cl::NDRange
struct Domain {

	Domain(int a) {
		sizes_[0] = a;
		sizes_[1] = 1;
		sizes_[2] = 1;
	}

	Domain(int a, int b) {
		sizes_[0] = a;
		sizes_[1] = b;
		sizes_[2] = 1;
	}

	Domain(int a, int b, int c) {
		sizes_[0] = a;
		sizes_[1] = b;
		sizes_[2] = c;
	}

	Domain& operator= (const Domain& other) {
		sizes_[0] = other.sizes_[0];
		sizes_[1] = other.sizes_[1];
		sizes_[2] = other.sizes_[2];
		return *this;
	}

	int& operator[] (int i) { return sizes_[i]; }

	int operator[] (int i) const { return sizes_[i]; }

	int sizes_[3];

};

/// Information kept by the library for functions that are been executing.
/// It can be seen as a relation between a kernel and a device.
class FRunner {

	/// Helper function to get the size of scalars
	template<int DIM, typename T>
	int getSizeDim(const T& v) {
		//      static_assert(std::is_arithmetic<T>::value, "argument to eval is neither an arithmetic scalar nor an Array");
		return 1;
	}

	template<int DIM, typename T>
	int getSizeDim(const T* v) {
		return 1;
	}

	template<int DIM, typename T>
	int getSizeDim(const std::vector<T*>& v) {
		return 1;
	}

	/// Helper function to get the size of Arrays
	template<int DIM, typename T0, int NDIM0, ArrayFlags AF0>
	int getSizeDim(Array<T0, NDIM0, AF0>* v) { return v->getXYZDim(DIM);}//getDimension(DIM); }

	/// Helper function to get the size of Arrays
	template<int DIM, typename T0, int NDIM0, ArrayFlags AF0>
	int getSizeDim(const Array<T0, NDIM0, AF0>& v) { return v.getXYZDim(DIM);}//getDimension(DIM); }

	/// Helper function to get the size of Arrays
	template<int DIM, typename T0, int NDIM0, ArrayFlags AF0>
	int getSizeDim(const std::vector<Array<T0, NDIM0, AF0>*>& v) { return v[0]->getXYZDim(DIM); }

public:

	/**
	 * @brief Constructs a FRunner associated to a HPL::FHandle.
	 * @param[in] _fhandle The reference to the kernel that will be executed.
	 */
	FRunner(FHandle* ifhandle = NULL);

	/**
	 * @brief Obtains the FHandle associated.
	 */
	FHandle* getFhandle() { return fhandle; }

	const Device& getDevice() const { return device_[0]; }

	Device& getDevice() { return device_[0]; }

	/** @name NDrange getters.
	 * They retrieve the ND range values local or global.
	 */
	///@{
	int getNdGlobalX() { return nd_global[0][0]; }

	int getNdGlobalY() { return nd_global[0][1]; }

	int getNdGlobalZ() { return nd_global[0][2]; }

	int getNdLocalX() { return nd_local[0][0]; }

	int getNdLocalY() { return nd_local[0][1]; }

	int getNdLocalZ() { return nd_local[0][2]; }

	///@}

	/// Overloaded global() makes global thread planes for NDRANGE
	FRunner& global(unsigned int x)
	{
		nd_global[0][0] = x;
		nd_global[0][1] = 1;
		nd_global[0][2] = 1;
		return *this;
	}

	FRunner& global(unsigned int x, unsigned int y)
	{
		nd_global[0][0] = x;
		nd_global[0][1] = y;
		nd_global[0][2] = 1;
		return *this;
	}

	FRunner& global(unsigned int x, unsigned int y, unsigned  z)
	{
		nd_global[0][0] = x;
		nd_global[0][1] = y;
		nd_global[0][2] = z;
		return *this;
	}

	FRunner& global(const Domain& dom) { nd_global[0] = dom; return *this; }

	/// Overloaded local() makes local thread planes for NDRANGE
	FRunner& local(unsigned int x)
	{
		nd_local[0][0] = x;
		nd_local[0][1] = 1;
		nd_local[0][2] = 1;
		return *this;
	}

	FRunner& local(unsigned int x, unsigned int y)
	{
		nd_local[0][0] = x;
		nd_local[0][1] = y;
		nd_local[0][2] = 1;
		return *this;
	}

	FRunner& local(unsigned int x, unsigned int y, unsigned  z)
	{
		nd_local[0][0] = x;
		nd_local[0][1] = y;
		nd_local[0][2] = z;
		return *this;
	}

	FRunner& local(const Domain& dom) { nd_local[0] = dom; return *this; }

	/**
	 * @brief Sets the device to this FRunner.
	 * @param[in] device that will run the kernel.
	 */
	FRunner& device(const HPL::Device& device) { device_[0] = device; return *this; }

	FRunner& device(HPL::Platform_t platform_id, HPL::Device_t device_t, int device_n = 0) {
		device_[0] = HPL::Device(platform_id, device_t, device_n);
		return *this;
	}

	FRunner& device(HPL::Device_t device_t, int device_n = 0) {
		device_[0] = HPL::Device(device_t, device_n);
		return *this;
	}

	FRunner& device(const std::vector<HPL::Device>& device)
	{
		is_multifrunner = 1;
		device_ = device;
		return *this;
	}

	FRunner& local(const std::vector<HPL::Domain>& local)
	{
		is_multifrunner = 1;
		nd_local = local;
		return *this;
	}

	FRunner& global(const std::vector<HPL::Domain>& global)
	{
		is_multifrunner = 1;
		nd_global = global;
		return *this;
	}

	bool test_pointer() { return false;}

	template<typename T, typename... Args>
	bool test_pointer(std::vector<T*>& arg0, Args&&... args) {
		return true;
	}

	template<typename T, typename... Args>
	bool test_pointer(std::vector<Array<T, 0>*>& arg0, Args&&... args) {
		return true;
	}

	template<typename T0, int NDIM0, ArrayFlags AF0, typename... Args>
	bool test_pointer(std::vector<Array<T0,NDIM0,AF0>*>& arg0, Args&&... args) {
		return true;
	}

	template<typename T, typename... Args>
	bool test_pointer(T& arg0, Args&&... args) {
		return (test_pointer(args...));
	}

	template<typename T, typename... Args>
	bool test_pointer(Array<T, 0>& arg0, Args&&... args) {
		return (test_pointer(args...));
	}

	template<typename T0, int NDIM0, ArrayFlags AF0, typename... Args>
	bool test_pointer(Array<T0,NDIM0,AF0>& arg0, Args&&... args) {
		if(arg0.getDistrib()!=D_NONE) return true;
		return (test_pointer(args...));
	}

	template<typename T, typename... Args>
	bool test_pointer(T* arg0, Args&&... args) {
		return true;
	}

	template<typename T, typename... Args>
	bool test_pointer(Array<T, 0>* arg0, Args&&... args) {
		return true;
	}

	template<typename T0, int NDIM0, ArrayFlags AF0, typename... Args>
	bool test_pointer(Array<T0,NDIM0,AF0> * arg0, Args&&... args) {
		return true;
	}

	/* The distribution information is stored in
       each array so when the FRunner execution finishes,
       we must disable the distribution information */
       void unset_distrib() { }

	template<typename T, typename... Args>
	void unset_distrib(T&& arg0, Args&&... args) {
		unset_distrib(args...);
	}

	template<typename T0, int NDIM0, ArrayFlags AF0, typename... Args>
	void unset_distrib(Array<T0,NDIM0,AF0>& arg0, Args&&... args) {
		arg0.setDistrib(D_NONE);
		unset_distrib(args...);
	}
  
	template<typename T0, ArrayFlags AF0, typename... Args>
	void unset_distrib(Array<T0,0,AF0>& arg0, Args&&... args)
	{
		unset_distrib(args...);
	}

	template<typename T, int NDIM, ArrayFlags AF0, typename... Args>
	void killChildren(Array<T,NDIM,AF0>& arg0, Args&&... args)
	{
		arg0.killChildren();
		killChildren(args...);
	}
  
	template<typename T, int NDIM, ArrayFlags AF0, typename... Args>
	void killChildren(Array<T,NDIM,AF0>& arg0)
	{
		arg0.killChildren();
	}
  
    template<typename T, typename... Args>
    void killChildren(T& arg0, Args&&... args)
    {
      killChildren(args...);
    }
  
    template<typename T>
    void killChildren(T& arg0)
    {
    }

	template<typename T, int NDIM, ArrayFlags AF0, typename... Args>
	void createAuxiliarBuffers(std::vector<char*> &backup, int i, Array<T,NDIM,AF0>& arg0, Args&&... args)
	{
		if(NDIM > 0)
		{
		arg0.getData(HPL_WR);
		if(fhandle->fhandleTable[i].isWrite && (!fhandle->fhandleTable[i].isWriteBeforeRead))
		{
                  const size_t sz = arg0.getXYZDim(0) * arg0.getXYZDim(1) * arg0.getXYZDim(2) * sizeof(T);
			void* nuevo = malloc(sz);

			memcpy(nuevo, arg0.getData(HPL_WR), sz);
			backup.push_back((char*)nuevo);

		}
		else
			backup.push_back((char*)arg0.getData());
		}

		createAuxiliarBuffers(backup, i+1, args...);
	}
  
	template<typename T, int NDIM, ArrayFlags AF0, typename... Args>
	void createAuxiliarBuffers(std::vector<char*> &backup, int i, Array<T,NDIM,AF0>& arg0)
	{
		if(NDIM>0){
		arg0.getData(HPL_WR);
		if(fhandle->fhandleTable[i].isWrite && (!fhandle->fhandleTable[i].isWriteBeforeRead))
		{
                  const size_t sz = arg0.getXYZDim(0) * arg0.getXYZDim(1) * arg0.getXYZDim(2) * sizeof(T);
			void* nuevo = malloc(sz);

			memcpy(nuevo, arg0.getData(HPL_WR), sz);
			backup.push_back((char*)nuevo);
		}
		else
			backup.push_back((char*)arg0.getData());
		}
	}

	template<typename T, typename... Args>
	void createAuxiliarBuffers(std::vector<char*> &backup, int i, T& arg0, Args&&... args)
	{
		createAuxiliarBuffers(backup, i+1, args...);
	}

	template<typename T, typename... Args>
	void createAuxiliarBuffers(std::vector<char*> &backup, int i, T& arg0)
	{
	}

	template<typename T, int NDIM, ArrayFlags AF0, typename... Args>
	void removeAuxiliarBuffers(std::vector<char*> &backup, int i, Array<T,NDIM,AF0>& arg0, Args&&... args)
	{
		//  if(AF0==Local || NDIM==0) return;
		//  Timer tt;
		//  tt.start();
		arg0.getData(HPL_RDWR);

		if(fhandle->fhandleTable[i].isWrite && (!fhandle->fhandleTable[i].isWriteBeforeRead))
		{
			//std::cout << "se hace la copia\n";
			std::copy_n((T*)backup[i], arg0.getXYZDim(0) * arg0.getXYZDim(1) * arg0.getXYZDim(2), (T*)arg0.getData());
			free(backup[i]);
		}

		removeAuxiliarBuffers(backup, i+1, args...);
	}

	template<typename T, int NDIM, ArrayFlags AF0, typename... Args>
	void removeAuxiliarBuffers(std::vector<char*> &backup, int i, Array<T,NDIM,AF0>& arg0)
	{

		arg0.getData(HPL_RDWR);
		if(fhandle->fhandleTable[i].isWrite && (!fhandle->fhandleTable[i].isWriteBeforeRead))
		{
			std::copy_n((T*)backup[i], arg0.getXYZDim(0) * arg0.getXYZDim(1) * arg0.getXYZDim(2), (T*)arg0.getData());
			//    memcpy(arg0.getData(HPL_WR), backup[i], arg0.getXYZDim(0) * arg0.getXYZDim(1) * arg0.getXYZDim(2) * sizeof(T));
			free(backup[i]);
		}
		backup.clear();
	}

	template<typename T, ArrayFlags AF0, typename... Args>
	void removeAuxiliarBuffers(std::vector<char*> &backup, int i, Array<T,0,AF0>& arg0, Args&&... args)
	{
		removeAuxiliarBuffers(backup, i+1, args...);
	}

	template<typename T, ArrayFlags AF0, typename... Args>
	void removeAuxiliarBuffers(std::vector<char*> &backup, int i, Array<T,0,AF0>& arg0)
	{
		backup.clear();
	}

	template<typename T, int NDIM, typename... Args>
	void removeAuxiliarBuffers(std::vector<char*> &backup, int i, Array<T,NDIM,Local>& arg0, Args&&... args)
	{
		removeAuxiliarBuffers(backup, i+1, args...);
	}

	template<typename T, int NDIM, typename... Args>
	void removeAuxiliarBuffers(std::vector<char*> &backup, int i, Array<T,NDIM,Local>& arg0)
	{
		backup.clear();
	}

	template<typename T, typename... Args>
	void removeAuxiliarBuffers(std::vector<char*> &backup, int i, T& arg0, Args&&... args)
	{
		removeAuxiliarBuffers(backup, i+1, args...);
	}

	template<typename T, typename... Args>
	void removeAuxiliarBuffers(std::vector<char*> &backup, int i, T& arg0)
	{
		backup.clear();
	}

	template<typename T, int NDIM, ArrayFlags AF0, typename... Args>
	void traerAHost(Array<T,NDIM,AF0>& arg0, Args&&... args)
	{
		if(NDIM>0)
		arg0.getData(HPL_RDWR);
		traerAHost(args...);
	}
	template<typename T, int NDIM, ArrayFlags AF0>
	void traerAHost(Array<T,NDIM,AF0>& arg0)
	{
		if(NDIM>0)
		arg0.getData(HPL_RDWR);
	}

	template<typename T, typename... Args>
	void traerAHost(T& arg0, Args&&... args)
	{
		traerAHost(args...);
	}

	template<typename T>
	void traerAHost(T& arg0)
	{
	}

	std::vector<int> balance(const std::vector<double>& mytimes)
    		{
		std::vector<double> res(mytimes.size());
		std::vector<int> ret;

		//get slowest time
		const double mmax = *(std::max_element(mytimes.begin(), mytimes.end()));
		//std::cout << mmax << std::endl;

		//slowest device can process 1 unit. Device that works in half the time can process 2 units, etc.
		std::transform(mytimes.begin(), mytimes.end(), res.begin(), [mmax](double t) { return mmax/t; });

		//basic units that can be processed by all the devices together
		const double total_units = std::accumulate(res.begin(), res.end(), 0.);

		//scale to range 0-100 so that the addition of all the percetages in res is 100
		std::for_each(res.begin(), res.end(), [total_units](double &t) { t = t * 100.0/total_units; });
		int accum = 0;
		for(unsigned int i = 0; i < res.size(); i++)
		{
			ret.push_back((int)floor( res[i] + 0.5 ));
			accum += ret[i];
		}
		if(accum!=100) ret[res.size()-1] += (100-accum);
		//this is only for seeing the result
//		std::for_each(res.begin(), res.end(), [] (double r) {std::cout << r << ' ';}); std::cout << std::endl;

		return ret;
    		}

	std::vector<int> adaptive_balance(const std::vector<double>& mytimes, const std::vector<int>& weights)
    		{
		std::vector<double> new_times(mytimes.size());

		for (unsigned int i =0; i < mytimes.size(); ++i) {
			new_times[i] = mytimes[i] * (100./mytimes.size()) / weights[i];
		}

		return balance(new_times);
    		}

	bool adjust_adapt(ExecutionPlan* exec_plan_, std::vector<int> &best_weights, std::vector<double> &times)
	{
		int adjust = 0;
		std::vector<Device> devices_backup;
		std::vector<int> weights_backup;
		for(unsigned int i = 0; i < best_weights.size(); i++)
		{

			if((times[i]*best_weights[i])/exec_plan_->weights[i] < 0.005 && best_weights[i]<5)
			{
				adjust += best_weights[i];
				best_weights[i] = 0;
			}
			else
				devices_backup.push_back(exec_plan_->devices[i]);
		}
		for(unsigned int i = 0; i < best_weights.size(); i++)
		{
			if(best_weights[i]!=0)
				weights_backup.push_back((int)floor(best_weights[i]*100.0/(100.0-adjust)+0.5));
		}
		if(best_weights.size()!=weights_backup.size()) {
			std::cout << "INFO: Weights: " << std::endl;
			std::for_each(weights_backup.begin(), weights_backup.end(), [](int t){std::cout << t << ", ";});
			std::cout << std::endl;
			exec_plan_->setFinal(weights_backup, devices_backup);
			return true;
		}
		else {
			std::cout << "INFO: Weights: ";
			std::for_each(best_weights.begin(), best_weights.end(), [](int t){std::cout << t << ", ";});
			std::cout << std::endl;
			exec_plan_->setFinal(best_weights);
			return false;
		}
	}


	template<typename T, typename... Args>
	FRunner& operator() (T&& arg0, Args&&... args)
	{
		if(exec_plan_ != NULL)
		{
			///CHOOSING WEIGHTS STARTS
			switch(exec_plan_->sa_)
			{
			case HPL::NOSEARCH :
			{
				break;
			}
			case HPL::EXHAUS :
			{
				double tiempo_mejor = 100000.0, tiempo_actual = 0.0; //numero alto
				std::vector<char*> punteros;

				std::vector<int> weights(exec_plan_->devices.size()), best_weights(exec_plan_->devices.size());
				while(exec_plan_->series(weights))
				{
					int j = 0;
					createAuxiliarBuffers(punteros,j, arg0, args...);

					Range *ranges = static_cast<Range *>(alloca(sizeof(Range) * exec_plan_->devices.size() * 3));
					exec_plan_->divideExecPlan(ranges, nd_global[0].sizes_, nd_local[0].sizes_);
					Timer t0;

					for(int kk = 0; kk < 2; kk++)
					{
						if(kk == 1) t0.start();
						for(int i = 0; i < exec_plan_->devices.size(); i++)
						{

							FRunner fr(fhandle);
							fr.local(nd_local[0][0], nd_local[0][1], nd_local[0][2]).device(exec_plan_->devices[i]);

							if((ranges[i*3+2].end)!=0)
							{
								fr.global(nd_local[0][0]*((ranges[i*3+2].end - ranges[i*3+2].origin + 1+(nd_local[0][0]-1))/nd_local[0][0]),
										nd_local[0][1]*((ranges[i*3+1].end - ranges[i*3+1].origin + 1+(nd_local[0][1]-1))/nd_local[0][1]),
										nd_local[0][2] * ((ranges[i*3+0].end - ranges[i*3+0].origin + 1 + (nd_local[0][2]-1))/nd_local[0][2]));
							}
							else
							{
								if((ranges[i*3+1].end)!=0)
								{
									fr.global((nd_local[0][0])*((ranges[i*3+1].end - ranges[i*3+1].origin + 1 + (nd_local[0][0]-1))/nd_local[0][0]),
											nd_local[0][1]*((ranges[i*3+0].end - ranges[i*3+0].origin + 1+(nd_local[0][1]-1))/nd_local[0][1]),
											ranges[i*3+2].end - ranges[i*3+2].origin + 1);
								}
								else
								{
									fr.global(nd_local[0][0]*((ranges[i*3].end - ranges[i*3].origin + 1+(nd_local[0][0]-1))/nd_local[0][0]),
											ranges[i*3+1].end - ranges[i*3+1].origin + 1, ranges[i*3+2].end - ranges[i*3+2].origin + 1);
								}
							}

							exec_plan_->applyTo(fr, ranges[i*3], arg0, args...);

						}
						if(kk==0)
						{
							for(int iii = 0; iii < exec_plan_->devices.size(); iii++)
								exec_plan_->devices[iii].sync();
							if(exec_plan_->tp_ == FULL)
								traerAHost(arg0, args...);
						}
					}

					if(exec_plan_->tp_ != FULL)
					{
					        for(int iii = 0; iii < exec_plan_->devices.size(); iii++)
					               exec_plan_->devices[iii].sync();
					        tiempo_actual = t0.stop();
					}


					j = 0;
					removeAuxiliarBuffers(punteros, j, arg0, args...);

					if(exec_plan_->tp_ == FULL)
					tiempo_actual = t0.stop();

					if(tiempo_actual < tiempo_mejor)
					{
						best_weights.clear();
						for(int iii = 0; iii < weights.size(); iii++) best_weights.push_back(weights[iii]);
						tiempo_mejor = tiempo_actual;
					}

                    std::cout << "INFO: Time " << tiempo_actual << ", Weights: ";
                          std::for_each(weights.begin(), weights.end(), [](int t){std::cout << t << ", ";});
                    std::cout << std::endl;

					killChildren(arg0, args...);
				}
				exec_plan_->setFinal(best_weights);
				break;
			}
			case HPL::ADAPT :
			{

				//double tiempo_mejor = 100000.0, tiempo_actual = 0.0; //numero alto
				std::vector<char*> punteros;
				std::vector<double> times;
				std::vector<int> weights(exec_plan_->devices.size());
				exec_plan_->equal_weights();

				{
					int j = 0;
					createAuxiliarBuffers(punteros,j, arg0, args...);

					Range *ranges = static_cast<Range *>(alloca(sizeof(Range) * exec_plan_->devices.size() * 3));
					exec_plan_->divideExecPlan(ranges, nd_global[0].sizes_, nd_local[0].sizes_);
					Timer t0;

					for(int kk = 0; kk < 2; kk++)
					{
						for(int i = 0; i < exec_plan_->devices.size(); i++)
						{
							if(kk == 1) t0.start();
							FRunner fr(fhandle);
							fr.local(nd_local[0][0], nd_local[0][1], nd_local[0][2]).device(exec_plan_->devices[i]);

							if((ranges[i*3+2].end)!=0)
							{
								fr.global(nd_local[0][0]*((ranges[i*3+2].end - ranges[i*3+2].origin + 1+(nd_local[0][0]-1))/nd_local[0][0]),
										nd_local[0][1]*((ranges[i*3+1].end - ranges[i*3+1].origin + 1+(nd_local[0][1]-1))/nd_local[0][1]),
										nd_local[0][2] * ((ranges[i*3+0].end - ranges[i*3+0].origin + 1 + (nd_local[0][2]-1))/nd_local[0][2]));
							}
							else
							{
								if((ranges[i*3+1].end)!=0)
								{
									fr.global((nd_local[0][0])*((ranges[i*3+1].end - ranges[i*3+1].origin + 1 + (nd_local[0][0]-1))/nd_local[0][0]),
											nd_local[0][1]*((ranges[i*3+0].end - ranges[i*3+0].origin + 1+(nd_local[0][1]-1))/nd_local[0][1]),
											ranges[i*3+2].end - ranges[i*3+2].origin + 1);
								}
								else
								{
									fr.global(nd_local[0][0]*((ranges[i*3].end - ranges[i*3].origin + 1+(nd_local[0][0]-1))/nd_local[0][0]),
											ranges[i*3+1].end - ranges[i*3+1].origin + 1, ranges[i*3+2].end - ranges[i*3+2].origin + 1);
								}
							}

							exec_plan_->applyTo(fr, ranges[i*3], arg0, args...);
							if(kk==1){
								exec_plan_->devices[i].sync();
								if(exec_plan_->tp_ == FULL)
									traerAHost(arg0, args...);
								times.push_back(t0.stop());
							}

						}
						if(kk==0)
						{
							for(int iii = 0; iii < exec_plan_->devices.size(); iii++)
								exec_plan_->devices[iii].sync();
							if(exec_plan_->tp_ == FULL)
							traerAHost(arg0, args...);
						}
					}

					j = 0;
					removeAuxiliarBuffers(punteros, j, arg0, args...);
				}
				std::vector<int> best_weights = balance(times);
				////////////AJUSTE
				adjust_adapt(exec_plan_, best_weights, times);

				break;
			}
			case HPL::ADAPT2 :
			{

				//double tiempo_mejor = 100000.0, tiempo_actual = 0.0; //numero alto
				std::vector<char*> punteros;
				std::vector<double> times;
				double umbral = 100.0;
				int iteraciones = 0;
				std::vector<float> weights_previos(exec_plan_->devices.size());
				while(1)
				{
					int j = 0;
					createAuxiliarBuffers(punteros,j, arg0, args...);

					Range *ranges = static_cast<Range *>(alloca(sizeof(Range) * exec_plan_->devices.size() * 3));
					exec_plan_->divideExecPlan(ranges, nd_global[0].sizes_, nd_local[0].sizes_);
					Timer t0;

					for(int kk = 0; kk < 2; kk++)
					{
						for(int i = 0; i < exec_plan_->devices.size(); i++)
						{
							if(kk == 1) t0.start();
							FRunner fr(fhandle);
							fr.local(nd_local[0][0], nd_local[0][1], nd_local[0][2]).device(exec_plan_->devices[i]);

							if((ranges[i*3+2].end)!=0)
							{
								fr.global(nd_local[0][0]*((ranges[i*3+2].end - ranges[i*3+2].origin + 1+(nd_local[0][0]-1))/nd_local[0][0]),
										nd_local[0][1]*((ranges[i*3+1].end - ranges[i*3+1].origin + 1+(nd_local[0][1]-1))/nd_local[0][1]),
										nd_local[0][2] * ((ranges[i*3+0].end - ranges[i*3+0].origin + 1 + (nd_local[0][2]-1))/nd_local[0][2]));
							}
							else
							{
								if((ranges[i*3+1].end)!=0)
								{
									fr.global((nd_local[0][0])*((ranges[i*3+1].end - ranges[i*3+1].origin + 1 + (nd_local[0][0]-1))/nd_local[0][0]),
											nd_local[0][1]*((ranges[i*3+0].end - ranges[i*3+0].origin + 1+(nd_local[0][1]-1))/nd_local[0][1]),
											ranges[i*3+2].end - ranges[i*3+2].origin + 1);
								}
								else
								{
									fr.global(nd_local[0][0]*((ranges[i*3].end - ranges[i*3].origin + 1+(nd_local[0][0]-1))/nd_local[0][0]),
											ranges[i*3+1].end - ranges[i*3+1].origin + 1, ranges[i*3+2].end - ranges[i*3+2].origin + 1);
								}
							}

							exec_plan_->applyTo(fr, ranges[i*3], arg0, args...);
							if(kk==1){
								exec_plan_->devices[i].sync();
								if(exec_plan_->tp_ == FULL)
								traerAHost(arg0, args...);
								times.push_back(t0.stop());
							}

						}
						if(kk==0)
						{
							for(int iii = 0; iii < exec_plan_->devices.size(); iii++)
								exec_plan_->devices[iii].sync();
							if(exec_plan_->tp_ == FULL)
							traerAHost(arg0, args...);
						}
					}
					j = 0;
					removeAuxiliarBuffers(punteros, j, arg0, args...);


					double maximo = *(std::max_element(times.begin(), times.end()));
					double minimo = *(std::min_element(times.begin(), times.end()));
					umbral = (maximo-minimo)/maximo;

					killChildren(arg0, args...);

					if(umbral < 0.05) break;
                    weights_previos.clear();
                    weights_previos.insert(weights_previos.end(), exec_plan_->weights.begin(), exec_plan_->weights.end());
					std::vector<int> best_weights = adaptive_balance(times, exec_plan_->weights);

					/////////ALGORITMO RELAJACION
                    if(iteraciones > 1)
                    {
                      int weights_totales = 0;
                      for(int ij = 0; ij < best_weights.size(); ij++)
                      {
                        if(ij != (best_weights.size()-1))
                        {
                              best_weights[ij] = (weights_previos[ij] + (best_weights[ij] - weights_previos[ij])/iteraciones);
                              weights_totales += best_weights[ij];
                        }
                        else
                          best_weights[ij] = 100 - weights_totales;
                      }

                    }

                    //////////AJUSTE
                    if(adjust_adapt(exec_plan_, best_weights, times)){ break;}
                    iteraciones++;

					times.clear();
				}
				exec_plan_->setFinal();
				break;
			}
			}
			////CHOOSING WEIGHTS FINISHES

			Range *ranges = static_cast<Range *>(alloca(sizeof(Range) * exec_plan_->devices.size() * 3));
			exec_plan_->divideExecPlan(ranges, nd_global[0].sizes_, nd_local[0].sizes_);

			for(int i = 0; i < exec_plan_->devices.size(); i++)
			{
				FRunner fr(fhandle);
				fr.local(nd_local[0][0], nd_local[0][1], nd_local[0][2]).device(exec_plan_->devices[i]);

				if((ranges[i*3+2].end)!=0)
				{
					fr.global(nd_local[0][0]*((ranges[i*3+2].end - ranges[i*3+2].origin + 1+(nd_local[0][0]-1))/nd_local[0][0]),
							nd_local[0][1]*((ranges[i*3+1].end - ranges[i*3+1].origin + 1+(nd_local[0][1]-1))/nd_local[0][1]),
							nd_local[0][2] * ((ranges[i*3+0].end - ranges[i*3+0].origin + 1 + (nd_local[0][2]-1))/nd_local[0][2]));
				}
				else
				{
					if((ranges[i*3+1].end)!=0)
					{
						fr.global((nd_local[0][0])*((ranges[i*3+1].end - ranges[i*3+1].origin + 1 + (nd_local[0][0]-1))/nd_local[0][0]),
								nd_local[0][1]*((ranges[i*3+0].end - ranges[i*3+0].origin + 1+(nd_local[0][1]-1))/nd_local[0][1]),
								ranges[i*3+2].end - ranges[i*3+2].origin + 1);
					}
					else
					{
						fr.global(nd_local[0][0]*((ranges[i*3].end - ranges[i*3].origin + 1+(nd_local[0][0]-1))/nd_local[0][0]),
								ranges[i*3+1].end - ranges[i*3+1].origin + 1, ranges[i*3+2].end - ranges[i*3+2].origin + 1);
					}
				}

				exec_plan_->applyTo(fr, ranges[i*3], arg0, args...);
			}
			return *this;
		}
		bool any_bool = test_pointer(std::forward<T>(arg0), std::forward<Args>(args)...);
		if(any_bool == 0)
		{
			try {

				fillInFRunnerTable(0, Tuple(0,0), arg0, args...);

				if(nd_global[0][0]==-1)
				{
					nd_global[0][0] = /*arg0.getXYZDim(0);*/getSizeDim<0>(arg0);
					nd_global[0][1] = /*arg0.getXYZDim(1);*/getSizeDim<1>(arg0);
					nd_global[0][2] = /*arg0.getXYZDim(2);*/getSizeDim<2>(arg0);
				}

				TheGlobalState().clbinding().setup_and_run(this);

			} catch(HPLException o) {
				if (AutoManageExceptions) {
					std::cout << o.what() << std::endl;
					exit(EXIT_FAILURE);
				} else {
					throw;
				}
			}
		}
		else
		{
			int device_n;
			// The user has not selected several devices. Only there is one selected
			// or none (one by default)
			//    		if(device_.size()==1)
			if(device_.size() == 0)
			{
				device_n = TheGlobalState().clbinding().getDeviceNumber(GPU);
				// By default, there is a selected device. We will use all the
				// GPUs that we found in the system.
				for(int i = 1; i < device_n; i++)
					device_.push_back(HPL::Device(GPU,i));
			}
			// The user has introduced more than one device
			else
			{
				device_n = static_cast<int>(device_.size());
			}
			int accum_work = 0;
			// We update the value of the global space of all selected devices.
			// We iterate for each device.
			while(nd_global.size() < (device_n +1)) {
				// There are no global space for the current device, so that the
				// dimensions of the first array will set the global space.
				if(nd_global[nd_global.size()-1][0]==-1)
				{
					set_global_(std::forward<T>(arg0), device_n, &accum_work);
				}
				// There are global space defined for the current device
				else
				{
					nd_global.push_back(nd_global[0]);
				}
			}
			//The local space is replicated for all devices
			while(nd_local.size() < (device_n)) {
				nd_local.push_back(nd_local[0]);
			}

			// Using OpenMP to launch the frunner execution, one per thread
			//omp_set_num_threads(device_n);
#pragma omp parallel for num_threads(device_n)
			for(int i = 0; i < device_n; i++)
			{
				try {

					FRunner fr(fhandle);
					fr.device(device_[i]);
#pragma omp critical
					{
						fr.fillInFRunnerTable(0, Tuple(i,device_n), arg0, args...);
					}
					fr.global(nd_global[i][0], nd_global[i][1], nd_global[i][2]).local(nd_local[i][0], nd_local[i][1], nd_local[i][2]);
					TheGlobalState().clbinding().setup_and_run(&fr);

				} catch(HPLException o) {
					if (AutoManageExceptions) {
						std::cout << o.what() << std::endl;
						exit(EXIT_FAILURE);
					} else {
						throw;
					}
				}
			}
			unset_distrib(std::forward<T>(arg0), std::forward<Args>(args)...);
		}
		return *this;
	}

	/**
	 * @brief Launches the execution of a FRunner with its stored data (dimensions, arrays, kernel, ...)
	 * Only sets the scalar arguments to the kernel.
	 * It can improve the performance when we are working with the same buffers, the same
	 * problem dimensions, ... as for example, inside a loop.
	 */
	void run();

	FRunner& executionPlan(ExecutionPlan &ep)
	{
		this->exec_plan_ = &ep;
		return *this;
	}

	// Terminates recursion
	void fillInFRunnerTable(int n, Tuple t) { }

	template<typename T, typename... Args>
	void fillInFRunnerTable(int n, Tuple t, T& arg0, Args&... args) {
		//BBF: Commented out so structs can also be used as direct arguments
		//static_assert(std::is_arithmetic<T>::value, "argument to eval is neither an arithmetic scalar nor an Array");
		frunnerTable[n].size = sizeof(T);
		frunnerTable[n].dbuffer = (cl::Buffer*)(&arg0);
		frunnerTable[n].base_array = 0;
		fillInFRunnerTable(n + 1, t, args...);
	}

	template<typename T, typename... Args>
	void fillInFRunnerTable(int n, Tuple t, Array<T, 0>& arg0, Args&... args) {
		frunnerTable[n].size = sizeof(T);
		frunnerTable[n].dbuffer = (cl::Buffer*)(&(arg0.value()));
		frunnerTable[n].base_array = 0;
		fillInFRunnerTable(n + 1, t,args...);
	}

	template<typename T, typename... Args>
	void fillInFRunnerTable(int n, Tuple t, std::vector<T*>& arg0, Args&... args) {
		frunnerTable[n].size = sizeof(T);
		if(t.origin < arg0.size())
			frunnerTable[n].dbuffer = (cl::Buffer*)((arg0[t.origin]));
		else throw HPL::HPLException(0, "Exception thrown in FRunner::fillInFRunnerTable: The vector must include a chunk of data for every device!\n");
		frunnerTable[n].base_array = 0;
		fillInFRunnerTable(n + 1,t, args...);

	}

	template<typename T, typename... Args>
	void fillInFRunnerTable(int n, Tuple t, std::vector<Array<T, 0>*>& arg0, Args&... args) {
		frunnerTable[n].size = sizeof(T);
		if(t.origin < arg0.size())
			frunnerTable[n].dbuffer = (cl::Buffer*)(&((arg0[t.origin])->value()));
		else throw HPL::HPLException(0, "Exception thrown in FRunner::fillInFRunnerTable: The vector must include a chunk of data for every device!\n");
		frunnerTable[n].base_array = 0;
		fillInFRunnerTable(n + 1,t, args...);
	}

	template<typename T0, int NDIM0, ArrayFlags AF0, typename... Args>
	void fillInFRunnerTable(int n, Tuple t, std::vector<Array<T0,NDIM0,AF0>*>& arg0, Args&... args) {
		if(t.origin < arg0.size())
			frunnerTable[n].size = arg0[t.origin]->getSize();
		else throw HPL::HPLException(0, "Exception thrown in FRunner::fillInFRunnerTable: The vector must include a chunk of data for every device!\n");
		frunnerTable[n].base_array = arg0[t.origin];
		fillInFRunnerTable(n + 1, t,args...);
	}
	////////////////////////////END WITH STDVECTORS


	/// Fills in the base_arrays of the fhandleTable from a list of arguments
	template<typename T0, int NDIM0, ArrayFlags AF0, typename... Args>
	void fillInFRunnerTable(int n, Tuple t, Array<T0,NDIM0,AF0> & arg0, Args&... args) {
		frunnerTable[n].size = arg0.getSize();
		frunnerTable[n].base_array = &arg0;
		fillInFRunnerTable(n + 1, t,args...);
	}

	template<typename T0, typename... Args>
	void fillInFRunnerTable(int n, Tuple t, Array<T0,1,Global> & arg0, Args&... args) {
		switch(arg0.getDistrib())
		{
		case D_NONE:
		{
			frunnerTable[n].size = arg0.getSize();
			frunnerTable[n].base_array = &arg0;
			break;
		}
		case D_FST:
		{
			double work = 1.0f / (t.end);
			int current_work;
			int lower_bound = 0;
			int upper_bound = 0;
			if(t.origin == (t.end-1))
			{
				upper_bound = 0;
				lower_bound = (t.origin == 0 ? 0 : arg0.getDistribSize());//lower_bound = arg0.getDistribSize();
				current_work = getSizeDim<0>(arg0) - ((t.origin) * (int)(getSizeDim<0>(arg0) * work));
			}
			else
			{
				upper_bound = arg0.getDistribSize();
				current_work = getSizeDim<0>(arg0) * work;
				if(t.origin == 0)
					lower_bound = 0;
				else
					lower_bound = arg0.getDistribSize();
			}

			//    			Tuple tuple(((t.origin) * getSizeDim<0>(arg0) * work), (((t.origin) * (int)(getSizeDim<0>(arg0) * work)) + (current_work -1)));
			Tuple tuple(((t.origin) * (int)(getSizeDim<0>(arg0) * (work)) - lower_bound), (((t.origin) * (int)(getSizeDim<0>(arg0) * work))) + (current_work -1) + upper_bound);
			frunnerTable[n].size = tuple.getSize();
			frunnerTable[n].base_array = &(arg0)(tuple);
			break;
		}
		default: throw HPL::HPLException(0, "Exception thrown in FRunner::fillInFRunnerTable: Incorrect distribution!\n");
		}

		fillInFRunnerTable(n + 1, t,args...);
	}

	template<typename T0, typename... Args>
	void fillInFRunnerTable(int n, Tuple t, Array<T0,2,Global> & arg0, Args&... args) {
		switch(arg0.getDistrib())
		{
		case D_NONE:
		{
			frunnerTable[n].size = arg0.getSize();
			frunnerTable[n].base_array = &arg0;
			break;
		}
		case D_FST:
		{
			double work = 1.0f / (t.end);
			int current_work;
			int lower_bound = 0;
			int upper_bound = 0;
			if(t.origin == (t.end-1))
			{
				upper_bound = 0;
				lower_bound = (t.origin == 0 ? 0 : arg0.getDistribSize());//lower_bound = arg0.getDistribSize();
				current_work = getSizeDim<1>(arg0) - ((t.origin) * (int)(getSizeDim<1>(arg0) * work));
			}
			else
			{
				upper_bound = arg0.getDistribSize();
				current_work = getSizeDim<1>(arg0) * work;
				if(t.origin == 0)
					lower_bound = 0;
				else
					lower_bound = arg0.getDistribSize();
			}

			//    			Tuple tuple(((t.origin) * getSizeDim<0>(arg0) * work), (((t.origin) * (int)(getSizeDim<0>(arg0) * work)) + current_work -1));
			Tuple tuple(((t.origin) * (int)(getSizeDim<1>(arg0) * (work)) - lower_bound), (((t.origin) * (int)(getSizeDim<1>(arg0) * work))) + (current_work -1) + upper_bound);
			frunnerTable[n].size = tuple.getSize() * getSizeDim<0>(arg0);
			Tuple t1(0,getSizeDim<0>(arg0)-1);
			frunnerTable[n].base_array = &(arg0)(tuple,t1);
			break;
		}

		case D_SND:
		{
			double work = 1.0f / (t.end);
			int current_work;
			int lower_bound = 0;
			int upper_bound = 0;
			if(t.origin == (t.end-1))
			{
				upper_bound = 0;
				lower_bound = (t.origin == 0 ? 0 : arg0.getDistribSize());//lower_bound = arg0.getDistribSize();
				current_work = getSizeDim<0>(arg0) - ((t.origin) * (int)(getSizeDim<0>(arg0) * work));
			}
			else
			{
				upper_bound = arg0.getDistribSize();
				current_work = getSizeDim<0>(arg0) * work;
				if(t.origin == 0)
					lower_bound = 0;
				else
					lower_bound = arg0.getDistribSize();
			}

			//Tuple tuple(((t.origin) * getSizeDim<1>(arg0) * work), (((t.origin) * (int)(getSizeDim<1>(arg0) * work)) + current_work -1));
			Tuple tuple(((t.origin) * (int)(getSizeDim<0>(arg0) * (work)) - lower_bound), (((t.origin) * (int)(getSizeDim<0>(arg0) * work))) + (current_work -1) + upper_bound);

			frunnerTable[n].size = tuple.getSize() * getSizeDim<1>(arg0);
			Tuple t1(0,getSizeDim<1>(arg0)-1);
			frunnerTable[n].base_array = &(arg0)(t1,tuple);
			break;
		}
		default: throw HPL::HPLException(0, "Exception thrown in FRunner::fillInFRunnerTable: Incorrect distribution!\n");
		}

		fillInFRunnerTable(n + 1, t,args...);
	}

	template<typename T0, typename... Args>
	void fillInFRunnerTable(int n, Tuple t, Array<T0,3,Global> & arg0, Args&... args) {
		switch(arg0.getDistrib())
		{
		case D_NONE:
		{
			frunnerTable[n].size = arg0.getSize();
			frunnerTable[n].base_array = &arg0;
			break;
		}
		case D_FST:
		{
			double work = 1.0f / (t.end);
			int current_work;
			int lower_bound = 0;
			int upper_bound = 0;
			if(t.origin == (t.end-1))
			{
				upper_bound = 0;
				lower_bound = (t.origin == 0 ? 0 : arg0.getDistribSize());//lower_bound = arg0.getDistribSize();
				current_work = getSizeDim<2>(arg0) - ((t.origin) * (int)(getSizeDim<2>(arg0) * work));
			}
			else
			{
				upper_bound = arg0.getDistribSize();
				current_work = getSizeDim<2>(arg0) * work;
				if(t.origin == 0)
					lower_bound = 0;
				else
					lower_bound = arg0.getDistribSize();
			}
			//    			Tuple tuple(((t.origin) * getSizeDim<0>(arg0) * work), (((t.origin) * (int)(getSizeDim<0>(arg0) * work)) + current_work -1));
			Tuple tuple(((t.origin) * (int)(getSizeDim<2>(arg0) * (work)) - lower_bound), (((t.origin) * (int)(getSizeDim<2>(arg0) * work))) + (current_work -1) + upper_bound);
			frunnerTable[n].size = tuple.getSize() * getSizeDim<1>(arg0) * getSizeDim<0>(arg0);
			Tuple t1(0,getSizeDim<1>(arg0)-1);
			Tuple t2(0,getSizeDim<0>(arg0)-1);
			frunnerTable[n].base_array = &(arg0)(tuple,t1,t2);
			break;
		}
		case D_SND:
		{
			double work = 1.0f / (t.end);
			int current_work;
			int lower_bound = 0;
			int upper_bound = 0;
			if(t.origin == (t.end-1))
			{
				upper_bound = 0;
				lower_bound = (t.origin == 0 ? 0 : arg0.getDistribSize());//lower_bound = arg0.getDistribSize();
				current_work = getSizeDim<1>(arg0) - ((t.origin) * (int)(getSizeDim<1>(arg0) * work));
			}
			else
			{
				upper_bound = arg0.getDistribSize();
				current_work = getSizeDim<1>(arg0) * work;
				if(t.origin == 0)
					lower_bound = 0;
				else
					lower_bound = arg0.getDistribSize();
			}

			//    			Tuple tuple(((t.origin) * getSizeDim<1>(arg0) * work), (((t.origin) * (int)(getSizeDim<1>(arg0) * work)) + current_work -1));
			Tuple tuple(((t.origin) * (int)(getSizeDim<1>(arg0) * (work)) - lower_bound), (((t.origin) * (int)(getSizeDim<1>(arg0) * work))) + (current_work -1) + upper_bound);

			frunnerTable[n].size = tuple.getSize() * getSizeDim<0>(arg0) * getSizeDim<2>(arg0);
			Tuple t1(0,getSizeDim<2>(arg0)-1);
			Tuple t2(0,getSizeDim<0>(arg0)-1);
			frunnerTable[n].base_array = &(arg0)(t1, tuple, t2);
			break;
		}
		case D_TRD:
		{
			double work = 1.0f / (t.end);
			int current_work;
			int lower_bound = 0;
			int upper_bound = 0;
			if(t.origin == (t.end-1))
			{
				upper_bound = 0;
				lower_bound = (t.origin == 0 ? 0 : arg0.getDistribSize());//lower_bound = arg0.getDistribSize();
				current_work = getSizeDim<0>(arg0) - ((t.origin) * (int)(getSizeDim<0>(arg0) * work));
			}
			else
			{
				upper_bound = arg0.getDistribSize();
				current_work = getSizeDim<0>(arg0) * work;
				if(t.origin == 0)
					lower_bound = 0;
				else
					lower_bound = arg0.getDistribSize();
			}

			//    			Tuple tuple(((t.origin) * getSizeDim<2>(arg0) * work), (((t.origin) * (int)(getSizeDim<2>(arg0) * work)) + current_work -1));
			Tuple tuple(((t.origin) * (int)(getSizeDim<0>(arg0) * (work)) - lower_bound), (((t.origin) * (int)(getSizeDim<0>(arg0) * work))) + (current_work -1) + upper_bound);

			frunnerTable[n].size = tuple.getSize() * getSizeDim<2>(arg0) * getSizeDim<1>(arg0);
			Tuple t1(0,getSizeDim<2>(arg0)-1);
			Tuple t2(0,getSizeDim<1>(arg0)-1);
			frunnerTable[n].base_array = &(arg0)(t1, t2, tuple);
			break;
		}
		}

		fillInFRunnerTable(n + 1, t,args...);
	}

	template<typename T, typename... Args>
	void fillInFRunnerTable(int n, Tuple t, T* arg0, Args&... args) {
		frunnerTable[n].size = sizeof(T);
		frunnerTable[n].dbuffer = (cl::Buffer*)(arg0+t.origin);
		frunnerTable[n].base_array = 0;
		fillInFRunnerTable(n + 1,t, args...);
	}


	template<typename T0, ArrayFlags AF0, typename... Args>
	void fillInFRunnerTable(int n, Tuple t, Array<T0,3,AF0> * arg0, Args&... args) {
		double work = 1.0f / (t.end);
		int current_work;
		if(t.origin == (t.end-1))
			current_work = getSizeDim<0>(*arg0) - (int)((t.origin) * getSizeDim<0>(*arg0) * work);
		else
			current_work = getSizeDim<0>(*arg0) * work;
		Tuple tuple(((t.origin) * getSizeDim<0>(*arg0) * work), (((t.origin) * (int)(getSizeDim<0>(*arg0) * work)) + current_work -1));

		frunnerTable[n].size = tuple.getSize() * getSizeDim<1>(*arg0) * getSizeDim<2>(*arg0);
		Tuple t1(0,getSizeDim<1>(*arg0)-1);
		Tuple t2(0,getSizeDim<2>(*arg0)-1);
		frunnerTable[n].base_array = &(*arg0)(tuple,t1,t2);

		fillInFRunnerTable(n + 1, t,args...);
	}


	template<typename T0, ArrayFlags AF0, typename... Args>
	void fillInFRunnerTable(int n, Tuple t, Array<T0,2,AF0> * arg0, Args&... args) {
		double work = 1.0f / (t.end);
		int current_work;
		if(t.origin == (t.end-1))
			current_work = getSizeDim<0>(*arg0) - (int)((t.origin) * getSizeDim<0>(*arg0) * work);
		else
			current_work = getSizeDim<0>(*arg0) * work;

		Tuple tuple(((t.origin) * getSizeDim<0>(*arg0) * work), (((t.origin) * (int)(getSizeDim<0>(*arg0) * work)) + current_work -1));

		frunnerTable[n].size = tuple.getSize() * getSizeDim<1>(*arg0);
		Tuple t1(0,getSizeDim<1>(*arg0)-1);
		frunnerTable[n].base_array = &(*arg0)(tuple,t1);

		fillInFRunnerTable(n + 1, t,args...);
	}


	template<typename T0, ArrayFlags AF0, typename... Args>
	void fillInFRunnerTable(int n, Tuple t, Array<T0,1,AF0> * arg0, Args&... args) {
		double work = 1.0f / (t.end);
		int current_work;
		if(t.origin == (t.end-1))
			current_work = getSizeDim<0>(*arg0) - (int)((t.origin) * getSizeDim<0>(*arg0) * work);
		else
			current_work = getSizeDim<0>(*arg0) * work;

		Tuple tuple(((t.origin) * getSizeDim<0>(*arg0) * work), (((t.origin) * (int)(getSizeDim<0>(*arg0) * work)) + (current_work -1)));
		frunnerTable[n].size = tuple.getSize();
		frunnerTable[n].base_array = &(*arg0)(tuple);
		fillInFRunnerTable(n + 1, t,args...);
	}

	template<typename T, typename... Args>
	void fillInFRunnerTable(int n, Tuple t, Array<T, 0>* arg0, Args&... args) {
		frunnerTable[n].size = sizeof(T);
		frunnerTable[n].dbuffer = (cl::Buffer*)(&((arg0+t.origin)->value()));
		frunnerTable[n].base_array = 0;
		fillInFRunnerTable(n + 1,t, args...);
	}


	struct arg_type {
		int size;			///< Size of the array or the scalar
		cl::Buffer *dbuffer;	///< Pointer to where this array or scalar is allocated in device memory.
		InternalMultiDimArray *base_array; ///< Pointer to where this array is allocated in host memory. NULL for scalars
		bool isAlias;             ///< Flag that indicates if this array is aliases with another array in the arglist.
	} frunnerTable[MAX_KERNEL_NARG]; ///< Contains the data of the arguments of the current kernel execution.

private:

	FHandle* fhandle;           ///< reference to the associated FHandle.
	std::vector<HPL::Domain> nd_local;
	std::vector<HPL::Domain> nd_global;
	std::vector<HPL::Device> device_;
	int is_multifrunner;
	ExecutionPlan* exec_plan_;


	//    template< typename T>
	//    void set_global_(T&& arg0, int device_n, int* accum_work)
	//    {
	//    	// If the first argument is a pointer, the global space will
	//    	// match with the size of the array divided by its first dimension
	//    	int dim = arg0.getNumberOfDimensions();
	//    	if( std::is_pointer<T>::value || arg0.getDistrib()!=D_NONE)
	//    	{
	//    		float work = 1.0f / (device_n);
	//    		int current_work;
	//
	//			nd_global[nd_global.size()-1][0] = getSizeDim<0>(arg0);
	//			nd_global[nd_global.size()-1][1] = getSizeDim<1>(arg0);
	//			nd_global[nd_global.size()-1][2] = getSizeDim<2>(arg0);
	//
	//			if(arg0.getDistrib() <= dim )
	//    		{
	//    			if((nd_global.size()-1) == (device_n-1))
	//    				current_work = arg0.getXYZDim(dim-arg0.getDistrib()) - *accum_work;
	//    			else
	//    				current_work = arg0.getXYZDim(dim-arg0.getDistrib()) * work;
	//
	//    			nd_global[nd_global.size()-1][dim-arg0.getDistrib()] = current_work;
	//    		}
	//
	//    		if((nd_global.size()-1)!= (device_n-1))
	//    			nd_global.emplace_back(-1, -1, -1);
	//    		*accum_work += current_work;
	//    	}
	//    	// The first array is not a pointer, the dimensions of the
	//    	// first array set the size of the global space
	//    	else
	//    	{
	//    		nd_global[nd_global.size()-1][0] = getSizeDim<0>(arg0);
	//    		nd_global[nd_global.size()-1][1] = getSizeDim<1>(arg0);
	//    		nd_global[nd_global.size()-1][2] = getSizeDim<2>(arg0);
	//    	}
	//    }

	template< typename T>
	void set_global_(T&& arg0, int device_n, int* accum_work)
	{
		double work = 1.0f / (device_n);
		int current_work;
		if((nd_global.size()-1) == (device_n-1))
			current_work = getSizeDim<0>(arg0) - *accum_work;
		else {
			current_work = getSizeDim<0>(arg0) * work;
		}

		nd_global[nd_global.size()-1][0] = current_work;
		nd_global[nd_global.size()-1][1] = getSizeDim<1>(arg0);
		nd_global[nd_global.size()-1][2] = getSizeDim<2>(arg0);

		if((nd_global.size()-1)!= (device_n-1))
			nd_global.emplace_back(-1, -1, -1);
		*accum_work += current_work;
	}


	template<typename T0, int NDIM0, ArrayFlags AF0>
	void set_global_(Array<T0,NDIM0,AF0>& arg0, int device_n, int* accum_work)
	{
		// If the first argument is a pointer, the global space will
		// match with the size of the array divided by its first dimension
		int dim = arg0.getNumberOfDimensions();
		if( arg0.getDistrib()!=D_NONE)
		{
			double work = 1.0f / (device_n);
			int current_work;

			nd_global[nd_global.size()-1][0] = getSizeDim<0>(arg0);
			nd_global[nd_global.size()-1][1] = getSizeDim<1>(arg0);
			nd_global[nd_global.size()-1][2] = getSizeDim<2>(arg0);

			if(arg0.getDistrib() <= dim )
			{
				if((nd_global.size()-1) == (device_n-1))
					current_work = arg0.getXYZDim(dim-arg0.getDistrib()) - *accum_work;
				else
					current_work = arg0.getXYZDim(dim-arg0.getDistrib()) * work;

				nd_global[nd_global.size()-1][dim-arg0.getDistrib()] = current_work;
			} else {
				throw HPLException(0, "Distribution on non valid dimension");
			}

			if((nd_global.size()-1)!= (device_n-1))
				nd_global.emplace_back(-1, -1, -1);
			*accum_work += current_work;
		}
		// The first array is not a pointer, the dimensions of the
		// first array set the size of the global space
		else
		{
			nd_global[nd_global.size()-1][0] = getSizeDim<0>(arg0);
			nd_global[nd_global.size()-1][1] = getSizeDim<1>(arg0);
			nd_global[nd_global.size()-1][2] = getSizeDim<2>(arg0);
		}
	}

};

};

#endif /* FRUNNER_H_ */
