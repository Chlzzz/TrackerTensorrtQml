cmake_minimum_required(VERSION 3.14)

# project(trt_pro VERSION 0.1 LANGUAGES CXX)

# set(CMAKE_CXX_STANDARD 11)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(CUDA_USE_STATIC_CUDA_RUNTIME OFF)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/workspace)


# 如果你是不同显卡，请设置为显卡对应的号码参考这里：https://developer.nvidia.com/zh-cn/cuda-gpus#compute
#set(CUDA_GEN_CODE "-gencode=arch=compute_75,code=sm_75")

# # 如果你的opencv找不到，可以自己指定目录
# set(OpenCV_DIR   "/usr/local/include/opencv4")
# set(CUDA_TOOLKIT_ROOT_DIR     "/usr/local/cuda")
# set(CUDNN_DIR    "/home/heu/Packages/cudnn-8.8.1.3")
# # RT-DETR 必须指定高版本的 tensorRT
# set(TENSORRT_DIR "/home/heu/Packages/TensorRT-8.6.1.6")
# # 因为protobuf，需要用特定版本，所以这里指定路径
# set(PROTOBUF_DIR "/usr/local/protobuf")

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/application/app_bytetrack)

find_package(CUDA)
find_package(OpenCV)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/application
    ${CMAKE_CURRENT_SOURCE_DIR}/src/tensorRT
    ${CMAKE_CURRENT_SOURCE_DIR}/src/tensorRT/common
    ${OpenCV_INCLUDE_DIRS}
    ${CUDA_TOOLKIT_ROOT_DIR}/include
    ${PROTOBUF_DIR}/include
    ${TENSORRT_DIR}/include
    ${CUDNN_DIR}/include
)

# 切记，protobuf的lib目录一定要比tensorRT目录前面，因为tensorRTlib下带有protobuf的so文件
# 这可能带来错误
link_directories(
    ${PROTOBUF_DIR}/lib
    ${TENSORRT_DIR}/lib
    ${CUDA_TOOLKIT_ROOT_DIR}/lib64
    ${CUDNN_DIR}/lib
    ${OpenCV_LIBS}
)

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -O0 -Wfatal-errors -pthread -w -g")
set(CUDA_NVCC_FLAGS "${CUDA_NVCC_FLAGS} -std=c++11 -O0 -Xcompiler -fPIC -g -w ${CUDA_GEN_CODE}")

option(BUILD_PANGO_BACKEND "Build with Pango backend" OFF)
option(BUILD_TRUETYPE_BACKEND "Build with TrueType backend" ON)

if(BUILD_PANGO_BACKEND)
    set(PANGO_LIBS ${PANGO_LIBS} pango-1.0 cairo pangocairo-1.0 glib-2.0 gobject-2.0)
    add_definitions(-DENABLE_TEXT_BACKEND_PANGO)
endif()

if(BUILD_TRUETYPE_BACKEND)
    add_definitions(-DENABLE_TEXT_BACKEND_STB)
endif()

file(GLOB_RECURSE cpp_srcs ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
list(FILTER cpp_srcs EXCLUDE REGEX ".*/app_bytetrack/.*")
file(GLOB_RECURSE cuda_srcs ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cu)

cuda_add_library(plugin_list SHARED ${cuda_srcs})
target_link_libraries(plugin_list nvinfer nvinfer_plugin)
target_link_libraries(plugin_list cuda cublas cudart cudnn)
target_link_libraries(plugin_list protobuf pthread)
target_link_libraries(plugin_list ${OpenCV_LIBS})
target_link_libraries(plugin_list opencv_core opencv_imgproc opencv_videoio opencv_highgui opencv_imgcodecs)

########################## custom_layernorm.so ################################
cuda_add_library(custom_layernorm SHARED
  src/tensorRT/onnxplugin/plugins/custom_layernorm.cu
)

target_link_libraries(custom_layernorm nvinfer nvinfer_plugin)


add_library(trt_pro SHARED ${cpp_srcs})
target_include_directories(trt_pro PUBLIC src/application src/tensorRT src/tensorRT/common)
# 如果提示插件找不到，请使用dlopen(xxx.so, NOW)的方式手动加载可以解决插件找不到问题
target_link_libraries(trt_pro  nvinfer nvinfer_plugin)
target_link_libraries(trt_pro  cuda cublas cudart cudnn)
target_link_libraries(trt_pro  protobuf pthread plugin_list)
target_link_libraries(trt_pro  ${OpenCV_LIBS})
target_link_libraries(trt_pro  opencv_core opencv_imgproc opencv_videoio opencv_highgui opencv_imgcodecs)


add_executable(pro ${cpp_srcs})
# 如果提示插件找不到，请使用dlopen(xxx.so, NOW)的方式手动加载可以解决插件找不到问题
target_link_libraries(pro  nvinfer nvinfer_plugin)
target_link_libraries(pro  cuda cublas cudart cudnn)
target_link_libraries(pro  protobuf pthread plugin_list)
target_link_libraries(pro  ${OpenCV_LIBS})
target_link_libraries(pro  opencv_core opencv_imgproc opencv_videoio opencv_highgui opencv_imgcodecs)

