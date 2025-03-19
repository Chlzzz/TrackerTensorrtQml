cmake_minimum_required(VERSION 3.14)

project(NetWorkShow VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CUDA_TOOLKIT_ROOT_DIR     "/usr/local/cuda")
set(CUDNN_DIR    "/home/nh/Packages/cudnn-8.8.1.3")
# RT-DETR 必须指定高版本的 tensorRT
set(TENSORRT_DIR "/home/nh/Packages/TensorRT-8.6.1.6")
# 因为protobuf，需要用特定版本，所以这里指定路径
set(PROTOBUF_DIR "/usr/local/protobuf")


add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tensorRT_Pro_Model)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
#set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_CURRENT_SOURCE_DIR}/iconwin/app.rc")

# ===== 宏定义
set(CMAKE_BUILD_TYPE DEBUG)
if (CMAKE_BUILD_TYPE STREQUAL "DEBUG")
    add_definitions(-D_DEBUG)
endif()
# 检测操作系统并定义宏
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    add_definitions(-DUNIX)  # 定义 LINUX 宏
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    add_definitions(-DWIN32)  # 定义 WINDOWS 宏
endif()
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_VIDEO")


find_package(CUDA REQUIRED)
find_package(OpenCV REQUIRED)
find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Widgets  Quick )
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Widgets  Quick )


include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${OpenCV_INCLUDE_DIRS}

    ${CUDA_TOOLKIT_ROOT_DIR}/include
    ${PROTOBUF_DIR}/include
    ${TENSORRT_DIR}/include
    ${CUDNN_DIR}/include

)

link_directories(
    ${PROTOBUF_DIR}/lib
    ${TENSORRT_DIR}/lib
    ${CUDA_TOOLKIT_ROOT_DIR}/lib64
    ${CUDNN_DIR}/lib
    ${OpenCV_LIBS}
)


set(PROJECT_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/include/utility.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/statusmonitor.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/threadcontroller.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/imageprocess.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ocvimageprovider.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/app_rtdetr.h
        
        ${CMAKE_CURRENT_SOURCE_DIR}/src/utility.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/statusmonitor.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/threadcontroller.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/imageprocess.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ocvimageprovider.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/app_rtdetr.cpp
        
        qml.qrc
)



if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(${PROJECT_NAME}
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET ${PROJECT_NAME} APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(${PROJECT_NAME} SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(${PROJECT_NAME}
          ${PROJECT_SOURCES}
          ${APP_ICON_RESOURCE_WINDOWS}
        )
    endif()
endif()


target_compile_definitions(${PROJECT_NAME}
  PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)

target_include_directories(${PROJECT_NAME} 
  PRIVATE 
      ${CMAKE_CURRENT_SOURCE_DIR}/tensorRT_Pro_Model/src/application
      ${CMAKE_CURRENT_SOURCE_DIR}/tensorRT_Pro_Model/src/tensorRT
      ${CMAKE_CURRENT_SOURCE_DIR}/tensorRT_Pro_Model/src/tensorRT/common
      ${CMAKE_CURRENT_SOURCE_DIR}/bytetrack/include
)

target_link_libraries(${PROJECT_NAME}
  PRIVATE
      Qt${QT_VERSION_MAJOR}::Core
      Qt${QT_VERSION_MAJOR}::Quick
      Qt${QT_VERSION_MAJOR}::Widgets
      ${OpenCV_LIBS}

     trt_pro
     plugin_list
)


set_target_properties(${PROJECT_NAME} PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_import_qml_plugins(${PROJECT_NAME})
    qt_finalize_executable(${PROJECT_NAME})
endif()




