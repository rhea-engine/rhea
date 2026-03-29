#ifndef PSIO_BACKEND_H
#define PSIO_BACKEND_H

#include <Psio/Psio.h>

#include <memory>

namespace Psio
{
    class Input;
    class Window;
	class Network;
	class IO;
	class Fs;

    class IBackend
    {
    public:
        virtual ~IBackend() = default;

        virtual const char* GetName() const = 0;

        virtual std::unique_ptr<Input> CreateInput(Engine* engine) = 0;
        virtual std::unique_ptr<Window> CreateNativeWindow(std::string name, int width, int height) = 0;
        virtual std::unique_ptr<Network> CreateNetwork(Engine* engine) = 0;
        virtual std::unique_ptr<IO> CreateIO(Engine* engine) = 0;
        virtual std::unique_ptr<Fs> CreateFs(Engine* engine) = 0;

        virtual uint64_t GetTicks() = 0;
    };
}
#endif // !PSIO_BACKEND_H
