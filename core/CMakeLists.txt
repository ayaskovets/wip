set(TARGET core)

add_library(${TARGET} STATIC)
target_sources(${TARGET}
    PRIVATE
    src/async/task.cpp
    src/datetime/iso8601.cpp
    src/io/fd.cpp
    src/ip/acceptor.cpp
    src/ip/address.cpp
    src/ip/connection.cpp
    src/ip/dns.cpp
    src/ip/endpoint.cpp
    src/ip/port.cpp
    src/ip/socket.cpp
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
