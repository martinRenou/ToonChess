cmake_minimum_required (VERSION 2.6)

# Enable debug symbols by default
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
endif()

project (3DToonChess)

# The version number
set (3DToonChess_VERSION_MAJOR 0)
set (3DToonChess_VERSION_MINOR 1)

# Define sources and executable
set(EXECUTABLE_NAME "3DToonChess")
add_executable(${EXECUTABLE_NAME} src/3DToonChess.cxx)

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

# Install target
install(TARGETS ${EXECUTABLE_NAME} DESTINATION bin)

# CPack packaging
include(InstallRequiredSystemLibraries)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/COPYING")
set(CPACK_PACKAGE_VERSION_MAJOR "${3DToonChess_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${3DToonChess_VERSION_MINOR}")
include(CPack)
