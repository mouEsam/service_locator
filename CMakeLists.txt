cmake_minimum_required(VERSION 3.21)

set(PROJECT_NAME service_locator)

project(${PROJECT_NAME} VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED 20)

set(EXECUTABLE_NAME service_locator)

add_executable(${EXECUTABLE_NAME} main.cpp 
                                    double_dispatch_service_locator.h 
                                    static_service_locator.h 
                                    dynamic_service_locator.h)

target_include_directories(${EXECUTABLE_NAME} PRIVATE ".")