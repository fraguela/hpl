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
/// \file     ArrayInfo.h
/// \author   Moisés Viñas        <moises.vinas@udc.es>
/// \author   Zeki Bozkus         <zeki.bozkus@khas.edu.tr>
/// \author   Basilio B. Fraguela <basilio.fraguela@udc.es>
///

#ifndef ArrayInfo_H
#define ArrayInfo_H

namespace HPL {

  /**
   * @brief Keeps information of an array about it is read or write into a kernel code.
   */
  class ArrayInfo
  {
  public:

	/**
	 * @brief Default constructor.
	 */
    ArrayInfo()
    : isWrite_(false), isWriteBeforeRead_(false), accesses_(0)
    { }

    /**
     * @brief Obtain if an array is write in the actual or in the following kernel.
     */
    bool get_isWritten() const { return isWrite_; }

    /**
     * @brief Set if an array will be write or written after read in the following kernel.
     */
    void set_isWritten()
    {
    	if(isWrite_) return;
    	this->isWrite_ = true;
    	if(accesses_==1){ isWriteBeforeRead_ = true;};
    }

    void force_isWritten()
    {
    	this->isWrite_ = true;
    }

    /**
     * @brief Set if an array is written after being read.
     * This method is necessary because it is needed to difference
     * between a reduction done with (+=, -=, ...) done with (=).
     * The first type of reduction reads the left element before overwriting.
     * The second one overwrites the left element.
     */
    void set_isReadandWritten()
    {
    	this->isWrite_ = true;
    }

    /**
     * @brief Obtain if an array will be written after read in the kernel.
     */
    bool get_isWrittenBeforeRead() const { return isWriteBeforeRead_; }

    /**
     * @brief Adds a reference to this array.
     */
    void addAccess() { accesses_++; }

    /**
     * @brief This function is used to pass the accesses made onto a argument
     * from a called function to the caller one.
     */
    void addAccess(unsigned int accesses) { accesses_ += accesses; }

    /**
     * @brief This function is used to obtain the accesses made to an argument.
     */
    unsigned int getAccesses() const { return accesses_; }

  protected:
    
    bool isWrite_;           ///< Indicates whether this array is written or not.
    bool isWriteBeforeRead_; ///< Indicates whether this array is written before read or not.
    unsigned int accesses_;  ///< Indicates the number of accesses to this array in the kernel.
  };
  
}

#endif
