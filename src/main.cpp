#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <cstdint>
#include <unordered_map>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "breakout/breakout_game.hpp"
#include "breakout/input.hpp"
#include "breakout/log.hpp"
#include "breakout/macro.hpp"
#include "breakout/resource_manager.hpp"

const uint32_t SCREEN_WIDTH = 800;
const uint32_t SCREEN_HEIGHT = 600;

const char *gl_source_to_string(const GLenum source) {
  switch (source) {
  case GL_DEBUG_SOURCE_API:
    return "DEBUG_SOURCE_API";
  case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
    return "DEBUG_SOURCE_WINDOW_SYSTEM";
  case GL_DEBUG_SOURCE_SHADER_COMPILER:
    return "GL_DEBUG_SOURCE_SHADER_COMPILER";
  case GL_DEBUG_SOURCE_THIRD_PARTY:
    return "DEBUG_SOURCE_THIRD_PARTY";
  case GL_DEBUG_SOURCE_APPLICATION:
    return "DEBUG_SOURCE_APPLICATION";
  case GL_DEBUG_SOURCE_OTHER:
    return "DEBUG_SOURCE_OTHER";

  default:
    return "UNKNOWN_DEBUG_SOURCE";
  }
}

const char *gl_type_to_string(const GLenum type) {
  switch (type) {
  case GL_DEBUG_TYPE_ERROR:
    return "DEBUG_TYPE_ERROR";
  case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
    return "DEBUG_TYPE_DEPRECATED_BEHAVIOR";
  case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
    return "DEBUG_TYPE_UNDEFINED_BEHAVIOR";
  case GL_DEBUG_TYPE_PORTABILITY:
    return "DEBUG_TYPE_PORTABILITY";
  case GL_DEBUG_TYPE_PERFORMANCE:
    return "DEBUG_TYPE_PERFORMANCE";
  case GL_DEBUG_TYPE_MARKER:
    return "DEBUG_TYPE_MARKER";
  case GL_DEBUG_TYPE_PUSH_GROUP:
    return "DEBUG_TYPE_PUSH_GROUP";
  case GL_DEBUG_TYPE_POP_GROUP:
    return "DEBUG_TYPE_POP_GROUP";
  case GL_DEBUG_TYPE_OTHER:
    return "DEBUG_TYPE_OTHER";

  default:
    return "UNKNOWN_DEBUG_TYPE";
  }
}

void gl_debug_handle(GLenum source, GLenum type, GLuint id, GLenum severity,
                     GLsizei length, const GLchar *message,
                     const void *userParam) {
  YU_UNUSED(length);
  YU_UNUSED(userParam);

  switch (severity) {
  case GL_DEBUG_SEVERITY_HIGH:
    LOG_ERROR("OpenGL Error: [{0}:{1}]({2}): {3}", gl_source_to_string(source),
              gl_type_to_string(type), id, message);
    break;
  case GL_DEBUG_SEVERITY_MEDIUM:
    LOG_WARN("OpenGL Warning: [{0}:{1}]({2}): {3}", gl_source_to_string(source),
             gl_type_to_string(type), id, message);
    break;
  case GL_DEBUG_SEVERITY_LOW:
    LOG_INFO("OpenGL Info: [{0}:{1}]({2}): {3}", gl_source_to_string(source),
             gl_type_to_string(type), id, message);
    break;
  case GL_DEBUG_SEVERITY_NOTIFICATION:
    LOG_INFO("OpenGL Notificaton: [{0}:{1}]({2}): {3}",
             gl_source_to_string(source), gl_type_to_string(type), id, message);
    break;
  default:
    LOG_ERROR("OpenGL Error: [{0}:{1}] ({2}) : {3}",
              gl_source_to_string(source), gl_type_to_string(type), id,
              message);
  }
}

int main(int argc, char *argv[]) {
  YU_UNUSED(argc);
  YU_UNUSED(argv);

  BreakoutGame Breakout(SCREEN_WIDTH, SCREEN_HEIGHT);

  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifndef NDEBUG
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif /* NDEBUG */

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif /* __APPLE__ */

  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
  GLFWwindow *window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Breakout",
                                        nullptr, nullptr);
  if (!window) {
    LOG_CRITICAL("Failed to create window");
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);

  /* Load glad */
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    LOG_CRITICAL("Failed to initialize GLAD");
    return -1;
  }

  glfwSetKeyCallback(window,
                     [](GLFWwindow *window, int key, int scancode, int action,
                        int mods) -> void {
                       YU_UNUSED(window);
                       YU_UNUSED(key);
                       YU_UNUSED(scancode);
                       YU_UNUSED(mods);
                       switch (action) {
                       case GLFW_PRESS:
                         Input::press_key(static_cast<KeyCode>(key));
                         break;
                       case GLFW_RELEASE:
                         Input::release_key(static_cast<KeyCode>(key));
                         break;
                       }
                     });

  glfwSetFramebufferSizeCallback(
      window, [](GLFWwindow *window, int width, int height) -> void {
        YU_UNUSED(window);
        glViewport(0, 0, width, height);
      });

#ifndef NDEBUG

  /* Check if we properly initialized OpenGL's debug context */
  int debug_context_flags;
  glGetIntegerv(GL_CONTEXT_FLAGS, &debug_context_flags);
  if (debug_context_flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
    LOG_INFO("Debug context is enabled");
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(gl_debug_handle, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE,
                          GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
  } else {
    LOG_WARN("Debug context is not supported");
  }

#endif /* NDEBUG */

  glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  Breakout.init();
  float delta_time = 0.0f;
  float next_frame = 0.0f;
  while (!glfwWindowShouldClose(window)) {
    float current_frame = glfwGetTime();

    Breakout.process_input(delta_time);
    Breakout.update(delta_time);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    Breakout.render();

    next_frame = glfwGetTime();
    delta_time = next_frame - current_frame;

    glfwPollEvents();
    glfwSwapBuffers(window);
  }

  ResourceManager::clear();
  glfwTerminate();
  return 0;
}
