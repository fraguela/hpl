## Installation</p>


### Requirements
 
 * OpenCL 1.1 or above.
 * A C++ compiler that supports C++11

### Installation procedures

There are two ways to install HPL:

* Based on the CMake tool (freely available for download from [http://www.cmake.org](http://www.cmake.org)). This is the mechanism recommended for users because it is more portable and it gives place to a streamlined installation in a directory chosen by the user. The binaries, header files and libraries are placed in the `bin/`, `include/hpl/` and `lib/` subdirectories, respectively.
   * The CMake files rely on the `FindOpenCL` module, which may not be installed by default in old versions of CMake. 

* Based on traditional Unix makefiles. This mechanism builds HPL inside the directory were its tarball is unpacked, leaving the header files and executables in the `src/` directory and the library in the `lib/` directory.

Both mechanisms are described in detail in Section 2 of the `HPL_programming_manual.pdf` document, found in directory `doc/`. The rest of this document replicates Section 2.2 (Installation based on Makefiles) for the impatient.

### Installation based on makefiles

1. First, make sure that you fulfill the HPL build requirements.

2. Unpack the hpl tarball (`hpl_xxx.tar.gz`) and enter the just created directory:

		tar -xzf hpl_xxx.tar.gz
		cd hpl_xxx

or, if you clone the project from its repository

		git clone git@github.com:fraguela/hpl.git
		cd hpl

3. run `./configure`

   This will generate a common.mk file with a default configuration

4. edit `common.mk` as needed to select your desired compiler, compilation flags, 
   location of OpenCL libraries, etc. modifying the corresponding variables

5. run `make`

   This builds the HPL library in directory `lib` and its tests in directory `tests`. 

   The library is built by default without optimization flags and with debugging information and assertions activated.

   If you define the environment variable `PRODUCTION` before you run `make`, HPL and its tests will be compiled with optimization flags, no debugging information and no assertions.

   For example, in the bash shell you can write `PRODUCTION=1 make`

   Note: You can use the flag -j to speedup the building process. For example,
   `make -j4` will use 4 parallel processes.

6. (Optionally) run `make check`

   This will run the HPL tests. 

   Notice that some tests may fail even if HPL works correctly (e.g. if they use double precision but it is not supported by the default device chosen).

7. (Optionally) run `make checkclBLAS`

   This will run the clBLAS integration tests if you set up clBLAS in common.mk.  Notice that some tests may fail even if HPL works correctly (e.g. if they use double precision but it is not supported by the default device chosen).

   Note: clBLAS may require loading its dynamic libraries at runtime. Please set up your environment (ususally by means of the environment variables `LD_LIBRARY_PATH` in Unix or `DYLD_LIBRARY_PATH` in Mac OS X) to allow clBLAS to find its libraries.

8. (Optionally) run `make clean`

   This removes the temporary object files generated during the build of HPL as well as the test binaries, leaving only `lib/libHPL.a`

   `make veryclean` also removes `lib/libHPL.a` and other internal HPL files.
