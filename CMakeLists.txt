cmake_minimum_required(VERSION 3.7)
project(conformalpp)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp SphericalPoint.cpp SphericalPoint.h RiemannSphere.cpp RiemannSphere.h ComplexRectangle.cpp ComplexRectangle.h Color.cpp Color.h)
add_executable(conformalpp ${SOURCE_FILES})

## Mathpresso stuff
#set(CONFORMALPP_DIR         "${CMAKE_CURRENT_LIST_DIR}"   CACHE PATH "Location of 'conformalpp'")
#set(MATHPRESSO_DIR             "${CONFORMALPP_DIR}/../mathpresso" CACHE PATH "Location of 'mathpresso'")
#
#if(NOT DEFINED MATHPRESSO_EMBED)
#    set(MATHPRESSO_EMBED TRUE CACHE BOOLEAN "")
#endif()
#include("${MATHPRESSO_DIR}/CMakeLists.txt")

# OpenCV stuff
find_package( OpenCV 3 REQUIRED )
target_link_libraries( conformalpp ${OpenCV_LIBS} )

