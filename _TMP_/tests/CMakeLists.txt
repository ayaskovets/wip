set(TARGET _TMP__tests)

find_package(GTest REQUIRED)

add_executable(${TARGET})
target_sources(${TARGET}
    PRIVATE
    datetime_test.cpp
    ip_test.cpp
    logging_test.cpp
    queue_test.cpp
)
target_compile_features(${TARGET}
    PRIVATE cxx_std_20
)
target_link_libraries(${TARGET}
    PRIVATE _TMP_ GTest::gtest_main
)

gtest_discover_tests(${TARGET})
