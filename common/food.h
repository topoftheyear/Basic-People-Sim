#include <SDL.h>

#include "chroma_texture.h"
#include "game_object.h"

#ifndef __FOOD_H_INCLUDED__
#define __FOOD_H_INCLUDED__

class Food: public GameObject{
    public:
        Food();

        void update();
};

#endif