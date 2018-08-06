function(build_app appname)
    add_executable(${appname} ${appname}.cpp)
    target_link_libraries(${appname} PRIVATE chromavec CLI11::CLI11)
    set_target_properties(${appname}
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${chromavec_BINARY_DIR}/bin
    )
endfunction()

build_app(apply-filter)
build_app(detect-edges)
