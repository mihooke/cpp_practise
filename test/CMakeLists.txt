cmake_minimum_required(VERSION 3.0)

project(Logging_test)

include_directories("../base/")

set(SRC_BASE
${PROJECT_SOURCE_DIR}/../base/LogStream.cpp
${PROJECT_SOURCE_DIR}/../base/Logging.cpp
)

set(SRC_LOGGING_TEST
${PROJECT_SOURCE_DIR}/Logging_test.cpp
)

add_compile_options(-std=c++14)

link_libraries(pthread)

SET(CMAKE_BUILD_TYPE "Debug")
add_executable(Logging_test ${SRC_BASE} ${SRC_LOGGING_TEST})
