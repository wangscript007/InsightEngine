#include <ie_pch.h>

#include "File_System.h"

#include "Insight/Core/Application.h"
#include "Insight/Core/Scene/scene.h"
#include "Insight/Core/ieException.h"
#include "Insight/Utilities/String_Helper.h"

#include "Insight/Rendering/APost_Fx.h"
#include "Insight/Rendering/ASky_Light.h"
#include "Insight/Rendering/ASky_Sphere.h"
#include "Insight/Rendering/Lighting/ASpot_Light.h"
#include "Insight/Rendering/Lighting/APoint_Light.h"
#include "Insight/Rendering/Lighting/ADirectional_Light.h"

namespace Insight {

	std::string FileSystem::ProjectDirectory = "";

	FileSystem::FileSystem()
	{
	}

	FileSystem::~FileSystem()
	{
	}

	bool FileSystem::Init(const char* ProjectName)
	{
		FileSystem::ProjectDirectory += FileSystem::GetUserDocumentsFolderPath();
		FileSystem::ProjectDirectory += "/Insight Projects/";
		FileSystem::ProjectDirectory += ProjectName;
		FileSystem::ProjectDirectory += "/";
		return true;
	}

	void FileSystem::SaveEngineUserSettings(Renderer::GraphicsSettings Settings)
	{

		rapidjson::Document RawSettingsFile;
		const std::string SettingsDir = ProjectDirectory + "/PROFSAVE.ini";
		if (!json::load(SettingsDir.c_str(), RawSettingsFile)) {
			IE_CORE_ERROR("Failed to load graphics settings from file: \"{0}\". Default graphics settings will be applied.", SettingsDir);

		}

		{
			rapidjson::StringBuffer StrBuffer;
			rapidjson::PrettyWriter<rapidjson::StringBuffer> Writer(StrBuffer);

			Writer.StartObject();
			{
				Writer.Key("Renderer");
				Writer.StartArray();
				{
					Writer.StartObject();
					Writer.Key("TargetAPI");
					Writer.Int((int)Settings.TargetRenderAPI);
					Writer.Key("TextureQuality");
					Writer.Double(Settings.MipLodBias);
					Writer.Key("TextureFiltering");
					Writer.Double(Settings.MaxAnisotropy);
					Writer.EndObject();
				}
				Writer.EndArray();
			}
			Writer.EndObject();
			

			// Final Export
			std::string sceneName = ProjectDirectory + "/PROFSAVE.ini";
			std::ofstream offstream(sceneName.c_str());
			offstream << StrBuffer.GetString();

			if (!offstream.good()) {
				IE_CORE_ERROR("Failed to save graphics properties to PROFSAVE.ini.");
			}
		}

	}

	std::string FileSystem::GetExecutbleDirectory()
	{
		WCHAR Path[512];
		UINT RawPathSize = _countof(Path);
		DWORD PathSize = GetModuleFileName(nullptr, Path, RawPathSize);
		if(PathSize == 0 || PathSize == RawPathSize) {
			throw ieException("Failed to get module path name or path may have been truncated.");
		}

		WCHAR* LastSlash = wcsrchr(Path, L'\\');
		if (LastSlash) {
			*(LastSlash + 1) = L'\0';
		}
		return StringHelper::WideToString(std::wstring{ Path });
	}

	std::string FileSystem::GetUserDocumentsFolderPath()
	{
		wchar_t Folder[1024];
		HRESULT hr = SHGetFolderPathW(NULL, CSIDL_MYDOCUMENTS, 0, 0, Folder);
		if (FAILED(hr)) {
			IE_CORE_ERROR("Failed to get path to user documents folder.");
		}
		
		return StringHelper::WideToString(std::wstring{ Folder });
	}

	std::string FileSystem::GetProjectRelativeAssetDirectory(std::string Path)
	{
		return ProjectDirectory + "/Assets/" + Path;
	}

	Renderer::GraphicsSettings FileSystem::LoadGraphicsSettingsFromJson()
	{
		using GraphicsSettings = Renderer::GraphicsSettings;
		GraphicsSettings UserGraphicsSettings = {};

		{
			Profiling::ScopedTimer timer("LoadSceneFromJson::LoadGraphicsSettingsFromJson");

			rapidjson::Document RawSettingsFile;
			const std::string SettingsDir = ProjectDirectory + "/PROFSAVE.ini";
			if (!json::load(SettingsDir.c_str(), RawSettingsFile)) {
				IE_CORE_ERROR("Failed to load graphics settings from file: \"{0}\". Default graphics settings will be applied.", SettingsDir);
				return UserGraphicsSettings;
			}

			const rapidjson::Value& RendererSettings = RawSettingsFile["Renderer"];
			int TargetAPI, TextureFiltering;
			float TextureQuality;
			json::get_int(RendererSettings[0], "TargetAPI", TargetAPI);
			json::get_float(RendererSettings[0], "TextureQuality", TextureQuality);
			json::get_int(RendererSettings[0], "TextureFiltering", TextureFiltering);
			UserGraphicsSettings.TargetRenderAPI = (Renderer::eTargetRenderAPI)TargetAPI;
			UserGraphicsSettings.MaxAnisotropy = TextureFiltering;
			UserGraphicsSettings.MipLodBias = TextureQuality;
		}

		return UserGraphicsSettings;
	}

	bool FileSystem::LoadSceneFromJson(const std::string& FileName, Scene* pScene)
	{
		// Load in Meta.json
		{
			Profiling::ScopedTimer timer("LoadSceneFromJson::LoadMetaData");

			rapidjson::Document rawMetaFile;
			const std::string metaDir = FileName + "/Meta.json";
			if (!json::load(metaDir.c_str(), rawMetaFile)) {
				IE_CORE_ERROR("Failed to load meta file from scene: \"{0}\" from file.", FileName);
				return false;
			}
			std::string sceneName;
			json::get_string(rawMetaFile, "SceneName", sceneName); // Find something that says 'SceneName' and load sceneName variable
			pScene->SetDisplayName(sceneName);
			Application::Get().GetWindow().SetWindowTitle(sceneName);

			IE_CORE_TRACE("Scene meta data loaded.");
		}

		// Load in Resources.json
		{
			Profiling::ScopedTimer timer("LoadSceneFromJson::LoadResources");

			rapidjson::Document rawResourceFile;
			const std::string resorurceDir = FileName + "/Resources.json";
			if (!json::load(resorurceDir.c_str(), rawResourceFile)) {
				IE_CORE_ERROR("Failed to load resource file from scene: \"{0}\" from file.", FileName);
				return false;
			}

			ResourceManager::Get().LoadResourcesFromJson(rawResourceFile);

			IE_CORE_TRACE("Scene resouces loaded.");
		}

		// Load in Actors.json last once resources have been intialized
		{
			Profiling::ScopedTimer timer("LoadSceneFromJson::LoadActors");

			rapidjson::Document rawActorsFile;
			const std::string actorsDir = FileName + "/Actors.json";
			if (!json::load(actorsDir.c_str(), rawActorsFile)) {
				IE_CORE_ERROR("Failed to load actor file from scene: \"{0}\" from file.", FileName);
				return false;
			}

			AActor* newActor = nullptr;
			UINT actorSceneIndex = 0;

			const rapidjson::Value& sceneObjects = rawActorsFile["Set"];
			for (rapidjson::SizeType a = 0; a < sceneObjects.Size(); a++)
			{
				const rapidjson::Value& jsonActor = sceneObjects[a];

				std::string actorDisplayName;
				std::string actorType;
				json::get_string(jsonActor, "DisplayName", actorDisplayName);
				json::get_string(jsonActor, "Type", actorType);

				if (actorType == "Actor") {
					newActor = new AActor(actorSceneIndex, actorDisplayName);
					newActor->LoadFromJson(jsonActor);
				}
				else if (actorType == "PointLight") {
					newActor = new APointLight(actorSceneIndex, actorDisplayName);
					newActor->LoadFromJson(jsonActor);
				}
				else if (actorType == "SpotLight") {
					newActor = new ASpotLight(actorSceneIndex, actorDisplayName);
					newActor->LoadFromJson(jsonActor);
				}
				else if (actorType == "DirectionalLight") {
					newActor = new ADirectionalLight(actorSceneIndex, actorDisplayName);
					newActor->LoadFromJson(jsonActor);
				}
				else if (actorType == "SkySphere") {
					newActor = new ASkySphere(actorSceneIndex, actorDisplayName);
					newActor->LoadFromJson(jsonActor);
				}
				else if (actorType == "SkyLight") {
					newActor = new ASkyLight(actorSceneIndex, actorDisplayName);
					newActor->LoadFromJson(jsonActor);
				}
				else if (actorType == "PostFxVolume") {
					newActor = new APostFx(actorSceneIndex, actorDisplayName);
					newActor->LoadFromJson(jsonActor);
				}

				if (newActor == nullptr) {
					IE_CORE_ERROR("Failed to parse actor \"{0}\" into scene", (actorDisplayName == "") ? "INVALID NAME" : actorDisplayName);
					continue;
				}

				pScene->GetRootNode()->AddChild(newActor);
				actorSceneIndex++;
			}

			IE_CORE_TRACE("Scene actors loaded.");
		}

		return true;
	}

	bool FileSystem::WriteSceneToJson(Scene* pScene)
	{
		// Save Out Meta.json
		{
			rapidjson::StringBuffer StrBuffer;
			rapidjson::PrettyWriter<rapidjson::StringBuffer> Writer(StrBuffer);

			Writer.StartObject();
			Writer.Key("SceneName");
			Writer.String(pScene->GetDisplayName().c_str());
			Writer.EndObject();

			// Final Export
			std::string sceneName = FileSystem::ProjectDirectory + "/Assets/Scenes/" + pScene->GetDisplayName() + ".iescene/Meta.json";
			std::ofstream offstream(sceneName.c_str());
			offstream << StrBuffer.GetString();

			if (!offstream.good()) {
				IE_CORE_ERROR("Failed to save meta data for scene: {0}", pScene->GetDisplayName());
				return false;
			}
		}

		// Save Out Actors.json
		{
			rapidjson::StringBuffer StrBuffer;
			rapidjson::PrettyWriter<rapidjson::StringBuffer> Writer(StrBuffer);

			pScene->WriteToJson(Writer);

			// Final Export
			std::string sceneName = FileSystem::ProjectDirectory + "/Assets/Scenes/" + pScene->GetDisplayName() + ".iescene/Actors.json";
			std::ofstream offstream(sceneName.c_str());
			offstream << StrBuffer.GetString();

			if (!offstream.good()) {
				IE_CORE_ERROR("Failed to save actors for scene: {0}", pScene->GetDisplayName());
				return false;
			}
		}

		return true;
	}

	bool FileSystem::FileExists(const std::string& Path)
	{
		std::string RawPath = ProjectDirectory + "Assets/" + Path;
		return PathFileExistsA(RawPath.c_str());
	}


}
