#include "SkyboxShader.h"
#include <SDL_messagebox.h>
#include <fstream>

SkyboxShader::SkyboxShader(Renderer* renderer) : m_Renderer(renderer)
{
}

bool SkyboxShader::Create()
{
	if (!CreateVertexShader("SkyboxVertexShader.cso"))
		return false;

	if (!CreatePixelShader("SkyboxPixelShader.cso"))
		return false;

	return true;
}

void SkyboxShader::Use()
{
	m_Renderer->GetDeviceContext()->IASetInputLayout(m_VertexLayout);
	m_Renderer->GetDeviceContext()->VSSetShader(m_VertexShader, nullptr, 0);
	m_Renderer->GetDeviceContext()->PSSetShader(m_PixelShader, nullptr, 0);
}

bool SkyboxShader::CreateVertexShader(const std::string& vertex_shader_path)
{
	std::ifstream vertexFile(vertex_shader_path, std::fstream::in | std::fstream::binary);
	if (!vertexFile.is_open())
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Could not read SkyboxVertexShader.cso", nullptr);
		return false;
	}

	vertexFile.seekg(0, vertexFile.end);
	int vertexsize = (int)vertexFile.tellg();
	vertexFile.seekg(0, vertexFile.beg);

	char* vertexbuffer = new char[vertexsize];
	vertexFile.read(vertexbuffer, vertexsize);

	m_Renderer->GetDevice()->CreateVertexShader(vertexbuffer, vertexsize, nullptr, &m_VertexShader);

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	UINT numElements = ARRAYSIZE(layout);
	m_Renderer->GetDevice()->CreateInputLayout(layout, numElements, vertexbuffer, vertexsize, &m_VertexLayout);

	delete[] vertexbuffer;
	return true;
}

bool SkyboxShader::CreatePixelShader(const std::string& pixel_shader_path)
{
	std::ifstream pixelFile(pixel_shader_path, std::fstream::in | std::fstream::binary);
	if (!pixelFile.is_open())
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Could not read SkyboxPixelShader.cso", nullptr);
		return false;
	}

	pixelFile.seekg(0, pixelFile.end);
	int pixelsize = (int)pixelFile.tellg();
	pixelFile.seekg(0, pixelFile.beg);

	char* pixelbuffer = new char[pixelsize];
	pixelFile.read(pixelbuffer, pixelsize);

	m_Renderer->GetDevice()->CreatePixelShader(pixelbuffer, pixelsize, nullptr, &m_PixelShader);

	delete[] pixelbuffer;
	return true;
}