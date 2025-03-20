set(TARGET benchmarks)

find_package(benchmark REQUIRED)

add_executable(${TARGET})
target_sources(${TARGET}
    PRIVATE
    datetime/iso8601_benchmark.cpp
    ip/address_benchmark.cpp
    ip/dns_benchmark.cpp
    logging/logging_benchmark.cpp
    threading/locked_mpmc_queue_benchmark.cpp
    threading/lockfree_spsc_queue_benchmark.cpp
    main.cpp
)
target_compile_features(${TARGET}
    PRIVATE cxx_std_20
)
target_link_libraries(${TARGET}
    PRIVATE core benchmark::benchmark
)
