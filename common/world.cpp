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
    std::list<Person>::iterator p = peopleList.begin();
    while (p != peopleList.end()){
        auto person = (p);
        person->update(peopleList, foodList, templeList, maxPopulation);

        // Check for death of each person
        if (person->dead){
            p = peopleList.erase(p);
        }

        ++p;
    }
    
    // Update each food
    std::list<Food>::iterator f = foodList.begin();
    while (f != foodList.end()){
        auto food = (f);
        food->update();

        // Check for expiry of each farm
        if (food->dead){
            f = foodList.erase(f);
        }

        ++f;
    }

    // Update each temple
    for (Temple &temple : templeList){
        temple.update();
    }
}

void World::render(SDL_Renderer* gRenderer, int xOffset, int yOffset){
    // Render each food
    for (Food &food : foodList){
        int tempX = food.position.x + xOffset;
        int tempY = food.position.y + yOffset;
        if (onScreen(tempX, tempY, 1920, 1080)){
            food.image->render(tempX, tempY, gRenderer);
        }
    }

    // Render each temple
    for (Temple &temple : templeList){
        int tempX = temple.position.x + xOffset;
        int tempY = temple.position.y + yOffset;
        if (onScreen(tempX, tempY, 1920, 1080)){
            temple.image->render(tempX, tempY, gRenderer);
        }
    }

    // Render each person
    for (Person &person : peopleList){
        int tempX = person.position.x + xOffset;
        int tempY = person.position.y + yOffset;
        if (onScreen(tempX, tempY, 1920, 1080)){
            person.image->render(tempX, tempY, gRenderer);
        }
    }

    
}

bool World::onScreen(int x, int y, int width, int height) {
    bool visible = false;

    if (-100 < x && x < width + 100 && -100 < y && y < height + 100) {
        visible = true;
    }

    return visible;
}