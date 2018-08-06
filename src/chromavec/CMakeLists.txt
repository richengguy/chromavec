set(CHROMAVEC_INCLUDES
    ${chromavec_SOURCE_DIR}/include/chromavec/chromavec.h
    ${chromavec_BINARY_DIR}/include/chromavec/version.h
)

set(CHROMAVEC_SOURCES
    chromavec.cpp
    version.cpp

    constants.h

    utilities/filter.h
    utilities/rgbvector.h
    utilities/roi.h
    utilities/roi.cpp

    filters/canny-edges.h
    filters/minimum-vector-dispersion.h
    filters/minimum-vector-dispersion.cpp
    filters/vmf.h
    filters/vmf.cpp
    filters/vector-range.h
    filters/vector-range.cpp
)

# Define the chromavec library and its targets.
add_library(chromavec ${CHROMAVEC_INCLUDES} ${CHROMAVEC_SOURCES})

target_compile_features(chromavec PUBLIC cxx_std_17)

target_include_directories(chromavec
    PUBLIC ${chromavec_SOURCE_DIR}/include
           ${chromavec_BINARY_DIR}/include
    PRIVATE ${chromavec_SOURCE_DIR}/src/chromavec
)

target_link_libraries(chromavec
    PUBLIC
    TBB::tbb
    opencv_core
    opencv_imgcodecs
    opencv_imgproc
)

set_target_properties(chromavec
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${chromavec_BINARY_DIR}/lib
    LIBRARY_OUTPUT_DIRECTORY ${chromavec_BINARY_DIR}/lib
)
