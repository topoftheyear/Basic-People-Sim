#include <SDL.h>

#include "chroma_texture.h"

#ifndef __GAME_OBJECT_H_INCLUDED__
#define __GAME_OBJECT_H_INCLUDED__

class GameObject{
    public:
        GameObject();
        
        SDL_Rect position;
        ChromaTexture* image;

        int id;
        bool dead;

        bool operator == (const GameObject& p) const { return id == p.id; }
        bool operator != (const GameObject& p) const { return !operator==(p); }
    private:
        static int currentID;
};

#endif