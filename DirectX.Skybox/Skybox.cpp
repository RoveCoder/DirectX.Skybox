#include "Skybox.h"
#include "GeometryGenerator.h"
#include "DDSTextureLoader.h"

_declspec(align(16)) struct SkyConstantBuffer
{
    DirectX::XMMATRIX mWorld;
    DirectX::XMMATRIX mView;
    DirectX::XMMATRIX mProjection;
};

Skybox::Skybox(Renderer* renderer, Camera* camera) : m_Renderer(renderer), m_Camera(camera)
{
}

bool Skybox::Load()
{
    Geometry::CreateSphere(1.0f, 8, 8, &m_MeshData);

    // Create vertex buffer
    D3D11_BUFFER_DESC vbd = {};
    vbd.Usage = D3D11_USAGE_DEFAULT;
    vbd.ByteWidth = (UINT)(sizeof(Vertex) * m_MeshData.vertices.size());
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA vInitData = {};
    vInitData.pSysMem = &m_MeshData.vertices[0];

    DX::ThrowIfFailed(m_Renderer->GetDevice()->CreateBuffer(&vbd, &vInitData, &m_VertexBuffer));

    // Create index buffer
    D3D11_BUFFER_DESC ibd = {};
    ibd.Usage = D3D11_USAGE_DEFAULT;
    ibd.ByteWidth = (UINT)(sizeof(UINT) * m_MeshData.indices.size());
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA iInitData = {};
    iInitData.pSysMem = &m_MeshData.indices[0];

    DX::ThrowIfFailed(m_Renderer->GetDevice()->CreateBuffer(&ibd, &iInitData, &m_IndexBuffer));

    // Create constant buffer
    D3D11_BUFFER_DESC skyboxbd = {};
    skyboxbd.Usage = D3D11_USAGE_DEFAULT;
    skyboxbd.ByteWidth = sizeof(SkyConstantBuffer);
    skyboxbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    skyboxbd.CPUAccessFlags = 0;
    DX::ThrowIfFailed(m_Renderer->GetDevice()->CreateBuffer(&skyboxbd, nullptr, &m_SkyboxConstantBuffer));

    // Load texture
    ID3D11Resource* resource = nullptr;
    DX::ThrowIfFailed(DirectX::CreateDDSTextureFromFile(m_Renderer->GetDevice(), L"Textures\\grass_cubemap.dds", &resource, &m_DiffuseTexture));
    resource->Release();

    // Set culling
    CreateRasterState();
    CreateDepthStencilState();

    return true;
}

void Skybox::Render()
{
    m_Renderer->GetDeviceContext()->RSSetState(m_RasterState);
    m_Renderer->GetDeviceContext()->OMSetDepthStencilState(m_DepthStencilState, 0);

    // Bind the vertex buffer
    UINT stride = sizeof(Vertex);
    UINT offset = 0;

    m_Renderer->GetDeviceContext()->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);

    // Bind the index buffer
    m_Renderer->GetDeviceContext()->IASetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // Set topology
    m_Renderer->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Set world buffer
    DirectX::XMFLOAT3 cameraPosition = m_Camera->GetPosition();

    DirectX::XMMATRIX world = DirectX::XMMatrixIdentity();
    world *= DirectX::XMMatrixTranslation(cameraPosition.x, cameraPosition.y, cameraPosition.z);

    SkyConstantBuffer cb;
    cb.mWorld = DirectX::XMMatrixTranspose(world);
    cb.mView = DirectX::XMMatrixTranspose(m_Camera->GetView());
    cb.mProjection = DirectX::XMMatrixTranspose(m_Camera->GetProjection());

    m_Renderer->GetDeviceContext()->VSSetConstantBuffers(0, 1, &m_SkyboxConstantBuffer);
    m_Renderer->GetDeviceContext()->PSSetConstantBuffers(0, 1, &m_SkyboxConstantBuffer);
    m_Renderer->GetDeviceContext()->UpdateSubresource(m_SkyboxConstantBuffer, 0, nullptr, &cb, 0, 0);

    m_Renderer->GetDeviceContext()->PSSetShaderResources(0, 1, &m_DiffuseTexture);

    // Render geometry
    m_Renderer->GetDeviceContext()->DrawIndexed((UINT)m_MeshData.indices.size(), 0, 0);
}

void Skybox::CreateRasterState()
{
    D3D11_RASTERIZER_DESC rasterizerState = {};
    rasterizerState.AntialiasedLineEnable = true;
    rasterizerState.CullMode = D3D11_CULL_BACK; ; // D3D11_CULL_FRONT or D3D11_CULL_NONE D3D11_CULL_BACK
    rasterizerState.FillMode = D3D11_FILL_SOLID;
    rasterizerState.DepthClipEnable = true;
    rasterizerState.FrontCounterClockwise = true;
    rasterizerState.MultisampleEnable = true;
    DX::ThrowIfFailed(m_Renderer->GetDevice()->CreateRasterizerState(&rasterizerState, &m_RasterState));
}

void Skybox::CreateDepthStencilState()
{
    D3D11_DEPTH_STENCIL_DESC dssDesc = {};
    dssDesc.DepthEnable = true;
    dssDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dssDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
    m_Renderer->GetDevice()->CreateDepthStencilState(&dssDesc, &m_DepthStencilState);
}