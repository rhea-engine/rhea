/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef PSIO_BACKEND_H
#define PSIO_BACKEND_H

#include <Psio/Psio.h>

#include <memory>

namespace psio
{
    class Input;
    class Window;
	class Network;
	class IO;
	class Fs;
	class Clipboard;

    class IBackend
    {
    public:
        virtual ~IBackend() = default;

        virtual const char* GetName() const = 0;

        virtual std::unique_ptr<Input> CreateInput(Engine* engine) = 0;
        virtual std::unique_ptr<Window> CreateNativeWindow(WindowDesc desc) = 0;
        virtual std::unique_ptr<Network> CreateNetwork(Engine* engine) = 0;
        virtual std::unique_ptr<IO> CreateIO(Engine* engine) = 0;
        virtual std::unique_ptr<Fs> CreateFs(Engine* engine) = 0;
		virtual std::unique_ptr<Clipboard> CreateClipboard(Engine* engine) = 0;

        virtual uint64_t GetTicks() = 0;
    };
}
#endif // !PSIO_BACKEND_H
