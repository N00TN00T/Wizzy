# CHANGELOG FOR WIZZY


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

## 0.1.1 - ECS Serialization
- Implemented Serialization and deserialization of and ECS System
	- Pointers are not supported. This means only stack-strings are supported in terms of strings.
- Made the Renderer2D use RenderTarget::Handle instead of RenderTargetPtr
- Minor fixes & improvements