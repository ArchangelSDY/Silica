//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#pragma once

#include <QSize>

#include <d2d1_3.h>
#include <d3d11_3.h>
#include <DirectXMath.h>
#include <dwrite_3.h>
#include <wrl/client.h>

class QWidget;

namespace DX
{
    // Provides an interface for an application that owns DeviceResources to be notified of the device being lost or created.
    class IDeviceNotify
    {
    public:
        virtual void OnDeviceLost() = 0;
        virtual void OnDeviceRestored() = 0;
    };

    // Controls all the DirectX device resources.
    class DeviceResources
    {
    public:
        DeviceResources();
        void SetSwapWidget(QWidget *widget);
        void SetLogicalSize(const QSize &logicalSize);
        void SetCurrentOrientation(Qt::ScreenOrientation currentOrientation);
        void SetDpi(float dpi);
        void SetCompositionScale(float compositionScaleX, float compositionScaleY);
        void ValidateDevice();
        void HandleDeviceLost();
        void RegisterDeviceNotify(IDeviceNotify* deviceNotify);
        void Trim();
        void Present();

        // Device Accessors.
        QSize  GetOutputSize() const                   { return m_outputSize; }
        QSize  GetLogicalSize() const                  { return m_logicalSize; }

        // D3D Accessors.
        ID3D11Device2*              GetD3DDevice() const                    { return m_d3dDevice.Get(); }
        ID3D11DeviceContext3*       GetD3DDeviceContext() const             { return m_d3dContext.Get(); }
        IDXGISwapChain1*            GetSwapChain() const                    { return m_swapChain.Get(); }
        D3D_FEATURE_LEVEL           GetDeviceFeatureLevel() const           { return m_d3dFeatureLevel; }
        ID3D11RenderTargetView*     GetBackBufferRenderTargetView() const   { return m_d3dRenderTargetView.Get(); }
        ID3D11DepthStencilView*     GetDepthStencilView() const             { return m_d3dDepthStencilView.Get(); }
        D3D11_VIEWPORT              GetScreenViewport() const               { return m_screenViewport; }
        DirectX::XMFLOAT4X4         GetOrientationTransform3D() const       { return m_orientationTransform3D; }

        // D2D Accessors.
        ID2D1Factory3*              GetD2DFactory() const                   { return m_d2dFactory.Get(); }
        ID2D1Device2*               GetD2DDevice() const                    { return m_d2dDevice.Get(); }
        ID2D1DeviceContext2*        GetD2DDeviceContext() const             { return m_d2dContext.Get(); }
        ID2D1Bitmap1*               GetD2DTargetBitmap() const              { return m_d2dTargetBitmap.Get(); }
        IDWriteFactory2*            GetDWriteFactory() const                { return m_dwriteFactory.Get(); }
        D2D1::Matrix3x2F            GetOrientationTransform2D() const       { return m_orientationTransform2D; }

    private:
        void CreateDeviceIndependentResources();
        void CreateDeviceResources();
        void CreateWindowSizeDependentResources();
        DXGI_MODE_ROTATION ComputeDisplayRotation();

        // Direct3D objects.
        Microsoft::WRL::ComPtr<ID3D11Device2>           m_d3dDevice;
        Microsoft::WRL::ComPtr<ID3D11DeviceContext3>    m_d3dContext;
        Microsoft::WRL::ComPtr<IDXGISwapChain1>         m_swapChain;

        // Direct3D rendering objects. Required for 3D.
        Microsoft::WRL::ComPtr<ID3D11RenderTargetView>  m_d3dRenderTargetView;
        Microsoft::WRL::ComPtr<ID3D11DepthStencilView>  m_d3dDepthStencilView;
        D3D11_VIEWPORT                                  m_screenViewport;

        // Direct2D drawing components.
        Microsoft::WRL::ComPtr<ID2D1Factory3>           m_d2dFactory;
        Microsoft::WRL::ComPtr<ID2D1Device2>            m_d2dDevice;
        Microsoft::WRL::ComPtr<ID2D1DeviceContext2>     m_d2dContext;
        Microsoft::WRL::ComPtr<ID2D1Bitmap1>            m_d2dTargetBitmap;

        // DirectWrite drawing components.
        Microsoft::WRL::ComPtr<IDWriteFactory2>         m_dwriteFactory;

        // Cached reference to widget
        QWidget                                         *m_swapChainWidget;

        // Cached device properties.
        D3D_FEATURE_LEVEL                               m_d3dFeatureLevel;
        QSize                                           m_d3dRenderTargetSize;
        QSize                                           m_outputSize;
        QSize                                           m_logicalSize;
        Qt::ScreenOrientation                           m_nativeOrientation;
        Qt::ScreenOrientation                           m_currentOrientation;
        float                                           m_dpi;
        float                                           m_compositionScaleX;
        float                                           m_compositionScaleY;


        // Transforms used for display orientation.
        D2D1::Matrix3x2F                                m_orientationTransform2D;
        DirectX::XMFLOAT4X4                             m_orientationTransform3D;

        // The IDeviceNotify can be held directly as it owns the DeviceResources.
        IDeviceNotify*                                  m_deviceNotify;
    };
}
