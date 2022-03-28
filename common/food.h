#include "game_object.h"

#include "stat.h"

#ifndef __FOOD_H_INCLUDED__
#define __FOOD_H_INCLUDED__

class Food: public GameObject{
    public:
        Food();

        int age;
        float currentFood;
        float maxFood;
        Stat stat;

        void update();
        void removeFood(float);
};

#endif