cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

project(HowToRuinGD VERSION 1.0.0)

file(GLOB SOURCES CONFIGURE_DEPENDS src/*.cpp)
add_library(${PROJECT_NAME} SHARED ${SOURCES})

# Pinned: Geode 6.0.0-alpha.1. Point GEODE_SDK at a checkout of tag v6.0.0-alpha.1.
if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "Set GEODE_SDK to a checkout of geode-sdk/geode at tag v6.0.0-alpha.1")
endif()
add_subdirectory($ENV{GEODE_SDK} ${CMAKE_CURRENT_BINARY_DIR}/geode)

setup_geode_mod(${PROJECT_NAME})
