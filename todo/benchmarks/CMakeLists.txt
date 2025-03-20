set(TARGET todo_benchmarks)

find_package(benchmark REQUIRED)

add_executable(${TARGET})
target_sources(${TARGET}
    PRIVATE
    datetime_benchmark.cpp
    logging_benchmark.cpp
    main.cpp
)
target_compile_features(${TARGET}
    PRIVATE cxx_std_20
)
target_link_libraries(${TARGET}
    PRIVATE todo benchmark::benchmark
)
