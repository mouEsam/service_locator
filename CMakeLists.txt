cmake_minimum_required(VERSION 3.21)

set(PROJECT_NAME service_locator)

project(${PROJECT_NAME} VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED 20)

set(EXECUTABLE_NAME service_locator)

file(GLOB APP_SOURCES */*.h)
foreach (testsourcefile ${APP_SOURCES})
    get_filename_component(testname ${testsourcefile} NAME_WLE)
    get_filename_component(dirname ${testsourcefile} DIRECTORY)
    file(GLOB dir_src CONFIGURE_DEPENDS
            "${dirname}/*.h"
            "${dirname}/*.cpp"
            )
    message("${testname}.cpp | ${dir_src}")
endforeach (testsourcefile ${APP_SOURCES})

add_executable(${EXECUTABLE_NAME} main.cpp)

target_include_directories(${EXECUTABLE_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR})