set(TARGET tests)

find_package(GTest REQUIRED)
set(CMAKE_GTEST_DISCOVER_TESTS_DISCOVERY_MODE PRE_TEST)

add_executable(${TARGET})
target_sources(${TARGET}
    PRIVATE
    datetime/iso8601_test.cpp
    io/fd_test.cpp
    logging/logger_test.cpp
    net/sockets/base_sockaddr_test.cpp
    net/sockets/base_socket_test.cpp
    net/unix/dgram/socket_test.cpp
    net/unix/sockaddr_test.cpp
    net/unix/stream/socket_test.cpp
    utils/conditionally_runtime_test.cpp
    utils/predicates_test.cpp
)
target_compile_features(${TARGET}
    PRIVATE cxx_std_20
)
target_link_libraries(${TARGET}
    PRIVATE core GTest::gtest_main
)

gtest_discover_tests(${TARGET})
