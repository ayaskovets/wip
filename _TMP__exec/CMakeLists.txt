set(TARGET _TMP__exec)

add_executable(${TARGET})
target_sources(${TARGET}
    PRIVATE
    src/main.cpp
)
target_compile_features(${TARGET}
    PRIVATE cxx_std_20
)
target_link_libraries(${TARGET}
    PRIVATE _TMP_
)
