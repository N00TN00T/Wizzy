# CHANGELOG FOR WIZZY

## 0.1.8 - Wizzy Maths
### Maths
- Templated vec2, vec3, vec4 & mat4 structs
- Typedefs for common types likes ivec3, fvec3, uvec3 etc
- All vector types can be upgraded and downgraded as well as converted to other data types with no errors (but warnings like implicit float to int conversion are still there)
- Can store their data like any datatype, as long as math-operations are applicable
- Projection & Transformation functions for mat4
- Color is now just a typedef of fvec4 but can be used the same way as vec4 has alias for xyzw being rgba (union)
### Misc
- Changed directory structure and divided all files into directories

## 0.1.7 - New & Improved Ecs, Wizzy Game Extension & 64-bit Tables
### Ecs
- Fully redesgined the Ecs implementation
- More things are done in compile & initialization time
- Improved cache friendlyness
- Better & Safer API
- Improved overall safety
- Improved SystemLayer system (Enable/Disable, Push/Pop, Insert/Remove)
### Game Extension
- An optional extension
- Ecs Systems & Components for basic video game tasks:
	- Sprites
	- Cameras
	- Point Lights
- Abstracts the concept of Pipelines into Sprites, Cameras & per object Custom Shaders.
- An API to easily implement the Game features (`Attach(systemLayer)`, `CreateSprite(ecs*)` etc)
### PropertyTable
- Refactored to support 64-bit integers & floats and
- to do all type checks in compile time (if constexpr)
- Changed Get<>() to return a copy of T instead of reference. This is for example if you request a an unsigned int, the value will be cast to that before returned (PropertyTable stores ints as signed ints).
- There is now instead a GetRef<>() function (so you don't need to copy strings & SubTables every time you read them). However, this function is strict so you have to specify the correct type i.e. if you want an integer you HAVE to specify ut as a 64-bit signed integer and nothing else. Otherwise a static assertion fails and the code won't compile.
### Demo Game
- Extracted some code from the demo game to the Game Extension & added some lighting to the game.
- Render the game with two other cameras and show the results in ImGui windows
### Misc
- Added Create & CreateRuntime in ResourceManagement to construct & add a resource for you
- Implemented the Bitset type that acts as a dynamic n-bit integer that you can do bitwise operations on and set/get bits represented in bool values.
- Added the squirrel submodule

## 0.1.6 - Optimized Text Rendering & First Game In Wizzy! (Breakout)
### Text Rendering
- Fonts now properly renders texts to a render target
- Renders atlas to RenderTarget with a source rectangle instead of the old way of creating a RenderTarget for each char, then rendering it to the string rendertarget and then disposing the char rendertarget
- Rendered strings are cached so a new texture isn't created every time a text is rendered
- Cache is limited to a memory budget (as of now, 100MB)
### Demo Game (Breakout)
- Created a simple Breakout game to showcase features of Wizzy
- Currently only shows basics like rendering, text rendering, input polling & resource file syncing
- Game currently consists of, as of 1th of May 2020, the following:
	- A platform controlled by A & D (The player)
	- A block of bricks
	- A ball that constantly moves with a velocity and bounces of the player/bricks and the window bounds except for the bottom when it's position is reset instead
	- The ball destroys bricks on collision
	- A bit of text is rendered in the corner displaying the total time passed
- Demonstrates how the ECS can be used efficiently
- Will constantly be expanded upon to demonstrate the 2D capabilities
### Renderer
- Added `SubmitText()` function
- Added a `clearColor` value in Renderer2D::Pipeline so pipelines can have different clear colors
- ClearColor is now cached in RendererAPI to avoid unnecessary api call if settings color to what it already is
- Added a text shader which works as a fallback shader for text submissions
- RenderTarget no longer creates a dummy RenderBuffer and thereby halfes its memory usage
- Fixed a bug where nested pipeline batches would mess up Textures. Moved texture binding from 
`Submit()` to `End()`
- Fixed a bug where Viewports were messed up when rendering to a pipeline with a non-window RenderTarget. Fixed by resetting viewport to Window viewport after changing it for the RenderTarget
- Fixed render rectangles so they work correctly
### ECS
- Added `ForEachEntity()` function in ECSManager so all entity handles can be iterated safely
- Added the `HasComponent<>()` function to check if a component type is associated with an EntityHandle
### Resource Management
- Now always writes the highest id to a file, and reads it when initialized. This is to prevent resource id duplicates.
### Misc
- `Rect::Intersect()` now has an optional out parameter that is set to the intersection area rectangle
- Changed unnecessary info level logs to trace level


## 0.1.5 - New 2D Renderer
### Renderer
- Changed the API to be based on a Pipeline handle so render batches can be seamlessly created between pipelines with different shaders and rendertargets.
- Use a RAM allocated buffer for writing submissions to and uploading to the GPU, instead of mapping a buffer ptr from the GPU each frame and uploading the changes.
- Implemented a metrics class to get information about the renderer workload
- Now has a fallback shader if none is specified (null or invalid handle)
### ECS
- Added enabled flags for Systems in the System layer to be able to enable/disable systems in a layer
- Fixed a crash that occured if there were no entites that had all components required in a system but some of the components exist
- Serializing & Deserializing now uses the PropertyTable
### OpenGL
- Now uses Debug Message Callbacks and logs it if GL 4.5
### Log
- Improved debug log messages to be more pleasing to the eye (and print more info)
### Resource Management
- Resource handles now asserts that they are the correct type when copied or assigned
- Resource Handles can no longer be constructed with an integer
- Resource Meta files are now correctly managed
### Scripting
- Added functions for checking types of fields in lua tables
### Misc
- Removed tests/examples and created a stress test instead in Sandbox
- Minor fixes & improvements
- Heavily improved performance by making sure resource handles are always passed as const &

## 0.1.4 - Better serialization & scripting improvements
### PropertyTable
- Revamped the PropertyTable serialization/deserialization to something much simpler and readable (text format)
- Removed anything GUI related from PropertyTable and instead added a function that expose all keys in a table
### ScriptContext
- Made the ScriptContext class more complete with necessary features
	- Pushing values to the stack
	- Creating/setting metatables and setting their metaevents
	- Getting/Setting/Invoking from stack values
	- Field setting/getting (for tables on stack)
### ResourceManagement
- Implemented a file watcher that detects changes in resource files in Validate()
### ECS
- Added the Clear() function to ECSManager which clears it and frees all allocated memory
### Misc
- Added some more exceptions
- Minor fixes & improvements

## 0.1.3 - Job System & Scripting
### Job System
- A job system that manages worker threads, with one worker for each CPU core. 
- Splits jobs unto worker threads
- Can simulate a for loop with the iterations divided unto the worker threads
### Scripting
- Script resource - A simple resource implementation that currently only holds the script code
- Script Context - A lua_State wrapper to abstract operations and avoid the need to use the lua API to do script stuff
- Deleted ScriptManagement class because it was stupid
### Misc
- ResourceManagement now dispatch the following events:
	- resource_register - Upon registering a new resource file
	- resource_load - Upon succesfully loading a resource into memory
	- resource_unload - Upon unloading a resource from memory
	- resource_delete - Upon deleting a resource file from ResourceManagement
	- resource_change - Upon ResourceManagement::Validate finding a change in resource file
- Resource implementations are now forced to define a static IsFileBinary() function so ResourceManagement can decide to read the file in binary or text format
- Minor fixes, improvements, logs & refactoring

## 0.1.2 - Project System, Scene System & Runtime Resources
### Projects & Scenes
- Implemented a basic project management system as a client example and for future reference
- Implemented a basic scene system as a client example and for future  reference
### Resource Management
- Extracted WrireResourceList() & LoadResourceList() into the client code as an example as it's a very implementation specific way to go about it.
- Added the Runtime Resource feature. Instead of using special case raw pointers for runtime resources, you can now use AddRuntimeResource() to use resources loaded in runtime to avoid associating all essential resources with files. This also lets the client define new Resources to be used in ResourceManagement.
- Fixed ResourceManagement::GetHandles()
### Font Rendering
- Implemented a non-working Font resource.
	- Atlas is loaded correctly
	- Characters are not rendered properly to a rendertarget
### Rendering API/Graphics
- Added the TextureChannelsToAPIFormat() function to get the native API value to represent a texture channel count between 1 and 4.
- Added the function overloads to render RenderTargets as Textures in Renderer2D.
- Refactored Texture to properly load from data provided and serialize it correctly.
- Creating Textures manually now requires the amount of channels as an extra parameter.
### Misc
- Added FreeType 2 as a vendor for Wizzy.
- Minor fixes, improvements & safety checks.

## 0.1.1 - ECS Serialization
- Implemented Serialization and deserialization of and ECS System
	- Pointers are not supported. This means only stack-strings are 
supported in terms of strings.
- Made the Renderer2D use RenderTarget::Handle instead of RenderTargetPtr
- Minor fixes & improvements

## 0.1.0 - Version history documentation
- Started recording changes and versioning Wizzy
- Features as of now:
	- Platform-abstracted rendering API
	- 2D Batch Renderer
	- Basic 3D Renderer
	- Advanced Resource Management system
	- Advanced Logging System
	- Platform-abstracted OS Window & Input polling
	- Windows & Linux build support
	- Event System
	- ECS System
