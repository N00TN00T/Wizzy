#pragma once

/* FOR USE BY WIZZY APPLICATIONS */

#include <imgui.h>


#include "Wizzy/Renderer/Buffers.h"
#include "Wizzy/Renderer/RenderTarget.h"
#include "Wizzy/Renderer/Renderer.h"
#include "Wizzy/Renderer/Renderer2D.h"
#include "Wizzy/Renderer/Shader.h"
#include "Wizzy/Renderer/Texture.h"
#include "Wizzy/Renderer/Model.h"
#include "Wizzy/Renderer/Font.h"
#include "Wizzy/Renderer/Material.h"
#include "Wizzy/Renderer/API.h"

#include "Wizzy/Utilities/Bitset.h"
#include "Wizzy/Utilities/Stopwatch.h"
#include "Wizzy/Utilities/Instrumentor.h"

#include "Wizzy/Events/AppEvent.h"
#include "Wizzy/Events/CharEvent.h"
#include "Wizzy/Events/KeyEvent.h"
#include "Wizzy/Events/MouseEvent.h"

#include "Wizzy/Core/Application.h"
#include "Wizzy/Core/ImGuiSystem.h"

#include "Wizzy/Audio/AudioClip.h"

#include "Wizzy/Ecs/Ecs.h"

#include "Wizzy/Math/Rect.h"

#include "Wizzy/JobSystem/JobSystem.h"

#include "Wizzy/Input/Input.h"

#include "Wizzy/Resource/ResourceManagement.h"

#include "Wizzy/Utilities/TemplateUtils.h"

namespace wz = ::Wizzy;
