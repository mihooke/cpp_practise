cmake_minimum_required(VERSION 3.0)

project(Logging_test)

include_directories("../base/")

set(SRC_BASE
${PROJECT_SOURCE_DIR}/../base/LogStream.cpp
${PROJECT_SOURCE_DIR}/../base/Logging.cpp
${PROJECT_SOURCE_DIR}/../base/AsyncLogging.cpp
${PROJECT_SOURCE_DIR}/../base/CurrentThread.cpp
${PROJECT_SOURCE_DIR}/../base/Thread.cpp
${PROJECT_SOURCE_DIR}/../base/ThreadPool.cpp

${PROJECT_SOURCE_DIR}/../net/INetAddress.cpp
${PROJECT_SOURCE_DIR}/../net/Sockets.cpp
${PROJECT_SOURCE_DIR}/../net/Buffer.cpp
${PROJECT_SOURCE_DIR}/../net/Channel.cpp
${PROJECT_SOURCE_DIR}/../net/TcpConnection.cpp
${PROJECT_SOURCE_DIR}/../net/Acceptor.cpp
${PROJECT_SOURCE_DIR}/../net/Epoller.cpp
${PROJECT_SOURCE_DIR}/../net/EventLoop.cpp
${PROJECT_SOURCE_DIR}/../net/EventLoopThread.cpp
${PROJECT_SOURCE_DIR}/../net/EventLoopThreadPool.cpp
${PROJECT_SOURCE_DIR}/../net/TcpServer.cpp
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

set(SRC_THREADPOOL_TEST
${PROJECT_SOURCE_DIR}/ThreadPool_test.cpp
)

set(SRC_BLOCKINGQUEUE_TEST
${PROJECT_SOURCE_DIR}/BlockingQueue_test.cpp
)

set(SRC_INETADDRESS_TEST
${PROJECT_SOURCE_DIR}/INetAddress_test.cpp
)

set(SRC_BUFFER_TEST
${PROJECT_SOURCE_DIR}/Buffer_test.cpp
)

set(SRC_TCPSERVER_TEST
${PROJECT_SOURCE_DIR}/TcpServer_test.cpp
)

add_compile_options(-std=c++17)

link_libraries(pthread)

SET(CMAKE_BUILD_TYPE "Debug")
set(BUILD_DIR "./build")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ./build)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ./build)
# add_executable(Logging_test ${SRC_BASE} ${SRC_LOGGING_TEST})
# add_executable(Mutex_test ${SRC_BASE} ${SRC_MUTEX_TEST})
# add_executable(Condition_test ${SRC_BASE} ${SRC_CONDITION_TEST})
# add_executable(CountDown_test ${SRC_BASE} ${SRC_COUNTDOWN_TEST})
# add_executable(AsyncLogging_test ${SRC_BASE} ${SRC_ASYNCLOGGING_TEST})
# add_executable(Thread_test ${SRC_BASE} ${SRC_THREAD_TEST})
# add_executable(ThreadPool_test ${SRC_BASE} ${SRC_THREADPOOL_TEST})
# add_executable(BlockingQueue_test ${SRC_BASE} ${SRC_BLOCKINGQUEUE_TEST})
# add_executable(INetAddress_test ${SRC_BASE} ${SRC_INETADDRESS_TEST})
# add_executable(Buffer_test ${SRC_BASE} ${SRC_BUFFER_TEST})
add_executable(TcpServer_test ${SRC_BASE} ${SRC_TCPSERVER_TEST})
