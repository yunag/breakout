#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <cstdint>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "breakout/breakout_game.hpp"
#include "breakout/input.hpp"
#include "breakout/log.hpp"
#include "breakout/macro.hpp"
#include "breakout/resource_manager.hpp"

const uint32_t SCREEN_WIDTH = 800;
const uint32_t SCREEN_HEIGHT = 600;

BreakoutGame Breakout(SCREEN_WIDTH, SCREEN_HEIGHT);

int main(int argc, char *argv[]) {
  YU_UNUSED(argc);
  YU_UNUSED(argv);

  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif /* __APPLE__ */

  glfwWindowHint(GLFW_RESIZABLE, false);
  GLFWwindow *window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Breakout",
                                        nullptr, nullptr);
  if (!window) {
    LOG_CRIT("Failed to create window");
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);

  /* Load glad */
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    LOG_CRIT("Failed to initialize GLAD");
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

  glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  Breakout.init();
  float delta_time = 0.0f;
  float last_frame = 0.0f;
  while (!glfwWindowShouldClose(window)) {
    float current_frame = glfwGetTime();
    delta_time = current_frame - last_frame;
    last_frame = current_frame;
    glfwPollEvents();

    Breakout.process_input(delta_time);
    Breakout.update(delta_time);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    Breakout.render();

    glfwSwapBuffers(window);
  }

  ResourceManager::clear();
  glfwTerminate();
  return 0;
}
