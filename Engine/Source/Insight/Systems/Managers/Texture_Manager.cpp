#include <ie_pch.h>

#include "Texture_Manager.h"
#include "Insight/Systems/File_System.h"
#include "Insight/Utilities/String_Helper.h"
#include "Insight/Rendering/Renderer.h"

#include "Platform/Windows/DirectX_12/Direct3D12_Context.h"
#include "Platform/Windows/DirectX_12/ie_D3D12_Texture.h"
#include "Platform/Windows/DirectX_11/ie_D3D11_Texture.h"

namespace Insight {
	
	TextureManager::TextureManager()
	{
	}
	
	TextureManager::~TextureManager()
	{
		Destroy();
	}

	void TextureManager::Destroy()
	{
	}

	void TextureManager::FlushTextureCache()
	{
		for (StrongTexturePtr& tex : m_AlbedoTextures) {
			tex.reset();
		}
		for (StrongTexturePtr& tex : m_NormalTextures) {
			tex.reset();
		}
		for (StrongTexturePtr& tex : m_MetallicTextures) {
			tex.reset();
		}
		for (StrongTexturePtr& tex : m_RoughnessTextures) {
			tex.reset();
		}
		for (StrongTexturePtr& tex : m_AOTextures) {
			tex.reset();
		}
	}

	bool TextureManager::Init()
	{
		LoadDefaultTextures();
		return true;
	}

	bool TextureManager::LoadResourcesFromJson(const rapidjson::Value& JsonTextures)
	{
		for (rapidjson::SizeType i = 0; i < JsonTextures.Size(); i++) {
			std::string Name, Filepath;
			int Type, ID;
			bool GenMipMaps;
			json::get_int(JsonTextures[i], "ID", ID);
			json::get_int(JsonTextures[i], "Type", Type);
			json::get_string(JsonTextures[i], "Name", Name);
			json::get_string(JsonTextures[i], "Filepath", Filepath);
			json::get_bool(JsonTextures[i], "GenerateMipMaps", GenMipMaps);

			Texture::IE_TEXTURE_INFO TexInfo = {};
			TexInfo.DisplayName = Name;
			TexInfo.Id = ID;
			TexInfo.Filepath = StringHelper::StringToWide(FileSystem::GetProjectRelativeAssetDirectory(Filepath));
			TexInfo.GenerateMipMaps = GenMipMaps;
			TexInfo.Type = (Texture::eTextureType)Type;
			
			RegisterTextureByType(TexInfo);

			m_HighestTextureId = ((int)m_HighestTextureId < ID) ? ID : m_HighestTextureId;
		}

		return true;
	}

	StrongTexturePtr TextureManager::GetTextureByID(Texture::ID textureID, Texture::eTextureType textreType)
	{
		switch (textreType) {
		case Texture::eTextureType::ALBEDO:
		{
			for (UINT i = 0; i < m_AlbedoTextures.size(); i++) {
				if (textureID == m_AlbedoTextures[i]->GetTextureInfo().Id) {
					return m_AlbedoTextures[i];
				}
			}
			break;
		}
		case Texture::eTextureType::NORMAL:
		{
			for (UINT i = 0; i < m_NormalTextures.size(); i++) {

				if (textureID == m_NormalTextures[i]->GetTextureInfo().Id) {
					return m_NormalTextures[i];
				}
			}
			break;
		}
		case Texture::eTextureType::ROUGHNESS:
		{
			for (UINT i = 0; i < m_RoughnessTextures.size(); i++) {

				if (textureID == m_RoughnessTextures[i]->GetTextureInfo().Id) {
					return m_RoughnessTextures[i];
				}
			}
			break;
		}
		case Texture::eTextureType::METALLIC:
		{
			for (UINT i = 0; i < m_MetallicTextures.size(); i++) {

				if (textureID == m_MetallicTextures[i]->GetTextureInfo().Id) {
					return m_MetallicTextures[i];
				}
			}
			break;
		}
		case Texture::eTextureType::AO:
		{
			for (UINT i = 0; i < m_AOTextures.size(); i++) {

				if (textureID == m_AOTextures[i]->GetTextureInfo().Id) {
					return m_AOTextures[i];
				}
			}
			break;
		}
		default:
		{
			IE_CORE_WARN("Failed to get texture handle for texture with ID: {0}", textureID);
			break;
		}
		}

		return nullptr;
	}
	
	bool TextureManager::LoadDefaultTextures()
	{
		Texture::IE_TEXTURE_INFO TexInfo = {};
		TexInfo.Id = -1;
		TexInfo.GenerateMipMaps = true;

		// Albedo
		TexInfo.DisplayName = "Default_Albedo";
		TexInfo.Type = Texture::eTextureType::ALBEDO;
		TexInfo.Filepath = StringHelper::StringToWide("Assets/Textures/Default_Object/Default_Albedo.png");
		// Normal
		TexInfo.DisplayName = "Default_Normal";
		TexInfo.Type = Texture::eTextureType::NORMAL;
		TexInfo.Filepath = StringHelper::StringToWide("Assets/Textures/Default_Object/Default_Normal.png");
		// Metallic
		TexInfo.DisplayName = "Default_Metallic";
		TexInfo.Type = Texture::eTextureType::METALLIC;
		TexInfo.Filepath = StringHelper::StringToWide("Assets/Textures/Default_Object/Default_Metallic.png");
		// Roughness
		TexInfo.DisplayName = "Default_Roughness";
		TexInfo.Type = Texture::eTextureType::ROUGHNESS;
		TexInfo.Filepath = StringHelper::StringToWide("Assets/Textures/Default_Object/Default_RoughAO.png");
		// AO
		TexInfo.DisplayName = "Default_AO";
		TexInfo.Type = Texture::eTextureType::AO;
		TexInfo.Filepath = StringHelper::StringToWide("Assets/Textures/Default_Object/Default_RoughAO.png");

		switch (Renderer::GetAPI())
		{
		case Renderer::eTargetRenderAPI::D3D_11:
		{
			m_DefaultAlbedoTexture = make_shared<ieD3D11Texture>(TexInfo);
			m_DefaultNormalTexture = make_shared<ieD3D11Texture>(TexInfo);
			m_DefaultMetallicTexture = make_shared<ieD3D11Texture>(TexInfo);
			m_DefaultRoughnessTexture = make_shared<ieD3D11Texture>(TexInfo);
			m_DefaultAOTexture = make_shared<ieD3D11Texture>(TexInfo);
			break;
		}
		case Renderer::eTargetRenderAPI::D3D_12:
		{
			Direct3D12Context* graphicsContext = reinterpret_cast<Direct3D12Context*>(&Renderer::Get());
			CDescriptorHeapWrapper& cbvSrvHeapStart = graphicsContext->GetCBVSRVDescriptorHeap();

			m_DefaultAlbedoTexture = make_shared<ieD3D12Texture>(TexInfo, cbvSrvHeapStart);
			m_DefaultNormalTexture = make_shared<ieD3D12Texture>(TexInfo, cbvSrvHeapStart);
			m_DefaultMetallicTexture = make_shared<ieD3D12Texture>(TexInfo, cbvSrvHeapStart);
			m_DefaultRoughnessTexture = make_shared<ieD3D12Texture>(TexInfo, cbvSrvHeapStart);
			m_DefaultAOTexture = make_shared<ieD3D12Texture>(TexInfo, cbvSrvHeapStart);
			break;
		}
		default:
		{
			IE_CORE_ERROR("Failed to load default textures for api: {0}", Renderer::GetAPI());
			break;
		}
		}
		return true;
	}

	void TextureManager::RegisterTextureByType(const Texture::IE_TEXTURE_INFO& texInfo)
	{

		switch (Renderer::GetAPI())
		{
		case Renderer::eTargetRenderAPI::D3D_11:
		{
			switch (texInfo.Type) {
			case Texture::eTextureType::ALBEDO:
			{
				m_AlbedoTextures.push_back(make_shared<ieD3D11Texture>(texInfo));
				break;
			}
			case Texture::eTextureType::NORMAL:
			{
				m_NormalTextures.push_back(make_shared<ieD3D11Texture>(texInfo));
				break;
			}
			case Texture::eTextureType::ROUGHNESS:
			{
				m_RoughnessTextures.push_back(make_shared<ieD3D11Texture>(texInfo));
				break;
			}
			case Texture::eTextureType::METALLIC:
			{
				m_MetallicTextures.push_back(make_shared<ieD3D11Texture>(texInfo));
				break;
			}
			case Texture::eTextureType::AO:
			{
				m_AOTextures.push_back(make_shared<ieD3D11Texture>(texInfo));
				break;
			}
			default:
			{
				IE_CORE_WARN("Failed to identify texture to create with name of {0} - ID({1})", texInfo.DisplayName, texInfo.Id);
				break;
			}
			}
			break;
		}
		case Renderer::eTargetRenderAPI::D3D_12:
		{
			Direct3D12Context* GraphicsContext = reinterpret_cast<Direct3D12Context*>(&Renderer::Get());
			CDescriptorHeapWrapper& cbvSrvHeapStart = GraphicsContext->GetCBVSRVDescriptorHeap();

			switch (texInfo.Type) {
			case Texture::eTextureType::ALBEDO:
			{
				m_AlbedoTextures.push_back(make_shared<ieD3D12Texture>(texInfo, cbvSrvHeapStart));
				break;
			}
			case Texture::eTextureType::NORMAL:
			{
				m_NormalTextures.push_back(make_shared<ieD3D12Texture>(texInfo, cbvSrvHeapStart));
				break;
			}
			case Texture::eTextureType::ROUGHNESS:
			{
				m_RoughnessTextures.push_back(make_shared<ieD3D12Texture>(texInfo, cbvSrvHeapStart));
				break;
			}
			case Texture::eTextureType::METALLIC:
			{
				m_MetallicTextures.push_back(make_shared<ieD3D12Texture>(texInfo, cbvSrvHeapStart));
				break;
			}
			case Texture::eTextureType::AO:
			{
				m_AOTextures.push_back(make_shared<ieD3D12Texture>(texInfo, cbvSrvHeapStart));
				break;
			}
			default:
			{
				IE_CORE_WARN("Failed to identify texture to create with name of {0} - ID({1})", texInfo.DisplayName, texInfo.Id);
				break;
			}
			}
			break;
		}
		default:
		{
			IE_CORE_ERROR("Failed to determine graphics api to initialize texture. The renderer may not have been initialized yet.");
			break;
		}
		}

		
	}
}
