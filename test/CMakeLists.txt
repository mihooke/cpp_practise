cmake_minimum_required(VERSION 3.0)

project(Logging_test)

include_directories("../base/")

set(SRC_BASE
${PROJECT_SOURCE_DIR}/../base/LogStream.cpp
${PROJECT_SOURCE_DIR}/../base/Logging.cpp
${PROJECT_SOURCE_DIR}/../base/AsyncLogging.cpp
${PROJECT_SOURCE_DIR}/../base/CurrentThread.cpp
${PROJECT_SOURCE_DIR}/../base/Thread.cpp
)

set(SRC_LOGGING_TEST
${PROJECT_SOURCE_DIR}/Logging_test.cpp
)

set(SRC_MUTEX_TEST
${PROJECT_SOURCE_DIR}/Mutex_test.cpp
)

set(SRC_CONDITION_TEST
${PROJECT_SOURCE_DIR}/Condition_test.cpp
)

set(SRC_COUNTDOWN_TEST
${PROJECT_SOURCE_DIR}/CountDown_test.cpp
)

set(SRC_ASYNCLOGGING_TEST
${PROJECT_SOURCE_DIR}/AsyncLogging_test.cpp
)

set(SRC_THREAD_TEST
${PROJECT_SOURCE_DIR}/Thread_test.cpp
)

add_compile_options(-std=c++14)

link_libraries(pthread)

SET(CMAKE_BUILD_TYPE "Debug")
set(BUILD_DIR "./build")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ./build)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ./build)
add_executable(Logging_test ${SRC_BASE} ${SRC_LOGGING_TEST})
add_executable(Mutex_test ${SRC_BASE} ${SRC_MUTEX_TEST})
add_executable(Condition_test ${SRC_BASE} ${SRC_CONDITION_TEST})
add_executable(CountDown_test ${SRC_BASE} ${SRC_COUNTDOWN_TEST})
add_executable(AsyncLogging_test ${SRC_BASE} ${SRC_ASYNCLOGGING_TEST})
add_executable(Thread_test ${SRC_BASE} ${SRC_THREAD_TEST})
