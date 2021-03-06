#pragma once

#include <Insight/Core.h>

#include "Insight/Rendering/Texture.h"
#include "Platform/Windows/DirectX_Shared/Constant_Buffer_Types.h"

namespace Insight {

	class INSIGHT_API Material
	{
	public:
		Material();
		Material(Material&& material) noexcept;
		~Material();
		
		void Destroy();

		static Material* CreateDefaultTexturedMaterial();
		bool LoadFromJson(const rapidjson::Value& jsonMaterial);
		bool WriteToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& Writer);

		CB_PS_VS_PerObjectAdditives GetMaterialOverrideConstantBuffer() { return m_ShaderCB; }

		void OnImGuiRender();
		
		void BindResources();

	private:

		StrongTexturePtr m_AlbedoMap;
		StrongTexturePtr m_NormalMap;
		StrongTexturePtr m_MetallicMap;
		StrongTexturePtr m_RoughnessMap;
		StrongTexturePtr m_AOMap;

		int m_AlbedoTextureManagerID;
		int m_NormalTextureManagerID;
		int m_MetallicTextureManagerID;
		int m_RoughnessTextureManagerID;
		int m_AoTextureManagerID;

		float m_RoughnessAdditive = 0.5f;
		float m_MetallicAdditive = 0.5f;
		Math::ieVector2 m_UVOffset;
		Math::ieVector2 m_Tiling;
		Math::ieVector3 m_ColorAdditive;

		CB_PS_VS_PerObjectAdditives m_ShaderCB;
	};

}