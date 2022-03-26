#include <random>

#include "food.h"

std::random_device foodrd;
std::uniform_int_distribution<int> foodPosDist(-3000, 3000);

Food::Food(){
    position.x = foodPosDist(foodrd);
    position.y = foodPosDist(foodrd);
    position.w = 32;
    position.h = 32;

    age = 0;
    currentFood = 0.0f;
}

void Food::update(){
    // Flag food if none remain
    if (currentFood <= 0 && age > 0){
        dead = true;
    }
    
    age++;
    currentFood += 1.0f;
}

void Food::removeFood(float amount){
    currentFood = std::max(0.0f, currentFood - amount);
}