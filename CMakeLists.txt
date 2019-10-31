cmake_minimum_required(VERSION 2.8.3)
project(pgm_segmentation)

set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_CXX_FLAGS "-std=c++11")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -Wall -g")

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

find_package(OpenCV REQUIRED)

message(WARNING "OpenCV_VERSION: ${OpenCV_VERSION}")

include_directories(
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_SOURCE_DIR}/ThirdParty/maxflow
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)

add_library(${PROJECT_NAME} SHARED
src/graphBasedSegmentation.cc
ThirdParty/maxflow/graph.cpp
ThirdParty/maxflow/maxflow.cpp
)

target_link_libraries(${PROJECT_NAME}
${OpenCV_LIBS}
)

add_executable(run_seg example/segment.cc)
target_link_libraries(run_seg 
  ${PROJECT_NAME}) 

add_executable(run_cut example/cut.cc)
target_link_libraries(run_cut
  ${PROJECT_NAME}) 


