/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_INTEGRATION_PSIOINPUTADAPTER_H
#define RHEA_INTEGRATION_PSIOINPUTADAPTER_H

#include "Rhea/Input/InputEvent.h"
#include <Psio/Input/InputEvent.h>
#include "RheaClient/Window/WindowService.h"

namespace rhea::client
{
    class PsioInputAdapter
    {
    public:
        static bool translate(const psio::InputEvent& src, InputEvent& dst);
        static WindowRegistryService* m_window_service;

    protected:
        static Key translateKey(uint32_t code);
        static MouseButton translateMouseButton(const psio::MouseInputEvent& e);
    };
}

#endif