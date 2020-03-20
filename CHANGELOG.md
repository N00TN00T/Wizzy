# CHANGELOG FOR WIZZY

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
