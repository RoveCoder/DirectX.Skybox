#pragma once

#include "Renderer.h"
#include "Camera.h"
#include "Mesh.h"

class Floor
{
public:
	Floor(Renderer* renderer);

	bool Load();
	void Render(Camera* camera);

private:
	Renderer* m_Renderer = nullptr;

	MeshData m_MeshData;

	ID3D11Buffer* m_VertexBuffer = nullptr;
	ID3D11Buffer* m_IndexBuffer = nullptr;

	ID3D11ShaderResourceView* m_DiffuseTexture = nullptr;
};