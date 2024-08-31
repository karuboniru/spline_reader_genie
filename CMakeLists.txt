cmake_minimum_required(VERSION 3.12)
project(spline_reader_genie)

# Set C++ Version to 23
set(CMAKE_CXX_STANDARD 23)

# Enable -O3 optimization level
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

# Enable Link-Time Optimization (LTO)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

# Find ROOT package
find_package(ROOT REQUIRED COMPONENTS Hist)

# Include ROOT headers
include(${ROOT_USE_FILE})
include_directories(${ROOT_INCLUDE_DIRS})

# Add the source files
set(SOURCES spline.cxx)

# Add the header files
set(HEADERS spline.h)

# Add the library target
add_library(spline SHARED ${SOURCES})

# Link the library with CERN ROOT
target_link_libraries(spline PRIVATE ROOT::Hist)

# Install the library and headers
install(TARGETS spline LIBRARY DESTINATION lib)
install(FILES ${HEADERS} DESTINATION include)

# A simple demo
# Add the executable target
add_executable(test test.cpp)

# Link the executable with the spline library
target_link_libraries(test PRIVATE spline)