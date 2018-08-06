add_executable(edge-image edge-image.cpp)
target_link_libraries(edge-image PRIVATE chromavec)
target_include_directories(edge-image
    PRIVATE
    ${chromavec_SOURCE_DIR}/src/chromavec
)
set_target_properties(edge-image
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${chromavec_BINARY_DIR}/test
)
