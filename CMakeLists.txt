cmake_minimum_required (VERSION 2.8.2)
set(CMAKE_CXX_STANDARD 11)

# Enable debug symbols by default
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
endif()

project(3DToonChess)

# The version number
set(3DToonChess_VERSION_MAJOR 0)
set(3DToonChess_VERSION_MINOR 1)

# Add headers
set(
  CXX_FILES
  ${CMAKE_SOURCE_DIR}/src/mesh/Mesh.cxx
  ${CMAKE_SOURCE_DIR}/src/mesh/meshes.cxx
  ${CMAKE_SOURCE_DIR}/src/chessBoard/chessBoard.cxx
  ${CMAKE_SOURCE_DIR}/src/shader/CompilationException.cxx
  ${CMAKE_SOURCE_DIR}/src/shader/LinkingException.cxx
  ${CMAKE_SOURCE_DIR}/src/shader/Shader.cxx
  ${CMAKE_SOURCE_DIR}/src/shader/ShaderProgram.cxx
  ${CMAKE_SOURCE_DIR}/src/shader/shaderPrograms.cxx
  ${CMAKE_SOURCE_DIR}/src/ColorPicking/ColorPicking.cxx
  ${CMAKE_SOURCE_DIR}/src/ShadowMapping/ShadowMapping.cxx
  ${CMAKE_SOURCE_DIR}/src/StockfishConnector/ConnectionException.cxx
  ${CMAKE_SOURCE_DIR}/src/StockfishConnector/StockfishConnector.cxx
  ${CMAKE_SOURCE_DIR}/src/utils.cxx
)

# Define sources and executable
set(EXECUTABLE_NAME "3DToonChess")
add_executable(${EXECUTABLE_NAME} src/3DToonChess.cxx ${CXX_FILES})

# Download and unpack googletest
configure_file(CMakeLists-googletest.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download
)
if(result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Add googletest.
add_subdirectory(
  ${CMAKE_BINARY_DIR}/googletest-src
  ${CMAKE_BINARY_DIR}/googletest-build)

# Download and install stockfish
message(STATUS "Installing stockfish with apt-get (needs root permission):")
execute_process(COMMAND sudo apt-get install stockfish
  RESULT_VARIABLE result)
if(result)
  message(FATAL_ERROR "Install step for stockfish failed: ${result}")
endif()


# Create test executable
set(TEST_NAME "tests")
add_executable(${TEST_NAME} tests/test_main.cxx ${CXX_FILES})
target_link_libraries(${TEST_NAME} gtest_main)


# Detect and add SFML
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})

find_package(SFML 2 REQUIRED graphics window system)
if(SFML_FOUND)
  include_directories(${SFML_INCLUDE_DIR})
  target_link_libraries(${EXECUTABLE_NAME} ${SFML_LIBRARIES} ${SFML_DEPENDENCIES})
  target_link_libraries(${TEST_NAME} ${SFML_LIBRARIES} ${SFML_DEPENDENCIES})
endif()

# OpenGL
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})
if (OPENGL_FOUND)
  target_link_libraries(${EXECUTABLE_NAME} ${OPENGL_LIBRARIES})
  target_link_libraries(${TEST_NAME} ${OPENGL_LIBRARIES})
  # target_link_libraries(${EXECUTABLE_NAME} m)
endif()
