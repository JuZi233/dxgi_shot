
#include "pch.h"
#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>

#include <d3d11.h>
#include <dxgi1_2.h>

#include "DXGIDuplicator.h"


/**
int main()
{
    // Initialize your DXGIDuplicator
    DXGIDuplicator* duplicator = new DXGIDuplicator;
    if (!duplicator->InitD3D11Device() || !duplicator->InitDuplication())
    {
        std::cerr << "Failed to initialize DXGIDuplicator." << std::endl;
        return 1;
    }

    // Set custom coordinates and dimensions for the region you want to capture
    int x = 100; // Example: left coordinate
    int y = 100; // Example: top coordinate
    int w = 800; // Example: width
    int h = 600; // Example: height

    // Get the custom desktop frame
    ID3D11Texture2D* texture = nullptr;
    if (GetCustomDesktopFrame(duplicator, x, y, w, h))
    {
        std::cerr << "Failed to capture custom desktop frame." << std::endl;
        return 1;
    }

    // Convert the captured frame to an OpenCV Mat
    cv::Mat capturedImage(h, w, CV_8UC4); // Assuming RGBA format
    // You may need to adjust the format (e.g., BGRA) based on your system

    // Copy the texture data to the Mat
    //D3D11_MAPPED_SUBRESOURCE mappedResource;
    //duplicator->deviceContext_->Map(texture, 0, D3D11_MAP_READ, 0, &mappedResource);
    //memcpy(capturedImage.data, mappedResource.pData, capturedImage.total() * capturedImage.elemSize());
    //duplicator->deviceContext_->Unmap(texture, 0);

    // Release the texture
    texture->Release();

    // Display the captured image using OpenCV
    cv::imshow("Custom Desktop Frame", capturedImage);
    cv::waitKey(0);

    return 0;
}
*/


/**
void SaveBmp(std::string filename, const uint8_t* data, int width, int height)
{
    HANDLE hFile = CreateFileA(filename.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == NULL)
    {
        return;
    }
    // 已写入字节数
    DWORD bytesWritten = 0;
    // 位图大小，颜色默认为32位即rgba
    int bmpSize = width * height * 4;

    // 文件头
    BITMAPFILEHEADER bmpHeader;
    // 文件总大小 = 文件头 + 位图信息头 + 位图数据
    bmpHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + bmpSize;
    // 固定
    bmpHeader.bfType = 0x4D42;
    // 数据偏移，即位图数据所在位置
    bmpHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    // 保留为0
    bmpHeader.bfReserved1 = 0;
    // 保留为0
    bmpHeader.bfReserved2 = 0;
    // 写文件头
    WriteFile(hFile, (LPSTR)&bmpHeader, sizeof(bmpHeader), &bytesWritten, NULL);

    // 位图信息头
    BITMAPINFOHEADER bmiHeader;
    // 位图信息头大小
    bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    // 位图像素宽度
    bmiHeader.biWidth = width;
    // 位图像素高度，负高度即上下翻转
    bmiHeader.biHeight = -height;
    // 必须为1
    bmiHeader.biPlanes = 1;
    // 像素所占位数
    bmiHeader.biBitCount = 32;
    // 0表示不压缩
    bmiHeader.biCompression = 0;
    // 位图数据大小
    bmiHeader.biSizeImage = bmpSize;
    // 水平分辨率(像素/米)
    bmiHeader.biXPelsPerMeter = 0;
    // 垂直分辨率(像素/米)
    bmiHeader.biYPelsPerMeter = 0;
    // 使用的颜色，0为使用全部颜色
    bmiHeader.biClrUsed = 0;
    // 重要的颜色数，0为所有颜色都重要
    bmiHeader.biClrImportant = 0;

    // 写位图信息头
    WriteFile(hFile, (LPSTR)&bmiHeader, sizeof(bmiHeader), &bytesWritten, NULL);
    // 写位图数据
    WriteFile(hFile, data, bmpSize, &bytesWritten, NULL);
    CloseHandle(hFile);
}
**/
/**
void SaveDesktopImage(std::string filename, ID3D11Texture2D* texture2D, DXGIDuplicator* duplicator)
{
    D3D11_TEXTURE2D_DESC desc;
    texture2D->GetDesc(&desc);
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    duplicator->deviceContext_->Map(texture2D, 0, D3D11_MAP_READ, 0, &mappedResource);

    size_t imageSize = desc.Width * desc.Height * 4;
    uint8_t* rgba = (uint8_t*)malloc(imageSize);
    if (rgba == nullptr)
    {
        return;
    }
    memset(rgba, 0, imageSize);
    uint8_t* pData = (uint8_t*)mappedResource.pData;
    for (size_t i = 0; i < desc.Height; i++)
    {
        memcpy(rgba + i * desc.Width * 4, pData + i * mappedResource.RowPitch, desc.Width * 4);
    }
    SaveBmp(filename, rgba, desc.Width, desc.Height);
    free(rgba);
}
**/

