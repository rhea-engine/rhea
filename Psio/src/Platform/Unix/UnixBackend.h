#ifndef PSIO_UNIX_BACKEND_H
#define PSIO_UNIX_BACKEND_H

#include "../../Backend/IBackend.h"

namespace Psio
{
    class UnixBackend final : public IBackend
    {
    public:
        UnixBackend();
        ~UnixBackend() override;

        const char* GetName() const override;

        std::unique_ptr<Input> CreateInput(Engine* engine) override;
        std::unique_ptr<Window> CreateNativeWindow(std::string name, int width, int height, WindowOption options) override;
        std::unique_ptr<Network> CreateNetwork(Engine* engine) override;
        std::unique_ptr<IO> CreateIO(Engine* engine) override;
        std::unique_ptr<Fs> CreateFs(Engine* engine) override;

        uint64_t GetTicks() override;
    };
}
#endif // !PSIO_UNIX_BACKEND_H