#pragma once

namespace Wizzy {
    enum class RendererAPI {
        NONE = 0, OPENGL = 1
    };

    class Renderer {
    public:
        inline static void SetAPI(RendererAPI api) { s_API = api; }
        inline static RendererAPI GetAPI() { return s_API; }

    private:
        static RendererAPI s_API;
    };
}
