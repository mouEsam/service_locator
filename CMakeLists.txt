cmake_minimum_required(VERSION 3.21)

set(PROJECT_NAME service_locator)

project(${PROJECT_NAME} VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED 20)

set(EXECUTABLE_NAME service_locator)

message(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(${EXECUTABLE_NAME} main.cpp)

target_include_directories(${EXECUTABLE_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})