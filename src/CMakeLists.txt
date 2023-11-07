add_executable(${PROJECT_NAME} 
  main.cpp breakoutgame.cpp shader.cpp
  resourcemanager.cpp texture2d.cpp
  memory.cpp input.cpp spriterenderer.cpp
  gameobject.cpp gamelevel.cpp ballobject.cpp
  particle.cpp postprocessor.cpp powerup.cpp
  textrenderer.cpp
)

target_include_directories(${PROJECT_NAME}
  PRIVATE
    ${INCLUDE_PATH}
)

target_compile_options(${PROJECT_NAME}
  PRIVATE
    ${COMPILE_OPTS}
)

target_compile_features(${PROJECT_NAME} 
  PUBLIC 
    cxx_std_11
)

target_link_libraries(${PROJECT_NAME}
  PRIVATE
    glad
    glfw
    glm
    stb_image
    spdlog
    miniaudio
    freetype
)

set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/)  
