# SetupSphinx.cmake
# See the LICENCE file for terms and conditions.

# Creates a Python virtual environment and then installs Sphinx into it.
function(setup_venv)
    set(_pyenv ${PROJECT_BINARY_DIR}/env)
    if (EXISTS ${_pyenv})
        return()
    endif()

    # Create the virtual environment.
    find_package(PythonInterp 3.5 REQUIRED)
    message(STATUS "Creating Python environment in ${_pyenv}.")
    execute_process(COMMAND ${PYTHON_EXECUTABLE} -m venv ${_pyenv})

    # Ensure pip is up-to-date and then install Sphinx.
    set(_pip ${_pyenv}/bin/pip)
    execute_process(COMMAND ${_pip} install --upgrade pip setuptools OUTPUT_QUIET)
    execute_process(COMMAND ${_pip} install sphinx OUTPUT_QUIET)
endfunction()

# Adds a Python package into the Sphinx environment.  This main use case is for
# adding extra plugings into Sphinx.
function(sphinx_install_package package)
    set(_pyenv ${PROJECT_BINARY_DIR}/env)
    set(_python ${_pyenv}/bin/python)
    set(_pip ${_pyenv}/bin/pip)

    execute_process(COMMAND
        ${_python} -c "import ${package}"
        RESULT_VARIABLE pkg_missing
        ERROR_QUIET
    )

    if (pkg_missing)
        message(STATUS "Fetching ${package} from pypi.")
        execute_process(COMMAND ${_pip} install ${package} OUTPUT_QUIET)
    endif()
endfunction()

# Adds a documentation build target to CMake.
function(sphinx_add_docs doc_root)
    set(_sphinx ${PROJECT_BINARY_DIR}/env/bin/sphinx-build)
    set(_output ${PROJECT_BINARY_DIR}/html)

    configure_file(${doc_root}/conf.py ${_output}/conf.py @ONLY)
    add_custom_target(docs
        COMMAND ${_sphinx} -b html -E -c ${_output} ${doc_root} ${_output}
        DEPENDS ${_output}/conf.py
        COMMENT "Building Sphinx documentation."
    )
endfunction()

setup_venv()
