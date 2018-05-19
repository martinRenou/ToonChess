cmake_minimum_required (VERSION 3.0.2)
set(CMAKE_CXX_STANDARD 11)

project(ToonChess)

# The version number
set(TOONCHESS_VERSION_MAJOR 0)
set(TOONCHESS_VERSION_MINOR 1)
set(TOONCHESS_VERSION "${TOONCHESS_VERSION_MAJOR}.${TOONCHESS_VERSION_MINOR}")

set(TOONCHESS_SHARE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/share)
set(TOONCHESS_CONFIG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/config)

set(TOONCHESS_SHARE_PATH "\"${CMAKE_INSTALL_PREFIX}/share/toonchess/\"")
set(TOONCHESS_CONFIG_PATH "\"$ENV{HOME}/.config/toonchess/\"")
configure_file (
    "${CMAKE_SOURCE_DIR}/src/get_share_path.cxx.in"
    "${CMAKE_SOURCE_DIR}/src/get_share_path.cxx"
)

# Add CXX files
set(
  CXX_FILES
  ${CMAKE_SOURCE_DIR}/src/Camera/Camera.cxx

  ${CMAKE_SOURCE_DIR}/src/ChessGame/ConnectionException.cxx
  ${CMAKE_SOURCE_DIR}/src/ChessGame/StockfishConnector.cxx
  ${CMAKE_SOURCE_DIR}/src/ChessGame/GameException.cxx
  ${CMAKE_SOURCE_DIR}/src/ChessGame/ChessGame.cxx

  ${CMAKE_SOURCE_DIR}/src/Clock/Clock.cxx

  ${CMAKE_SOURCE_DIR}/src/ColorPicking/ColorPicking.cxx

  ${CMAKE_SOURCE_DIR}/src/Event/EventStack.cxx

  ${CMAKE_SOURCE_DIR}/src/mesh/Mesh.cxx
  ${CMAKE_SOURCE_DIR}/src/mesh/meshes.cxx
  ${CMAKE_SOURCE_DIR}/src/mesh/loadObjFile.cxx

  ${CMAKE_SOURCE_DIR}/src/PhysicsWorld/Fragment.cxx
  ${CMAKE_SOURCE_DIR}/src/PhysicsWorld/PhysicsWorld.cxx

  ${CMAKE_SOURCE_DIR}/src/shader/CompilationException.cxx
  ${CMAKE_SOURCE_DIR}/src/shader/LinkingException.cxx
  ${CMAKE_SOURCE_DIR}/src/shader/Shader.cxx
  ${CMAKE_SOURCE_DIR}/src/shader/ShaderProgram.cxx
  ${CMAKE_SOURCE_DIR}/src/shader/shaderPrograms.cxx

  ${CMAKE_SOURCE_DIR}/src/ShadowMapping/ShadowMapping.cxx

  ${CMAKE_SOURCE_DIR}/src/SmokeGenerator/SmokeGenerator.cxx

  ${CMAKE_SOURCE_DIR}/src/utils/utils.cxx
  ${CMAKE_SOURCE_DIR}/src/utils/math.cxx

  ${CMAKE_SOURCE_DIR}/src/get_share_path.cxx
)

# Define option for building python UI or not
OPTION(TOONCHESS_BUILD_PYTHON_GUI "Python GUI for ToonChess" ON)
set(EXECUTABLE_NAME "toonchess")
if(TOONCHESS_BUILD_PYTHON_GUI)
    set(EXECUTABLE_NAME "toonchess3d")
endif()

# Define sources and executable
add_executable(${EXECUTABLE_NAME} src/ToonChess.cxx ${CXX_FILES})

# Tests
OPTION(TOONCHESS_BUILD_TESTS "ToonChess tests" OFF)
if(TOONCHESS_BUILD_TESTS)
  # Create test executable
  set(TEST_NAME "toonchess_tests")
  add_executable(${TEST_NAME} tests/test_main.cxx ${CXX_FILES})

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
  target_link_libraries(${TEST_NAME} gmock_main)
endif()

# Download and unpack glfw
configure_file(CMakeLists-glfw.txt.in glfw-download/CMakeLists.txt)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/glfw-download
)
if(result)
  message(FATAL_ERROR "CMake step for glfw failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/glfw-download )
if(result)
  message(FATAL_ERROR "Build step for glfw failed: ${result}")
endif()

# Add googletest.
add_subdirectory(
    ${CMAKE_BINARY_DIR}/glfw-src
    ${CMAKE_BINARY_DIR}/glfw-build)
target_link_libraries(${EXECUTABLE_NAME} glfw)
if(TOONCHESS_BUILD_TESTS)
  target_link_libraries(${TEST_NAME} glfw)
endif()

# Detect and add OpenGL
find_package(OpenGL REQUIRED)
if (OPENGL_FOUND)
  include_directories(${OPENGL_INCLUDE_DIR})
  target_link_libraries(${EXECUTABLE_NAME} ${OPENGL_LIBRARIES})
  if(TOONCHESS_BUILD_TESTS)
    target_link_libraries(${TEST_NAME} ${OPENGL_LIBRARIES})
  endif()
endif()

# Detect and add Bullet Physics
find_package(Bullet 2.8 REQUIRED)
if(BULLET_FOUND)
  include_directories(${BULLET_INCLUDE_DIR})
  target_link_libraries(${EXECUTABLE_NAME} ${BULLET_LIBRARIES})
  if(TOONCHESS_BUILD_TESTS)
    target_link_libraries(${TEST_NAME} ${BULLET_LIBRARIES})
  endif()
endif()

# Detect and add libpng
find_package(PNG REQUIRED)
if(PNG_FOUND)
  include_directories(${PNG_INCLUDE_DIRS})
  target_link_libraries(${EXECUTABLE_NAME} ${PNG_LIBRARIES})
  if(TOONCHESS_BUILD_TESTS)
    target_link_libraries(${TEST_NAME} ${PNG_LIBRARIES})
  endif()
endif()

# Install python gui
add_subdirectory("${CMAKE_SOURCE_DIR}/gui")

# Install assets, icons, config file
install(DIRECTORY "${TOONCHESS_SHARE_DIR}/toonchess"
        DESTINATION "share")
install(DIRECTORY "${TOONCHESS_SHARE_DIR}/applications"
        DESTINATION "$ENV{HOME}/.local/share"
        USE_SOURCE_PERMISSIONS)
install(DIRECTORY "${TOONCHESS_SHARE_DIR}/icons"
        DESTINATION "$ENV{HOME}/.local/share"
        USE_SOURCE_PERMISSIONS)
install(DIRECTORY "${TOONCHESS_CONFIG_DIR}/toonchess"
        DESTINATION "$ENV{HOME}/.config"
        PATTERN "toonchess/config.txt"
        PERMISSIONS WORLD_READ WORLD_WRITE)

install(TARGETS ${EXECUTABLE_NAME}
        RUNTIME DESTINATION bin)
