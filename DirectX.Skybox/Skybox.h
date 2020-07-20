#pragma once

#include <d3d11.h>
#include "Shader.h"
#include "Camera.h"
#include "Mesh.h"

class Skybox
{
public:
	Skybox(Renderer* renderer, Camera* camera);

	bool Load();
	void Render();

private:
	Renderer* m_Renderer = nullptr;
	Camera* m_Camera = nullptr;

	MeshData m_MeshData;

	// Direct3D
	ID3D11Buffer* m_VertexBuffer = nullptr;
	ID3D11Buffer* m_IndexBuffer = nullptr;
	ID3D11Buffer* m_SkyboxConstantBuffer = nullptr;

	ID3D11ShaderResourceView* m_CubeMapSRV = nullptr;
	ID3D11Buffer* m_ConstantBuffer = nullptr;

	ID3D11ShaderResourceView* m_DiffuseTexture = nullptr;

	ID3D11RasterizerState* m_RasterState = nullptr;
	ID3D11DepthStencilState* m_DepthStencilState = nullptr;

	void CreateRasterState();
	void CreateDepthStencilState();
};