# Small tasks
- Build FreeType 2 binaries for linux
- Awake function in system that is invoked when entity that use the system is created
- Ensure a resource id can never be resued
- Make Table.Set safe

# Medium tasks
- Job system for editor
- Implement an optional and custom serialization of imgui (avoid imgui.ini)
- Redesign Rendering API to avoid Bind() & Unbind() outside OpenGL implementations

# Large tasks
## Rendering
- Refactor renderer2D to use manually allocated RAM for the buffer and upload to 
  VRAM (glBufferSubData) instead of mapping the buffer
- Font rendering
- Implement DirectX rendering
- Send opengl calls to a queue and invoke on main thread.
## Misc
- Create documentation on charlie-malmqvist.com/Wizzy/documentation.html
- Resource VFS
- Rewrite trash ecs
- Audio :(

# Wishful thinking tasks
- Implement Vulkan rendering













