#pragma once
#include "ComPtr.h"

namespace Hippios
{
	class D3D11Device
	{
	protected:
		// dxd11 device
		ComPtr<ID3D11Device>        mD3dDevice;
		ComPtr<ID3D11DeviceContext> mD3dDeviceContext; // immediate context		

		// helper
		D3D_FEATURE_LEVEL       mFeatureLevel;		
	public:
		HRESULT CreateDevice();
		
		
	};
}
