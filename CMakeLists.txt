cmake_minimum_required(VERSION 3.5)

project(unitellercontainer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(threads REQUIRED)

add_executable(${PROJECT_NAME} src/main.cpp src/container.h src/value.h)

set (PROJECT_DIRS src)

target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_DIRS})


