# HPL : Heterogeneous Programming Library
# Copyright (c) 2012-2015 The HPL Team as listed in CREDITS.txt
#
# This file is part of HPL
#
# HPL is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# HPL is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with HPL.  If not, see <http://www.gnu.org/licenses/>.

# \file     Makefile
# \author   Moisés Viñas        <moises.vinas@udc.es>
# \author   Zeki Bozkus         <zeki.bozkus@khas.edu.tr>
# \author   Basilio B. Fraguela <basilio.fraguela@udc.es>

include ../common.mk

MAKEOPS := ../lib/pete-2.1.0/src/Tools/MakeOperators
GENERATED_HEADERS := IndexedArrayOperators.h  Array_T_extraOps.h  IndexedArray_T_NDIM_extraOps.h
HPL_LIB_OBJECTS   := HPL.o Array.o ExprManipulation.o Stringizer.o GlobalState.o Codifier.o FRunner.o FHandle.o HPLHelpers.o FCall.o CLbinding.o Env.o Device.o VTypes.o HPLException.o HDM.o HPL_utils.o
PETELINK := PETE

ifneq ($(CLBLAS_DIR),)
  HPL_LIB_OBJECTS += HPL_clBLAS.o HPL_clBLAS_exceptions.o
endif

# HPL_config.txt removed as default target. Replaced by PerfAdapt
all: $(MAKEOPS) $(GENERATED_HEADERS) $(PROJECTLIB) PerfAdapt

run: tests
	$(MAKE) --no-print-directory -C tests run config

$(PETELINK):
	ln -s ../lib/pete-2.1.0/src/PETE PETE
	
$(MAKEOPS): $(PETELINK)
	$(MAKE) --no-print-directory -C $(PETEDIR)

IndexedArrayOperators.h: $(MAKEOPS) IndexedArrayDefs.in StringOps.in
	$(MAKEOPS) --classes IndexedArrayDefs.in --operators StringOps.in --guard INDEXEDARRAYOPS_H --op-tags > $@

Array_T_extraOps.h IndexedArray_T_NDIM_extraOps.h : generate_extraOps

generate_extraOps : generate_extraOps.cpp
	$(CXX) -o ./$@ $<
	./$@

$(PROJECTLIB) : $(HPL_LIB_OBJECTS)
	ar -r $@ $^
	ranlib $@

PerfAdapt: PerfAdapt.cpp $(PROJECTLIB)
	$(CXX) $(CPPFLAGS) -o $@ $^ $(OPENCLLIBS)

HPL_config.txt : PerfAdapt
	./$< >> $@

HPLException.o : exceptions/HPLException.cpp exceptions/HPLException.hpp $(GENERATED_HEADERS)
	$(CXX) -c $(CPPFLAGS) -o $@ exceptions/HPLException.cpp

HPL_clBLAS_exceptions.o: exceptions/HPL_clBLAS_exceptions.cpp exceptions/HPL_clBLAS_exceptions.hpp HPL_clBLAS.h $(GENERATED_HEADERS)
	$(CXX) -c -w $(CPPFLAGS) -I$(CLBLAS_DIR)/include -o $@ $<

HPL_clBLAS.o: HPL_clBLAS.cpp HPL_clBLAS.h HPL_clBLAS_exceptions.o $(GENERATED_HEADERS)
	$(CXX) -c -w $(CPPFLAGS) -I$(CLBLAS_DIR)/include -o $@ $<

%.o : %.cpp %.h $(GENERATED_HEADERS)
	$(CXX) -c $(CPPFLAGS) -o $@ $<

clean:
	-@rm -f *.o generate_extraOps

veryclean: clean
	-@rm -f $(GENERATED_HEADERS) $(MAKEOPS) $(PROJECTLIB) PerfAdapt HPL_config.txt generate_extraOps
	-@rm -rf *.dSYM
	-@rm -f $(PETELINK)

