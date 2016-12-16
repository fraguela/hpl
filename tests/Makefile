# HPL : Heterogeneous Programming Library
# Copyright (c) 2012-2016 The HPL Team as listed in CREDITS.txt
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

# COMMON_DEPENDENCES :=  $(COMMON_DEPENDENCES:%=../src/%)

testBinaries :=	TestVectorTypes \
		TestArrayCreate \
		TestBasicArray \
		TestInterFnCall \
		TestReduction \
		TestArrayFlags \
		TestSelfUpdateOps \
		TestScalarReduce \
		TestScalarReduce2D \
		TestScalarReduceLinear \
		TestScalarReduceTeams \
		TestNonScalarReduce \
		TestNonScalarReduceLinear \
		TestReuseCode \
		TestStruct \
		TestWhere \
		TestAlias \
		TestAliasArray \
		TestDouble \
		VectorAdd \
		TemplatedVectorAdd \
		MatrixAdd \
		MatrixVector \
		MultiHPLF \
		Matmul \
		Matmul_unroll \
		Matmul_granularity0 \
		Matmul_granularity \
		TestLocalArrayArg \
		Timer \
		SelectFromDeviceList \
		Dotproduct \
		TestIntent \
		AsyncSync \
		TestIntentInterFnCall \
		TestHDMcases \
		TestReeval \
		VectorAddMultiGPU \
		TestAtomics \
		TestComplex \
		ScalarArg \
		TestRun \
		TestNative \
		HPLDeviceQuery \
		TestFastMem \
		TestUnifiedMem \
		TestSubarray \
		TestExecutionPlan \
		TestSaxpy \
		TestSaxpyEPBalanced


clBlasInterface_TEST_SRC_DIR=./clBlasInterface_tests/src
clBlasInterface_TEST_BIN_DIR=./clBlasInterface_tests/bin

#$(patsubst %,$(clBlasInterface_TEST_BIN_DIR)%, $(shell cat clBlasInterface_tests/test_list))
ifneq ($(CLBLAS_DIR),)
	clBlasInterface_testBinaries = $(shell cat clBlasInterface_tests/test_list)
else
	clBlasInterface_testBinaries =
endif


all: tests

tests: $(testBinaries) $(clBlasInterface_TEST_BIN_DIR) $(clBlasInterface_testBinaries)

check: tests
	-@echo Running tests  
	-@for a in $(testBinaries) ;                    \
	do                                              \
	echo test $$a : -------------;                  \
	./$$a || exit 1;                                \
	echo ------------------------;                  \
	done;                                           \
	echo \* STANDARD TESTS DONE. SUCCESSFUL

ifneq ($(CLBLAS_DIR),)
checkclBLAS: tests
	-@echo Running HPL-clBLAS integration tests
	-@for a in $(clBlasInterface_testBinaries) ;    \
	do                                              \
	echo test $$a :; 						        \
	echo ------------------------;         		    \
	$(clBlasInterface_TEST_BIN_DIR)/$$a || exit 1;  \
	echo ------------------------;                  \
	done;                                           \
	echo \* clBLAS TESTS DONE. SUCCESSFUL

else
checkclBLAS:
	echo clBLAS wat not set up
endif

$(testBinaries) : % : %.cpp $(PROJECTLIB)
	$(CXX) $(CPPFLAGS) -o $@ $^ $(OPENCLLIBS)

$(clBlasInterface_TEST_BIN_DIR) :
	mkdir -p $@

$(clBlasInterface_testBinaries) : % : $(clBlasInterface_TEST_BIN_DIR)/%

$(clBlasInterface_TEST_BIN_DIR)/% : $(clBlasInterface_TEST_SRC_DIR)/%.cpp $(PROJECTLIB)
	$(CXX) -w $(CPPFLAGS) -I$(CLBLAS_DIR)/include -L$(CLBLAS_DIR)/lib -o $@ $^ -lclBLAS $(OPENCLLIBS)

clean: 
	-@rm -rf $(testBinaries) *.o *.dSYM $(clBlasInterface_TEST_BIN_DIR)/*

veryclean: clean
	-@rm -rf $(DEPDIR)

$(DEPDIR)/%.d : %.cpp
	-@if [ ! -d `dirname $@` ]; then mkdir -p `dirname $@`; fi; \
	set -e; rm -f $@; \
	$(CXX) -MM $(CPPFLAGS) $< | sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' > $@

-include $(testBinaries:%=$(DEPDIR)/%.d)
