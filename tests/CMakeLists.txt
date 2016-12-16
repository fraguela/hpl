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

# \file     tests/CMakeLists.txt
# \author   Moisés Viñas        <moises.vinas@udc.es>
# \author   Zeki Bozkus         <zeki.bozkus@khas.edu.tr>
# \author   Basilio B. Fraguela <basilio.fraguela@udc.es>


cmake_minimum_required( VERSION 2.8 )

include_directories( ${PROJECT_SOURCE_DIR}/src )
link_libraries( HPL ${OpenCL_LIBRARIES} )

set(tests
	TestVectorTypes 
	TestArrayCreate 
	TestBasicArray 
	TestInterFnCall 
	TestReduction 
	TestArrayFlags 
	TestSelfUpdateOps 
	TestScalarReduce 
	TestScalarReduce2D 
	TestScalarReduceLinear 
	TestScalarReduceTeams 
	TestNonScalarReduce 
	TestNonScalarReduceLinear 
	TestReuseCode 
	TestStruct 
	TestWhere 
	TestAlias 
	TestAliasArray 
	TestDouble
	VectorAdd 
	TemplatedVectorAdd 
	MatrixAdd 
	MatrixVector 
	MultiHPLF 
	Matmul 
	Matmul_unroll 
	Matmul_granularity0 
	Matmul_granularity 
	TestLocalArrayArg 
	Timer 
	SelectFromDeviceList 
	Dotproduct 
	TestIntent 
	AsyncSync 
	TestIntentInterFnCall 
	TestHDMcases 
	TestReeval 
	VectorAddMultiGPU 
	TestAtomics 
	TestComplex 
	ScalarArg 
	TestRun 
	TestNative 
	HPLDeviceQuery 
	TestFastMem 
	TestUnifiedMem
	TestSubarray
	TestExecutionPlan
	TestSaxpy
	TestSaxpyEPBalanced )
		
foreach(test ${tests})
  add_executable( ${test} ${test}.cpp )
endforeach(test)

add_custom_target(check)
foreach(test ${tests})
  add_custom_command(TARGET check PRE_BUILD
                     COMMAND echo test ${test} : -------------
					 COMMAND ${test} || exit 1
					 COMMAND echo ------------------------
					 WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/tests)
endforeach(test)
add_custom_command(TARGET check PRE_BUILD COMMAND echo ${tests} TESTS DONE. SUCCESSFUL)

if( CLBLAS_DIR )
  add_subdirectory( clBlasInterface_tests )
endif( CLBLAS_DIR )
