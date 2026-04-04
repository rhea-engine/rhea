#ifndef PSIO_SDLKEYTRANSLATION_H
#define PSIO_SDLKEYTRANSLATION_H    

#include <Psio/Input/Keyboard.h>

#include <SDL3/SDL_scancode.h>

namespace psio
{
    Key TranslateSDLScancode(SDL_Scancode scancode);
}
#endif // !PSIO_SDLKEYTRANSLATION_H