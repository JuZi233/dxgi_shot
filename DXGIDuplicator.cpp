#include "pch.h"
#include <d3d11.h>
#include <dxgi1_2.h>
#include "chrono"
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

#include "DXGIDuplicator.h"

// 构造函数，里面什么也不做
DXGIDuplicator::DXGIDuplicator()
{
}
// 析构函数，释放相关对象
DXGIDuplicator::~DXGIDuplicator()
{
    if (duplication_)
    {
        duplication_->Release();
    }
    if (device_)
    {
        device_->Release();
    }
    if (deviceContext_)
    {
        deviceContext_->Release();
    }
    if (m_texture2D)
    {
        m_texture2D->Release();
    }
}
bool DXGIDuplicator::InitD3D11Device()
{
    D3D_DRIVER_TYPE DriverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };
    UINT NumDriverTypes = ARRAYSIZE(DriverTypes);

    D3D_FEATURE_LEVEL FeatureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_1
    };
    UINT NumFeatureLevels = ARRAYSIZE(FeatureLevels);
    D3D_FEATURE_LEVEL FeatureLevel;

    for (UINT DriverTypeIndex = 0; DriverTypeIndex < NumDriverTypes; ++DriverTypeIndex)
    {
        HRESULT hr = D3D11CreateDevice(
            nullptr,
            DriverTypes[DriverTypeIndex],
            nullptr, 0,
            FeatureLevels,
            NumFeatureLevels,
            D3D11_SDK_VERSION,
            &device_,
            &FeatureLevel,
            &deviceContext_);
        if (SUCCEEDED(hr))
        {
            break;
        }
    }

    if (device_ == nullptr || deviceContext_ == nullptr)
    {
        return false;
    }

    return true;
}
//实现函数InitDuplication。这里又分为好几个步骤，如下：获取IDXGIDevice对象,获取IDXGIAdapter对象,获取IDXGIOutput对象,获取IDXGIOutput1对象,获得IDXGIOutputDuplication对象
bool DXGIDuplicator::InitDuplication()
{
    HRESULT hr = S_OK;

    IDXGIDevice* dxgiDevice = nullptr;
    hr = device_->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgiDevice));
    if (FAILED(hr))
    {
        return false;
    }

    IDXGIAdapter* dxgiAdapter = nullptr;
    hr = dxgiDevice->GetAdapter(&dxgiAdapter);
    dxgiDevice->Release();
    if (FAILED(hr))
    {
        return false;
    }

    UINT output = 0;
    IDXGIOutput* dxgiOutput = nullptr;
    while (true)
    {
        hr = dxgiAdapter->EnumOutputs(output++, &dxgiOutput);
        if (hr == DXGI_ERROR_NOT_FOUND)
        {
            return false;
        }
        else
        {
            DXGI_OUTPUT_DESC desc;
            dxgiOutput->GetDesc(&desc);
            int width = desc.DesktopCoordinates.right - desc.DesktopCoordinates.left;
            int height = desc.DesktopCoordinates.bottom - desc.DesktopCoordinates.top;
            break;
        }
    }
    dxgiAdapter->Release();

    IDXGIOutput1* dxgiOutput1 = nullptr;
    hr = dxgiOutput->QueryInterface(__uuidof(IDXGIOutput1), reinterpret_cast<void**>(&dxgiOutput1));
    dxgiOutput->Release();
    if (FAILED(hr))
    {
        return false;
    }

    hr = dxgiOutput1->DuplicateOutput(device_, &duplication_);
    dxgiOutput1->Release();
    if (FAILED(hr))
    {
        return false;
    }

    return true;
}

/// <summary>
/// 获取桌面图像
/// </summary>
/// <param name="texture"></param>
/// <returns></returns>
bool DXGIDuplicator::GetDesktopFrame(ID3D11Texture2D** texture)
{
    auto start = std::chrono::system_clock::now();
    HRESULT hr = S_OK;
    DXGI_OUTDUPL_FRAME_INFO frameInfo;
    IDXGIResource* resource = nullptr;
    ID3D11Texture2D* acquireFrame = nullptr;

    hr = duplication_->AcquireNextFrame(0, &frameInfo, &resource);
    if (FAILED(hr))
    {
        if (hr == DXGI_ERROR_WAIT_TIMEOUT)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    hr = resource->QueryInterface(__uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&acquireFrame));
    resource->Release();
    if (FAILED(hr))
    {
        return false;
    }

    D3D11_TEXTURE2D_DESC desc;
    acquireFrame->GetDesc(&desc);
    desc.Usage = D3D11_USAGE_STAGING;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    desc.BindFlags = 0;
    desc.MiscFlags = 0;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.SampleDesc.Count = 1;
    device_->CreateTexture2D(&desc, NULL, texture);
    if (texture && *texture)
    {
        deviceContext_->CopyResource(*texture, acquireFrame);
    }
    acquireFrame->Release();

    hr = duplication_->ReleaseFrame();
    if (FAILED(hr))
    {
        return false;
    }
    auto end = std::chrono::system_clock::now();
    auto tc = (double)std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000.;
    printf("cost %2.4lf ms\n", tc);
    return true;
}

bool DXGIDuplicator::GetCustomDesktopFrame(UINT x, UINT y, UINT width, UINT heigh)
{


   // auto start = std::chrono::system_clock::now();
    HRESULT hr = S_OK;
    DXGI_OUTDUPL_FRAME_INFO frameInfo;
    IDXGIResource* resource = nullptr;
    ID3D11Texture2D* acquireFrame = nullptr;

    hr = duplication_->AcquireNextFrame(0, &frameInfo, &resource);
    if (FAILED(hr))
    {
        if (hr == DXGI_ERROR_WAIT_TIMEOUT)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    hr = resource->QueryInterface(__uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&acquireFrame));
    resource->Release();
    if (FAILED(hr))
    {
        return false;
    }

    D3D11_TEXTURE2D_DESC desc;
    acquireFrame->GetDesc(&desc);
    desc.Usage = D3D11_USAGE_STAGING;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    desc.BindFlags = 0;
    desc.MiscFlags = 0;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.SampleDesc.Count = 1;
    desc.Width = width;  // Set custom width
    desc.Height = heigh; // Set custom height

    // Define the source region
    D3D11_BOX sourceRegion;
    sourceRegion.left = x;
    sourceRegion.top = y;
    sourceRegion.right = x + width;
    sourceRegion.bottom = y + heigh;
    sourceRegion.front = 0;
    sourceRegion.back = 1;

    // 指向指针的指针texture
    ID3D11Texture2D** texture = &(m_texture2D);
    if (m_texture2D)
    {
        m_texture2D->Release();
        m_texture2D = nullptr;
    }
    device_->CreateTexture2D(&desc, NULL, texture);
    if (texture && *texture)
    {
        deviceContext_->CopySubresourceRegion(*texture, 0, 0, 0, 0, acquireFrame, 0, &sourceRegion);
    }
    acquireFrame->Release();

    hr = duplication_->ReleaseFrame();
    if (FAILED(hr))
    {
        return false;
    }

    //auto end = std::chrono::system_clock::now();
    //auto tc = (double)std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000.;
    //printf("cost %2.4lf ms\n", tc);
    return true;
}

BYTE* DXGIDuplicator::GetImageData(D3D11_MAPPED_SUBRESOURCE& mappedResource, UINT x, UINT y, UINT width, UINT heigh, BYTE* buffer) {
    GetCustomDesktopFrame(x, y, width, heigh);
    deviceContext_->Map(m_texture2D, 0, D3D11_MAP_READ, 0, &mappedResource);

    UINT lBmpRowPitch = width * 4;
    auto sptr = static_cast<BYTE*>(mappedResource.pData);
    auto dptr = static_cast<BYTE*>(buffer);
    UINT lRowPitch = std::min<UINT>(lBmpRowPitch, mappedResource.RowPitch);
    for (size_t h = 0; h < heigh; ++h)
    {
        memcpy_s(dptr, lBmpRowPitch, sptr, lRowPitch);
        sptr += mappedResource.RowPitch;
        //sptr -= resource.RowPitch;
        //dptr -= lBmpRowPitch;
        dptr += lBmpRowPitch;
    }

    deviceContext_->Unmap(m_texture2D, NULL);
    return buffer;
}
