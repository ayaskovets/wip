set(TARGET todo)

add_library(${TARGET} STATIC)
target_sources(${TARGET}
    PRIVATE
    src/datetime/fmt.cpp
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
