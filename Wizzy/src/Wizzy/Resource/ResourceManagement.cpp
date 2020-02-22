#include "wzpch.h"

#include "Wizzy/Resource/ResourceManagement.h"
#include "Wizzy/Utils.h"

#include "Wizzy/Scripting/Script.h"
#include "Wizzy/Renderer/Texture.h"
#include "Wizzy/Renderer/Shader.h"
#include "Wizzy/Renderer/Material.h"
#include "Wizzy/Renderer/Model.h"
#include "Wizzy/Renderer/RenderTarget.h"

#define __CHECK_HANDLE(handle, loaded)\
{\
	if (!IsValid(handle))\
	{\
		WZ_THROW(ResourceInvalidHandleException, handle); \
	}\
	\
	auto& resourceInfo = s_resourceInfo[handle]; \
	\
	if (IsLoaded(handle) != loaded)\
	{\
		if (loaded) \
		{\
			WZ_THROW(ResourceUnloadedException, resourceInfo.resPath, handle.id, resourceInfo.name()); \
		}\
		else\
		{\
			WZ_THROW(ResourceLoadedException, resourceInfo.resPath, handle.id, resourceInfo.name()); \
		}\
		\
	}\
}\

namespace Wizzy
{
	std::vector<Resource*> ResourceManagement::s_resource;
	Resource::HandleMap<ResourceInfo> ResourceManagement::s_resourceInfo;
	std::set<Resource::Handle> ResourceManagement::s_handles;
	string ResourceManagement::s_resourceDir = "";
	u32 ResourceManagement::s_freeIndicesCount = 0;
	uId ResourceManagement::s_idCounter;

#if WZ_VERSION_SUM(WZ_VERSION_MAJOR, WZ_VERSION_MINOR, WZ_VERSION_PATCH) <= WZ_VERSION_SUM(0, 1, 0)

	void ResourceManagement::LoadResourceList(const string& listFile)
	{
		string fullPath = s_resourceDir + listFile;

		string data = "";
		if (!ulib::File::read(fullPath, &data, true))
		{
			WZ_THROW(ResourceFileAccessException, fullPath);
		}

		std::stringstream s(data);
		string line;

		while (std::getline(s, line))
		{
			if (line.find(';') == string::npos) WZ_THROW(ResourceParseErrorException, listFile);
			string resPath = line.substr(0, line.find_first_of(';'));
			line = line.substr(line.find_first_of(';') + 1);
			string typeStr = line.substr(0, line.find_first_of(';'));
			line = line.substr(line.find_first_of(';') + 1);
			string idStr = line;
			uId id = 0;
			try
			{
				id = std::stoi(idStr);
			}
			catch (const std::exception& e)
			{
				WZ_THROW(Exception, "Could not convert string '" + idStr + "' to int: " + string(e.what()));
			}

			try
			{
				if (typeStr == typestr(Script))
				{
					Load<Script>(resPath, id);
				}
				else if (typeStr == typestr(Shader))
				{
					Load<Shader>(resPath, id);
				}
				else if (typeStr == typestr(Material))
				{
					Load<Material>(resPath, id);
				}
				/*else if (typeStr == typestr(Model))
				{
					Load<Model>(resPath, id);
				}*/
				else if (typeStr == typestr(Texture))
				{
					Load<Texture>(resPath, id);
				}
				else if (typeStr == typestr(RenderTarget))
				{
					Load<RenderTarget>(resPath, id);
				}
				else
				{
					WZ_THROW(Exception, "Unhandled or invalid type string: " + typeStr);
				}
			}
			catch (const Exception& e)
			{
				WZ_CORE_ERROR("Exception when loading from resource list");
				WZ_CORE_ERROR(e.GetUnhandledMessage());
			}
		}
	}

	void ResourceManagement::WriteResourceList(const string& listFile)
	{
		string list = "";

		for (auto handle : s_handles)
		{
			auto& info = s_resourceInfo[handle];

			list += info.resPath + ";" + info.type + ";" +  std::to_string(handle.id) + "\n";
		}

		if (!ulib::File::write(s_resourceDir + listFile, list, true))
		{
			WZ_THROW(ResourceFileAccessException, s_resourceDir + listFile);
		}

		for (auto& hnd : s_handles)
		{
			if (IsValid(hnd) && IsLoaded(hnd))
			{
				Save(hnd);
			}
		}
	}

	void ResourceManagement::Load(Resource::Handle handle)
	{
		__CHECK_HANDLE(handle, false);
		auto& info = s_resourceInfo[handle];
		
		WZ_CORE_ASSERT(info.resourceIndex < s_resource.size(), "Resource index " + std::to_string(info.resourceIndex) + "' out of range");
		auto& pResource = s_resource[info.resourceIndex];

		WZ_CORE_ASSERT(pResource == nullptr, "Resource already loaded");

		try
		{
			info.props.SetProperty("SourceFile", s_resourceDir + info.resPath);
			pResource = info.createFn(info.source, info.props);
		}
		catch (const Exception& e)
		{
			if (pResource) delete pResource;
			WZ_CORE_ERROR("Error when creating resource for handle id {0}:", handle.id);
			Log::SetExtra(false);
			WZ_CORE_ERROR("    {0}", e.GetMessage());
			Log::SetExtra(true);
			return;
		}
	}

	void ResourceManagement::Save(Resource::Handle handle)
	{
		__CHECK_HANDLE(handle, true);

		auto& info = s_resourceInfo[handle];

		Resource* resource = s_resource[info.resourceIndex];
		ResData serialized = resource->Serialize();

		info.source = serialized;

		string file = s_resourceDir + info.resPath;
		if (!ulib::File::write(file, info.source, true))
		{
			WZ_THROW(ResourceFileAccessException, file);
		}

		try
		{
			// Take the props in the resource and apply the ones in info
			PropertyLibrary& resourceProps = resource->GetProps();
			resourceProps.Deserialize(info.props.Serialize());

			string cfgFile = file + ".wz";
			if (!ulib::File::write(cfgFile, resourceProps.Serialize(), true))
			{
				WZ_THROW(ResourceFileAccessException, cfgFile);
			}
		}
		catch (const Exception& e)
		{
			WZ_CORE_ERROR("Error when saving config file:");
			WZ_CORE_ERROR(e.GetUnhandledMessage());
			WZ_CORE_WARN("Configuration was not saved");
		}

		try
		{
			if (!ulib::File::write(file, serialized, true))
			{
				WZ_THROW(ResourceFileAccessException, file);
			}
		}
		catch (const Exception& e)
		{
			WZ_CORE_ERROR("Error when saving source file:");
			WZ_CORE_ERROR(e.GetUnhandledMessage());
			WZ_CORE_WARN("Source file was not saved");
		}
	}

	void ResourceManagement::Unload(Resource::Handle handle)
	{
		__CHECK_HANDLE(handle, true);

		auto& info = s_resourceInfo[handle];

		delete s_resource[info.resourceIndex];
		s_resource[info.resourceIndex] = nullptr;
	}

	void ResourceManagement::Reload(Resource::Handle handle, const PropertyLibrary& props)
	{
		__CHECK_HANDLE(handle, true);

		auto& resourceInfo = s_resourceInfo[handle];
		resourceInfo.props = props;

		Unload(handle);
		Load(handle);
	}

	void ResourceManagement::Reload(Resource::Handle handle)
	{
		__CHECK_HANDLE(handle, true);

		Unload(handle);
		Load(handle);
	}

	void ResourceManagement::Delete(Resource::Handle handle)
	{
		if (!IsValid(handle))
		{
			WZ_THROW(ResourceInvalidHandleException, handle); 
		}

		if (IsLoaded(handle))
		{
			Unload(handle);
		}

		auto& info = s_resourceInfo[handle];

		delete s_resource[info.resourceIndex];
		s_resource[info.resourceIndex] = nullptr;
		s_freeIndicesCount++;

		s_handles.erase(handle);
		s_resourceInfo.erase(handle);
	}

	void ResourceManagement::SetResourceDir(const string& dir)
	{
		if (!ulib::Directory::exists(dir))
		{
			WZ_THROW(ResourceInvalidPathException, dir);
		}
		s_resourceDir = dir;
	}

	void ResourceManagement::Validate(bool checkSources)
	{
		for (auto handle : s_handles)
		{
			if (s_resourceInfo.find(handle) == s_resourceInfo.end())
			{
				WZ_THROW(ResourceInvalidHandleException, handle);
			}
		}

		for (auto& [handle, info] : s_resourceInfo)
		{
			string fullPath = s_resourceDir + info.resPath;

			if (s_handles.find(handle) == s_handles.end())
			{
				WZ_THROW(ResourceInvalidHandleException, handle.id);
			}

			if (info.resourceIndex >= s_resource.size())
			{
				WZ_THROW(ResourceIndexOutOfRangeException, info.resPath, handle.id, info.name());
			}

			if (!ulib::File::exists(fullPath))
			{
				WZ_THROW(ResourceFileMissingException, info.resPath, handle.id, info.name());
			}

			if (checkSources && IsLoaded(handle))
			{
				ResData fileData;
				if (!ulib::File::read(fullPath, &fileData, true))
				{
					WZ_THROW(ResourceFileAccessException, info.resPath);
				}

				if (fileData != info.source)
				{
					/*WZ_CORE_WARN("Changed detected, reloading file {0}", info.resPath);
					info.source = fileData;
					Reload(handle);*/
				}
			}
		}
	}

	const ResourceInfo& ResourceManagement::GetInfoFor(Resource::Handle handle)
	{
		if (IsValid(handle))
		{
			return s_resourceInfo[handle];
		}
		else
		{
			WZ_THROW(ResourceInvalidHandleException, handle);
		}
	}

	string ResourceManagement::UniquenizePathName(const string& path)
	{
		if (!ulib::File::exists(s_resourceDir + path)) return path;
		string p = path;
		int suffix = 1;

		while (ulib::File::exists(s_resourceDir + ulib::File::without_extension(p) + "(" + std::to_string(suffix) + ")" + ulib::File::extension_of(p)))
		{
			suffix++;
		}

		p = ulib::File::without_extension(p) + "(" + std::to_string(suffix) + ")" + ulib::File::extension_of(p);

		return p;
	}

	bool ResourceManagement::IsValid(Resource::Handle handle)
	{
		return s_handles.find(handle) != s_handles.end() && s_resourceInfo.find(handle) != s_resourceInfo.end();
	}

	bool ResourceManagement::IsValid(const string& resPath)
	{
		Resource::Handle hndMatch = WZ_NULL_RESOURCE_HANDLE;
		for (const auto& handle : s_handles)
		{
			auto& info = s_resourceInfo[handle];

			if (info.resPath == resPath)
			{
				hndMatch = handle;
				break;
			}
		}

		return IsValid(hndMatch);
	}

	bool ResourceManagement::IsLoaded(Resource::Handle handle)
	{
		return IsValid(handle) && s_resource[s_resourceInfo[handle].resourceIndex] != nullptr;
	}

	Resource::Handle ResourceManagement::HandleOf(const string& str)
	{
		if (ulib::File::exists(s_resourceDir + str))
		{
			Resource::Handle hndMatch = WZ_NULL_RESOURCE_HANDLE;
			for (const auto& handle : s_handles)
			{
				auto& info = s_resourceInfo[handle];

				if (info.resPath == str)
				{
					hndMatch = handle;
					break;
				}
			}

			return hndMatch;
		}
		else
		{
			Resource::Handle hndMatch = WZ_NULL_RESOURCE_HANDLE;
			for (const auto& handle : s_handles)
			{
				auto& info = s_resourceInfo[handle];

				if (info.name() == str)
				{
					hndMatch = handle;
					break;
				}
			}

			return hndMatch;
		}
	}

	/* #elif WZ_VERSION_SUM(WZ_VERSION_MAJOR, WZ_VERSION_MINOR, WZ_VERSION_PATCH) <= WZ_VERSION_SUM(0, 1, 1) */

#endif

}

//#include "Wizzy/Renderer/Material.h"
//#include "Wizzy/Renderer/Texture.h"
//#include "Wizzy/Renderer/Shader.h"
//#include "Wizzy/Renderer/Model.h"
//#include "Wizzy/Scripting/Script.h"
//#include "Wizzy/Utils.h"
//
//#pragma region Raw scripts
//
//constexpr char graphics_config_script[] = 
//R"(
//Graphics.ClearColor = Color.gray
//Graphics.CullMode = CULL_MODE_BACK
//Graphics.DepthTesting = true
//Graphics.Blending = true
//)";
//
//#pragma endregion
//
//#pragma region Raw shaders
//constexpr char albedo_shader_raw[] = R"(#shader vertex
//
//#version 330 core
//
//layout(location = 0) in vec4 vertexPosition;
//layout(location = 1) in vec2 vertexUv;
//layout(location = 2) in vec3 vertexNormal;
//
//uniform int test;
//uniform vec3 testvec;
//uniform mat4 camTransform;
//uniform mat4 worldTransform;
//
//out vec4 worldPosition;
//out vec2 uv;
//
//void main() {
//    worldPosition = camTransform * worldTransform * vertexPosition;
//    gl_Position = worldPosition;
//
//    uv = vertexUv;
//}
//
//
//
//
//#shader fragment
//
//#version 330 core
//
//out vec4 outColor;
//
//in vec4 worldPosition;
//in vec2 uv;
//
//uniform vec4 albedo = vec4(1.0, 1.0, 1.0, 1.0);
//
//void main()
//{
//    outColor = albedo;
//}
//)";
//
//constexpr char phong_shader_raw[] = R"(
//	#lightmode phong
//
//#shader vertex
//
//#version 330 core
//
//layout(location = 0) in vec3 vertexPosition;
//layout(location = 1) in vec2 vertexUv;
//layout(location = 2) in vec3 vertexNormal;
//
//uniform mat4 u_camTransform;
//uniform mat4 u_worldTransform;
//
//out vec3 fragPos;
//out vec2 uv;
//out mat4 camTransform;
//out vec3 normal;
//
//void main() {
//    gl_Position = u_camTransform * u_worldTransform * vec4(vertexPosition, 1.0);
//
//    fragPos = (u_camTransform * vec4(vertexPosition, 1.0)).xyz;
//    uv = vertexUv;
//    camTransform = u_camTransform;
//    normal = mat3(transpose(inverse(u_worldTransform))) * vertexNormal;
//	fragPos = (u_worldTransform * vec4(vertexPosition, 1.0)).xyz;
//}
//
//
//
//
//#shader fragment
//
//#version 330 core
//
//#define LIGHT_TYPE_DIRECTIONAL 0
//#define LIGHT_TYPE_POINT 1
//#define LIGHT_TYPE_SPOT 2
//
//out vec4 outColor;
//
//in vec3 fragPos;
//in mat4 camTransform;
//in vec2 uv;
//in vec3 normal;
//
//struct Light {
//    int type;
//    vec3 position;
//    vec3 direction;
//    vec4 color;
//    float intensity;
//    float range;
//    float cutOff;
//    float smoothness;
//};
//
//uniform vec4 u_albedo = vec4(1.0, 1.0, 1.0, 1.0);
//uniform vec4 u_diffuseColor = vec4(1.0, 1.0, 1.0, 1.0);
//uniform bool u_useDiffuseMap = false;
//uniform sampler2D u_diffuseMap;
//uniform vec4 u_specularColor = vec4(1.0, 1.0, 1.0, 0.5);
//uniform bool u_useSpecularMap = false;
//uniform sampler2D u_specularMap;
//
//uniform vec3 u_viewPos = vec3(-10.0, 0.0, 0.0);
//uniform vec4 u_ambient = vec4(0.2, 0.2, 0.2, 1.0);
//uniform int u_nLights = 0;
//uniform Light u_lights[64];
//
//void CalculateDirectionalLight(vec3 direction, vec4 color, float intensity, float metallic, inout vec3 diffuse, inout vec3 specular) {
//
//    vec3 norm = normalize(normal);
//    vec3 lightDir = normalize(-direction);
//
//    float diff = max(dot(norm, lightDir), 0.0);
//    diffuse += diff * color.xyz  * intensity;
//
//    // specular
//    vec3 viewDir = normalize(u_viewPos - fragPos);
//    vec3 reflectDir = reflect(-lightDir, norm);
//
//    float spec = pow(max(dot(viewDir, reflectDir), 0.0), metallic);
//    specular += spec * color.xyz * intensity;
//}
//
//void CalculatePointLight(vec3 position, vec4 color, float intensity, float metallic, float range, inout vec3 diffuse, inout vec3 specular) {
//    vec3 lightPos = position;
//    vec3 lightDiff = lightPos - fragPos;
//    float lightDist = length(lightDiff);
//
//    float distanceIntensity = (range - lightDist) / range;
//    if (distanceIntensity > 1.0) distanceIntensity = 1.0;
//
//    // diffuse
//    vec3 norm = normalize(normal);
//    vec3 lightDir = normalize(lightDiff);
//
//    float diff = max(dot(norm, lightDir), 0.0);
//    diffuse += diff * color.xyz  * intensity * distanceIntensity;
//
//    // specular
//    vec3 viewDir = normalize(u_viewPos - fragPos);
//    vec3 reflectDir = reflect(-lightDir, norm);
//
//    float spec = pow(max(dot(viewDir, reflectDir), 0.0), metallic);
//    specular += spec * color.xyz * intensity * distanceIntensity;
//}
//
//void CalculateSpotLight(vec3 position, vec3 direction, vec4 color, float intensity, float metallic, float range, float cutOff, float smoothness, inout vec3 diffuse, inout vec3 specular) {
//
//    vec3 lightPos = position;
//    vec3 lightDiff = lightPos - fragPos;
//    vec3 lightDir = normalize(lightDiff);
//
//    smoothness = clamp(smoothness, 0.01, 1.0);
//
//    float theta = dot(lightDir, normalize(-direction));
//    float epsilon = (1.0 - smoothness) * cutOff;
//    intensity *= clamp((theta - cutOff) / epsilon, 0.0, 1.0);
//
//    if(theta > cutOff) {
//        CalculatePointLight(position, color, intensity, metallic, range, diffuse, specular);
//    }
//}
//
//void main()
//{
//    float metallic = u_specularColor.w * 256;
//    if (metallic < 0.137) metallic = 0.137;
//    vec4 diffuseColor = u_diffuseColor;
//    vec3 diffuse = vec3(0.0, 0.0, 0.0);
//    vec4 specularColor = u_specularColor;
//    vec3 specular = vec3(0.0, 0.0, 0.0);
//
//    if (u_useDiffuseMap) {
//        diffuseColor = texture(u_diffuseMap, uv);
//    }
//    if (u_useSpecularMap) {
//        specularColor = texture(u_specularMap, uv);
//    }
//
//    for (int i = 0; i < u_nLights; i++) {
//
//        switch (u_lights[i].type) {
//            case LIGHT_TYPE_DIRECTIONAL:
//                CalculateDirectionalLight(u_lights[i].direction, u_lights[i].color, u_lights[i].intensity, metallic, diffuse, specular);
//                break;
//            case LIGHT_TYPE_POINT:
//                CalculatePointLight(u_lights[i].position, u_lights[i].color, u_lights[i].intensity, metallic, u_lights[i].range, diffuse, specular);
//                break;
//            case LIGHT_TYPE_SPOT:
//                CalculateSpotLight(u_lights[i].position, u_lights[i].direction, u_lights[i].color, u_lights[i].intensity, metallic, u_lights[i].range, u_lights[i].cutOff, u_lights[i].smoothness, diffuse, specular);
//                break;
//        }
//
//    }
//
//    outColor = u_albedo * (u_ambient + vec4(diffuse + specular * specularColor.xyz, 1.0)) * diffuseColor;
//}
//
//)";
//
//#pragma endregion
//
//namespace Wizzy {
//
//	std::set<ResourceHandle> ResourceManagement::s_handles;
//	std::vector<Resource*> ResourceManagement::s_resourceMemory;
//	std::unordered_map<ResourceHandle, ResourceInfo> ResourceManagement::s_resourceInfo;
//	Path ResourceManagement::s_resourceDir;
//
//	template <typename TCollection>
//	string UniquenizeString(const string& str, TCollection source) {
//
//		int _num = 1;
//		string _unique = str + "(" + std::to_string(_num) + ")";
//		while (std::find(source.begin(), source.end(), _unique) != source.end()) {
//			_num++;
//			_unique = str + "(" + std::to_string(_num) + ")";
//		}
//
//		return _unique;
//	}
//
//	bool ResourceManagement::Unregister(const ResourceHandle& handle) {
//		WZ_CORE_ASSERT(IsValid(handle), "Tried unregistering invalid handle");
//
//		const auto& _resourceInfo = s_resourceInfo[handle];
//		int32 _dataIndex = _resourceInfo.dataIndex;
//
//		s_handles.erase(handle);
//		s_resourceMemory[_dataIndex] = nullptr;
//		s_resourceInfo.erase(handle);
//
//		return true;
//	}
//
//	void ResourceManagement::RegisterResourceDirectory() {
//
//		std::set<string> _handledRawFiles;
//
//		ulib::Directory::for_each(s_resourceDir, ulib::Directory::FILES | ulib::Directory::RECURSIVE, [&](string file) {
//
//			if (ulib::File::has_directory(file, ToFullPath("core"))) return;
//
//			string _data = "";
//			bool _readSuccess = ulib::File::read(file, &_data);
//			WZ_CORE_ASSERT(_readSuccess, "Failed reading file '{0}'", file);
//
//			bool _isMeta = ulib::File::extension_of(file) == "." + string(WZ_META_FILE_EXTENSION)
//						&& _data.find(WZ_META_HEADER_BEGIN) == 0;
//
//			if (!_isMeta) return;
//
//			std::stringstream _source(_data);
//			string _line;
//
//			string _resPath = "";
//			string _handle = "";
//			string _resExt = "";
//
//			auto _metaData = parse_wz_meta(_data);
//
//			for (const auto& [_token, _value] : _metaData) {
//				if (_token == WZ_META_TOKEN_RESPATH)		_resPath = _value;
//				else if (_token == WZ_META_TOKEN_HANDLE)	_handle = _value;
//				else if (_token == WZ_META_TOKEN_EXTENSION) _resExt = _value;
//				else if (_token != WZ_META_TOKEN_CUSTOM)	WZ_CORE_ASSERT(false, "Unknown meta token");
//			}
//
//			WZ_CORE_ASSERT(_resPath != "" && _handle != "" && _resExt != "", "Missing obligatory meta data");
//
//			if (_resExt == WZ_EXTENSION_MATERIAL) {
//				Register<Material>(ToFullPath(_resPath), _resPath, _handle);
//			} else if (_resExt == WZ_EXTENSION_TEXTURE) {
//				Register<Texture>(ToFullPath(_resPath), _resPath, _handle);
//			} else if (_resExt == WZ_EXTENSION_SHADER) {
//				Register<Shader>(ToFullPath(_resPath), _resPath, _handle);
//			} else if (_resExt == WZ_EXTENSION_SCRIPT) {
//				Register<Script>(ToFullPath(_resPath), _resPath, _handle);
//			} else if (_resExt == WZ_EXTENSION_MODEL) {
//				Register<Model>(ToFullPath(_resPath), _resPath, _handle);
//			} else {
//				WZ_CORE_ASSERT(false, "Unknown resource extension");
//			}
//			Load(_handle);
//			Build(_handle);
//			Unload(_handle);
//			
//			_handledRawFiles.emplace(ToFullPath(_resPath));
//
//		});
//
//		LoadCoreResources();
//
//		ulib::Directory::for_each(s_resourceDir, ulib::Directory::FILES | ulib::Directory::RECURSIVE, [&](string file) {
//			
//			if (ulib::File::has_directory(file, ToFullPath("core"))) return;
//			
//			WZ_CORE_TRACE("Registering file '{0}' found in res dir", file);
//
//			string _data = "";
//			bool _readSuccess = ulib::File::read(file, &_data);
//			WZ_CORE_ASSERT(_readSuccess, "Failed reading file '{0}'", file);
//
//			bool _isMeta = ulib::File::extension_of(file) == "." + string(WZ_META_FILE_EXTENSION)
//				&& _data.find(WZ_META_HEADER_BEGIN) == 0;
//
//			WZ_CORE_TRACE((_isMeta ? "File is metafile" : "File is not metafile"));
//
//			if (!_isMeta && _handledRawFiles.find(file) == _handledRawFiles.end()) {
//				
//				string _fileExtension = ulib::File::extension_of(file);
//				if (IS_IMAGE_EXTENSION(_fileExtension)) {
//					Register<Texture>(file, ResPathOfFile(file), ulib::File::without_extension(ulib::File::name_of(file)));
//				} else if (IS_SHADER_EXTENSION(_fileExtension)) {
//					Register<Shader>(file, ResPathOfFile(file), ulib::File::without_extension(ulib::File::name_of(file)));
//				} else if (IS_MODEL_EXTENSION(_fileExtension)) {
//					Register<Model>(file, ResPathOfFile(file), ulib::File::without_extension(ulib::File::name_of(file)));
//				} else if (IS_SCRIPT_EXTENSION(_fileExtension)) {
//					WZ_CORE_WARN("File '{0}' was not imported as a script as it has no meta data so script type cannot be determined. Please import manually.", ResPathOfFile(file));
//				} else {
//					WZ_CORE_WARN("File '{0}' was not recognized as a resource by extension, try importing it manually", file);
//					return;
//				}
//
//			}
//
//		});
//
//	}
//
//	bool ResourceManagement::Load(const ResourceHandle& handle) {
//		WZ_CORE_ASSERT(IsValid(handle), "Tried loading invalid handle");
//		
//		if (IsLoaded(handle)) return true;
//
//		WZ_CORE_ASSERT(s_handles.find(handle) != s_handles.end(), "Tried loading resource from non-registered handle");
//
//		WZ_CORE_TRACE("Loading resource '{0}'");
//
//		auto& _resourceInfo = s_resourceInfo[handle];
//
//		u32 _dataIndex = _resourceInfo.dataIndex;
//		
//		string _file = FilePathOf(handle);
//
//		if (!ulib::File::exists(_file)) {
//			WZ_CORE_ERROR("Could not load resource associated with handle '{0}', missing file '{1}'", handle, _file);
//			return false;
//		}
//
//		string _wzFile = ulib::File::without_extension(_file) + ".wz";
//		if (!_resourceInfo.isBuilt || !ulib::File::exists(_wzFile)) {
//			WZ_CORE_TRACE("Resource needs to be built...");
//			s_resourceMemory[_dataIndex] = InternalLoad(handle, _file, _resourceInfo.flags);
//			bool _loadSuccess = s_resourceMemory[_dataIndex] != nullptr;
//			if (_loadSuccess) {
//				if (Build(handle)) {
//					Unload(handle);
//				} else {
//					WZ_CORE_ERROR("Failed building resource associated with handle '{0}'", handle);
//					return false;
//				} 
//			} else {
//				WZ_CORE_ERROR("Loading aborted as building failed");
//				return false;
//			}
//		}
//
//		s_resourceMemory[_dataIndex] = InternalLoad(handle, _wzFile, _resourceInfo.flags);
//
//		return IsLoaded(handle);
//	}
//
//	bool ResourceManagement::Reload(const ResourceHandle& handle) {
//		WZ_CORE_ASSERT(IsValid(handle), "Tried reloading an invalid handle");
//		return Load(handle) && Unload(handle);
//	}
//
//	bool ResourceManagement::Unload(const ResourceHandle& handle) {
//		WZ_CORE_ASSERT(IsValid(handle), "Tried unloading an invalid handle");
//		
//		if (!IsLoaded(handle)) return true;
//
//		WZ_CORE_TRACE("Unloading resource '{0}'...", handle);
//
//		auto& _resourceInfo = s_resourceInfo[handle];
//
//		u32 _dataIndex = _resourceInfo.dataIndex;
//
//		delete s_resourceMemory[_dataIndex];
//		s_resourceMemory[_dataIndex] = NULL;
//
//		return !IsLoaded(handle);
//	}
//
//	bool ResourceManagement::Validate(string& failReason) {
//
//		for (const auto& _handle : s_handles) {
//			
//			WZ_CORE_ASSERT(IsValid(_handle), "A registered handle is invalid");
//			
//			auto& _resourceInfo = s_resourceInfo[_handle];
//
//			if (!ulib::File::exists(FilePathOf(_handle))) {
//				if (IsLoaded(_handle)) Unload(_handle);
//				Unregister(_handle);
//			}
//
//			WZ_CORE_ASSERT(_resourceInfo.dataIndex < s_resourceMemory.size(), "Data index " + std::to_string(_resourceInfo.dataIndex) + " associated with handle '" + _handle + "' was out of range");
//			WZ_CORE_ASSERT(ulib::File::exists(FilePathOf(_handle)), "File '" + FilePathOf(_handle) + "' associated with handle '" + _handle + "' does not exist");
//
//			string _file = FilePathOf(_handle);
//			string _wzFile = ulib::File::without_extension(_file) + string(".") + WZ_META_FILE_EXTENSION;
//
//			WZ_CORE_ASSERT(ulib::File::exists(_file), "File for '{0}' missing", _handle);
//			if (_resourceInfo.isBuilt) {
//				WZ_CORE_ASSERT(ulib::File::exists(_wzFile), "Resource {0} was flagged as built but no meta file found", _handle);
//			}
//
//			string _fileData = "";
//			string _wzFileData = "";
//
//			bool _readSucces = ulib::File::read(_file, &_fileData);
//			if (_readSucces) _readSucces = ulib::File::read(_wzFile, &_wzFileData);
//
//			WZ_CORE_ASSERT(_readSucces, "Failed reading file");
//
//			string _withoutMeta;
//			extract_wz_meta(_wzFileData, _withoutMeta);
//
//			if (_fileData != _withoutMeta) {
//				WZ_CORE_DEBUG("A change was found in file '{0}', rebuilding resource...", _file);
//
//				if (!IsLoaded(_handle)) Load(_handle);
//			}
//		}
//
//		return false;
//	}
//
//	bool ResourceManagement::IsLoaded(const ResourceHandle& handle) {
//		WZ_CORE_ASSERT(IsValid(handle), "Cannot check load state with invalid handle");
//		return s_resourceMemory[s_resourceInfo[handle].dataIndex] != NULL;
//	}
//	string ResourceManagement::ResPathOfFile(const Path& fullPath) {
//		
//		string _resPath = "";
//		string _temp = fullPath;
//		
//		while (!ulib::File::is_same_path(_temp, s_resourceDir)) {
//			_resPath.insert(0, ulib::File::name_of(_temp) + (_resPath == "" ? "" : "/"));
//			_temp = ulib::File::directory_of(_temp);
//			WZ_CORE_ASSERT(_temp != "", "Path is not under resource directory");
//		}
//
//		return _resPath;
//
//	}
//
//	template <typename TResource>
//	bool LoadCoreResource(string resPath, string handle, const Flagset& flags = Flagset()) {
//		if (ResourceManagement::IsValid(handle)) return true;
//		string _failPath = ResourceManagement::ToFullPath("core/core-failures") + "/" + ulib::File::name_of(resPath);
//		if (ulib::File::exists(ResourceManagement::ToFullPath(resPath))) {
//			if (ResourceManagement::Register<TResource>(ResourceManagement::ToFullPath(resPath), resPath, handle, flags)) {
//				if (ResourceManagement::Load(handle)) {
//					return true;
//				} else {
//					string _cpyErr = "";
//					bool _copySuccess = ulib::File::copy(ResourceManagement::ToFullPath(resPath), _failPath, ULIB_REPLACE, _cpyErr);
//					WZ_CORE_ASSERT(_copySuccess, "Failed copying file: " + _cpyErr);
//					return false;
//				}
//			} else {
//				bool _copySuccess = ulib::File::copy(ResourceManagement::ToFullPath(resPath), _failPath, ULIB_REPLACE);
//				WZ_CORE_ASSERT(_copySuccess, "Failed copying file");
//				return false;
//			}
//		} else {
//			return false;
//		}
//	}
//
//	void ResourceManagement::LoadCoreResources() {
//
//		WZ_CORE_TRACE("Importing core resources");
//		
//		string _failDir = ToFullPath("core/failures") + "/";
//
//		bool _anyLoadError = false;
//		bool _anyFinalLoadError = false;
//
//		if (!LoadCoreResource<Shader>("core/albedo.shader", WZ_DEFAULT_SHADER_HANDLE)) {
//			_anyLoadError = true;
//			ulib::File::write(ToFullPath("core/albedo.shader"), albedo_shader_raw);
//			if (!LoadCoreResource<Shader>("core/albedo.shader", WZ_DEFAULT_SHADER_HANDLE)) _anyFinalLoadError = true;
//		}
//
//		if (!LoadCoreResource<Shader>("core/phong_basic.shader", "PhongShader")) {
//			_anyLoadError = true;
//			ulib::File::write(ToFullPath("core/phong_basic.shader"), phong_shader_raw);
//			if (!LoadCoreResource<Shader>("core/phong_basic.shader", "PhongShader")) _anyFinalLoadError = true;
//		}
//
//		if (!LoadCoreResource<Material>("core/default_material.mat", WZ_DEFAULT_MATERIAL_HANDLE)) {
//			_anyLoadError = true;
//			Register<Material>(new Material(WZ_DEFAULT_SHADER_HANDLE), "core/default_material.mat", WZ_DEFAULT_MATERIAL_HANDLE);
//			if (!LoadCoreResource<Material>("core/default_material.mat", WZ_DEFAULT_MATERIAL_HANDLE)) _anyFinalLoadError = true;
//		}
//
//		if (!LoadCoreResource<Script>("core/graphics_config.lua", WZ_GRAPHICS_CONFIG_HANDLE, BIT(SCRIPT_TYPE_CONFIG))) {
//			_anyLoadError = true;
//			ulib::File::write(ToFullPath("core/graphics_config.lua"), graphics_config_script);
//			if (!LoadCoreResource<Script>("core/graphics_config.lua", WZ_GRAPHICS_CONFIG_HANDLE, BIT(SCRIPT_TYPE_CONFIG))) _anyFinalLoadError = true;
//		}
//
//		if (_anyLoadError) {
//			WZ_CORE_WARN("Could not load core resource (user modified?), reset to default");
//		}
//
//		WZ_CORE_ASSERT(!_anyFinalLoadError, "One or more core resources were not loaded correctly");
//
//		WZ_CORE_INFO("Successfully imported core resources");
//	}
//
//	bool ResourceManagement::Build(const ResourceHandle& handle) {
//		WZ_CORE_ASSERT(IsValid(handle), "Tried building invalid handle");
//		WZ_CORE_ASSERT(IsLoaded(handle), "Resource of handle must be in memory to be built");
//
//		WZ_CORE_TRACE("Building resource associated with handle '{0}'", handle);
//
//		auto& _resourceInfo = s_resourceInfo[handle];
//
//		string _wzFile = ulib::File::without_extension(FilePathOf(handle)) + ".wz";
//
//		u32 _dataIndex = _resourceInfo.dataIndex;
//		Resource& _resource = *s_resourceMemory[_dataIndex];
//
//		string _metaData = string(WZ_META_HEADER_BEGIN) + "\n"
//						 + WZ_MAKE_META_FIELD(WZ_META_TOKEN_RESPATH, ResPathOf(handle))
//						 + WZ_MAKE_META_FIELD(WZ_META_TOKEN_HANDLE, handle)
//						 + WZ_MAKE_META_FIELD(WZ_META_TOKEN_EXTENSION, _resource.GetExtension())
//						 + WZ_MAKE_META_FIELD(WZ_META_TOKEN_CUSTOM, _resource.GetMetadata())
//						 + string(WZ_META_HEADER_END);
//
//		string _serialized = _metaData + "\n" + _resource.Serialize();
//
//		bool _writeSuccess = ulib::File::write(_wzFile, _serialized);
//
//		WZ_CORE_ASSERT(_writeSuccess, "Failed writing file");
//
//		_resourceInfo.isBuilt = true;
//
//		WZ_CORE_INFO("Successfully built '{0}'", ResPathOf(handle));
//
//		return true;
//	}
//	Resource* ResourceManagement::InternalLoad(const ResourceHandle& handle, const Path& file, const Flagset& flags) {
//		WZ_CORE_ASSERT(ulib::File::exists(file), "File for loading does not exist");
//
//		auto _resource = s_resourceInfo[handle].createFunction(file, flags);
//
//		if (_resource->IsValid()) {
//			WZ_CORE_TRACE("Resource was valid after creation");
//		} else {
//			WZ_CORE_ERROR("Resource was invalid after creation, loading cancelled");
//			delete _resource;
//			_resource = nullptr;
//		}
//
//		return _resource;
//	}
//	string ResourceManagement::UniquenizePathName(const Path& path) {
//
//		std::vector<Path> _fileNames;
//
//		_fileNames.reserve(s_resourceInfo.size());
//
//		for (const auto& [x, _info] : s_resourceInfo) {
//			_fileNames.push_back(ulib::File::without_extension(_info.resPath));
//		}
//
//		string _ext = ulib::File::extension_of(path);
//		return UniquenizeString(ulib::File::without_extension(path), _fileNames) + _ext;
//	}
//	string ResourceManagement::UniquenizeHandle(const ResourceHandle& handle) {
//		return UniquenizeString(handle, s_handles);
//	}
//}