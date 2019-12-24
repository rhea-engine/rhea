#include <d3d11.h>
#include <d3d11shader.h>
#include <d3dcompiler.h>

#include <iostream>

#include "D3D11Device.h"

namespace Hippios
{
	HRESULT D3D11Device::CreateDevice()
	{
		HRESULT hr = S_OK;

		D3D_FEATURE_LEVEL levels[] = {
			D3D_FEATURE_LEVEL_9_1,
			D3D_FEATURE_LEVEL_9_2,
			D3D_FEATURE_LEVEL_9_3,
			D3D_FEATURE_LEVEL_10_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_11_1
		};

		// This flag adds support for surfaces with a color-channel ordering different
		// from the API default. It is required for compatibility with Direct2D.
		UINT deviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#if defined(DEBUG) || defined(_DEBUG)
		deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		// Create the Direct3D 11 API device object and a corresponding context.
		ComPtr<ID3D11Device>        device;
		ComPtr<ID3D11DeviceContext> context;

		hr = D3D11CreateDevice(
			nullptr,                    // Specify nullptr to use the default adapter.
			D3D_DRIVER_TYPE_HARDWARE,   // Create a device using the hardware graphics driver.
			0,                          // Should be 0 unless the driver is D3D_DRIVER_TYPE_SOFTWARE.
			deviceFlags,                // Set debug and Direct2D compatibility flags.
			levels,                     // List of feature levels this app can support.
			ARRAYSIZE(levels),          // Size of the list above.
			D3D11_SDK_VERSION,          // Always set this to D3D11_SDK_VERSION for Windows Store apps.
			device.GetAddressOf(),                    // Returns the Direct3D device created.
			&mFeatureLevel,            // Returns feature level of device created.
			context.GetAddressOf()                    // Returns the device immediate context.
		);

		if (FAILED(hr))
		{
			// Handle device interface creation failure if it occurs.
			// For example, reduce the feature level requirement, or fail over 
			// to WARP rendering.
		}

		// Store pointers to the Direct3D 11.1 API device and immediate context.
		device.As(&mD3dDevice);
		context.As(&mD3dDeviceContext);

		return hr;
	}

	

	
}