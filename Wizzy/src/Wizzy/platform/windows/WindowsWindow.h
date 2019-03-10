#pragma once

#include "Wizzy/IWindow.h"

struct GLFWwindow;

namespace Wizzy {

    class WindowsWindow
        : public IWindow {
    public:
        WindowsWindow(const WindowProps& props);
        virtual ~WindowsWindow();

        virtual void OnFrameBegin() override;
        virtual void OnFrameEnd() override;

        inline virtual u32 GetWidth() const override { return m_data.width; }
        inline virtual u32 GetHeight() const override { return m_data.height; }

        virtual void SetWidth(u32 width) override;
        virtual void SetHeight(u32 height) override;

        inline virtual void SetEventCallback(const EventCallbackFn& callbackFn) override { m_data.eventCallbackFn = callbackFn; }
        virtual void SetVSync(bool enabled) override;
        virtual bool IsVsync() const override;

		virtual void SetClearColor(float r, float g, float b, float a) override;
    private:
        virtual void Init(const WindowProps& props);
        virtual void Shutdown();
    private:        
        struct WindowData {
            string title;
            EventCallbackFn eventCallbackFn;
            u32 width;
            u32 height;
            bool vsync;
        } m_data;
        GLFWwindow *m_glfwWindow;
        static bool s_glfwInitialized;
    };

}