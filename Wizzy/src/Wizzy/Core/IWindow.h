#pragma once

#include "Wizzy/Events/Event.h"
#include "Wizzy/Renderer/GraphicsContext.h"

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

        virtual int32 GetPosX() const = 0;
        virtual int32 GetPosY() const = 0;

        virtual void SetWidth(u32 width) = 0;
        virtual void SetHeight(u32 height) = 0;

        virtual void SetEventCallback(const EventCallbackFn& callbackFn) = 0;
        virtual void SetVSync(bool enabled) = 0;
        virtual bool IsVsync() const = 0;

		virtual void* GetNativeWindow() = 0;

        virtual float GetDeltaTime() const = 0;

        virtual GraphicsContext& GetContext() = 0;

        static IWindow *Create(const WindowProps& props = WindowProps());
    };
}
