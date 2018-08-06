cmake_minimum_required(VERSION 3.6.0)
project(chromavec VERSION 1.0.0)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
add_compile_options(-Wall)

# Build options
# --
option(CHROMAVEC_BUILD_APPS "Build the command line applications." ON)
option(CHROMAVEC_BUILD_DOCS "Build the chromavec documentation." OFF)

# Project dependencies
# --

# Update any submodules.
find_package(Git REQUIRED)
execute_process(
    COMMAND ${GIT_EXECUTABLE} submodule update --init
    WORKING_DIRECTORY ${chromavec_SOURCE_DIR}
)

# Setup TBB for build.
set(_tbb_root ${PROJECT_SOURCE_DIR}/extern/tbb)
include(${_tbb_root}/cmake/TBBBuild.cmake)
tbb_build(TBB_ROOT ${_tbb_root} CONFIG_DIR TBB_DIR)

# Add the requirements (TBB and OpenCV)
find_package(TBB REQUIRED)
find_package(OpenCV 3 COMPONENTS core imgproc imgcodecs REQUIRED)


# Build Setup
# --

# Build the CLI apps.
if(CHROMAVEC_BUILD_APPS)
    message(STATUS "Building command line applications.")
    add_subdirectory(${chromavec_SOURCE_DIR}/extern/CLI11)
endif()

# Generate the version file.
configure_file(
    ${chromavec_SOURCE_DIR}/include/chromavec/version.h.in
    ${chromavec_BINARY_DIR}/include/chromavec/version.h
    @ONLY
)
message(STATUS "Wrote 'version.h' to '${chromavec_BINARY_DIR}/include/${PROJECT_NAME}'.")

# Add the project sources.
add_subdirectory(src)

# Documentation
# --
if (CHROMAVEC_BUILD_DOCS)
    include(${chromavec_SOURCE_DIR}/cmake/SetupSphinx.cmake)
    sphinx_install_package(recommonmark)
    sphinx_install_package(sphinx_rtd_theme)
    sphinx_add_docs(${chromavec_SOURCE_DIR}/docs)
endif()
