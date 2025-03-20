set(TARGET _TMP__benchmarks)

find_package(benchmark REQUIRED)

add_executable(${TARGET})
target_sources(${TARGET}
    PRIVATE
    # datetime_benchmark.cpp
    # ip_benchmark.cpp
    # logging_benchmark.cpp
    main.cpp
    queue_benchmark.cpp
)
target_compile_features(${TARGET}
    PRIVATE cxx_std_20
)
target_link_libraries(${TARGET}
    PRIVATE _TMP_ benchmark::benchmark
)
