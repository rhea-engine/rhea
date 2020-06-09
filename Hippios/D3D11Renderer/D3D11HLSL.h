#include <d3d11.h>
#include <d3d11shader.h>
#include <d3dcompiler.h>

#include "../GPUProgram.h"

namespace Hippios
{
	class D3D11HLSL : public GPUProgram
	{
	protected:
		ID3D11Device* m_d3dDevice;
		ID3D11DeviceContext* m_d3dDeviceContext;
		
		ID3DBlob* shaderBlob = nullptr;
		ID3DBlob* errorBlob = nullptr;
		ID3D11InputLayout* mInputLayout = 0;

		// the shaders
		ID3D11VertexShader* pVS = 0;    // the vertex shader
		ID3D11PixelShader* pPS = 0;     // the pixel shader

		int m_vertex_type = 0;

	public:
		D3D11HLSL::D3D11HLSL(ID3D11Device* d3dDevice, ID3D11DeviceContext* d3dDeviceContext);
		HRESULT LoadFromFile(std::string filename, LPCSTR entrypoint, LPCSTR shaderType, LPCSTR shaderTypeId);
		HRESULT LoadFromBuffer(unsigned char* buf, size_t buflen, LPCSTR entrypoint, LPCSTR shaderType, LPCSTR shaderTypeId);
		HRESULT D3D11HLSL::defaultShader(LPCSTR entryPoint, LPCSTR shaderType, LPCSTR shaderTypeId);
		ID3D11InputLayout* GetInputLayout();
		void setVertexType(int type);
		ID3DBlob* get_code() { return shaderBlob; };
		ID3D11VertexShader* getVertexShader() { return pVS; }
		ID3D11PixelShader* getPixelShader() { return pPS; }
	};
}