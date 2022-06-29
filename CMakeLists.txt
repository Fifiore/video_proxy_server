cmake_minimum_required(VERSION 3.15)
project(video_proxy)

find_package(GTest)
find_package(Boost)
find_package(OpenSSL)

add_executable(video_proxy_server src/server.cpp src/video_proxy_data.hpp 
	src/listen.hpp src/listen.cpp src/session.hpp src/session.cpp src/target_source.hpp 
	src/plain_stream.hpp src/plain_stream.cpp src/ssl_stream.hpp src/ssl_stream.cpp
	src/traces.hpp src/traces.cpp src/parser.hpp src/parser.cpp src/error_message.hpp)
target_compile_features(video_proxy_server PRIVATE cxx_std_17)

add_executable(video_proxy_test src/test/parser_test.cpp src/test/traces_test.cpp src/traces.hpp src/traces.cpp src/parser.hpp src/parser.cpp)
target_compile_features(video_proxy_test PRIVATE cxx_std_17)
target_include_directories(video_proxy_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(video_proxy_server PRIVATE Boost::headers Boost::coroutine OpenSSL::SSL OpenSSL::Crypto)
target_link_libraries(video_proxy_test PRIVATE GTest::gtest GTest::gtest_main Boost::headers)

