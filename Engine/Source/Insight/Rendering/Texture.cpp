#include <ie_pch.h>

#include "Texture.h"

#include "Insight/Core/Application.h"
#include "Insight/Utilities/String_Helper.h"
#include "Insight/Rendering/Renderer.h"

#include "Platform/Windows/DirectX_12/Direct3D12_Context.h"

#define CBVSRV_HEAP_TEXTURE_START 7

namespace Insight {


	Texture::Texture(IE_TEXTURE_INFO CreateInfo)
		: m_TextureInfo(CreateInfo)
	{
	}

	Texture::~Texture()
	{
	}
}