/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include "D3D11ShaderProgram.h"
#include <iostream>
#include <comdef.h>

#include <d3d11shader.h>
#include <d3dcompiler.h>

#include <Psio/Log.h>

#pragma comment(lib,"d3dcompiler.lib")

namespace hippios
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

		return true;
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

		return true;
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
				LOG(LOG_ERROR) << errorMsg;
				OutputDebugStringA((char*)errorBlob->GetBufferPointer());
				errorBlob->Release();
			}

			if (m_vertexBlob) {
				m_vertexBlob->Release();
				m_vertexBlob = nullptr;
			}

			return false;
		}
		else if (errorBlob)
		{
			errorBlob->Release();
		}
		hr = m_device->CreateVertexShader(m_vertexBlob->GetBufferPointer(), m_vertexBlob->GetBufferSize(), NULL, &m_vertexShader);
		if (FAILED(hr)) {
			LOG(LOG_ERROR) << "could not load vertex shader";
		}
		return true;
	}

	bool D3D11ShaderProgram::compilePixelShaderFromSource(const unsigned char* buf, size_t buflen, const std::string& entryPoint)
	{
		ID3DBlob* errorBlob = nullptr;

		UINT compileFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
		compileFlags |= D3DCOMPILE_DEBUG;
#endif

		HRESULT hr = D3DCompile(buf,
			buflen,
			nullptr,
			0, 0, entryPoint.c_str(), "ps_5_0", 0, 0, &m_pixelBlob, &errorBlob);

		if (FAILED(hr))
		{
			if (errorBlob)
			{
				const char* errorMsg = (const char*)errorBlob->GetBufferPointer();
				LOG(LOG_ERROR) << errorMsg;
				OutputDebugStringA((char*)errorBlob->GetBufferPointer());
				errorBlob->Release();
			}

			if (m_pixelBlob) {
				m_pixelBlob->Release();
				m_pixelBlob = nullptr;
			}

			return false;
		}
		else if (errorBlob)
		{
			errorBlob->Release();
		}
#ifdef DEBUG_SHADER_REFLECTION
		else
		{
			LOG(LOG_DEBUG) << "Pixel shader compiled successfully, size: " << m_pixelBlob->GetBufferSize() << " bytes";
		}
		// Shader Reflection
		ID3D11ShaderReflection* reflection = nullptr;
		HRESULT reflectHr = D3DReflect(
			m_pixelBlob->GetBufferPointer(),
			m_pixelBlob->GetBufferSize(),
			__uuidof(ID3D11ShaderReflection),
			reinterpret_cast<void**>(&reflection)
		);

		if (SUCCEEDED(reflectHr) && reflection) {
			D3D11_SHADER_DESC shaderDesc;
			reflection->GetDesc(&shaderDesc);
			
			LOG(LOG_DEBUG) << "Pixel Shader Reflection: " << shaderDesc.BoundResources << " bound resources";
			
			// Alle Resource-Bindings durchlaufen
			for (UINT i = 0; i < shaderDesc.BoundResources; ++i) {
				D3D11_SHADER_INPUT_BIND_DESC bindDesc;
				if (SUCCEEDED(reflection->GetResourceBindingDesc(i, &bindDesc))) {
					const char* typeStr = "UNKNOWN";
					switch (bindDesc.Type) {
						case D3D_SIT_CBUFFER: typeStr = "CBUFFER"; break;
						case D3D_SIT_TBUFFER: typeStr = "TBUFFER"; break;
						case D3D_SIT_TEXTURE: typeStr = "TEXTURE"; break;
						case D3D_SIT_SAMPLER: typeStr = "SAMPLER"; break;
						case D3D_SIT_UAV_RWTYPED: typeStr = "UAV_RWTYPED"; break;
						case D3D_SIT_STRUCTURED: typeStr = "STRUCTURED"; break;
						case D3D_SIT_UAV_RWSTRUCTURED: typeStr = "UAV_RWSTRUCTURED"; break;
						case D3D_SIT_BYTEADDRESS: typeStr = "BYTEADDRESS"; break;
						case D3D_SIT_UAV_RWBYTEADDRESS: typeStr = "UAV_RWBYTEADDRESS"; break;
						case D3D_SIT_UAV_APPEND_STRUCTURED: typeStr = "UAV_APPEND_STRUCTURED"; break;
						case D3D_SIT_UAV_CONSUME_STRUCTURED: typeStr = "UAV_CONSUME_STRUCTURED"; break;
						case D3D_SIT_UAV_RWSTRUCTURED_WITH_COUNTER: typeStr = "UAV_RWSTRUCTURED_WITH_COUNTER"; break;
					}
					
					const char* dimStr = "UNKNOWN";
					switch (bindDesc.Dimension) {
						case D3D_SRV_DIMENSION_UNKNOWN: dimStr = "UNKNOWN"; break;
						case D3D_SRV_DIMENSION_BUFFER: dimStr = "BUFFER"; break;
						case D3D_SRV_DIMENSION_TEXTURE1D: dimStr = "TEXTURE1D"; break;
						case D3D_SRV_DIMENSION_TEXTURE1DARRAY: dimStr = "TEXTURE1DARRAY"; break;
						case D3D_SRV_DIMENSION_TEXTURE2D: dimStr = "TEXTURE2D"; break;
						case D3D_SRV_DIMENSION_TEXTURE2DARRAY: dimStr = "TEXTURE2DARRAY"; break;
						case D3D_SRV_DIMENSION_TEXTURE2DMS: dimStr = "TEXTURE2DMS"; break;
						case D3D_SRV_DIMENSION_TEXTURE2DMSARRAY: dimStr = "TEXTURE2DMSARRAY"; break;
						case D3D_SRV_DIMENSION_TEXTURE3D: dimStr = "TEXTURE3D"; break;
						case D3D_SRV_DIMENSION_TEXTURECUBE: dimStr = "TEXTURECUBE"; break;
						case D3D_SRV_DIMENSION_TEXTURECUBEARRAY: dimStr = "TEXTURECUBEARRAY"; break;
						case D3D_SRV_DIMENSION_BUFFEREX: dimStr = "BUFFEREX"; break;
					}
					
					LOG(LOG_DEBUG) << "  [" << i << "] " << bindDesc.Name 
								   << " bindPoint=" << bindDesc.BindPoint 
								   << " type=" << typeStr
								   << " dimension=" << dimStr
								   << " numSamples=" << bindDesc.NumSamples;
				}
			}
			reflection->Release();
		}
		else {
			LOG(LOG_WARNING) << "D3D11ShaderProgram: Shader reflection failed for pixel shader";
		}
#endif

		hr = m_device->CreatePixelShader(m_pixelBlob->GetBufferPointer(), m_pixelBlob->GetBufferSize(), NULL, &m_pixelShader);
		if (FAILED(hr)) {
			LOG(LOG_ERROR) << "D3D11ShaderProgram: Could not create pixel shader";
			return false;
		}
		return true;
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