#ifndef HIPPIOS_D3D11SHADERPROGRAM
#define HIPPIOS_D3D11SHADERPROGRAM

#include <Hippios/ShaderProgram.h>
#include <d3d11.h>

namespace hippios
{
    class D3D11ShaderProgram : public ShaderProgram
    {
    public:
        D3D11ShaderProgram(ID3D11Device* device);
        ~D3D11ShaderProgram() override;

        bool loadFromFiles(
            const std::string& vertexFile,
            const std::string& vertexEntry,
            const std::string& pixelFile,
            const std::string& pixelEntry);

        bool loadFromSource(
            const unsigned char* vertexSource,
            size_t vertexLength,
            const std::string& vertexEntry,
            const unsigned char* pixelSource,
            size_t pixelLength,
            const std::string& pixelEntry);

        ID3D11VertexShader* getVertexShader() const { return m_vertexShader; }
        ID3D11PixelShader* getPixelShader() const { return m_pixelShader; }

        ID3DBlob* getVertexBlob() const { return m_vertexBlob; }
        ID3DBlob* getPixelBlob() const { return m_pixelBlob; }

    private:
        bool compileVertexShaderFromFile(const std::string& filename, const std::string& entryPoint);
        bool compilePixelShaderFromFile(const std::string& filename, const std::string& entryPoint);

        bool compileVertexShaderFromSource(const unsigned char* buf, size_t buflen, const std::string& entryPoint);
        bool compilePixelShaderFromSource(const unsigned char* buf, size_t buflen, const std::string& entryPoint);

        void releaseResources();

    private:
        ID3D11Device* m_device = nullptr;

        ID3D11VertexShader* m_vertexShader = nullptr;
        ID3D11PixelShader* m_pixelShader = nullptr;

        // useful for input layout creation
        ID3DBlob* m_vertexBlob = nullptr;
        ID3DBlob* m_pixelBlob = nullptr; 
    };
}

#endif // !HIPPIOS_D3D11SHADERPROGRAM
