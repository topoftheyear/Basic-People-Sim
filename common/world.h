#include <list>

#include "food.h"
#include "person.h"
#include "temple.h"

#ifndef __WORLD_H_INCLUDED__
#define __WORLD_H_INCLUDED__

class World{
    public:
        World(int, int, int);

        void initialize(ChromaTexture*, ChromaTexture*, ChromaTexture*);
        void update();
        void render(SDL_Renderer*, int, int);
        bool onScreen(int, int, int, int);

        std::list<Person> peopleList;
        std::list<Food> foodList;
        std::list<Temple> templeList;

        int age;
        
        int startingPopulation;
        int startingFood;
        int startingTemples; // Sets of temples, so the 1 = 9 temples, 2 = 18 temples, etc

    private:
        int maxPopulation;
        int maxFood;
};

#endif