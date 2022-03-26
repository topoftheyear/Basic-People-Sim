#include "game_object.h"

#ifndef __FOOD_H_INCLUDED__
#define __FOOD_H_INCLUDED__

class Food: public GameObject{
    public:
        Food();

        int age;
        float currentFood;

        void update();
        void removeFood(float);
};

#endif