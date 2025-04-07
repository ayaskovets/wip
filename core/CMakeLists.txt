set(TARGET core)

add_library(${TARGET} STATIC)
target_sources(${TARGET}
    PRIVATE
    src/datetime/iso8601.cpp
    src/io/fd.cpp
    src/logging/logger.cpp
    src/logging/renderer.cpp
    src/logging/writer.cpp
    src/net/inet/ip.cpp
    src/net/inet/port.cpp
    src/net/inet/sockaddr.cpp
    src/net/inet/tcp/socket.cpp
    src/net/inet/udp/socket.cpp
    src/net/sockets/base_sockaddr.cpp
    src/net/sockets/base_socket.cpp
    src/net/unix/dgram/socket.cpp
    src/net/unix/sockaddr.cpp
    src/net/unix/socket.cpp
    src/net/unix/stream/socket.cpp
)
target_compile_features(${TARGET}
    PRIVATE cxx_std_20
)
target_include_directories(${TARGET}
    PUBLIC include
)

add_subdirectory(benchmarks)
add_subdirectory(tests)
