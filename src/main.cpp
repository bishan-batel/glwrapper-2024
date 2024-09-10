#include <fstream>
#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <cstdlib>
#include <fmt/format.h>
#include <iostream>
#include <preamble.hpp>
#include <sstream>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

constexpr const char *glsl_version = "#version 420";

[[noreturn]] static void fatal(const StringView msg, bool terminate_glfw = true) {
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

namespace gl {
  auto compile_shader(u32 id) -> void {
    glCompileShader(id);

    // glGetShaderiv(COMPILE_ST);
  }
} // namespace gl

int main() {
  if (not glfwInit()) {
    fatal(fmt::format("Failed to initialized GLFW: {}", glfw::error_description()), false);
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window = glfwCreateWindow(800, 800, "", nullptr, nullptr);
  if (window == nullptr) {
    fatal(fmt::format("Failed to open window: {}", glfw::error_description()));
  }

  glfwMakeContextCurrent(window);
  // gladLoadGL();
  gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

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

  f32 brightness = 0.0f;

  u32 program_id{0};
  {
    String vertex_contents = (std::stringstream{} << std::ifstream{"canvas.vert.glsl"}.rdbuf()).str();
    String frag_contents = (std::stringstream{} << std::ifstream{"canvas.frag.glsl"}.rdbuf()).str();

    std::cout << vertex_contents << std::endl;
    std::cout << frag_contents << std::endl;

    u32 vert = glCreateShader(GL_VERTEX_SHADER);
    u32 frag = glCreateShader(GL_FRAGMENT_SHADER);

    const char *contents = frag_contents.c_str();
    glShaderSource(frag, 1, &contents, nullptr);
    glCompileShader(frag);

    contents = vertex_contents.c_str();
    glShaderSource(vert, 1, &contents, nullptr);
    glCompileShader(vert);

    program_id = glCreateProgram();
    glAttachShader(program_id, vert);
    glAttachShader(program_id, frag);
    glLinkProgram(program_id);
    glDeleteShader(vert);
    glDeleteShader(frag);
  }

  static f32 vertices[] = {
      -0.5f,
      -0.5f,
      0

          - 0.5f,
      0.5f,
      0,

      0.5f,
      0.5f,
      0,
  };

  u32 vbo;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  u32 vao = 0;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(f32) * 3, nullptr);

  while (not glfwWindowShouldClose(window)) {
    glfwPollEvents();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::Render();

    {
      int display_w, display_h;
      glfwGetFramebufferSize(window, &display_w, &display_h);
      glViewport(0, 0, display_w, display_h);
    }

    glClearColor(30 / 255.0f, 30 / 255.0f, 46 / 255.0f + brightness, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(program_id);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);

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
