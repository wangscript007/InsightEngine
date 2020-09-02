#pragma once

#include <Insight/Core.h>

#include "Actor_Component.h"

namespace Insight {

	class INSIGHT_API InputComponent : public ActorComponent
	{
	public:
		struct EventData
		{
			EventCallbackFn EventCallback;
		};
	public:
		InputComponent(AActor* pOwner);
		virtual ~InputComponent();

		virtual bool LoadFromJson(const rapidjson::Value& jsonStaticMeshComponent) override;
		virtual bool WriteToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& Writer) override;

		virtual inline void SetEventCallback(const EventCallbackFn& callback) override { m_EventData.EventCallback = callback; }
		void OnEvent(Event& e);

		virtual void OnInit() override;
		virtual void OnPostInit() {}
		virtual void OnDestroy() override;
		virtual void OnRender() override;
		virtual void OnUpdate(const float& deltaTime);
		virtual void OnChanged() {}
		virtual void OnImGuiRender() override;

		virtual void BeginPlay() override;
		virtual void Tick(const float DeltaMs) override;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

	private:
		EventData m_EventData;
		
	};

}
