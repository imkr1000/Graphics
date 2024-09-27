#include "pch.h"
#define STB_IMAGE_IMPLEMENTATION
#include "Example.h"

#include <omp.h>

Example::Example(HWND window, UINT width, UINT height)
    : rayTracer(width, height)
{
    Initialize(window, width, height);

    // 시간 측정
    //const auto startTime = std::chrono::high_resolution_clock::now();
    //
    //
    //
    //const auto elapsedTime = std::chrono::high_resolution_clock::now() - startTime;
    //
    //std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(elapsedTime).count() / 1000.0 << " sec" << '\n';

}

// https://docs.microsoft.com/en-us/windows/win32/direct3d11/how-to--compile-a-shader
void Example::InitShaders()
{
    ComPtr<ID3DBlob> vsBlob;
    ComPtr<ID3DBlob> psBlob;

#ifdef _DEBUG
    ComPtr<ID3DBlob> errorBlob;

    if (FAILED(D3DCompileFromFile(L"_Shaders/VertexShader.hlsl", 0, 0, "main", "vs_5_0", 0, 0, &vsBlob, &errorBlob)))
    {
        if (errorBlob.Get())
        {
            const std::string& str = (const char*)errorBlob->GetBufferPointer();
            MessageBoxA(nullptr, str.c_str(), "Vertex Shader Compile Error!", MB_OK);
        }
        assert(false);
    }

    if (FAILED(D3DCompileFromFile(L"_Shaders/PixelShader.hlsl", 0, 0, "main", "ps_5_0", 0, 0, &psBlob, &errorBlob)))
    {
        if (errorBlob.Get())
        {
            const std::string& str = (const char*)errorBlob->GetBufferPointer();
            MessageBoxA(nullptr, str.c_str(), "Pixel Shader Compile Error!", MB_OK);
        }
        assert(false);
    }

#elif NDEBUG
    //https://learn.microsoft.com/ko-kr/windows/win32/direct3dhlsl/dx-graphics-hlsl-part1
    //https://raw.githubusercontent.com/wiki/microsoft/DirectXTK/ReadData.h
    
    DX::ReadData(L"_Shaders/VertexShader.cso", &vsBlob);
    
    DX::ReadData(L"_Shaders/PixelShader.cso", &psBlob);

#endif

    CHECK(device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &vertexShader));
    CHECK(device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &pixelShader));

    // Create the input layout object
    std::vector<D3D11_INPUT_ELEMENT_DESC> ieDescs =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    CHECK(device->CreateInputLayout(ieDescs.data(), UINT(ieDescs.size()), vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &inputLayout));
    deviceContext->IASetInputLayout(inputLayout.Get());
}

void Example::Initialize(HWND window, UINT width, UINT height)
{
    this->width = width;
    this->height = height;

    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
    swapChainDesc.BufferDesc.Width = width;                             // set the back buffer width
    swapChainDesc.BufferDesc.Height = height;                           // set the back buffer height
    swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
    swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;       // use 32-bit color
    swapChainDesc.SampleDesc.Count = 1;                                 // how many multisamples
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;        // how swap chain is to be used
    swapChainDesc.BufferCount = 2;                                      // one back buffer
    swapChainDesc.OutputWindow = window;                                // the window to be used
    swapChainDesc.Windowed = true;                                      // windowed/full-screen mode
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;       // allow full-screen switching

    UINT createDeviceFlags = 0;
    //createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;

    const std::vector<D3D_FEATURE_LEVEL> fetureLevelArray = { D3D_FEATURE_LEVEL_11_0 };

    CHECK(D3D11CreateDeviceAndSwapChain(nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        createDeviceFlags,
        fetureLevelArray.data(),
        UINT(fetureLevelArray.size()),
        D3D11_SDK_VERSION,
        &swapChainDesc,
        &swapChain,
        &device,
        nullptr,
        &deviceContext));

    // CreateRenderTarget
    ComPtr<ID3D11Texture2D> backBuffer;
    CHECK(swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer)));
    CHECK(device->CreateRenderTargetView(backBuffer.Get(), nullptr, &renderTargetView));
    backBuffer.Reset();

    // Set the viewport
    ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    viewport.Width = float(width);
    viewport.Height = float(height);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;           //Note: important for depth buffering
    deviceContext->RSSetViewports(1, &viewport);

    InitShaders();

    // Create the Sample State
    D3D11_SAMPLER_DESC samplerDesc;
    ZeroMemory(&samplerDesc, sizeof(samplerDesc));
    samplerDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT; // D3D11_FILTER_MIN_MAG_MIP_LINEAR
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    samplerDesc.BorderColor[0] = 1.0f;
    samplerDesc.BorderColor[1] = 0.0f;
    samplerDesc.BorderColor[2] = 0.0f;
    samplerDesc.BorderColor[3] = 1.0f;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    CHECK(device->CreateSamplerState(&samplerDesc, &colorSampler));
    
    // Create texture and rendertarget
    D3D11_TEXTURE2D_DESC textureDesc;
    ZeroMemory(&textureDesc, sizeof(textureDesc));
    textureDesc.Width = width;
    textureDesc.Height = height;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.Usage = D3D11_USAGE_DYNAMIC;
    textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    textureDesc.MiscFlags = 0;

    CHECK(device->CreateTexture2D(&textureDesc, nullptr, &canvasTexture));

    CHECK(device->CreateShaderResourceView(canvasTexture.Get(), nullptr, &canvasTextureView));

    // Create a vertex buffer
    {
        const std::vector<Vertex> vertices =
        {
            { { -1.0f, -1.0f, 0.0f, 1.0f }, { 0.f, 1.f }, },
            { {  1.0f, -1.0f, 0.0f, 1.0f }, { 1.f, 1.f }, },
            { {  1.0f,  1.0f, 0.0f, 1.0f }, { 1.f, 0.f }, },
            { { -1.0f,  1.0f, 0.0f, 1.0f }, { 0.f, 0.f }, },
        };

        D3D11_BUFFER_DESC bufferDesc;
        ZeroMemory(&bufferDesc, sizeof(bufferDesc));
        bufferDesc.ByteWidth = UINT(sizeof(vertices[0]) * vertices.size());             // size is the VERTEX struct * 3
        bufferDesc.Usage = D3D11_USAGE_DYNAMIC;                                         // write access access by CPU and GPU
        bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;                                // use as a vertex buffer
        bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;                             // allow CPU to write in buffer
        bufferDesc.StructureByteStride = sizeof(vertices[0]);

        D3D11_SUBRESOURCE_DATA vertexBufferData = { 0, };
        vertexBufferData.pSysMem = vertices.data();
        vertexBufferData.SysMemPitch = 0;
        vertexBufferData.SysMemSlicePitch = 0;

        CHECK(device->CreateBuffer(&bufferDesc, &vertexBufferData, &vertexBuffer));
    }

    // Create an index buffer
    {
        const std::vector<uint16_t> indices =
        {
            3,1,0,
            2,1,3,
        };

        indexCount = UINT(indices.size());

        D3D11_BUFFER_DESC bufferDesc = {};
        bufferDesc.ByteWidth = UINT(sizeof(indices[0]) * indices.size());
        bufferDesc.Usage = D3D11_USAGE_DYNAMIC;                 // write access access by CPU and GPU
        bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;         // use as a vertex buffer
        bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;     // allow CPU to write in buffer
        bufferDesc.StructureByteStride = sizeof(indices[0]);

        D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
        indexBufferData.pSysMem = indices.data();
        indexBufferData.SysMemPitch = 0;
        indexBufferData.SysMemSlicePitch = 0;

        CHECK(device->CreateBuffer(&bufferDesc, &indexBufferData, &indexBuffer));
    }
}

void Example::Update()
{
    // 화면을 배경 색으로 초기화
    std::vector<Color> pixels(width * height, backgroundColor);

    rayTracer.Render(pixels);

    // Update texture buffer
    D3D11_MAPPED_SUBRESOURCE mappedSubresource;
    deviceContext->Map(canvasTexture.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
    memcpy(mappedSubresource.pData, pixels.data(), pixels.size() * sizeof(pixels[0]));
    deviceContext->Unmap(canvasTexture.Get(), 0);
}

void Example::Render()
{
    float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
    deviceContext->RSSetViewports(1, &viewport);
    deviceContext->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), nullptr);
    deviceContext->ClearRenderTargetView(renderTargetView.Get(), clearColor);

    // set the shader objects
    deviceContext->VSSetShader(vertexShader.Get(), 0, 0);
    deviceContext->PSSetShader(pixelShader.Get(), 0, 0);

    // select which vertex buffer to display
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    deviceContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
    deviceContext->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

    // https://github.com/Microsoft/DirectXTK/wiki/Getting-Started
    // https://github.com/microsoft/Xbox-ATG-Samples/tree/main/PCSamples/IntroGraphics/SimpleTexturePC
    deviceContext->PSSetSamplers(0, 1, colorSampler.GetAddressOf());//TODO: samplers to array
    deviceContext->PSSetShaderResources(0, 1, canvasTextureView.GetAddressOf());
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    deviceContext->DrawIndexed(indexCount, 0, 0);
}
