#pragma once
#include "GameObject3D.h"


class Camera3D : public GameObject3D
{
public:
	Camera3D();
	void SetProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ);

	const XMMATRIX & GetViewMatrix() const;
	const XMMATRIX & GetProjectionMatrix() const;


	float GetNearZ() const { return m_nearZ; }
	float GetFarZ() const { return m_farZ; }

	float* GetViewMatAsFloatArr();
	float* GetProjMatAsFloatArr();

private:
	void UpdateMatrix() override;

	XMMATRIX viewMatrix;
	XMMATRIX projectionMatrix;

	float* viewFloatMat;
	float* projFloatMat;

	float m_nearZ = 0.0f;
	float m_farZ = 0.0f;

};