#include <random>

#include "food.h"

std::random_device foodrd;
std::uniform_int_distribution<int> posDist(-3000, 3000);

Food::Food(){
    position.x = posDist(foodrd);
    position.y = posDist(foodrd);
    position.w = 32;
    position.h = 32;
}

void Food::update(){
    // Does nothing for now
}