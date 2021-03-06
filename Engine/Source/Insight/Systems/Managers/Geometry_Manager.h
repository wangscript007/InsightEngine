#pragma once

#include <Insight/Core.h>

#include "Insight/Rendering/Geometry/Model.h"

namespace Insight {

	using namespace Microsoft::WRL;

	class GeometryManager
	{
	public:
		typedef std::vector<StrongModelPtr> SceneModels;
		friend class D3D12GeometryManager;
		friend class D3D11GeometryManager;
	public:
		GeometryManager();
		virtual ~GeometryManager();

		static bool InitGlobalInstance();
		static void Shutdown();

		static GeometryManager& Get() { return *s_Instance; }
		SceneModels* GetSceneModels() { return &m_Models; }

		static bool Init() { return s_Instance->InitImpl(); }

		// Issue draw commands to all models attached to the geometry manager.
		static void Render(eRenderPass RenderPass) { s_Instance->RenderImpl(RenderPass); }
		// Gather all geometry in the scene and uplaod their constant buffers to the GPU.
		// Should only be called once, before 'Render()'. Does not draw models.
		static void GatherGeometry() { s_Instance->GatherGeometryImpl(); }
		// Reset incrementor for model geometry gather phase.
		// See 'GatherGeometry()' for more information.
		static void PostRender() { s_Instance->PostRenderImpl(); }
		// UnRegister all model in the model cache. Usually used 
		// when switching scenes.
		static void FlushModelCache();
		
		// Register a model to be drawn in the geometry pass
		static void RegisterModel(StrongModelPtr Model) { s_Instance->m_Models.push_back(Model); }
		// Unregister a model to not be drawn in the geometry pass
		static void UnRegisterModel(StrongModelPtr Model);

	protected:
		virtual bool InitImpl() = 0;
		virtual void RenderImpl(eRenderPass RenderPass) = 0;
		virtual void GatherGeometryImpl() = 0;
		virtual void PostRenderImpl() = 0;

	protected:
		SceneModels m_Models;  

	private:
		static GeometryManager* s_Instance;
	};

}
