#include <glad/glad.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <cstdlib>
#include <fmt/format.h>
#include <iostream>
#include <preamble.hpp>

const char *glsl_version = "#version 420";

[[noreturn]] static void fatal(StringView msg, bool terminate_glfw = true) {
  std::cerr << msg << std::endl;
  glfwTerminate();
  std::exit(1);
}

namespace glfw {
static StringView error_description() {
  const char *description = nullptr;
  glfwGetError(&description);
  return description;
}
}; // namespace glfw

int main() {
  if (not glfwInit()) {
    fatal(fmt::format("Failed to initialized GLFW: {}",
                      glfw::error_description()),
          false);
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window = glfwCreateWindow(800, 800, "", nullptr, nullptr);
  if (window == nullptr) {
    fatal(fmt::format("Failed to open window: {}", glfw::error_description()));
  }

  glfwMakeContextCurrent(window);
  gladLoadGL();

  glfwSwapInterval(1);

  IMGUI_CHECKVERSION();


  ImGui::CreateContext();

  [[maybe_unused]] ImGuiIO &io = ImGui::GetIO();

  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

  ImGui::StyleColorsDark();

  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);

  bool show_demo_window = true;

  float brightness = 0.0f;

  while (not glfwWindowShouldClose(window)) {
    glfwPollEvents();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    if (show_demo_window)
      ImGui::ShowDemoWindow();


    ImGui::Begin("Show Demo Window");
    ImGui::Checkbox("Show Demo Window", &show_demo_window);


    ImGui::SliderFloat("Blue Tint", &brightness, -1.f, 1.f);
    ImGui::End();

    ImGui::Render();

    {
      int display_w, display_h;
      glfwGetFramebufferSize(window, &display_w, &display_h);
      glViewport(0, 0, display_w, display_h);
    }

    glClearColor(30 / 255.0f, 30 / 255.0f, 46 / 255.0f + brightness, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);
  }

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}