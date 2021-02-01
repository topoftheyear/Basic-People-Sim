#include <random>

#include "temple.h"

std::random_device templerd;
std::uniform_int_distribution<int> templePosDist(-5000, 5000);

Temple::Temple(StatType stype){
    position.x = templePosDist(templerd);
    position.y = templePosDist(templerd);
    position.w = 64;
    position.h = 64;

    statType = stype;
}

void Temple::update(){
    // Does nothing for now
}