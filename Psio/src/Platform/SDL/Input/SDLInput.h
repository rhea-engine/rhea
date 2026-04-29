/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef PSIO_SDLINPUT_H
#define PSIO_SDLINPUT_H

#include <Psio/Input/Input.h>

namespace psio
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