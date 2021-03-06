#pragma once

#include <Insight/Core.h>

#include "Actor_Component.h"
#include "Insight/Rendering/Geometry/Model.h"

namespace Insight {

	class INSIGHT_API StaticMeshComponent : public ActorComponent
	{
	public:
		StaticMeshComponent(AActor* pOwner);
		virtual ~StaticMeshComponent();

		virtual bool LoadFromJson(const rapidjson::Value& jsonStaticMeshComponent) override;
		virtual bool WriteToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& Writer) override;

		virtual void OnInit() override;
		virtual void OnPostInit() {}
		virtual void OnDestroy() override;
		virtual void CalculateParent(const XMMATRIX& parentMatrix) override;
		virtual void OnRender() override;
		virtual void OnUpdate(const float& deltaTime);
		virtual void OnChanged() {}
		virtual void OnImGuiRender() override;

		void RenderSceneHeirarchy() override;
		void AttachMesh(const std::string& AssesDirectoryRelPath);
		void SetMaterial(Material* pMaterial);

		virtual void BeginPlay() override;
		virtual void Tick(const float& deltaMs) override;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

	private:
		std::string m_DynamicAssetDir;
		StrongModelPtr m_pModel;
		Material* m_pMaterial;
		std::future<bool> m_ModelLoadFuture;

		uint32_t m_SMWorldIndex = 0U;

	private:
		static uint32_t s_NumActiveSMComponents;
	};

}
