cmake_minimum_required (VERSION 3.0.2)

configure_file (
    "${CMAKE_SOURCE_DIR}/gui/toonchess/get_config_path.py.in"
    "${CMAKE_SOURCE_DIR}/gui/toonchess/get_config_path.py"
)
configure_file (
    "${CMAKE_SOURCE_DIR}/gui/toonchess/get_share_path.py.in"
    "${CMAKE_SOURCE_DIR}/gui/toonchess/get_share_path.py"
)

find_program(PYTHON "python" REQUIRED)

if (PYTHON)
    set(SETUP_PY_IN "${CMAKE_CURRENT_SOURCE_DIR}/setup.py.in")
    set(SETUP_PY "${CMAKE_CURRENT_SOURCE_DIR}/setup.py")
    set(DEPS "${CMAKE_CURRENT_SOURCE_DIR}/toonchess/__init__.py")
    set(OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/build/timestamp")

    configure_file(${SETUP_PY_IN} ${SETUP_PY})

    add_custom_command(OUTPUT ${OUTPUT_DIR}
                       COMMAND ${PYTHON} ${SETUP_PY} build
                       COMMAND ${CMAKE_COMMAND} -E touch ${OUTPUT_DIR}
                       DEPENDS ${DEPS})

    add_custom_target(target ALL DEPENDS ${OUTPUT_DIR})

    install(CODE "execute_process(COMMAND ${PYTHON} ${SETUP_PY} install)")
endif()
