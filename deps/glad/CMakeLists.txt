cmake_minimum_required(VERSION 3.12)

project(glad)

set(GLAD_HEADERS
  include/glad/glad.h
)

set(GLAD_SOURCES
  src/glad.c
)

add_library(${PROJECT_NAME} STATIC
  ${GLAD_HEADERS}
  ${GLAD_SOURCES}
)

target_include_directories(${PROJECT_NAME} 
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
