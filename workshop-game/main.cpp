#include <chrono>
#include <iostream>
#include <memory>
#include <stdio.h>
#include <thread>

#include "imgui/imgui.h"
#include "imgui_impl_glfw_game.h"
#include "imgui_impl_opengl3_game.h"

#include "box2d/b2_contact.h"
#include "box2d/box2d.h"

#include "draw_game.h"
#include "game.hpp"

// This is so bad
static Game* g_game = nullptr;
static bool g_running = true;

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    // code for keys here https://www.glfw.org/docs/3.3/group__keys.html
    // and modifiers https://www.glfw.org/docs/3.3/group__mods.html

    if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
        g_running = false;
        std::cerr << "Bye bye!" << std::endl;
        return;
    }

    g_game->keyCallback(key, scancode, action, mods);
}

class Application {
public:
    Application() = default;
    ~Application() {
        glfwTerminate();
        g_debugDraw.Destroy();
        g_game = nullptr;
    }

    bool init() {
        /**
         * GLFW
         */
        if (glfwInit() == 0) {
            fprintf(stderr, "Failed to initialize GLFW\n");
            return false;
        }

        m_mainWindow = glfwCreateWindow(g_camera.m_width, g_camera.m_height, "My game", NULL, NULL);

        if (m_mainWindow == NULL) {
            fprintf(stderr, "Failed to open GLFW g_mainWindow.\n");
            glfwTerminate();
            return false;
        }

        // Set callbacks using GLFW
        glfwSetKeyCallback(m_mainWindow, keyCallback);
        glfwMakeContextCurrent(m_mainWindow);

        // Load OpenGL functions using glad
        int version = gladLoadGL(glfwGetProcAddress);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        /**
         * Window/UI
         *
         * Create debug draw. We will be using the debugDraw visualization to create
         * our games. Debug draw calls all the OpenGL functions for us.
         */
        g_debugDraw.Create();
        CreateUI(m_mainWindow, 20.0f /* font size in pixels */);

        /**
         * Game init
         */
        b2Vec2 gravity { 0.f, 0.f };
        m_world = std::make_shared<b2World>(gravity);
        m_world->SetDebugDraw(&g_debugDraw);

        m_game.init(m_world);
        g_game = &m_game;

        return true;
    }

    void run() {
        // Control the frame rate. One draw per monitor refresh.
        std::chrono::duration<double> frameTime(0.0);
        std::chrono::duration<double> sleepAdjust(0.0);

        // Main application loop
        while (!glfwWindowShouldClose(m_mainWindow)) {
            if (!g_running) {
                break;
            }

            // Use std::chrono to control frame rate. Objective here is to maintain
            // a steady 60 frames per second (no more, hopefully no less)
            std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();

            glfwGetWindowSize(m_mainWindow, &g_camera.m_width, &g_camera.m_height);

            int bufferWidth, bufferHeight;
            glfwGetFramebufferSize(m_mainWindow, &bufferWidth, &bufferHeight);
            glViewport(0, 0, bufferWidth, bufferHeight);

            // Clear previous frame (avoid creates shadows)
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            // Setup ImGui attributes so we can draw text on the screen. Basically
            // create a window of the size of our viewport.
            ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
            ImGui::SetNextWindowSize(ImVec2(float(g_camera.m_width), float(g_camera.m_height)));
            ImGui::SetNextWindowBgAlpha(0.0f);
            ImGui::Begin("Overlay", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar);
            m_game.imGuiUpdate();
            ImGui::End();

            // Enable objects to be draw
            uint32 flags = 0;
            flags += b2Draw::e_shapeBit;
            g_debugDraw.SetFlags(flags);

            // When we call Step(), we run the simulation for one frame
            float timeStep = 60 > 0.0f ? 1.0f / 60 : float(0.0f);
            m_world->Step(timeStep, 8, 3);

            m_game.update();

            // Render everything on the screen
            m_world->DebugDraw();
            g_debugDraw.Flush();
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            glfwSwapBuffers(m_mainWindow);

            // Process events (mouse and keyboard) and call the functions we
            // registered before.
            glfwPollEvents();

            // Throttle to cap at 60 FPS. Which means if it's going to be past
            // 60FPS, sleeps a while instead of doing more frames.
            std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();
            std::chrono::duration<double> target(1.0 / 60.0);
            std::chrono::duration<double> timeUsed = t2 - t1;
            std::chrono::duration<double> sleepTime = target - timeUsed + sleepAdjust;
            if (sleepTime > std::chrono::duration<double>(0)) {
                // Make the framerate not go over by sleeping a little.
                std::this_thread::sleep_for(sleepTime);
            }
            std::chrono::steady_clock::time_point t3 = std::chrono::steady_clock::now();
            frameTime = t3 - t1;

            // Compute the sleep adjustment using a low pass filter
            sleepAdjust = 0.9 * sleepAdjust + 0.1 * (target - frameTime);
        }
    }

private:
    GLFWwindow* m_mainWindow = nullptr; // Owned by GLFW
    std::shared_ptr<b2World> m_world;
    Game m_game;
};

int main() {
    Application app;
    if (!app.init()) {
        return -1;
    }

    app.run();

    return 0;
}
