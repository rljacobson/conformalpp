cmake_minimum_required(VERSION 3.7)
project(conformalpp)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES src/main.cpp src/SphericalPoint.cpp src/SphericalPoint.h src/RiemannSphere.cpp src/RiemannSphere.h src/ComplexRectangle.cpp src/ComplexRectangle.h src/Color.cpp src/Color.h src/FastMath.cpp src/FastMath.h)
add_executable(conformalpp ${SOURCE_FILES} )

# Enable SSE instructions.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse -msse2 -msse3")

## Mathpresso stuff
set(MATHPRESSO_DIR ../mathpresso)
include("${MATHPRESSO_DIR}/CMakeLists.txt")
include_directories(${MATHPRESSO_INCLUDE_DIR})

# OpenCV stuff
find_package( OpenCV 3 REQUIRED )

target_link_libraries( conformalpp ${OpenCV_LIBS} mathpresso)

