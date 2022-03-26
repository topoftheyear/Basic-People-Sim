#include <iostream>
#include <math.h>
#include <random>
#include <vector>

#include "food.h"
#include "person.h"
#include "stat.h"

using namespace std;

random_device personrd;
uniform_int_distribution<int> personMoveDist(-1, 1);
uniform_int_distribution<int> statDist(1, 15);
uniform_real_distribution<double> lifeSaveDist(0, 1);
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
    
    vigorStat = Stat(statDist(personrd));
    attunementStat = Stat(statDist(personrd));
    enduranceStat = Stat(statDist(personrd));
    vitalityStat = Stat(statDist(personrd));
    strengthStat = Stat(statDist(personrd));
    dexterityStat = Stat(statDist(personrd));
    intelligenceStat = Stat(statDist(personrd));
    faithStat = Stat(statDist(personrd));
    luckStat = Stat(statDist(personrd));

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

    vigorStat = Stat((parent1->vigorStat.getLevel() + parent2->vigorStat.getLevel()) / 2);
    attunementStat = Stat((parent1->attunementStat.getLevel() + parent2->attunementStat.getLevel()) / 2);
    enduranceStat = Stat((parent1->enduranceStat.getLevel() + parent2->enduranceStat.getLevel()) / 2);
    vitalityStat = Stat((parent1->vitalityStat.getLevel() + parent2->vitalityStat.getLevel()) / 2);
    strengthStat = Stat((parent1->strengthStat.getLevel() + parent2->strengthStat.getLevel()) / 2);
    dexterityStat = Stat((parent1->dexterityStat.getLevel() + parent2->dexterityStat.getLevel()) / 2);
    intelligenceStat = Stat((parent1->intelligenceStat.getLevel() + parent2->intelligenceStat.getLevel()) / 2);
    faithStat = Stat((parent1->faithStat.getLevel() + parent2->faithStat.getLevel()) / 2);
    luckStat = Stat((parent1->luckStat.getLevel() + parent2->luckStat.getLevel()) / 2);

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
        // Each person has their luck stat put through this formula then compared against a random number
        bool live = lifeSaveDist(personrd) < log((double)luckStat.getLevel() + 1.0) / 8.0;

        if (!live){
            health = 0;
            dead = true;
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
        
        // Determine if it will use smart selection
        // Won't take food from spot if it will result in its destruction
        bool smartSelection = (float)intelligenceStat.getLevel() / (float)this->statsCombined() > 0.11;

        float foodNeeded = maxFood - currentFood;

        // Determine closest food
        Food* closest = NULL;
        float closestDistance = 1000000;
        for (Food& food : foodList){
            float distance = sqrtf(pow(food.position.x - position.x, 2) + pow(food.position.y - position.y, 2));
            
            // Smart selection short stop
            if (smartSelection && food.currentFood < foodNeeded){
                continue;
            }
            
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
        else if (age / 60 >= 18 && age / 60 <= 80 && (strengthStat.getLevel() > intelligenceStat.getLevel() || dexterityStat.getLevel() > intelligenceStat.getLevel()) && killDist(personrd) == 1){
            task = "murder";
        }
    }

    // Determine other tasks
    if (task == "nothing"){
        vector<int> statDist = {
            vigorStat.getLevel(),
            attunementStat.getLevel(),
            enduranceStat.getLevel(),
            vitalityStat.getLevel(),
            strengthStat.getLevel(),
            dexterityStat.getLevel(),
            intelligenceStat.getLevel(),
            faithStat.getLevel(),
            luckStat.getLevel(),
            statsCombined() / 2,
        };
        
        discrete_distribution<int> d(statDist.begin(), statDist.end());
        switch(d(personrd)){
            case 0: 
                // Vigor
                task = "train";
                break;
            case 1: 
                // Attunement
                task = "train";
                break;
            case 2: 
                // Endurance
                task = "train";
                break;
            case 3: 
                // Vitality
                task = "train";
                break;
            case 4: 
                // Strength
                task = "train";
                break;
            case 5: 
                // Dexterity
                task = "train";
                break;
            case 6: 
                // Intelligence
                task = "train";
                break;
            case 7: 
                // Faith
                task = "train";
                break;
            case 8: 
                // Luck
                task = "train";
                break;
            case 9: 
                // Train
                task = "train";
                break;
            default: 
                task = "train";
        }
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
                if (person.statsCombined() <= this->statsCombined() - intelligenceStat.getLevel()){
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
                if (vigorStat.getLevel() <= vitalityStat.getLevel() && vigorStat.getLevel() <= strengthStat.getLevel()){
                    trainedStat = vigor;
                }
                else if (vitalityStat.getLevel() <= vigorStat.getLevel() && vitalityStat.getLevel() <= strengthStat.getLevel()){
                    trainedStat = vitality;
                }
                else{
                    trainedStat = strength;
                }
            }
            else if (trainingArea == "neutral"){
                if (dexterityStat.getLevel() <= enduranceStat.getLevel() && dexterityStat.getLevel() <= luckStat.getLevel()){
                    trainedStat = dexterity;
                }
                else if (enduranceStat.getLevel() <= dexterityStat.getLevel() && enduranceStat.getLevel() <= luckStat.getLevel()){
                    trainedStat = endurance;
                }
                else{
                    trainedStat = luck;
                }
            }
            else if (trainingArea == "mental"){
                if (intelligenceStat.getLevel() <= faithStat.getLevel() && intelligenceStat.getLevel() <= attunementStat.getLevel()){
                    trainedStat = intelligence;
                }
                else if (faithStat.getLevel() <= intelligenceStat.getLevel() && faithStat.getLevel() <= attunementStat.getLevel()){
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
                if (vigorStat.getLevel() >= vitalityStat.getLevel() && vigorStat.getLevel() >= strengthStat.getLevel()){
                    trainedStat = vigor;
                }
                else if (vitalityStat.getLevel() >= vigorStat.getLevel() && vitalityStat.getLevel() >= strengthStat.getLevel()){
                    trainedStat = vitality;
                }
                else{
                    trainedStat = strength;
                }
            }
            else if (trainingArea == "neutral"){
                if (dexterityStat.getLevel() >= enduranceStat.getLevel() && dexterityStat.getLevel() >= luckStat.getLevel()){
                    trainedStat = dexterity;
                }
                else if (enduranceStat.getLevel() >= dexterityStat.getLevel() && enduranceStat.getLevel() >= luckStat.getLevel()){
                    trainedStat = endurance;
                }
                else{
                    trainedStat = luck;
                }
            }
            else if (trainingArea == "mental"){
                if (intelligenceStat.getLevel() >= faithStat.getLevel() && intelligenceStat.getLevel() >= attunementStat.getLevel()){
                    trainedStat = intelligence;
                }
                else if (faithStat.getLevel() >= intelligenceStat.getLevel() && faithStat.getLevel() >= attunementStat.getLevel()){
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
                auto other = (Food*) target;
                float foodGained = min(maxFood - currentFood, other->currentFood);
                other->removeFood(foodGained);
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
                    case vigor: vigorStat.addXP(1); break;
                    case attunement: attunementStat.addXP(1); break;
                    case endurance: enduranceStat.addXP(1); break;
                    case vitality: vitalityStat.addXP(1); break;
                    case strength: strengthStat.addXP(1); break;
                    case dexterity: dexterityStat.addXP(1); break;
                    case intelligence: intelligenceStat.addXP(1); break;
                    case faith: faithStat.addXP(1); break;
                    case luck: luckStat.addXP(1); break;
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

    // Flag dead if no health remaining
    if (health <= 0){
        dead = true;
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
    maxHealth = 200 + (vigorStat.getLevel() * 30) + (vitalityStat.getLevel() * 2);
    speed = 1.0 + (enduranceStat.getLevel() / 4.0) + (dexterityStat.getLevel() / 16.0);
    damage = 10 + (strengthStat.getLevel() * 20) + (dexterityStat.getLevel() * 10);
    defense = (vitalityStat.getLevel() * 15) + (strengthStat.getLevel() * 5);

    maxFood = 100.0 + (vitalityStat.getLevel() * 2) + strengthStat.getLevel();
    foodDepletionRate = 0.1 + (strengthStat.getLevel() / 40);
}

int Person::statsCombined(){
    return vigorStat.getLevel() + attunementStat.getLevel() + enduranceStat.getLevel() + vitalityStat.getLevel() + strengthStat.getLevel() + dexterityStat.getLevel() + intelligenceStat.getLevel() + faithStat.getLevel() + luckStat.getLevel();
}

string Person::toString(){
    return ("Person " + to_string(id) + ":\n" +
            "Health: " + to_string(health) + "/" + to_string(maxHealth) + "\n" +
            "Age: " + to_string(age/60) + "/" + to_string(lifespan/60) + "\n");
}