#pragma once

#define _STRUCT_DECL(tname) struct tname : public wz::Component<tname>

_STRUCT_DECL(ResourceManagerComponent)
{
	ResourceManagerComponent()
	{
		string d = ulib::File::directory_of(GetExecutablePath()) + "/../../../res/";
		strcpy(resourceDir, d.c_str());
	}
	char resourceDir[1024];

	// ResourceManagement validations per seconds
	float validationRate = 1.f;

	float timeSinceValidate = 0.f;
};

_STRUCT_DECL(RendererManagerComponent)
{
	u32 cameraIndex = 0;
	
};

_STRUCT_DECL(CameraComponent)
{
public:
	float zoom = 0;
	mat4 transform = mat4(1.f);
	mat4 projection = mat4(1.f);
	wz::RenderTarget::Handle hRenderTarget = WZ_NULL_RESOURCE_HANDLE;
	wz::Shader::Handle hShader = WZ_NULL_RESOURCE_HANDLE;
	bool rendererReady = false;
};

_STRUCT_DECL(TextureComponent)
{
	wz::Texture::Handle hTexture = WZ_NULL_RESOURCE_HANDLE;
	wz::Color color;
};

_STRUCT_DECL(TransformComponent)
{
	vec2 position = vec2(0.f);
	vec2 scale = vec2(1.f);
	float rotation = 0;
};