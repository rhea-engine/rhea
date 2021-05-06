#include "D3D11Window.h"
#include "D3D11Window.h"
#include <d3d11.h>
#include <d3d11shader.h>
#include <d3dcompiler.h>
#include <dxgi1_3.h>

#include "D3D11Window.h"
#include "Psio/Psio.h"
#include "Psio/Window.h"
#include "Hippios/Viewport.h"

namespace Hippios
{
	D3D11Window::D3D11Window()
	{
	/*	mIsFullScreen = false;
		mIsExternal = false;
		mActive = false;
		mSizing = false;
		mClosed = false;
		mHidden = false;
		*/
	}
	/*
	//---------------------------------------------------------------------
	D3D11Window::~D3D11Window()
	{
		D3D11RenderSystem* rsys = static_cast<D3D11RenderSystem*>(Root::getSingleton().getRenderSystem());
		rsys->fireDeviceEvent(&mDevice, "RenderWindowDestroyed", this);

		destroy();
	}
	*/


	void D3D11Window::create(Psio::Engine* psio)
	{
		mWin = psio->createWindow();
		mWin->create();
	}

	/*
	HRESULT D3D11Window::CreateSwapchain()
	{
		HRESULT hr = S_OK;
		if (!mWin) {
			create();
		}
		HWND hWnd = (HWND)mWin->getHandle();

		DXGI_SWAP_CHAIN_DESC desc;
		ZeroMemory(&desc, sizeof(DXGI_SWAP_CHAIN_DESC));
		desc.Windowed = TRUE; // Sets the initial state of full-screen mode.
		desc.BufferCount = 2;
		desc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.SampleDesc.Count = 1;      //multisampling setting
		desc.SampleDesc.Quality = 0;    //vendor-specific flag
		desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
		desc.OutputWindow = hWnd;

		// Create the DXGI device object to use in other factories, such as Direct2D.
		ComPtr<IDXGIDevice3> dxgiDevice;
		mD3dDevice.As(&dxgiDevice);

		// Create swap chain.
		ComPtr<IDXGIAdapter> adapter;
		ComPtr<IDXGIFactory> factory;

		hr = dxgiDevice->GetAdapter(adapter.GetAddressOf());

		if (SUCCEEDED(hr))
		{
			adapter->GetParent(IID_PPV_ARGS(factory.GetAddressOf()));

			hr = factory->CreateSwapChain(
				mD3dDevice.Get(),
				&desc,
				mDXGISwapChain.GetAddressOf()
			);
		}

		// Configure the back buffer, stencil buffer, and viewport.
		hr = ConfigureBackBuffer();

		return hr;
	}

	HRESULT D3D11Window::ConfigureBackBuffer()
	{
		HRESULT hr = S_OK;

		hr = mDXGISwapChain->GetBuffer(
			0,
			__uuidof(ID3D11Texture2D),
			(void**)&mBackBuffer);

		hr = mD3dDevice->CreateRenderTargetView(
			m_pBackBuffer.Get(),
			nullptr,
			mRenderTarget.GetAddressOf()
		);

		mBackBuffer->GetDesc(&m_bbDesc);

		// Create a depth-stencil view for use with 3D rendering if needed.
		CD3D11_TEXTURE2D_DESC depthStencilDesc(
			DXGI_FORMAT_D24_UNORM_S8_UINT,
			static_cast<UINT> (m_bbDesc.Width),
			static_cast<UINT> (m_bbDesc.Height),
			1, // This depth stencil view has only one texture.
			1, // Use a single mipmap level.
			D3D11_BIND_DEPTH_STENCIL
		);

		mD3dDevice->CreateTexture2D(
			&depthStencilDesc,
			nullptr,
			&mDepthStencil
		);

		CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);

		mD3dDevice->CreateDepthStencilView(
			mDepthStencil.Get(),
			&depthStencilViewDesc,
			&mDepthStencilView
		);


		ZeroMemory(&mViewport, sizeof(D3D11_VIEWPORT));
		mViewport.Height = (float)m_bbDesc.Height;
		mViewport.Width = (float)m_bbDesc.Width;
		mViewport.MinDepth = 0;
		mViewport.MaxDepth = 1;

		m_pd3dDeviceContext->RSSetViewports(
			1,
			&mViewport
		);

		return hr;
	}
	*/
	void D3D11Window::destroy()
	{		
		mWin->destroy();
		mWin = 0;
	}

	int D3D11Window::getHeight()
	{
		if (mWin) {
			return mWin->GetHeight();
		}
		return 0;
	}

	int D3D11Window::getWidth()
	{
		if (mWin) {
			return mWin->GetWidth();
		}
		return 0;
	}

	void D3D11Window::update()
	{
		// call rendering on all viewports - currently only one
		if (m_viewport)
			m_viewport->render();
	}

	void D3D11Window::addViewport(Viewport* viewport)
	{
		m_viewport = viewport;
	}

}