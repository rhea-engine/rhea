#include "D3D11ShaderProgram.h"
#include "D3D11ShaderProgram.h"
#include "D3D11ShaderProgram.h"
#include "D3D11ShaderProgram.h"
#include "D3D11ShaderProgram.h"
#include "D3D11ShaderProgram.h"
#include "D3D11ShaderProgram.h"
#include <iostream>
#include <comdef.h>

#include <d3d11shader.h>
#include <d3dcompiler.h>

#include "D3D11ShaderProgram.h"

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

	D3D11ShaderProgram::D3D11ShaderProgram(ID3D11Device* d3dDevice)
	{
		m_device = d3dDevice;
	}

	D3D11ShaderProgram::~D3D11ShaderProgram()
	{
		releaseResources();
	}

	bool D3D11ShaderProgram::loadFromFiles(const std::string& vertexFile, const std::string& vertexEntry, const std::string& pixelFile, const std::string& pixelEntry)
	{
		releaseResources();

		if (!compileVertexShaderFromFile(vertexFile, vertexEntry))
			return false;

		if (!compilePixelShaderFromFile(pixelFile, pixelEntry))
		{
			releaseResources();
			return false;
		}

		return true;
	}

	bool D3D11ShaderProgram::loadFromSource(const unsigned char* vertexSource, size_t vertexLength, const std::string& vertexEntry, const unsigned char* pixelSource, size_t pixelLength, const std::string& pixelEntry)
	{		
		releaseResources();

		if (!compileVertexShaderFromSource(vertexSource, vertexLength, vertexEntry))
			return false;

		if (!compilePixelShaderFromSource(pixelSource, pixelLength, pixelEntry))
		{
			releaseResources();
			return false;
		}

		return true;
	}

	bool D3D11ShaderProgram::compileVertexShaderFromFile(const std::string& filename, const std::string& entryPoint)
	{
		ID3DBlob* errorBlob = nullptr;

		UINT compileFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
		compileFlags |= D3DCOMPILE_DEBUG;
#endif
		// Prefer higher CS shader profile when possible as CS 5.0 provides better performance on 11-class hardware.
		//LPCSTR profile = (m_device->GetFeatureLevel() >= D3D_FEATURE_LEVEL_11_0) ? "cs_5_0" : "cs_4_0";
		const D3D_SHADER_MACRO defines[] =
		{
			"EXAMPLE_DEFINE", "1",
			NULL, NULL
		};

		std::wstring filename_wc = s2ws(filename);
		LPCWSTR filename_lpwc = filename_wc.c_str();

		HRESULT hr = D3DCompileFromFile(
			filename_lpwc,			  // filename
			NULL,             // Optional. Pointer to a NULL-terminated array of macro definitions. See D3D_SHADER_MACRO. If not used, set this to NULL. 
			D3D_COMPILE_STANDARD_FILE_INCLUDE, // default include compile
			entryPoint.c_str(),  // [in] Name of the shader-entrypoint function where shader execution begins. 
			"vs_5_0",               // [in] A string that specifies the shader model; can be any profile in shader model 4 or higher. 
			0,         // [in] Effect compile flags - no D3DCOMPILE_ENABLE_BACKWARDS_COMPATIBILITY at the first try...
			0,                 // [in] Effect compile flags
			&m_vertexBlob,  // [out] A pointer to an ID3DBlob Interface which contains the compiled shader, as well as any embedded debug and symbol-table information. 
			&errorBlob   // [out] A pointer to an ID3DBlob Interface which contains a listing of errors and warnings that occurred during compilation. These errors and warnings are identical to the the debug output from a debugger.
		);

		if (FAILED(hr))
		{
			if (errorBlob) {
				OutputDebugStringA((char*)errorBlob->GetBufferPointer()); 
				errorBlob->Release(); 
			}
			return false;
		}

		if (errorBlob) errorBlob->Release();

		hr = m_device->CreateVertexShader(
			m_vertexBlob->GetBufferPointer(),
			m_vertexBlob->GetBufferSize(),
			nullptr,
			&m_vertexShader);

		return SUCCEEDED(hr);
	}

	bool D3D11ShaderProgram::compilePixelShaderFromFile(const std::string& filename, const std::string& entryPoint)
	{
		ID3DBlob* errorBlob = nullptr;

		UINT compileFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
		compileFlags |= D3DCOMPILE_DEBUG;
#endif
		// Prefer higher CS shader profile when possible as CS 5.0 provides better performance on 11-class hardware.
		//LPCSTR profile = (m_device->GetFeatureLevel() >= D3D_FEATURE_LEVEL_11_0) ? "cs_5_0" : "cs_4_0";
		const D3D_SHADER_MACRO defines[] =
		{
			"EXAMPLE_DEFINE", "1",
			NULL, NULL
		};

		std::wstring filename_wc = s2ws(filename);
		LPCWSTR filename_lpwc = filename_wc.c_str();

		HRESULT hr = D3DCompileFromFile(
			filename_lpwc,			  // filename
			NULL,             // Optional. Pointer to a NULL-terminated array of macro definitions. See D3D_SHADER_MACRO. If not used, set this to NULL. 
			D3D_COMPILE_STANDARD_FILE_INCLUDE, // default include compile
			entryPoint.c_str(),  // [in] Name of the shader-entrypoint function where shader execution begins. 
			"ps_5_0",               // [in] A string that specifies the shader model; can be any profile in shader model 4 or higher. 
			0,         // [in] Effect compile flags - no D3DCOMPILE_ENABLE_BACKWARDS_COMPATIBILITY at the first try...
			0,                 // [in] Effect compile flags
			&m_pixelBlob,  // [out] A pointer to an ID3DBlob Interface which contains the compiled shader, as well as any embedded debug and symbol-table information. 
			&errorBlob   // [out] A pointer to an ID3DBlob Interface which contains a listing of errors and warnings that occurred during compilation. These errors and warnings are identical to the the debug output from a debugger.
		);

		if (FAILED(hr))
		{
			if (errorBlob) {
				OutputDebugStringA((char*)errorBlob->GetBufferPointer());
				errorBlob->Release();
			}
			return false;
		}

		if (errorBlob) errorBlob->Release();

		hr = m_device->CreatePixelShader(
			m_pixelBlob->GetBufferPointer(),
			m_pixelBlob->GetBufferSize(),
			nullptr,
			&m_pixelShader);

		return SUCCEEDED(hr);
	}

	bool D3D11ShaderProgram::compileVertexShaderFromSource(const unsigned char* buf, size_t buflen, const std::string& entryPoint)
	{
		ID3DBlob* errorBlob = nullptr;

		UINT compileFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
		compileFlags |= D3DCOMPILE_DEBUG;
#endif
		// Prefer higher CS shader profile when possible as CS 5.0 provides better performance on 11-class hardware.
		//LPCSTR profile = (m_device->GetFeatureLevel() >= D3D_FEATURE_LEVEL_11_0) ? "cs_5_0" : "cs_4_0";
		const D3D_SHADER_MACRO defines[] =
		{
			"EXAMPLE_DEFINE", "1",
			NULL, NULL
		};

		HRESULT hr = D3DCompile(buf,
			buflen,
			nullptr,
			0, 0, entryPoint.c_str(), "vs_5_0", 0, 0, &m_vertexBlob, &errorBlob);

		if (FAILED(hr))
		{
			if (errorBlob)
			{
				const char* errorMsg = (const char*)errorBlob->GetBufferPointer();
				printf("%s\n", errorMsg);
				OutputDebugStringA((char*)errorBlob->GetBufferPointer());
				errorBlob->Release();
			}

			if (m_vertexBlob) {
				m_vertexBlob->Release();
				m_vertexBlob = nullptr;
			}

			return hr;
		}
		else if (errorBlob)
		{
			errorBlob->Release();
		}
		hr = m_device->CreateVertexShader(m_vertexBlob->GetBufferPointer(), m_vertexBlob->GetBufferSize(), NULL, &m_vertexShader);
		if (FAILED(hr)) {
			printf("could not load vertex shader");
		}
		return SUCCEEDED(hr);
	}

	bool D3D11ShaderProgram::compilePixelShaderFromSource(const unsigned char* buf, size_t buflen, const std::string& entryPoint)
	{
		ID3DBlob* errorBlob = nullptr;

		UINT compileFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
		compileFlags |= D3DCOMPILE_DEBUG;
#endif
		// Prefer higher CS shader profile when possible as CS 5.0 provides better performance on 11-class hardware.
		//LPCSTR profile = (m_device->GetFeatureLevel() >= D3D_FEATURE_LEVEL_11_0) ? "cs_5_0" : "cs_4_0";
		const D3D_SHADER_MACRO defines[] =
		{
			"EXAMPLE_DEFINE", "1",
			NULL, NULL
		};

		HRESULT hr = D3DCompile(buf,
			buflen,
			nullptr,
			0, 0, entryPoint.c_str(), "ps_5_0", 0, 0, &m_pixelBlob, &errorBlob);

		if (FAILED(hr))
		{
			if (errorBlob)
			{
				const char* errorMsg = (const char*)errorBlob->GetBufferPointer();
				printf("%s\n", errorMsg);
				OutputDebugStringA((char*)errorBlob->GetBufferPointer());
				errorBlob->Release();
			}

			if (m_pixelBlob) {
				m_pixelBlob->Release();
				m_pixelBlob = nullptr;
			}

			return hr;
		}
		else if (errorBlob)
		{
			errorBlob->Release();
		}
		hr = m_device->CreatePixelShader(m_pixelBlob->GetBufferPointer(), m_pixelBlob->GetBufferSize(), NULL, &m_pixelShader);
		if (FAILED(hr)) {
			printf("could not load vertex shader");
		}
		return SUCCEEDED(hr);

	}

	void D3D11ShaderProgram::releaseResources()
	{
		if (m_vertexBlob)
			m_vertexBlob->Release();
		if (m_pixelBlob)
			m_pixelBlob->Release();
		if (m_vertexShader)
			m_vertexShader->Release();
		if (m_pixelShader)
			m_pixelShader->Release();
	}
}