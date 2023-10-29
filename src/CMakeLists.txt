add_executable(${PROJECT_NAME} 
  main.cpp breakoutgame.cpp shader.cpp
  resourcemanager.cpp texture2d.cpp
  memory.cpp input.cpp spriterenderer.cpp
)

target_include_directories(${PROJECT_NAME}
  PUBLIC
    ${INCLUDE_PATH}
)

target_compile_options(${PROJECT_NAME}
  PRIVATE
    ${COMPILE_OPTS}
)

target_link_libraries(${PROJECT_NAME}
  PRIVATE
    glad
    glfw
    glm
    stb_image
    spdlog
)


set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/)  
