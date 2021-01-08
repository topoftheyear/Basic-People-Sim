#include <SDL.h>
#include <string>

#include "world.h"

#ifndef __HUD_H_INCLUDED__
#define __HUD_H_INCLUDED__

class HUD{
    public:
        HUD(SDL_Renderer*, World*);
        
        void render(SDL_Point, int, int);

        SDL_Renderer* gRenderer;
        World* world;
        TTF_Font* joystixFont;
    private:
        void addText(std::string, int, int, SDL_Color);
};

#endif