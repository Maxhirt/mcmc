# CMakeLists.txt

cmake_minimum_required(VERSION 3.10)
project(MCMCLib CXX)

# --- Options from the bash script ---

# Build type options
option(MCMCLIB_COVERAGE_BUILD "Enable code coverage build" OFF)
option(MCMCLIB_DEV_BUILD "Enable developmental build" OFF)
option(MCMCLIB_DEBUG_BUILD "Enable debugging build (-O0 -g)" OFF)
option(MCMCLIB_ENABLE_OPENMP "Enable OpenMP parallelization" OFF)

# Configuration options
set(MCMCLIB_FPN_TYPE "double" CACHE STRING "Floating-point number type (e.g., float, double)")
set(MCMCLIB_LINEAR_ALG_LIB "arma" CACHE STRING "Choice of linear algebra library (e.g., arma, eigen)")
set(MCMCLIB_BLAS_LAPACK_LIBS "" CACHE STRING "Specify the BLAS and Lapack libraries to link against")
set(MCMCLIB_OPTIMIZATION_FLAGS "" CACHE STRING "Compiler optimization options")

# Special option for header-only version
option(MCMCLIB_GENERATE_HEADER_ONLY "Generate a header-only version of MCMCLib" OFF)

# --- System and Compiler Checks ---

if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(IS_DARWIN TRUE)
else()
    set(IS_DARWIN FALSE)
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(IS_GCC TRUE)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(IS_CLANG TRUE)
endif()

# --- Header-Only Generation ---

if(MCMCLIB_GENERATE_HEADER_ONLY)
    # This logic can be more complex to translate directly. A custom script might be easier.
    # However, a simplified CMake version could look like this:

    set(HEADER_ONLY_DIR ${CMAKE_CURRENT_BINARY_DIR}/header_only_version)
    file(REMOVE_RECURSE ${HEADER_ONLY_DIR})
    file(MAKE_DIRECTORY ${HEADER_ONLY_DIR})

    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION ${HEADER_ONLY_DIR})

    # The sed logic is complex to replicate in pure CMake.
    # It might be better to call a separate script if this is a critical feature.
    # For now, this part is left as a placeholder.
    message(STATUS "Header-only generation is a complex script and is not fully translated.")
    return()
endif()

# --- Find Linear Algebra Library ---

if(MCMCLIB_LINEAR_ALG_LIB STREQUAL "arma")
    find_package(Armadillo REQUIRED)
    if(Armadillo_FOUND)
        set(MCMCLIB_CXX_STD "c++11")
        set(MCMCLIB_MATLIB_FLAGS "-DMCMC_ENABLE_ARMA_WRAPPERS -DARMA_NO_DEBUG")
        include_directories(${Armadillo_INCLUDE_DIRS})
        list(APPEND MCMCLIB_BLAS_LAPACK_LIBS ${Armadillo_LIBRARIES})
    endif()
elseif(MCMCLIB_LINEAR_ALG_LIB STREQUAL "eigen")
    find_package(Eigen3 REQUIRED)
    if(Eigen3_FOUND)
        set(MCMCLIB_CXX_STD "c++14")
        set(MCMCLIB_MATLIB_FLAGS "-DMCMC_ENABLE_EIGEN_WRAPPERS")
        include_directories(${EIGEN3_INCLUDE_DIRS})
    endif()
else()
    message(FATAL_ERROR "Unrecognized linear algebra library: ${MCMCLIB_LINEAR_ALG_LIB}")
endif()

# --- Set Compiler Flags ---

set(CMAKE_CXX_STANDARD ${MCMCLIB_CXX_STD})
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_definitions(${MCMCLIB_MATLIB_FLAGS})
add_definitions(-DMCMC_FPN_TYPE=${MCMCLIB_FPN_TYPE})

if(MCMCLIB_COVERAGE_BUILD)
    if(IS_GCC)
        add_compile_options(-g -O0 --coverage -fno-inline -fno-inline-small-functions -fno-default-inline)
    else() # Clang
        add_compile_options(-g -O0 --coverage -fno-inline)
    endif()
elseif(MCMCLIB_DEBUG_BUILD)
    set(CMAKE_BUILD_TYPE Debug)
    add_compile_options(-O0 -g)
else()
    set(CMAKE_BUILD_TYPE Release)
    if(NOT MCMCLIB_OPTIMIZATION_FLAGS)
        set(MCMCLIB_OPTIMIZATION_FLAGS "-O3 -ffp-contract=fast -flto -DNDEBUG")
        if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm")
            set(MCMCLIB_OPTIMIZATION_FLAGS "-mcpu=native ${MCMCLIB_OPTIMIZATION_FLAGS}")
        else()
            set(MCMCLIB_OPTIMIZATION_FLAGS "-march=native ${MCMCLIB_OPTIMIZATION_FLAGS}")
        endif()
    endif()
    add_compile_options(${MCMCLIB_OPTIMIZATION_FLAGS})
endif()

if(MCMCLIB_ENABLE_OPENMP)
    find_package(OpenMP)
    if(OpenMP_FOUND)
        add_compile_options(${OpenMP_CXX_FLAGS})
    endif()
endif()

add_compile_options(-Wall)

# --- Add the library ---

add_library(mcmc SHARED src/mcmc.cpp) # Add all your source files here

# --- Link Libraries ---

target_link_libraries(mcmc PRIVATE ${MCMCLIB_BLAS_LAPACK_LIBS})

# --- Installation ---

include(GNUInstallDirs)
install(TARGETS mcmc
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mcmclib)

# --- Final Summary ---

message(STATUS "--- MCMCLib Configuration Summary ---")
message(STATUS "OS: ${CMAKE_SYSTEM_NAME}")
message(STATUS "Arch: ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "C++ Compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Linear Algebra Library: ${MCMCLIB_LINEAR_ALG_LIB}")
if(MCMCLIB_ENABLE_OPENMP)
    message(STATUS "OpenMP enabled")
else()
    message(STATUS "OpenMP disabled")
endif()
message(STATUS "Floating-point type: ${MCMCLIB_FPN_TYPE}")
message(STATUS "Install path: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "--- End Summary ---")