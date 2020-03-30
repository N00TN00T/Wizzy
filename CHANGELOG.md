# CHANGELOG FOR WIZZY

## 0.1.4 - Better serialization & scripting improvements
### PropertyTable
- Revamped the PropertyTable serialization/deserialization to something much simpler and readable (text format)
- Removed anything GUI related from PropertyTable and instead added a function that expose all keys in a table
### ScriptContext
- Made the ScriptContext class more complete with necessary features
	- Pushing values to the stack
	- Creating/setting metatables and setting their metaeents
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
