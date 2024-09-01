#include <fstream>
#include <inttypes.h>

#include <GLFW/glfw3.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <vector>

#include "PE/PE.h"
#include <iostream>
#include <iomanip> 

const char* GetArchitectureName(MachineArc machine) 
{
    return "";
}


int main()
{
    if (!glfwInit())
    {
        std::printf("Failed to initialize GLFW!");

        return -1;
    }

    std::fstream file("/home/user/Downloads/DiscordSetup.exe", std::ios::in | std::ios::binary);

    if(!file.is_open())
    {
        std::printf("Failed to open file\n");

        return -1;
    }

    PE pe(file);

    IMAGE_DOS_HEADER* dos = pe.get_dos();
    if(dos == nullptr)
    {
        std::printf("Executeable isn't in DOS format!\n");

        return -1;
    }

    IMAGE_NT_HEADERS* nt = pe.get_nt();
    if(nt == nullptr)
    {
        std::printf("Executeable isn't in PE format!\n");

        return -1;
    }

    std::vector<std::string> rdata_strings;
    std::vector<IMAGE_SECTION_HEADER> sections = pe.get_sections();

   

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
#if defined(__APPLE__)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#else
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#endif

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();

    if(monitor == nullptr)
    {
        std::printf("Failed to get primary monitor!");

        return -1;
    }

    int xpos = 0;
    int ypos = 0;
    int width = 0;
    int height = 0;

    glfwGetMonitorWorkarea(monitor, &xpos, &ypos, &width, &height);

    GLFWwindow* window = glfwCreateWindow(width, height, "BinaryView", nullptr, nullptr);

    if (window == nullptr)
    {
        std::printf("Failed to create window!");

        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGuiIO& io = ImGui::GetIO();

    ImGui_ImplGlfw_InitForOpenGL(window, true);

    ImGui_ImplOpenGL3_Init(nullptr);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        int w, h;
        glfwGetFramebufferSize(window, &w, &h);

        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(io.DisplaySize);

        ImGui::Begin("BinaryView", nullptr, ImGuiWindowFlags_NoTitleBar);
        {
            ImGui::BeginChild("Sidebar", ImVec2(200, 0), true, ImGuiWindowFlags_HorizontalScrollbar);
            {
                pe.render_sidebar();

                ImGui::EndChild();
            }
            
            ImGui::SameLine();

            ImGui::BeginChild("Content", ImVec2(0, 0), true);
            {
                pe.render_main();
                
                ImGui::EndChild();
            }
        }
        ImGui::End();

        ImGui::Render();
        
        glViewport(0, 0, w, h);
        glClear(GL_COLOR_BUFFER_BIT);

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