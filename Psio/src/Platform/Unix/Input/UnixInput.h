/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef PSIO_INPUT_UNIX_H
#define PSIO_INPUT_UNIX_H

#include <Psio/Input/Input.h>

namespace psio
{
    class InputUnix : public Input
    {
    public:
        InputUnix() = default;
        ~InputUnix() override = default;
        void update() override;
    };
}
#endif // !PSIO_INPUT_UNIX_H