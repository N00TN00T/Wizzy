#pragma once

#include "Core.h"
#include "Events/Event.h"

namespace Wizzy {
    struct WindowProps {
        string title;
        u32 width;
        u32 height;

        WindowProps(const string& title = "Wizzy",
                    u32 width = 1600,
                    u32 height = 900)
            : title(title), width(width), height(height) {}
    };

    class  IWindow {
    public:
        typedef std::function<void(Event&)> EventCallbackFn;

		virtual ~IWindow() {}

        virtual void OnFrameBegin() = 0;
        virtual void OnFrameEnd() = 0;

        virtual u32 GetWidth() const = 0;
        virtual u32 GetHeight() const = 0;

        virtual void SetWidth(u32 width) = 0;
        virtual void SetHeight(u32 height) = 0;

        /* virtual vec2 GetViewportSize() const = 0;
        virtual void SetViewportSize(vec2 size) = 0;

        virtual vec2 GetViewportPosition() const = 0;
        virtual void SetViewportPosition(vec2 pos) = 0; */

        virtual void SetEventCallback(const EventCallbackFn& callbackFn) = 0;
        virtual void SetVSync(bool enabled) = 0;
        virtual bool IsVsync() const = 0;

        virtual void SetClearColor(float r, float g, float b, float a) = 0;

		virtual void* GetNativeWindow() = 0;

        static IWindow *Create(const WindowProps& props = WindowProps());
    };
}
