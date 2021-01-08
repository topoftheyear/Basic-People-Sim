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

Person::Person(){
    position.x = 0;
    position.y = 0;
    position.w = 32;
    position.h = 32;

    target = NULL;
    
    vigor = statDist(personrd);
    attunement = statDist(personrd);
    endurance = statDist(personrd);
    vitality = statDist(personrd);
    strength = statDist(personrd);
    dexterity = statDist(personrd);
    intelligence = statDist(personrd);
    faith = statDist(personrd);
    luck = statDist(personrd);

    lifespan = 4800;
    age = 0;

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

    vigor = (parent1->vigor + parent2->vigor) / 2;
    attunement = (parent1->attunement + parent2->attunement) / 2;
    endurance = (parent1->endurance + parent2->endurance) / 2;
    vitality = (parent1->vitality + parent2->vitality) / 2;
    strength = (parent1->strength + parent2->strength) / 2;
    dexterity = (parent1->dexterity + parent2->dexterity) / 2;
    intelligence = (parent1->intelligence + parent2->intelligence) / 2;
    faith = (parent1->faith + parent2->faith) / 2;
    luck = (parent1->luck + parent2->luck) / 2;

    lifespan = 4800;
    age = 0;
    image = parent1->image;
    
    // Update the functional stats
    deriveFromStats();
    health = maxHealth;
    currentFood = maxFood;
};

void Person::update(list<Person>& peopleList, list<Food>& foodList, int maxPopulation){
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
        else if (age / 60 >= 18 && age / 60 <= 80 && (strength > intelligence || dexterity > intelligence) && killDist(personrd) == 1){
            task = "murder";
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
                if (person.statsCombined() <= this->statsCombined() - intelligence){
                    best = &person;
                    break;
                }
            }
        }

        target = best;
    }

    // Verify target exists and at target
    if (target != NULL){
        float distance = sqrtf(pow(target->position.x - position.x, 2) + pow(target->position.y - position.y, 2));
        if (distance <= 1){
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
    maxHealth = 200 + (vigor * 30) + (vitality * 2);
    speed = 1.0 + (endurance / 2.0) + (dexterity / 8.0);
    damage = 10 + (strength * 20) + (dexterity * 10);
    defense = (vitality * 15) + (strength * 5);

    maxFood = 100.0 + (vitality * 2) + strength;
    foodDepletionRate = 0.1 + (strength / 40);
}

int Person::statsCombined(){
    return vigor + attunement + endurance + vitality + strength + dexterity + intelligence + faith + luck;
}

string Person::toString(){
    return ("Person " + to_string(id) + ":\n" +
            "Health: " + to_string(health) + "/" + to_string(maxHealth) + "\n" +
            "Age: " + to_string(age/60) + "/" + to_string(lifespan/60) + "\n");
}