#ifndef PSIO_WIN32_BACKEND_H
#define PSIO_WIN32_BACKEND_H

#include "../../Backend/IBackend.h"

namespace Psio
{
    class Win32Backend final : public IBackend
    {
    public:
        Win32Backend();
        ~Win32Backend() override;

        const char* GetName() const override;

        std::unique_ptr<Input> CreateInput(Engine* engine) override;
        std::unique_ptr<Window> CreateNativeWindow(WindowDesc desc) override;
		std::unique_ptr<Network> CreateNetwork(Engine* engine) override;
		std::unique_ptr<IO> CreateIO(Engine* engine) override;
		std::unique_ptr<Fs> CreateFs(Engine* engine) override;

        uint64_t GetTicks() override;
    };
}
#endif // !PSIO_WIN32_BACKEND_H
