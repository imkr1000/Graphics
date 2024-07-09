#include "pch.h"
#include "Example.h"

#include <omp.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

// std::clamp (C++17)

//std::vector<std::mutex> mutexes(std::thread::hardware_concurrency());

void Image::ReadFromFile(const char* fileName)
{
    /*
    vcpkg install stb:x64-windows
    프로젝트 설정에서 _CRT_SECURE_NO_WARNINGS 추가 ('sprintf' in stb_image_write.h)
    #define STB_IMAGE_IMPLEMENTATION
    #include <stb_image.h>
    #define STB_IMAGE_WRITE_IMPLEMENTATION
    #include <stb_image_write.h>
    */

    unsigned char* img = stbi_load(fileName, &width, &height, &channels, 0);

    if (width)
        std::cout << width << " " << height << " " << channels << '\n';
    else
        std::cout << "Error: reading " << fileName << " failed." << '\n';

    // channels가 3(RGB) 또는 4(RGBA)인 경우만 가정
    // unsigned char(0에서 255)을 4채널 float(0.0f에서 1.0f)로 변환
    pixels.resize(width * height);
    for (int i = 0; i < width * height; ++i)
    {
        pixels[i].x = img[i * channels] / 255.0f;
        pixels[i].y = img[i * channels + 1] / 255.0f;
        pixels[i].z = img[i * channels + 2] / 255.0f;
        pixels[i].w = 1.0f;
    }

    SAFE_DELETE_ARRAY(img);
}

void Image::WritePNG(const char* fileName)
{
    // 32bit에서 8bit로 변환 후에 저장

    std::vector<unsigned char> img(width * height * channels, 0);
    for (int i = 0; i < width * height; ++i)
    {
        img[i * channels] = uint8_t(pixels[i].x * 255.0f); // v[0]이 0.0f 이상 1.0f 이하 가정
        img[i * channels + 1] = uint8_t(pixels[i].y * 255.0f);
        img[i * channels + 2] = uint8_t(pixels[i].z * 255.0f);
    }

    stbi_write_png(fileName, width, height, channels, img.data(), width * channels);
}

Color& Image::GetPixel(int x, int y)
{
    x = std::clamp(x, 0, width - 1);
    y = std::clamp(y, 0, height - 1);
    
    return pixels[x + width * y];
}

void Image::BoxBlur5()
{
    std::vector<Color> pixelsBuffer(pixels.size()); // 사본 복사

    /*
    * Separable convolution
    * 한 번에 2차원 Kernel을 적용하는 대신에 1차원 Kernel을 두 번 적용
    * 이해하기 쉽고 효율적이다.
    */

    // 가로 방향 (x 방향)
#pragma omp parallel for
    for (int j = 0; j < height; ++j)
    {
        for (int i = 0; i < width; ++i)
        {
            const int idx = width * j + i;

            float r = 0.0f, g = 0.0f, b = 0.0f;

            for (int k = 0; k < 5; ++k)
            {
                Color neighborColor = GetPixel(i + k - 2, j);

                r += neighborColor.x;
                g += neighborColor.y;
                b += neighborColor.z;
            }

            // 주변 픽셀들의 색을 평균내어서 (i, j)에 있는 픽셀의 색을 변경
            // this->pixels로부터 읽어온 값들을 평균내어서 pixelsBuffer의 값들을 바꾸기
            pixelsBuffer[idx].x = r * 0.2f;
            pixelsBuffer[idx].y = g * 0.2f;
            pixelsBuffer[idx].z = b * 0.2f;
        }
    }

    // Swap
    std::swap(this->pixels, pixelsBuffer);

    //return; // 여기까지 구현하고 테스트

    // 세로 방향 (y 방향)
#pragma omp parallel for
    for (int j = 0; j < height; j++)
    {
        for (int i = 0; i < width; i++)
        {
            const int idx = width * j + i;

            float r = 0.0f, g = 0.0f, b = 0.0f;

            for (int k = 0; k < 5; ++k)
            {
                Color neighborColor = GetPixel(i, j + k - 2);

                r += neighborColor.x;
                g += neighborColor.y;
                b += neighborColor.z;
            }

            // 주변 픽셀들의 색을 평균내어서 (i, j)에 있는 픽셀의 색을 변경
            // this->pixels로부터 읽어온 값들을 평균내어서 pixelsBuffer의 값들을 바꾸기
            pixelsBuffer[idx].x = r * 0.2f;
            pixelsBuffer[idx].y = g * 0.2f;
            pixelsBuffer[idx].z = b * 0.2f;
        }
    }

    // Swap
    std::swap(this->pixels, pixelsBuffer);
}

void Image::GaussianBlur5()
{
    std::vector<Color> pixelsBuffer(this->pixels.size());

    /*
    * 참고자료
    * https://en.wikipedia.org/wiki/Gaussian_filter
    * https://followtutorials.com/2013/03/gaussian-blurring-using-separable-kernel-in-c.html
    */
    const float weights[5] = { 0.0545f, 0.2442f, 0.4026f, 0.2442f, 0.0545f };

    // 가로 방향 (x 방향)
#pragma omp parallel for
    for (int j = 0; j < this->height; j++)
    {
        for (int i = 0; i < this->width; i++)
        {
            const int idx = width * j + i;

            float r = 0.0f, g = 0.0f, b = 0.0f;

            for (int k = 0; k < 5; ++k)
            {
                Color neighborColor = GetPixel(i + k - 2, j);

                r += neighborColor.x * weights[k];
                g += neighborColor.y * weights[k];
                b += neighborColor.z * weights[k];
            }

            // 주변 픽셀들의 색을 평균내어서 (i, j)에 있는 픽셀의 색을 변경
            // this->pixels로부터 읽어온 값들을 평균내어서 pixelsBuffer의 값들을 바꾸기
            pixelsBuffer[idx].x = r;
            pixelsBuffer[idx].y = g;
            pixelsBuffer[idx].z = b;
        }
    }

    // Swap
    std::swap(this->pixels, pixelsBuffer);

    // 세로 방향 (y 방향)
#pragma omp parallel for
    for (int j = 0; j < this->height; j++)
    {
        for (int i = 0; i < this->width; i++)
        {
            const int idx = width * j + i;

            float r = 0.0f, g = 0.0f, b = 0.0f;

            for (int k = 0; k < 5; ++k)
            {
                Color neighborColor = GetPixel(i, j + k - 2);

                r += neighborColor.x * weights[k];
                g += neighborColor.y * weights[k];
                b += neighborColor.z * weights[k];
            }

            // 주변 픽셀들의 색을 평균내어서 (i, j)에 있는 픽셀의 색을 변경
            // this->pixels로부터 읽어온 값들을 평균내어서 pixelsBuffer의 값들을 바꾸기
            pixelsBuffer[idx].x = r;
            pixelsBuffer[idx].y = g;
            pixelsBuffer[idx].z = b;
        }
    }

    // Swap
    std::swap(this->pixels, pixelsBuffer);
}

void Image::Bloom(float theta, int numRepeat, float weight)
{
    //std::unique_lock<std::mutex> lock1(mutexes[0], std::defer_lock);
    //std::unique_lock<std::mutex> lock2(mutexes[1], std::defer_lock);
    //std::unique_lock<std::mutex> lock3(mutexes[2], std::defer_lock);
    //std::unique_lock<std::mutex> lock4(mutexes[3], std::defer_lock);
    ////std::vector<std::unique_lock<std::mutex>> locks(mutexes.size());
    //
    ////for (size_t i = 0; i < locks.size(); ++i)
    ////    locks[i] = std::unique_lock<std::mutex>(mutexes[i], std::defer_lock);
    //
    //std::lock(lock1, lock2, lock3, lock4);
    //https://learnopengl.com/Advanced-Lighting/Bloom

    const std::vector<Color> pixelsBackup = pixels;// 메모리 내용물까지 모두 복사

    /* Brightness가 th 보다 작은 픽셀들을 모두 검은색으로 바꾸기
    * https://en.wikipedia.org/wiki/Relative_luminance
    * Relative Luminance Y = 0.2126*R + 0.7152*G + 0.0722*B
    */
    for (int j = 0; j < height; j++)
        for (int i = 0; i < width; i++)
        {
            Color& pixel = GetPixel(i, j);

            const float relativeLuminance = pixel.x * 0.2126f + pixel.y * 0.7152f + pixel.z * 0.0722f;

            if (relativeLuminance < theta)
            {
                pixel = Color();
                pixel.w = 1.0f;
            }
        }

    // 여기서 Blur하지 않고 결과 확인

    // 밝은 부분만 Blur 
    for (int i = 0; i < numRepeat; i++)
    {
        GaussianBlur5();
    }

    // 여기서 또 한 번 결과 확인

    // 밝은 부분만 Blur한 것과 원본 이미지를 더하기 (밝은 부분 Blur에 weight 곱해서 강도 조절)
    for (int i = 0; i < pixelsBackup.size(); i++)
    {
        pixels[i].x = std::clamp(pixels[i].x * weight + pixelsBackup[i].x, 0.0f, 1.0f);
        pixels[i].y = std::clamp(pixels[i].y * weight + pixelsBackup[i].y, 0.0f, 1.0f);
        pixels[i].z = std::clamp(pixels[i].z * weight + pixelsBackup[i].z, 0.0f, 1.0f);
    }
}

Example::Example(HWND window, UINT width, UINT height)
{
    // 이미지 읽어들이기
    image.ReadFromFile("_Textures/CityImage.jpg");

    // 시간 측정
    const auto startTime = std::chrono::high_resolution_clock::now();

    auto& pixels = image.GetPixels();

    // TODO: 밝기의 한계를 제거해보자(픽셀 색상 값 중 아예 0인 값이 있어서 밝아질 때 잔상처럼 남는다)
    for (int i = 0; i < image.GetWidth() * image.GetHeight(); ++i)
    {
        pixels[i].x += 1e-2f;
        pixels[i].y += 1e-2f;
        pixels[i].z += 1e-2f;
    }

    /*
    * 이미지 컨벌루션 참고 자료들
    * https://en.wikipedia.org/wiki/Kernel_(image_processing) // 마지막 괄호 주의
    * https://en.wikipedia.org/wiki/Convolution
    * https://medium.com/@bdhuma/6-basic-things-to-know-about-convolution-daef5e1bc411
    * https://towardsdatascience.com/intuitively-understanding-convolutions-for-deep-learning-1f6f42faee1
    */

    /*
    * 여기서 사용하는 방법은 이해하기 더 쉬운 Separable convolution 입니다.
    */

    //for(int i = 0; i < 100; i++)
    //	image.BoxBlur5();

    //for (int i = 0; i < 100; i++)
    //	image.GaussianBlur5();

    /*std::vector<std::thread> threads;
    threads.reserve(std::thread::hardware_concurrency());

    size_t numReapeatPerThread = 100 / threads.capacity();

    for (size_t i = 0; i < threads.capacity(); ++i)
        threads.emplace_back(std::thread(&Image::Bloom, &image, 0.3f, (int)numReapeatPerThread, 1.0f));

    for (auto& thread : threads)
        thread.join();*/

    image.Bloom(0.3f, 100, 1.0f);

    const auto elapsedTime = std::chrono::high_resolution_clock::now() - startTime;

    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(elapsedTime).count() / 1000.0 << " sec" << '\n';

    image.WritePNG("result.png");

    Initialize(window, width, height, image.GetWidth(), image.GetHeight());
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

void Example::Initialize(HWND window, UINT width, UINT height, UINT canvasWidth, UINT canvasHeight)
{
    this->canvasWidth = canvasWidth;
    this->canvasHeight = canvasHeight;

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
    textureDesc.Width = canvasWidth;
    textureDesc.Height = canvasHeight;
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
            { { -1.0f, -1.0f, 0.0f, 1.0f }, { 0.f, 1.f },},
            { {  1.0f, -1.0f, 0.0f, 1.0f }, { 1.f, 1.f },},
            { {  1.0f,  1.0f, 0.0f, 1.0f }, { 1.f, 0.f },},
            { { -1.0f,  1.0f, 0.0f, 1.0f }, { 0.f, 0.f },},
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
    auto& pixels = image.GetPixels();

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
