#include <list>

#include "food.h"
#include "person.h"

#ifndef __WORLD_H_INCLUDED__
#define __WORLD_H_INCLUDED__

class World{
    public:
        World(int, int);

        void initialize(ChromaTexture*, ChromaTexture*);
        void update();
        void render(SDL_Renderer*, int, int);

        std::list<Person> peopleList;
        std::list<Food> foodList;

        int age;
        
        int startingPopulation;
        int startingFood;

    private:
        int maxPopulation;
        int maxFood;
};

#endif