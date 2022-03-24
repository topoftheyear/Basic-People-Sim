#include "stat.h"

Stat::Stat(){
	level = -1;
	currentXP = -1;
	XPToNextLevel = -1;
}

Stat::Stat(int startingLevel){
    level = startingLevel;
	currentXP = 0;
	XPToNextLevel = level * 3;
}

void Stat::addXP(int amountToAdd){
    currentXP += amountToAdd;

	if (currentXP >= XPToNextLevel){
		level++;
		currentXP -= XPToNextLevel;
		XPToNextLevel = level * 3;
	}
}

int Stat::getLevel(){
	return level;
}

int Stat::getXP(){
	return currentXP;
}