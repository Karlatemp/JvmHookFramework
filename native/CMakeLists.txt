cmake_minimum_required(VERSION 3.12)
project(native)

include_directories(includes/jni)
include_directories(../core/build/generated/sources/headers/java/main)

IF (WIN32)
   include_directories(includes/jni/win32)
ELSEIF(UNIX)
    include_directories(includes/jni/unix)
ELSE()
    set(JAVA_AWT_LIBRARY NotNeeded)
    find_package(JNI REQUIRED)

    include_directories(${JAVA_INCLUDE_PATH})
    include_directories(${JAVA_INCLUDE_PATH2})
ENDIF ()

AUX_SOURCE_DIRECTORY(src SRC_DIR)

set(CMAKE_ANDROID_ARM_MODE ON)
set(CMAKE_ANDROID_ARM_NEON ON)
set(CMAKE_ANDROID_STL_TYPE system)
add_compile_definitions(DEBUG)

add_library(native SHARED ${SRC_DIR} src/lib.cpp)
