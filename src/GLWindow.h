//
// Created by robko on 10/1/24.
//

#ifndef ZPG_GLWINDOW_H
#define ZPG_GLWINDOW_H

#include <optional>
#include "assertions.h"
#include "gl_info.h"
#include <iostream>
#include <memory>
#include <list>
#include <functional>
#include "ResizeObserver.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

class GLWindow {
private:
    GLFWwindow* window;
    // Normal printable keys defined in glfw3.h from GLFW_KEY_SPACE up to GLFW_KEY_GRAVE_ACCENT
    uint64_t printableKeyStatus;

    // the difference between first and last function keys as defined by GLFW is 96, so we
    // cannot fit it in one 64-bit number, and we have to slice it up.
    // keys GLFW_KEY_ESCAPE up to GLFW_KEY_KP_0
    uint64_t functionKeys0;
    // keys GLFW_KEY_KP_1 up to GLFW_KEY_MENU (that is 28 used bits, maybe we can fit modifier keys
    // here as well? tbd)
    uint64_t functionKeys1;

    double lastTime;
    double delta;

    int currentWidth;
    int currentHeight;

    double currentMouseX;
    double currentMouseY;

    std::list<std::shared_ptr<ResizeObserver>> resizeObservers;

    //region Key management
    inline void setKey(int key) {
        if (key <= GLFW_KEY_GRAVE_ACCENT && key >= GLFW_KEY_SPACE) {
            printableKeyStatus |= 1 << (GLFW_KEY_GRAVE_ACCENT - key);
        } else if (key <= GLFW_KEY_KP_0 && key >= GLFW_KEY_ESCAPE) {
            functionKeys0 |= 1 << (GLFW_KEY_KP_0 - key);
        } else if (key <= GLFW_KEY_MENU && key >= GLFW_KEY_KP_1) {
            functionKeys1 |= 1 << (GLFW_KEY_MENU - key);
        }
    }

    inline void clearKey(int key) {
        if (key <= GLFW_KEY_GRAVE_ACCENT && key >= GLFW_KEY_SPACE) {
            printableKeyStatus &= ~(1 << (GLFW_KEY_GRAVE_ACCENT - key));
        } else if (key <= GLFW_KEY_KP_0 && key >= GLFW_KEY_ESCAPE) {
            functionKeys0 &= ~(1 << (GLFW_KEY_KP_0 - key));
        } else if (key <= GLFW_KEY_MENU && key >= GLFW_KEY_KP_1) {
            functionKeys1 &= ~(1 << (GLFW_KEY_MENU - key));
        }
    }

    [[nodiscard]] inline bool getKey(int key) const {
        if (key <= GLFW_KEY_GRAVE_ACCENT && key >= GLFW_KEY_SPACE) {
            return (printableKeyStatus & (1 << (GLFW_KEY_GRAVE_ACCENT - key))) != 0;
        } else if (key <= GLFW_KEY_KP_0 && key >= GLFW_KEY_ESCAPE) {
            return (functionKeys0 & (1 << (GLFW_KEY_KP_0 - key))) != 0;
        } else if (key <= GLFW_KEY_MENU && key >= GLFW_KEY_KP_1) {
            return (functionKeys1 & 1 << (GLFW_KEY_MENU - key)) != 0;
        }
        return false;
    }
    //endregion

    explicit GLWindow(GLFWwindow* window, double lastTime, int currentWidth, int currentHeight)
            : window(window),
              printableKeyStatus(0),
              functionKeys0(0),
              functionKeys1(0),
              lastTime(lastTime),
              delta(0),
              currentWidth(currentWidth),
              currentHeight(currentHeight),
              currentMouseX(0), currentMouseY(0), resizeObservers() {
    }

    //region Callbacks
    void onKeyPress(GLFWwindow* target, int key, int scancode, int action, int mods) {
        if (GLFW_PRESS == action) {
            setKey(key);
        } else if (GLFW_RELEASE == action) {
            clearKey(key);
        }
//        printf("key_callback [%d,%d,%d,%d] \n", key, scancode, action, mods);
    }

    void onFocusChange(GLFWwindow* target, int focused) {
//        printf("window_focus_callback \n");
    }

    void onResize(GLFWwindow* target, int newWidth, int newHeight) {
        currentWidth = newWidth;
        currentHeight = newHeight;
        for (const auto &item: this->resizeObservers) {
            item->onWindowResize(newWidth, newHeight);
        }
        glViewport(0, 0, currentWidth, currentHeight);
    }

    void onMouseMove(GLFWwindow* target, double x, double y) {
        currentMouseX = x;
        currentMouseY = y;
    }

    void onMousePress(GLFWwindow* target, int button, int action, int mode) {
//        if (action == GLFW_PRESS) printf("button_callback [%d,%d,%d]\n", button, action, mode);
    }
    //endregion


    //region Callback registration
#pragma clang diagnostic push
#pragma ide diagnostic ignored "NullDereference"

    void registerCallbacks() {
        DEBUG_ASSERT(nullptr == glfwGetWindowUserPointer(window));
        glfwSetWindowUserPointer(window, this);

        {
            auto prev =
                    glfwSetKeyCallback(window,
                                       [](GLFWwindow* target, int key, int scancode, int action,
                                          int mods) -> void {
                                           auto* self = static_cast<GLWindow*>(glfwGetWindowUserPointer(
                                                   target));
                                           DEBUG_ASSERT_NOT_NULL(self);
                                           self->onKeyPress(target, key, scancode, action, mods);
                                       });
            DEBUG_ASSERT(nullptr == prev);
        }

        {
            auto prev = glfwSetWindowFocusCallback(window,
                                                   [](GLFWwindow* target, int focused) -> void {
                                                       auto* self = static_cast<GLWindow*>(glfwGetWindowUserPointer(
                                                               target));
                                                       DEBUG_ASSERT_NOT_NULL(self);
                                                       self->onFocusChange(target, focused);
                                                   });
            DEBUG_ASSERT(nullptr == prev);
        }

        {
            auto prev = glfwSetWindowSizeCallback(window, [](GLFWwindow* target, int newWidth,
                                                             int newHeight) -> void {
                auto* self = static_cast<GLWindow*>(glfwGetWindowUserPointer(target));
                DEBUG_ASSERT_NOT_NULL(self);
                self->onResize(target, newWidth, newHeight);
            });
            DEBUG_ASSERT(nullptr == prev);
        }

        {
            auto prev = glfwSetCursorPosCallback(window, [](GLFWwindow* target, double x,
                                                            double y) -> void {
                auto* self = static_cast<GLWindow*>(glfwGetWindowUserPointer(target));
                DEBUG_ASSERT_NOT_NULL(self);
                self->onMouseMove(target, x, y);
            });
            DEBUG_ASSERT(nullptr == prev)
        }

        {
            auto prev = glfwSetMouseButtonCallback(window,
                                                   [](GLFWwindow* target, int button, int action,
                                                      int mode) -> void {
                                                       auto* self = static_cast<GLWindow*>(glfwGetWindowUserPointer(
                                                               target));
                                                       DEBUG_ASSERT_NOT_NULL(self);
                                                       self->onMousePress(target, button, action,
                                                                          mode);
                                                   });
            DEBUG_ASSERT(nullptr == prev);
        }
    }

#pragma clang diagnostic pop
    //endregion

public:

    //region Copy/move constructors
    GLWindow(GLWindow &other) : window(other.window), printableKeyStatus(other.printableKeyStatus),
                                functionKeys0(other.functionKeys0),
                                functionKeys1(other.functionKeys1), lastTime(other.lastTime),
                                delta(other.delta),
                                currentWidth(other.currentWidth),
                                currentHeight(other.currentHeight),
                                currentMouseX(other.currentMouseX),
                                currentMouseY(other.currentMouseY),
                                resizeObservers(other.resizeObservers) {
        other.window = nullptr;
        other.printableKeyStatus = 0;
        other.lastTime = 0;
        other.functionKeys0 = 0;
        other.functionKeys1 = 0;
        other.delta = 0;
        other.resizeObservers = std::list<std::shared_ptr<ResizeObserver>>();
        other.window = nullptr;
        other.currentHeight = 0;
    }

    GLWindow(GLWindow &&other) noexcept: window(other.window),
                                         printableKeyStatus(other.printableKeyStatus),
                                         functionKeys0(other.functionKeys0),
                                         functionKeys1(other.functionKeys1),
                                         lastTime(other.lastTime),
                                         delta(other.delta), currentWidth(other.currentWidth),
                                         currentHeight(other.currentHeight),
                                         currentMouseX(other.currentMouseX),
                                         currentMouseY(other.currentMouseY),
                                         resizeObservers(std::move(other.resizeObservers)) {
        other.window = nullptr;
        other.printableKeyStatus = 0;
        other.lastTime = 0;
        other.functionKeys0 = 0;
        other.functionKeys1 = 0;
        other.delta = 0;
        other.resizeObservers = std::list<std::shared_ptr<ResizeObserver>>();
        other.window = nullptr;
        other.currentHeight = 0;
    }
    //endregion

    [[nodiscard]] inline bool isPressed(int key) const noexcept {
        return getKey(key);
    }

    bool isPressedAndClear(int key) {
        bool val = getKey(key);
        clearKey(key);
        return val;
    }

    [[nodiscard]] double getDelta() const {
        return delta;
    }

    static std::optional<std::shared_ptr<GLWindow>> create(const char* title) {
        int startWidth = 800;
        int startHeight = 600;
        GLFWwindow* window = glfwCreateWindow(startWidth, startHeight, title, nullptr, nullptr);
        if (nullptr == window) {
            std::cerr << "ERROR: could not create GLFW window. are you in glfw context?"
                      << std::endl;
            return {};
        }
        glfwMakeContextCurrent(window);
        glfwSwapInterval(1);

        glewExperimental = GL_TRUE;
        glewInit();

        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        (void) io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        io.ConfigDebugHighlightIdConflicts = true;


        double now = glfwGetTime();

        DEBUG_ASSERT_NOT_NULL(window);
        auto instance = std::make_shared<GLWindow>(GLWindow(window, now, startWidth, startHeight));
        instance->registerCallbacks();

        instance->onResize(window, startWidth, startHeight);

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();

        // imgui changes glfw callbacks and chains it to ours,
        // so we have to init it after registering callbacks
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 330");

        glfwMakeContextCurrent(nullptr);

        return std::move(instance);
    }

    void inContext(const std::function<void()> &func) {
        DEBUG_ASSERT(nullptr != window)
        if (nullptr != glfwGetCurrentContext()) {
            throw std::logic_error("Double enter to glfw window context");
        }
        glfwMakeContextCurrent(window);

        glfwSwapInterval(1);
        glEnable(GL_DEPTH_TEST);

        func();
        glfwMakeContextCurrent(nullptr);
    }

    /*
     * Check whether this window is current glfw context
     */
    [[nodiscard]] inline bool isActive() const {
        auto current = glfwGetCurrentContext();
        return current == this->window;
    }

    void captureCursor() {
        if (window) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
    }

    void releaseCursor() {
        if (window) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }

    void close() {
        if (window) {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }
    }

    void registerResizeCallback(const std::shared_ptr<ResizeObserver> &observer) {
        DEBUG_ASSERTF(nullptr != window, "Attempting to register callback on non-existent window")
        resizeObservers.emplace_back(observer);
    }

    void endFrame() noexcept {
        if (window) {
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            glfwSwapBuffers(window);
#ifdef DEBUG_ASSERTIONS
            int err = glfwGetError(nullptr);
            DEBUG_ASSERT(err == GLFW_NO_ERROR);
#endif

            double now = glfwGetTime();
            delta = now - lastTime;
            lastTime = now;
            glfwPollEvents();
        }
    }

    void startFrame() noexcept {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    [[nodiscard]] inline bool shouldClose() const noexcept {
        if (window) {
            return glfwWindowShouldClose(window);
        } else {
            return true;
        }
    }

    [[nodiscard]] inline int width() const noexcept {
        return currentWidth;
    }

    [[nodiscard]] inline int height() const noexcept {
        return currentHeight;
    }

    [[nodiscard]] inline double mouseX() const noexcept {
        return currentMouseX;
    }

    [[nodiscard]] inline double mouseY() const noexcept {
        return currentMouseY;
    }

    ~GLWindow() {
        if (window) {
            ImGui_ImplOpenGL3_Shutdown();
            ImGui_ImplGlfw_Shutdown();
            ImGui::DestroyContext();

            glfwDestroyWindow(window);
        }
    }
};

#endif //ZPG_GLWINDOW_H
