set(TARGET _TMP_)

add_library(${TARGET} STATIC)
target_sources(${TARGET}
    PRIVATE
    src/async/task.cpp
    src/async/thread_pool.cpp
    src/datetime/iso8601.cpp
    src/ip/address.cpp
    src/ip/dns.cpp
    src/ip/port.cpp
    src/logging/logger.cpp
    src/logging/renderer.cpp
    src/logging/writer.cpp
)
target_compile_features(${TARGET}
    PRIVATE cxx_std_20
)
target_include_directories(${TARGET}
    PUBLIC include
)

add_subdirectory(benchmarks)
add_subdirectory(tests)
