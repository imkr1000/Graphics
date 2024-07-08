#pragma once

struct Vertex
{
    Vector4 pos;
    Vector2 uv;
};

class Image
{
public:
    void ReadFromFile(const char* fileName);
    void WritePNG(const char* fileName);
    Vector4& GetPixel(int x, int y);

public:
    int GetWidth() const { return width; }
    int GetHeight() const { return height; }
    std::vector<Vector4>& GetPixels() { return pixels; }

private:
    int width = 0, height = 0, channels = 0;
    std::vector<Vector4> pixels; // �̹��� ó���� ���� ���� float�� �����ϴ� ���� �� ����
};

class Example
{
public:
    Example(HWND window, UINT width, UINT height);

    void InitShaders();

    void Initialize(HWND window, UINT width, UINT height, UINT canvasWidth, UINT canvasHeight);

public:
    void Update();
    void Render();

public:
    IDXGISwapChain* GetSwapChain() const { return swapChain.Get(); }
    ID3D11Device* GetDevice() const { return device.Get(); }
    ID3D11DeviceContext* GetDeviceContext() const { return deviceContext.Get(); }
    
    Vector4 GetBackgroundColor() const { return backgroundColor; }
    Vector4& GetBackgroundColor() { return backgroundColor; }

private:
    ComPtr<ID3D11Device> device;
    ComPtr<ID3D11DeviceContext> deviceContext;
    ComPtr<IDXGISwapChain> swapChain;
    D3D11_VIEWPORT viewport = { 0 };
    ComPtr<ID3D11RenderTargetView> renderTargetView;
    ComPtr<ID3D11VertexShader> vertexShader;
    ComPtr<ID3D11PixelShader> pixelShader;
    ComPtr<ID3D11InputLayout> inputLayout;

    ComPtr<ID3D11Buffer> vertexBuffer;
    ComPtr<ID3D11Buffer> indexBuffer;
    ComPtr<ID3D11Texture2D> canvasTexture;
    ComPtr<ID3D11ShaderResourceView> canvasTextureView;
    ComPtr<ID3D11SamplerState> colorSampler;

    UINT indexCount = 0;

    UINT canvasWidth = 0, canvasHeight = 0;
    Vector4 backgroundColor = { 0.8f, 0.8f, 0.8f, 1.0f };
    // SeaGreen Color Test
    //Vector4 backgroundColor = Vector4(46.0f, 139.0f, 87.0f, 255.0f) / 255.0f;

    Image image;
};