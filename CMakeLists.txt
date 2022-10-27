cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(LibtorchDetection)

SET(CMAKE_BUILD_TYPE "Debug")

# set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} "/path2libtorch")
set(Torch_DIR $ENV{Torch_DIR}/share/cmake/Torch)


find_package(Torch REQUIRED)
find_package(OpenCV REQUIRED)   
if(NOT OpenCV_FOUND)
    message("Not found oepncv")
endif()
if(NOT Torch_FOUND)
    message("Not found Torch")
endif()


FILE(GLOB ALL_SOURCES "*.cpp" "./src/*.cpp" "./src/models/*.cpp" "./src/utils/*.cpp")
add_executable(LibtorchDetection src/main.cpp ${ALL_SOURCES})
target_link_libraries(LibtorchDetection "${TORCH_LIBRARIES}")
target_link_libraries(LibtorchDetection "${OpenCV_LIBS}")

set_property(TARGET LibtorchDetection PROPERTY CXX_STANDARD 14)
