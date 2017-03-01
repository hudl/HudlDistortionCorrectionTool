cmake_minimum_required(VERSION 2.8)

project( DistortionCorrection )

IF (${CMAKE_HOST_APPLE})
    cmake_policy(SET CMP0042 NEW)
ENDIF()

find_package( OpenCV REQUIRED )
include_directories(.)
add_compile_options(-std=gnu++11 -stdlib=libc++)
FILE(GLOB DC_SOURCE_FILES
        CameraCalibrator/*.cpp
        cvui/*.cpp
        Undistorter/*.cpp)

ADD_LIBRARY(DCLib SHARED ${DC_SOURCE_FILES})
target_link_libraries( DCLib ${OpenCV_LIBS} )
add_executable( DistortionCorrection DistortionCorrection.cpp )
target_link_libraries(DistortionCorrection DCLib)
