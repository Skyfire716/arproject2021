# Install script for directory: /home/jonas/openCVBuild/opencv/include

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/home/jonas/openCVBuild/opencv/android_armeapi_v7/install")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "0")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "TRUE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/home/jonas/Android/ndk/21.3.6528147/toolchains/llvm/prebuilt/linux-x86_64/bin/arm-linux-androideabi-objdump")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/sdk/native/jni/include/opencv" TYPE FILE FILES
    "/home/jonas/openCVBuild/opencv/include/opencv/cv.h"
    "/home/jonas/openCVBuild/opencv/include/opencv/cv.hpp"
    "/home/jonas/openCVBuild/opencv/include/opencv/cvaux.h"
    "/home/jonas/openCVBuild/opencv/include/opencv/cvaux.hpp"
    "/home/jonas/openCVBuild/opencv/include/opencv/cvwimage.h"
    "/home/jonas/openCVBuild/opencv/include/opencv/cxcore.h"
    "/home/jonas/openCVBuild/opencv/include/opencv/cxcore.hpp"
    "/home/jonas/openCVBuild/opencv/include/opencv/cxeigen.hpp"
    "/home/jonas/openCVBuild/opencv/include/opencv/cxmisc.h"
    "/home/jonas/openCVBuild/opencv/include/opencv/highgui.h"
    "/home/jonas/openCVBuild/opencv/include/opencv/ml.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/sdk/native/jni/include/opencv2" TYPE FILE FILES "/home/jonas/openCVBuild/opencv/include/opencv2/opencv.hpp")
endif()

