#include <iostream>
#include <math.h>
#include <random>

#include "food.h"
#include "person.h"

using namespace std;

random_device personrd;
uniform_int_distribution<int> personMoveDist(-1, 1);
uniform_int_distribution<int> statDist(1, 15);
uniform_int_distribution<int> lifeSaveDist(1, 10);
uniform_int_distribution<int> procreateDist(1, 30);
uniform_int_distribution<int> killDist(1, 5);

// Balanced makes sure each of the stats are equal
// Neutral will pick a stat to level at random
// Obsessive will exclusively level the highest stat
string trainingFocusList[] = {"balanced", "neutral", "obsessive"};
// Physical stats are vigor, vitality, and strength
// Neutral (or middle) stats are dexterity, endurance, and luck
// Mental stats are intelligence, faith, and attunement
string trainingAreaList[] = {"physical", "neutral", "mental"};

Person::Person(){
    position.x = 0;
    position.y = 0;
    position.w = 32;
    position.h = 32;

    target = NULL;
    
    vigorStat = statDist(personrd);
    attunementStat = statDist(personrd);
    enduranceStat = statDist(personrd);
    vitalityStat = statDist(personrd);
    strengthStat = statDist(personrd);
    dexterityStat = statDist(personrd);
    intelligenceStat = statDist(personrd);
    faithStat = statDist(personrd);
    luckStat = statDist(personrd);

    lifespan = 4800;
    age = 0;

    // Set training personality
    trainingFocus = trainingFocusList[rand() % 3];
    trainingArea = trainingAreaList[rand() % 3];

    // Update the functional stats
    deriveFromStats();
    health = maxHealth;
    currentFood = maxFood;
};

Person::Person(Person* parent1, Person* parent2){
    position.x = (parent1->position.x + parent2->position.x) / 2;
    position.y = (parent1->position.y + parent1->position.y) / 2;
    position.w = 32;
    position.h = 32;

    target = NULL;

    vigorStat = (parent1->vigorStat + parent2->vigorStat) / 2;
    attunementStat = (parent1->attunementStat + parent2->attunementStat) / 2;
    enduranceStat = (parent1->enduranceStat + parent2->enduranceStat) / 2;
    vitalityStat = (parent1->vitalityStat + parent2->vitalityStat) / 2;
    strengthStat = (parent1->strengthStat + parent2->strengthStat) / 2;
    dexterityStat = (parent1->dexterityStat + parent2->dexterityStat) / 2;
    intelligenceStat = (parent1->intelligenceStat + parent2->intelligenceStat) / 2;
    faithStat = (parent1->faithStat + parent2->faithStat) / 2;
    luckStat = (parent1->luckStat + parent2->luckStat) / 2;

    lifespan = 4800;
    age = 0;
    image = parent1->image;

    // Set training personality
    string tempFocusList[] = {parent1->trainingFocus, parent2->trainingFocus};
    trainingFocus = tempFocusList[rand() % 2];
    string tempAreaList[] = {parent1->trainingArea, parent2->trainingArea};
    trainingArea = tempAreaList[rand() % 2];

    // Update the functional stats
    deriveFromStats();
    health = maxHealth;
    currentFood = maxFood;
};

void Person::update(list<Person>& peopleList, list<Food>& foodList, list<Temple>& templeList, int maxPopulation){
    // Verify target exists
    if ((target != NULL && (target->position.w != 32 || target->position.h != 32)) || target == NULL){
        // Target has died before they got there (or because they got there), or just doesn't exist
        task = "nothing";
        target = NULL;
    }
    
    // Update age
    age++;
    if (age > lifespan && age % 60 == 0){
        // Each person gets their luck stat number of throws to see if they get to live
        // They require a single success at 10% odds to succeed
        bool live = false;
        for (int x = 1; x < luck; ++x){
            if (lifeSaveDist(personrd) == 10){
                live = true;
                break;
            }
        }
        if (!live){
            health = 0;
            return;
        }
    }
    
    // Update food
    currentFood = max(0.f, currentFood - foodDepletionRate);
    
    // Reduce health if food is too low
    if (currentFood <= 0){
        health = max(0, health - 10);
    }
    // Increase health if food is high enough
    if (currentFood >= 3 * maxFood / 4){
        health = min(health + 10, maxHealth);
    }

    // Move towards closest food if current amount is low enough
    if (currentFood <= maxFood / 4 && task != "food"){
        task = "food";
        
        // Determine closest food
        Food* closest = NULL;
        float closestDistance = 1000000;
        for (Food& food : foodList){
            float distance = sqrtf(pow(food.position.x - position.x, 2) + pow(food.position.y - position.y, 2));
            if (distance < closestDistance){
                closest = &food;
                closestDistance = distance;
            }
        }

        target = closest;
    }

    // Determine tasks that require another person
    if (task == "nothing" && target == NULL && age % 60 == 0){
        if (age / 60 >= 18 && procreateDist(personrd) == 1 && peopleList.size() + 1 < maxPopulation){
            task = "procreate";
        }
        else if (age / 60 >= 18 && age / 60 <= 80 && (strengthStat > intelligenceStat || dexterityStat > intelligenceStat) && killDist(personrd) == 1){
            task = "murder";
        }
    }
    // Determine other tasks
    if (task == "nothing"){
        task = "train";
    }

    // Determine target
    if (target == NULL && (task == "procreate" || task == "murder")){
        Person* best = NULL;
        int bestScore = 10000000;

        for (Person& person : peopleList){
            // Target is never self
            if (person.id == this->id){
                continue;
            }

            // Determine what to do based on task
            if (task == "procreate"){
                // Skip inelligible
                if (person.age < 18 * 60){
                    continue;
                }

                // Best person is the person considered closest to the current person's stats
                int score = abs(person.statsCombined() - this->statsCombined());
                if (score < bestScore){
                    best = &person;
                    bestScore = score;

                    // Short end loop if score is 0
                    if (score == 0){
                        break;
                    }
                }
            }
            else if (task == "murder"){
                // Skip inelligible
                if (person.age < 18 * 60){
                    continue;
                }

                // Elligible person is a person with a stats considerably lower than their own
                if (person.statsCombined() <= this->statsCombined() - intelligenceStat){
                    best = &person;
                    break;
                }
            }
        }

        target = best;
    }
    else if (target == NULL && task == "train"){
        // Determine stat that will be trained
        StatType trainedStat;

        if (trainingFocus == "balanced"){
            if (trainingArea == "physical"){
                if (vigorStat <= vitalityStat && vigorStat <= strengthStat){
                    trainedStat = vigor;
                }
                else if (vitalityStat <= vigorStat && vitalityStat <= strengthStat){
                    trainedStat = vitality;
                }
                else{
                    trainedStat = strength;
                }
            }
            else if (trainingArea == "neutral"){
                if (dexterityStat <= enduranceStat && dexterityStat <= luckStat){
                    trainedStat = dexterity;
                }
                else if (enduranceStat <= dexterityStat && enduranceStat <= luckStat){
                    trainedStat = endurance;
                }
                else{
                    trainedStat = luck;
                }
            }
            else if (trainingArea == "mental"){
                if (intelligenceStat <= faithStat && intelligenceStat <= attunementStat){
                    trainedStat = intelligence;
                }
                else if (faithStat <= intelligenceStat && faithStat <= attunementStat){
                    trainedStat = faith;
                }
                else{
                    trainedStat = attunement;
                }
            }
        }
        else if (trainingFocus == "neutral"){
            if (trainingArea == "physical"){
                StatType temp[] = {vigor, vitality, strength};
                trainedStat = temp[rand() % 3];
            }
            else if (trainingArea == "neutral"){
                StatType temp[] = {dexterity, endurance, luck};
                trainedStat = temp[rand() % 3];
            }
            else if (trainingArea == "mental"){
                StatType temp[] = {intelligence, faith, attunement};
                trainedStat = temp[rand() % 3];
            }
        }
        else if (trainingFocus == "obsessive"){
            if (trainingArea == "physical"){
                if (vigorStat >= vitalityStat && vigorStat >= strengthStat){
                    trainedStat = vigor;
                }
                else if (vitalityStat >= vigorStat && vitalityStat >= strengthStat){
                    trainedStat = vitality;
                }
                else{
                    trainedStat = strength;
                }
            }
            else if (trainingArea == "neutral"){
                if (dexterityStat >= enduranceStat && dexterityStat >= luckStat){
                    trainedStat = dexterity;
                }
                else if (enduranceStat >= dexterityStat && enduranceStat >= luckStat){
                    trainedStat = endurance;
                }
                else{
                    trainedStat = luck;
                }
            }
            else if (trainingArea == "mental"){
                if (intelligenceStat >= faithStat && intelligenceStat >= attunementStat){
                    trainedStat = intelligence;
                }
                else if (faithStat >= intelligenceStat && faithStat >= attunementStat){
                    trainedStat = faith;
                }
                else{
                    trainedStat = attunement;
                }
            }
        }

        int closest = 1000000;
        for (Temple& temple : templeList){
            if (temple.statType != trainedStat){
                continue;
            }

            float distance = sqrtf(pow(temple.position.x - position.x, 2) + pow(temple.position.y - position.y, 2));
            if (distance < closest){
                closest = distance;
                target = &temple;
            }
        }
    }

    // Verify target exists and at target
    if (target != NULL){
        float distance = sqrtf(pow(target->position.x - position.x, 2) + pow(target->position.y - position.y, 2));
        if (distance <= 2){
            if (task == "food"){
                // Perform task
                currentFood = maxFood;

                // Tasks reset
                task = "nothing";
                target = NULL;
            }
            else if (task == "procreate"){
                // Create person and add them to the list
                Person newPerson(this, (Person*) target);
                peopleList.push_back(newPerson);

                // Tasks reset
                task = "nothing";
                target = NULL;
            }
            else if (task == "murder"){
                // Deal damage to the other person
                auto other = (Person*) target;
                int damage_dealt = max(0, damage - other->defense);
                other->health = max(0, other->health - damage_dealt);

                // Other person deals damage back
                int damage_dealt_back = max(0, other->damage - defense);
                health = max(0, health - damage_dealt_back);

                // Task not reset unless target hit 0 health
                if (other->health == 0){
                    task = "nothing";
                    target = NULL;
                }
            }
            else if (task == "train"){
                auto other = (Temple*) target;
                switch (other->statType){
                    case vigor: vigorStat++; break;
                    case attunement: attunementStat++; break;
                    case endurance: enduranceStat++; break;
                    case vitality: vitalityStat++; break;
                    case strength: strengthStat++; break;
                    case dexterity: dexterityStat++; break;
                    case intelligence: intelligenceStat++; break;
                    case faith: faithStat++; break;
                    case luck: luckStat++; break;
                }

                deriveFromStats();

                task = "nothing";
                target = NULL;
            }

            return;
        }
    }

    // Move towards target
    if (target != NULL){
        updatePosition(target->position.x, target->position.y);
    }
    // Move randomly otherwise
    else{
        updatePosition(personMoveDist(personrd) * speed + position.x, personMoveDist(personrd) * speed + position.y);
    }
}

void Person::updatePosition(int targetX, int targetY){
    if (targetX > position.x){
        position.x = min(position.x + speed, targetX * 1.0f);
    }
    else if (targetX < position.x){
        position.x = max(position.x - speed, targetX * 1.0f);
    }

    if (targetY > position.y){
        position.y = min(position.y + speed, targetY * 1.0f);
    }
    else if (targetY < position.y){
        position.y = max(position.y - speed, targetY * 1.0f);
    }
}

void Person::deriveFromStats(){
    maxHealth = 200 + (vigorStat * 30) + (vitalityStat * 2);
    speed = 1.0 + (enduranceStat / 2.0) + (dexterityStat / 8.0);
    damage = 10 + (strengthStat * 20) + (dexterityStat * 10);
    defense = (vitalityStat * 15) + (strengthStat * 5);

    maxFood = 100.0 + (vitalityStat * 2) + strengthStat;
    foodDepletionRate = 0.1 + (strengthStat / 40);
}

int Person::statsCombined(){
    return vigorStat + attunementStat + enduranceStat + vitalityStat + strengthStat + dexterityStat + intelligenceStat + faithStat + luckStat;
}

string Person::toString(){
    return ("Person " + to_string(id) + ":\n" +
            "Health: " + to_string(health) + "/" + to_string(maxHealth) + "\n" +
            "Age: " + to_string(age/60) + "/" + to_string(lifespan/60) + "\n");
}