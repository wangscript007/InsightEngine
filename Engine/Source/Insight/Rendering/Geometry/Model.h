#pragma once

#include "Mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "Insight/Core/Scene/Scene_Node.h"
#include "Insight/Rendering/Geometry/Mesh_Node.h"

namespace Insight {

	class INSIGHT_API Model : public SceneNode
	{
	public:
		typedef std::stack<XMMATRIX> InstanceMatrixStack;
	public:
		Model(const std::string& path, Material* material);
		Model() {}
		~Model();

		bool Init(const std::string& path);
		void OnImGuiRender();
		void RenderSceneHeirarchy();
		void BindResources();

		Material& GetMaterialRef() { return *m_Material; }
		std::string GetDirectory() { return m_Directory; }
		std::string GetAssetDirectoryRelativePath() { return m_AssetDirectoryRelativePath; }

		unique_ptr<Mesh>& GetMeshAtIndex(const int& index) { return m_Meshes[index]; }
		const size_t GetNumChildMeshes() const { return m_Meshes.size(); }

		void PreRender(const XMMATRIX& parentMat);
		void Render(ID3D12GraphicsCommandList* pCommandList);
		void Destroy();
		bool LoadModelFromFile(const std::string& path);
	private:
		unique_ptr<MeshNode> ParseNode_r(aiNode* pNode);
		unique_ptr<Mesh> ProcessMesh(aiMesh* pMesh, const aiScene* pScene);
	private:
		std::vector<unique_ptr<Mesh>> m_Meshes;
		unique_ptr<MeshNode> m_pRoot;
		
		Material* m_Material;

		std::string m_AssetDirectoryRelativePath;
		std::string m_Directory;
		std::string m_FileName;
	};

}

