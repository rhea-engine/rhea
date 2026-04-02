#ifndef PSIO_SDLINPUT_H
#define PSIO_SDLINPUT_H

#include <Psio/Input/Input.h>

namespace Psio
{
    class Engine;

    class SDLInput final : public Input
    {
    public:
        explicit SDLInput(Engine* engine);
        ~SDLInput() override = default;

        void update() override;

    private:
        Engine* m_engine = nullptr;
    };
}
#endif // !PSIO_SDLINPUT_H