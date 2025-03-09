set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

# set(OpenCV_DIR   "/usr/local/include/opencv4")
# set(CUDA_TOOLKIT_ROOT_DIR     "/usr/local/cuda")
# set(TENSORRT_DIR "/home/nh/Packages/TensorRT-8.6.1.6")

find_package(CUDA)
find_package(OpenCV)
find_package(Eigen3 REQUIRED)

include_directories(
    ${OpenCV_INCLUDE_DIRS}
    ${EIGEN3_INCLUDE_DIR}
    ${CUDA_TOOLKIT_ROOT_DIR}/include
    ${TENSORRT_DIR}/include
)

link_directories(
    ${TENSORRT_DIR}/lib
    ${CUDA_TOOLKIT_ROOT_DIR}/lib64
    ${OpenCV_DIR}/lib
)

# =========== bytetrack lib ===========
include_directories(${PROJECT_SOURCE_DIR}/bytetrack/include)
file(GLOB_RECURSE SRCS01 ${PROJECT_SOURCE_DIR}/bytetrack/src/*.cpp)
add_library(bytetrack SHARED ${SRCS01})
target_link_libraries(bytetrack cudart nvinfer ${OpenCV_LIBS})