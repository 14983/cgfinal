#include "imgui_util.h"

void setupImGUI(GLFWwindow* window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");
}

void clearImGUIContext() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void setupImGUIFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void drawImGUIPanel(const char *name, const std::vector<float *> &floats, const std::vector<const char *> &names) {
    ImGui::Begin(name);
    for (size_t i = 0; i < floats.size(); i++) {
        ImGui::SliderFloat(names[i], floats[i], 0.0, 1.0, "%4f");
    }
    ImGui::End();
}

void endImGUIFrame() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}