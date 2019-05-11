#pragma once

namespace Wizzy {

	/* Very temporary, for testing only */

    class Texture2D;
    class Shader;

    class Renderer {
    public:
		Renderer();
		void Draw(Texture2D *tex, Shader *shader, const mat4& model);
    private:

    };

}
