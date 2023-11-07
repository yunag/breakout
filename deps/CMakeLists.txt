# Don't build docs, tests, examples and install info
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

add_subdirectory(glad)
add_subdirectory(glfw)
add_subdirectory(glm)
add_subdirectory(stb)
add_subdirectory(spdlog)
add_subdirectory(miniaudio)

set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
set(FT_DISABLE_HARFBUZZ TRUE)
set(FT_REQUIRE_HARFBUZZ FALSE)
add_subdirectory(freetype)
