set(TARGET _TMP__tests)

find_package(GTest REQUIRED)

add_executable(${TARGET})
target_sources(${TARGET}
    PRIVATE
    datetime/iso8601_test.cpp
    ip/address_test.cpp
    ip/dns_test.cpp
    logging/logging_test.cpp
    threading/lockfree_spsc_queue_test.cpp
    threading/locked_mpmc_queue_test.cpp
    utils/conditionally_runtime_test.cpp
    utils/predicates_test.cpp
)
target_compile_features(${TARGET}
    PRIVATE cxx_std_20
)
target_link_libraries(${TARGET}
    PRIVATE _TMP_ GTest::gtest_main
)

gtest_discover_tests(${TARGET})
