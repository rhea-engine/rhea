#include <iostream>
#include <comdef.h>
using namespace std;

#include <d3d11shader.h>
#include <d3dcompiler.h>

#include "D3D11HLSL.h"

#pragma comment(lib,"d3dcompiler.lib")

namespace Hippios
{
	std::wstring s2ws(const std::string& s)
	{
		int len;
		int slength = (int)s.length() + 1;
		len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
		wchar_t* buf = new wchar_t[len];
		MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
		std::wstring r(buf);
		delete[] buf;
		return r;
	}

	D3D11HLSL::D3D11HLSL(ID3D11Device* d3dDevice, ID3D11DeviceContext* d3dDeviceContext)
	{
		m_d3dDevice = d3dDevice;
		m_d3dDeviceContext = d3dDeviceContext;
	}

	HRESULT D3D11HLSL::LoadFromFile(std::string filename, LPCSTR entryPoint, LPCSTR shaderType, LPCSTR shaderTypeId)
	{	
		UINT compileFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
		compileFlags |= D3DCOMPILE_DEBUG;
#endif
		// Prefer higher CS shader profile when possible as CS 5.0 provides better performance on 11-class hardware.
		//LPCSTR profile = (m_d3dDevice->GetFeatureLevel() >= D3D_FEATURE_LEVEL_11_0) ? "cs_5_0" : "cs_4_0";
		const D3D_SHADER_MACRO defines[] =
		{
			"EXAMPLE_DEFINE", "1",
			NULL, NULL
		};

		std::wstring filename_wc = s2ws(filename);
		LPCWSTR filename_lpwc = filename_wc.c_str();

	/*	HRESULT hr = D3DCompileFromFile(
			filename_lpwc,			  // filename
			defines,             // Optional. Pointer to a NULL-terminated array of macro definitions. See D3D_SHADER_MACRO. If not used, set this to NULL. 
			D3D_COMPILE_STANDARD_FILE_INCLUDE, // default include compile
			entryPoint,  // [in] Name of the shader-entrypoint function where shader execution begins. 
			shaderTypeId,               // [in] A string that specifies the shader model; can be any profile in shader model 4 or higher. 
			compileFlags,         // [in] Effect compile flags - no D3DCOMPILE_ENABLE_BACKWARDS_COMPATIBILITY at the first try...
			NULL,                 // [in] Effect compile flags
			&shaderBlob,  // [out] A pointer to an ID3DBlob Interface which contains the compiled shader, as well as any embedded debug and symbol-table information. 
			&errorBlob   // [out] A pointer to an ID3DBlob Interface which contains a listing of errors and warnings that occurred during compilation. These errors and warnings are identical to the the debug output from a debugger.
		);
		*/
		HRESULT hr = D3DCompileFromFile(
			filename_lpwc,			  // filename
			NULL,             // Optional. Pointer to a NULL-terminated array of macro definitions. See D3D_SHADER_MACRO. If not used, set this to NULL. 
			D3D_COMPILE_STANDARD_FILE_INCLUDE, // default include compile
			entryPoint,  // [in] Name of the shader-entrypoint function where shader execution begins. 
			shaderTypeId,               // [in] A string that specifies the shader model; can be any profile in shader model 4 or higher. 
			0,         // [in] Effect compile flags - no D3DCOMPILE_ENABLE_BACKWARDS_COMPATIBILITY at the first try...
			0,                 // [in] Effect compile flags
			&shaderBlob,  // [out] A pointer to an ID3DBlob Interface which contains the compiled shader, as well as any embedded debug and symbol-table information. 
			&errorBlob   // [out] A pointer to an ID3DBlob Interface which contains a listing of errors and warnings that occurred during compilation. These errors and warnings are identical to the the debug output from a debugger.
		);


		if (FAILED(hr))
		{
			if (errorBlob)
			{
				OutputDebugStringA((char*)errorBlob->GetBufferPointer());
				errorBlob->Release();
			}

			if (shaderBlob)
				shaderBlob->Release();

			return hr;
		}		
		// encapsulate shaders into shader objects
		// and set the shader objects
		// encapsulate shaders into shader objects
		// and set the shader objects
		if (!strcmp(shaderType, "vs")) {
			hr = m_d3dDevice->CreateVertexShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, &pVS);
			if (FAILED(hr)) {
				printf("could not load vertex shader");
			}
		}
		else {
			try {
				m_d3dDevice->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, &pPS);
				if (FAILED(hr)) {
					printf("could not load pixel shader");
				}
			}
			catch (_com_error & e) {
				printf("could not load pixel shader\n");
			}
		}
		return hr;
	}

	HRESULT D3D11HLSL::LoadFromBuffer(unsigned char* buf, size_t buflen, LPCSTR entry_point, LPCSTR shader_type, LPCSTR shader_type_id)
	{
		UINT compileFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
		compileFlags |= D3DCOMPILE_DEBUG;
#endif
		// Prefer higher CS shader profile when possible as CS 5.0 provides better performance on 11-class hardware.
		//LPCSTR profile = (m_d3dDevice->GetFeatureLevel() >= D3D_FEATURE_LEVEL_11_0) ? "cs_5_0" : "cs_4_0";
		const D3D_SHADER_MACRO defines[] =
		{
			"EXAMPLE_DEFINE", "1",
			NULL, NULL
		};

		HRESULT hr = D3DCompile(buf,
			buflen,
			nullptr,
			0, 0, entry_point, shader_type_id, 0, 0, &shaderBlob, &errorBlob);

		if (FAILED(hr))
		{
			if (errorBlob)
			{
				OutputDebugStringA((char*)errorBlob->GetBufferPointer());
				errorBlob->Release();
			}

			if (shaderBlob)
				shaderBlob->Release();

			return hr;
		}
		// encapsulate shaders into shader objects
		// and set the shader objects
		// encapsulate shaders into shader objects
		// and set the shader objects
		if (!strcmp(shader_type, "vs")) {
			hr = m_d3dDevice->CreateVertexShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, &pVS);
			if (FAILED(hr)) {
				printf("could not load vertex shader");
			}
		}
		else {
			try {
				m_d3dDevice->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, &pPS);
				if (FAILED(hr)) {
					printf("could not load pixel shader");
				}
			}
			catch (_com_error& e) {
				printf("could not load pixel shader\n");
			}
		}
		return hr;
	}

	/** default shader:
	*     a very simple color shader used for testing
	*/
	HRESULT D3D11HLSL::defaultShader(LPCSTR entryPoint, LPCSTR shaderType, LPCSTR shaderTypeId)
	{



		char* prog = "struct VOut \
		{ \
			float4 position : SV_POSITION; \
			float4 color : COLOR; \
		}; \
\
		VOut VShader(float4 position : POSITION, float4 color : COLOR) \
		{\
			VOut output;\
\
			output.position = position; \
			output.color = color; \
\
			return output; \
		}\
\
\
		float4 PShader(float4 position : SV_POSITION, float4 color : COLOR) : SV_TARGET \
		{\
			return color;\
		}\
			";

		UINT compileFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
		compileFlags |= D3DCOMPILE_DEBUG;
#endif
		// Prefer higher CS shader profile when possible as CS 5.0 provides better performance on 11-class hardware.
		//LPCSTR profile = (m_d3dDevice->GetFeatureLevel() >= D3D_FEATURE_LEVEL_11_0) ? "cs_5_0" : "cs_4_0";
		const D3D_SHADER_MACRO defines[] =
		{
			"EXAMPLE_DEFINE", "1",
			NULL, NULL
		};
		/*
		HRESULT hr = D3DCompile(
			prog,
			strlen(prog),
			"defshader", // filename
			defines,             // Optional. Pointer to a NULL-terminated array of macro definitions. See D3D_SHADER_MACRO. If not used, set this to NULL. 
			D3D_COMPILE_STANDARD_FILE_INCLUDE, // default include compile
			entryPoint,  // [in] Name of the shader-entrypoint function where shader execution begins. 
			shaderTypeId,               // [in] A string that specifies the shader model; can be any profile in shader model 4 or higher. 
			compileFlags,         // [in] Effect compile flags - no D3DCOMPILE_ENABLE_BACKWARDS_COMPATIBILITY at the first try...
			NULL,                 // [in] Effect compile flags
			&shaderBlob,  // [out] A pointer to an ID3DBlob Interface which contains the compiled shader, as well as any embedded debug and symbol-table information. 
			&errorBlob   // [out] A pointer to an ID3DBlob Interface which contains a listing of errors and warnings that occurred during compilation. These errors and warnings are identical to the the debug output from a debugger.
		);
		*/
		// from example (but with errBlob)
		HRESULT hr = D3DCompile(prog,
			strlen(prog),
			"defshader", // filename
			0, 0, entryPoint, shaderTypeId, 0, 0, &shaderBlob, &errorBlob);

		if (FAILED(hr))
		{
			if (errorBlob)
			{
				OutputDebugStringA((char*)errorBlob->GetBufferPointer());
				printf((char*)errorBlob->GetBufferPointer());
				errorBlob->Release();
			}

			if (shaderBlob)
				shaderBlob->Release();

			return hr;
		}
		// encapsulate shaders into shader objects
		// and set the shader objects
		if (!strcmp(shaderType,"vs")) {
			hr = m_d3dDevice->CreateVertexShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, &pVS);
			if (FAILED(hr)) {
				printf("could not load vertex shader");
			}
			m_d3dDeviceContext->VSSetShader(pVS, 0, 0);
		}
		else {
			try {
				m_d3dDevice->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, &pPS);
				if (FAILED(hr)) {
					printf("could not load pixel shader");
				}
			}
			catch (_com_error & e) {				
				printf("could not load pixel shader\n");
			}
			m_d3dDeviceContext->PSSetShader(pPS, 0, 0);
		}
		return hr;
	}

	void D3D11HLSL::setVertexType(int type)
	{
		m_vertex_type = type;
	}

	ID3D11InputLayout* D3D11HLSL::GetInputLayout()
	{		
		HRESULT HR;
		if (!mInputLayout) {
			if (!m_vertex_type) {
				// create the input layout object
				D3D11_INPUT_ELEMENT_DESC ied[] =
				{
					{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
					{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
				};

				HR = m_d3dDevice->CreateInputLayout(ied, 2, shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), &mInputLayout);				
			}
			else {
				// create the input layout object for uv
				D3D11_INPUT_ELEMENT_DESC ied[] =
				{
					{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
					{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
				};

				HR = m_d3dDevice->CreateInputLayout(ied, 2, shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), &mInputLayout);
			}
			if (FAILED(HR)) {
				printf("D3D11HLSL::GetInputLayout(): could not create input layout");
			}
		}
		return mInputLayout;
	}
}