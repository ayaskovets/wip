set(TARGET tests)

find_package(GTest REQUIRED)
set(CMAKE_GTEST_DISCOVER_TESTS_DISCOVERY_MODE PRE_TEST)

add_executable(${TARGET})
target_sources(${TARGET}
    PRIVATE
    datetime/iso8601_test.cpp
    io/fd_test.cpp
    io/poller_test.cpp
    logging/logger_test.cpp
    net/dns/resolve_test.cpp
    net/inet/ip_test.cpp
    net/inet/port_test.cpp
    net/inet/sockaddr_test.cpp
    net/inet/tcp/socket_test.cpp
    net/inet/udp/socket_test.cpp
    net/inet6/ip_test.cpp
    net/inet6/port_test.cpp
    net/inet6/sockaddr_test.cpp
    net/inet6/tcp/socket_test.cpp
    net/inet6/udp/socket_test.cpp
    net/sockets/base_sockaddr_test.cpp
    net/unix/dgram/socket_test.cpp
    net/unix/sockaddr_test.cpp
    net/unix/stream/socket_test.cpp
    queues/locked_mpmc_queue_test.cpp
    queues/lockfree_mpmc_queue_test.cpp
    queues/lockfree_mpsc_queue_test.cpp
    queues/lockfree_spmc_queue_test.cpp
    queues/lockless_mpmc_queue_test.cpp
    queues/waitfree_spsc_queue_test.cpp
    utils/conditionally_runtime_test.cpp
    utils/predicates_test.cpp
    utils/static_pimpl_test.cpp
)
target_compile_features(${TARGET}
    PRIVATE cxx_std_20
)
target_link_libraries(${TARGET}
    PRIVATE core GTest::gtest_main
)

gtest_discover_tests(${TARGET})
