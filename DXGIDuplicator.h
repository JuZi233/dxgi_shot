#pragma once
#include <d3d11.h>
#include <dxgi1_2.h>
#include <string>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

#ifdef DXGX_EXPORTS
#define DXGX_API extern"C" _declspec(dllexport)
#else
#define DXGX_API __declspec(dllimport)
#endif

class DXGIDuplicator
{
public:
    DXGIDuplicator();
    ~DXGIDuplicator();

    bool InitD3D11Device();

    bool InitDuplication();

    bool GetDesktopFrame(ID3D11Texture2D** texture);

    bool GetCustomDesktopFrame(UINT x, UINT y, UINT width, UINT heigh);

    BYTE* GetImageData(D3D11_MAPPED_SUBRESOURCE& mappedResource, UINT x, UINT y, UINT width, UINT heigh, BYTE* buffer);

    //friend bool GetCustomDesktopFrame(DXGIDuplicator* duplicator, int x, int y, int w, int h);

    // 友元函数，在main函数里面会用到，需要访问其私有成员
    //friend void SaveDesktopImage(std::string filename, ID3D11Texture2D* texture, DXGIDuplicator* duplicator);
private:
    ID3D11Texture2D* m_texture2D = nullptr;
    ID3D11Device* device_ = nullptr;
    ID3D11DeviceContext* deviceContext_ = nullptr;
    IDXGIOutputDuplication* duplication_ = nullptr;
};


DXGX_API DXGIDuplicator* create() {
    return new DXGIDuplicator();
}

DXGX_API bool init(DXGIDuplicator* obj) {
    bool success;
    success = obj->InitD3D11Device();
    success = obj->InitDuplication();
    return success;
}

DXGX_API void* shot(DXGIDuplicator* obj, UINT x, UINT y, UINT width, UINT heigh, BYTE* buffer) {
    static D3D11_MAPPED_SUBRESOURCE mappedResource;
    //obj->GetImageData(mappedResource, x, y, width, heigh,buffer);
    //return mappedResource.pData;
    return obj->GetImageData(mappedResource, x, y, width, heigh, buffer);
}
DXGX_API void destroy(DXGIDuplicator* obj) {
    delete obj;
}
