#pragma once

#include "Wizzy/Core/IWindow.h"
#include "Wizzy/Renderer/GraphicsContext.h"

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

		inline virtual int32 GetPosX() const override { return m_data.posx; }
		inline virtual int32 GetPosY() const override { return m_data.posy; }

		inline virtual void SetEventCallback(const EventCallbackFn& callbackFn) override { m_data.eventCallbackFn = callbackFn; }
		virtual void SetVSync(bool enabled) override;
		virtual bool IsVsync() const override;

		inline virtual void* GetNativeWindow() { return m_glfwWindow; }

		virtual GraphicsContext& GetContext() override;

		inline virtual float GetDeltaTime() const override {
			return m_data.deltaTime;
		}

	private:
		virtual void Init(const WindowProps& props);
		virtual void Shutdown();

	private:
		struct WindowData {
			string title;
			EventCallbackFn eventCallbackFn;
			u32 width;
			u32 height;
			int32 posx, posy;
			bool vsync;
			float deltaTime = 1.0f / 60.0f;
		} m_data;
		GLFWwindow *m_glfwWindow;
		double m_lastTime = 0;
		GraphicsContext *m_context;

	private:
		static bool s_glfwInitialized;
	};

}
