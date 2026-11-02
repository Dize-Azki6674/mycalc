#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>

#include <gui/calc/manager.hpp>

#include <optional>
#include <string_view>

/* MyCalc ****************************************/
/*  シンプルなGUIの電卓アプリ                    */
/*  プラットフォーム依存の処理とかは作っていない */
/*  （今後追加の可能性もあるかも）               */
/*                                               */
/*   Version: 0.0                                */
/*   Author : Azkey                              */
/*************************************************/

std::string_view
operator_to_string(std::optional<Manager::operator_enum> opr)
{
    if (!opr) return "";

    switch (*opr) {
        using enum Manager::operator_enum;
    case Add: return "+";
    case Sub: return "-";
    case Mul: return "*";
    case Div: return "/";
    }
}

int main()
{
    if (!glfwInit())
        return -1;

    GLFWwindow* window =
        glfwCreateWindow(360, 335, "MyCalc", nullptr, nullptr);

    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    [[maybe_unused]] ImGuiIO& io = ImGui::GetIO();

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    Manager manager{};

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // 電卓ウィンドウの位置とサイズを固定
        ImGui::SetNextWindowPos(
            ImVec2(0.0f, 4.0f),
            ImGuiCond_Always
        );

        ImGui::SetNextWindowSize(
            ImGui::GetIO().DisplaySize,
            ImGuiCond_Always
        );

        ImGui::Begin(
            "MyCalc",
            nullptr,
            ImGuiWindowFlags_NoMove
            | ImGuiWindowFlags_NoResize
            | ImGuiWindowFlags_NoCollapse
            | ImGuiWindowFlags_NoTitleBar
        );

        // 表示欄
        ImGui::SetWindowFontScale(3.0f);

        const auto dispinfo = manager.dispinfo();

        const float width = 300.0f;
        const float start_x = ImGui::GetCursorPosX();
        const float start_y = ImGui::GetCursorPosY();

        const auto opr = operator_to_string(dispinfo.opr);
        const auto& value = dispinfo.value;

        ImGui::SetCursorPos({ start_x, start_y });
        ImGui::TextUnformatted(opr.data(), opr.data() + opr.size());

        const float value_width = ImGui::CalcTextSize(value.c_str()).x;
        ImGui::SetCursorPos({
            start_x + ImGui::GetContentRegionAvail().x - value_width,
            start_y
        });
        ImGui::TextUnformatted(value.c_str());

        ImGui::Separator();

        // 4列の電卓レイアウト
        constexpr ImVec2 button_size{ 80.0f, 50.0f };

        const auto button = [&](const char* label, CalcEvent event) {
            if (ImGui::Button(label, button_size)) {
                manager.on_event(event);
            }
        };

        const auto binary_button = [&](const char* label, Calc::BinOpr op) {
            button(label, Binary{ op });
        };

        // 1行目: クリア・符号・除算
        button("AC", AC{});
        ImGui::SameLine();
        button("C", C{});
        ImGui::SameLine();
        button("+/-", Sign{});
        ImGui::SameLine();
        binary_button("/", Calc::BinOpr::Div);

        // 2行目: 7 8 9 ×
        button("7", Number{ 7 });
        ImGui::SameLine();
        button("8", Number{ 8 });
        ImGui::SameLine();
        button("9", Number{ 9 });
        ImGui::SameLine();
        binary_button("*", Calc::BinOpr::Mul);

        // 3行目: 4 5 6 −
        button("4", Number{ 4 });
        ImGui::SameLine();
        button("5", Number{ 5 });
        ImGui::SameLine();
        button("6", Number{ 6 });
        ImGui::SameLine();
        binary_button("-", Calc::BinOpr::Sub);

        // 4行目: 1 2 3 +
        button("1", Number{ 1 });
        ImGui::SameLine();
        button("2", Number{ 2 });
        ImGui::SameLine();
        button("3", Number{ 3 });
        ImGui::SameLine();
        binary_button("+", Calc::BinOpr::Add);

        // 5行目: 0 . =
        button("0", Number{ 0 });
        ImGui::SameLine();
        button(".", Point{});
        ImGui::SameLine();
        button("=", Equal{});

        ImGui::End();

        ImGui::Render();

        int display_w;
        int display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);

        glViewport(0, 0, display_w, display_h);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
}