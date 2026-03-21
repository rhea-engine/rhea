#ifndef HIPPIOS_SHADERPROGRAM_H
#define HIPPIOS_SHADERPROGRAM_H

#include <string>
#include <Hippios/BaseTypes.h>

namespace Hippios
{
    struct ShaderProgramDesc
    {
        std::string vertexFile;
        std::string vertexEntry = "VSMain";

        std::string pixelFile;
        std::string pixelEntry = "PSMain";
    };

    struct ShaderProgramSourceDesc
    {
        const unsigned char* vertexSource = nullptr;
        size_t vertexSourceLength = 0;
        std::string vertexEntry = "VSMain";

        const unsigned char* pixelSource = nullptr;
        size_t pixelSourceLength = 0;
        std::string pixelEntry = "PSMain";
    };

    class ShaderProgram
    {
    public:
        virtual ~ShaderProgram() = default;
    };
}

#endif