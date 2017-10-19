cmake_minimum_required (VERSION 2.8.2)
set(CMAKE_CXX_STANDARD 11)

# Enable debug symbols by default
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
endif()

project (3DToonChess)

# The version number
set (3DToonChess_VERSION_MAJOR 0)
set (3DToonChess_VERSION_MINOR 1)

# Add headers
set (
  CXX_FILES
  ${CMAKE_SOURCE_DIR}/src/mesh/Mesh.cxx
  ${CMAKE_SOURCE_DIR}/src/mesh/utils.cxx
)

# Define sources and executable
set(EXECUTABLE_NAME "3DToonChess")
add_executable(${EXECUTABLE_NAME} src/3DToonChess.cxx ${CXX_FILES})

# Detect and add SFML
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})

find_package(SFML 2 REQUIRED graphics window system)
if(SFML_FOUND)
  include_directories(${SFML_INCLUDE_DIR})
  target_link_libraries(${EXECUTABLE_NAME} ${SFML_LIBRARIES} ${SFML_DEPENDENCIES})
endif()

# OpenGL
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})
if (OPENGL_FOUND)
  target_link_libraries(${EXECUTABLE_NAME} ${OPENGL_LIBRARIES})
  # target_link_libraries(${EXECUTABLE_NAME} m)
endif()

# Glut
find_package(GLUT REQUIRED)
include_directories(${GLUT_INCLUDE_DIR})
if (GLUT_FOUND)
  target_link_libraries(${EXECUTABLE_NAME} ${GLUT_LIBRARIES} )
endif()

# Install target
install(TARGETS ${EXECUTABLE_NAME} DESTINATION bin)


# Download and unpack googletest at configure time
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(
  ${CMAKE_BINARY_DIR}/googletest-src
  ${CMAKE_BINARY_DIR}/googletest-build)

# Now simply link against gtest or gtest_main as needed. Eg
add_executable(tests tests/test_main.cxx ${CXX_FILES})
target_link_libraries(tests gtest_main)
