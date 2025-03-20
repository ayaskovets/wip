set(TARGET todo)

add_library(${TARGET} STATIC)
target_sources(${TARGET}
    PRIVATE
    src/datetime/iso8601.cpp
    src/logging/logger.cpp
    src/logging/renderer.cpp
    src/logging/writer.cpp
    src/ip/address.cpp
    src/ip/dns.cpp
)
target_compile_features(${TARGET}
    PRIVATE cxx_std_20
)
target_include_directories(${TARGET}
    PUBLIC include
)

add_subdirectory(benchmarks)
add_subdirectory(tests)
