cmake_minimum_required(VERSION 3.14)
project(PiBench)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_C_COMPILER "/usr/bin/gcc-8")
set(CMAKE_CXX_COMPILER "/usr/bin/g++-8")

#set(CMAKE_CXX_FLAGS " -ltbb -latomic  -g -O3 -Wno-invalid-offsetof -mcx16 -DNDEBUG")

#test the masstree without complier optimization
set(CMAKE_CXX_FLAGS " -ltbb -latomic  -g   -Wno-invalid-offsetof -mcx16 -DNDEBUG")


include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-march=native" COMPILER_SUPPORTS_MARCH_NATIVE)
if(COMPILER_SUPPORTS_MARCH_NATIVE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
endif()


option(PIBENCH_BUILD_LEVELDB "Build LevelDB wrapper" OFF)

include(CTest)

find_package(OpenMP REQUIRED)

find_package(TBB REQUIRED)
include_directories("/opt/intel/oneapi/tbb/2021.2.0/include/tbb")
list(APPEND "" ${TBB_LIBRARIES})
MESSAGE(STATUS "Found GCC's libtbb: lib=${TBB_LIBRARIES}")




FIND_LIBRARY(GCCLIBATOMIC_LIBRARY NAMES atomic atomic.so.1 libatomic.so.1
        HINTS
        $ENV{HOME}/local/lib64
        $ENV{HOME}/local/lib
        /usr/local/lib64
        /usr/local/lib
        /opt/local/lib64
        /opt/local/lib
        /usr/lib64
        /usr/lib
        /lib64
        /lib
        )

IF (GCCLIBATOMIC_LIBRARY)
  SET(GCCLIBATOMIC_FOUND TRUE)
  MESSAGE(STATUS "Found GCC's libatomic.so: lib=${GCCLIBATOMIC_LIBRARY}")
ELSE ()
  SET(GCCLIBATOMIC_FOUND FALSE)
  MESSAGE(STATUS "GCC's libatomic.so not found. This library is essential in AArch64 (if you are on x86, this doesn't matter)")
  MESSAGE(STATUS "Try: 'sudo apt-get install libatomic' (or sudo apt-get install libatomic1)")
ENDIF ()
#list(APPEND "" ${GCCLIBATOMIC_LIBRARY})


if(CMAKE_C_COMPILER_ID MATCHES "Clang")
  set(OpenMP_C "${CMAKE_C_COMPILER}")
  set(OpenMP_C_FLAGS "-fopenmp=libomp")
  set(OpenMP_C_LIB_NAMES "libomp" "libgomp" "libiomp5")
  set(OpenMP_libomp_LIBRARY ${OpenMP_C_LIB_NAMES})
  set(OpenMP_libgomp_LIBRARY ${OpenMP_C_LIB_NAMES})
  set(OpenMP_libiomp5_LIBRARY ${OpenMP_C_LIB_NAMES})
endif()
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(OpenMP_CXX "${CMAKE_CXX_COMPILER}")
  set(OpenMP_CXX_FLAGS "-fopenmp=libomp")
  set(OpenMP_CXX_LIB_NAMES "libomp" "libgomp" "libiomp5")
  set(OpenMP_libomp_LIBRARY ${OpenMP_CXX_LIB_NAMES})
  set(OpenMP_libgomp_LIBRARY ${OpenMP_CXX_LIB_NAMES})
  set(OpenMP_libiomp5_LIBRARY ${OpenMP_CXX_LIB_NAMES})
endif()

######################## Intel PCM ########################
add_custom_command(OUTPUT libPCM.a
                    COMMAND make lib
                    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/pcm)

add_custom_target(pcm DEPENDS libPCM.a)
###########################################################

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})

include_directories("${PROJECT_SOURCE_DIR}/include"
                    "${PROJECT_SOURCE_DIR}/pcm")

add_subdirectory(src)

#if(BUILD_TESTING)
#    add_subdirectory(googletest)
#    add_subdirectory(tests)
#endif()

#if(PIBENCH_BUILD_LEVELDB)
#  add_subdirectory(wrappers/leveldb)
#endif(PIBENCH_BUILD_LEVELDB)

# Example libraries
#add_library(tbbmap_wrapper SHARED wrappers/tbbmap/tbbmap_wrapper.cpp)
#add_library(dummy_wrapper SHARED wrappers/dummy/dummy_wrapper.cpp)
add_library(stlmap_wrapper SHARED wrappers/stlmap/stlmap_wrapper.cpp)
add_library(cuckoomap_wrapper SHARED wrappers/cuckoomap/cuckoomap_wrapper.cpp)
add_library(hopscotchmap_wrapper SHARED wrappers/hopscotchmap/hopscotchmap_wrapper.cpp)
add_library(googletree_wrapper SHARED wrappers/googletree/googletree_wrapper.cpp)
add_library(masstree_wrapper SHARED wrappers/masstree/masstree_wrapper.cpp)
add_library(bwtree_wrapper SHARED wrappers/bwtree/bwtree_wrapper.cpp)