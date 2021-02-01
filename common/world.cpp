#include <chrono>
#include <ctime>
#include <iostream>
#include <SDL.h>
#include <stdio.h>

#include "enums.h"
#include "world.h"

World::World(int population, int food, int temples){
    age = 0;
    
    startingPopulation = population;
    startingFood = food;
    startingTemples = temples;

    maxPopulation = 80000;
    maxFood = 1000;
}

void World::initialize(ChromaTexture* personTexture, ChromaTexture* foodTexture, ChromaTexture* templeTexture){
    // Create list of people
    printf("Creating %i people.\n", startingPopulation);
    for (int i = 0; i < startingPopulation; ++i){
        Person temp;
        temp.image = personTexture;

        peopleList.push_back(temp);
    }

    // Create list of food
    printf("Creating %i food.\n", startingFood);
    for (int i = 0; i < startingFood; ++i){
        Food temp;
        temp.image = foodTexture;

        foodList.push_back(temp);
    }

    // Create list of temples
    printf("Creating %i sets of temples.\n", startingTemples);
    for (int i = 0; i < startingTemples; ++i){
        for (int statInt = vigor; statInt < luck + 1; statInt++){
            StatType stype = static_cast<StatType>(statInt);

            Temple temp(stype);
            temp.image = templeTexture;

            templeList.push_back(temp);
        }
    }

    printf("World initialization complete.\n");
}

void World::update(){
    age++;
    
    // Update each person
    std::list<Person>::iterator i = peopleList.begin();
    while (i != peopleList.end()){
        auto person = (i);
        person->update(peopleList, foodList, templeList, maxPopulation);

        // Check for death of each person
        if (person->health == 0){
            i = peopleList.erase(i);
        }

        ++i;
    }
    
    // Update each food
    for (Food &food : foodList){
        food.update();
    }

    // Update each temple
    for (Temple &temple : templeList){
        temple.update();
    }
}

void World::render(SDL_Renderer* gRenderer, int xOffset, int yOffset){
    // Render each food
    for (Food &food : foodList){
        food.image->render(food.position.x + xOffset, food.position.y + yOffset, gRenderer);
    }

    // Render each temple
    for (Temple &temple : templeList){
        temple.image->render(temple.position.x + xOffset, temple.position.y + yOffset, gRenderer);
    }

    // Render each person
    for (Person &person : peopleList){
        person.image->render(person.position.x + xOffset, person.position.y + yOffset, gRenderer);
    }

    
}