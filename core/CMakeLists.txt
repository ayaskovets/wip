set(TARGET core)

add_library(${TARGET} STATIC)
target_sources(${TARGET}
    PRIVATE
    src/async/task.cpp
    src/datetime/iso8601.cpp
    src/io/fd.cpp
    src/net/ip/address.cpp
    src/net/ip/dns.cpp
    src/net/ip/endpoint.cpp
    src/net/ip/port.cpp
    src/net/ip/socket.cpp
    src/net/ip/tcp/acceptor.cpp
    src/net/ip/tcp/connection.cpp
    src/net/ip/tcp/socket.cpp
    src/net/ip/udp/socket.cpp
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
