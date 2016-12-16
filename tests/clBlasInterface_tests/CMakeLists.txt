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

# \file     tests/clBlasInterface_tests/CMakeLists.txt
# \author   Moisés Viñas        <moises.vinas@udc.es>
# \author   Zeki Bozkus         <zeki.bozkus@khas.edu.tr>
# \author   Basilio B. Fraguela <basilio.fraguela@udc.es>


cmake_minimum_required( VERSION 2.8 )

include_directories( ${CLBLAS_DIR}/include )
link_directories( ${CLBLAS_DIR}/lib )
link_libraries( clBLAS )

set(clBLASTests 
TestclBLASexceptions
TestclBLASSasum
TestclBLASSaxpy
TestclBLASScasum
TestclBLASScnrm2
TestclBLASScopy
TestclBLASSdot
TestclBLASSgbmv
TestclBLASSgemm
TestclBLASSgemv
TestclBLASSger
TestclBLASSnrm2
TestclBLASSrot
TestclBLASSrotg
TestclBLASSrotm
TestclBLASSrotmg
TestclBLASSsbmv
TestclBLASSscal
TestclBLASSspmv
TestclBLASSspr
TestclBLASSspr2
TestclBLASSswap
TestclBLASSsymm
TestclBLASSsymv
TestclBLASSsyr
TestclBLASSsyr2
TestclBLASSsyr2k
TestclBLASSsyrk
TestclBLASStbmv
TestclBLASStbsv
TestclBLASStpmv
TestclBLASStpsv
TestclBLASStrmm
TestclBLASStrmv
TestclBLASStrsm
TestclBLASStrsv
TestclBLASiSamax
TestclBLASCaxpy
TestclBLASCcopy
TestclBLASCdotc
TestclBLASCdotu
TestclBLASCgbmv
TestclBLASCgemm
TestclBLASCgemv
TestclBLASCgerc
TestclBLASCgeru
TestclBLASChbmv
TestclBLASChemm
TestclBLASChemv
TestclBLASCher
TestclBLASCher2
TestclBLASCher2k
TestclBLASCherk
TestclBLASChpmv
TestclBLASChpr
TestclBLASChpr2
TestclBLASCrotg
TestclBLASCscal
TestclBLASCsrot
TestclBLASCsscal
TestclBLASCswap
TestclBLASCsymm
TestclBLASCsyr2k
TestclBLASCsyrk
TestclBLASCtbmv
TestclBLASCtbsv
TestclBLASCtpmv
TestclBLASCtpsv
TestclBLASCtrmm
TestclBLASCtrmv
TestclBLASCtrsm
TestclBLASCtrsv
TestclBLASiCamax
TestclBLASDasum
TestclBLASDaxpy
TestclBLASDcopy
TestclBLASDdot
TestclBLASDgbmv
TestclBLASDgemm
TestclBLASDgemv
TestclBLASDger
TestclBLASDnrm2
TestclBLASDrot
TestclBLASDrotg
TestclBLASDrotm
TestclBLASDrotmg
TestclBLASDsbmv
TestclBLASDscal
TestclBLASDspmv
TestclBLASDspr
TestclBLASDspr2
TestclBLASDswap
TestclBLASDsymm
TestclBLASDsymv
TestclBLASDsyr
TestclBLASDsyr2
TestclBLASDsyr2k
TestclBLASDsyrk
TestclBLASDtbmv
TestclBLASDtbsv
TestclBLASDtpmv
TestclBLASDtpsv
TestclBLASDtrmm
TestclBLASDtrmv
TestclBLASDtrsm
TestclBLASDtrsv
TestclBLASiDamax )

add_compile_options( "-w" )

if( APPLE )
  set( ENV{DYLD_LIBRARY_PATH} $ENV{DYLD_LIBRARY_PATH}:${CLBLAS_DIR}/lib )
elseif( UNIX )
  set( ENV{LD_LIBRARY_PATH} $ENV{LD_LIBRARY_PATH}:${CLBLAS_DIR}/lib )
endif( APPLE )

foreach(test ${clBLASTests})
  add_executable( ${test} src/${test}.cpp )
endforeach(test)

add_custom_target(checkclBLAS)
foreach(test ${clBLASTests})
  add_custom_command(TARGET checkclBLAS PRE_BUILD
                     COMMAND echo test ${test} : -------------
					 COMMAND ${test} || exit 1
					 COMMAND echo ------------------------)
endforeach(test)

add_custom_command(TARGET checkclBLAS PRE_BUILD COMMAND echo ${clBLASTests} clBLAS TESTS DONE. SUCCESSFUL)
