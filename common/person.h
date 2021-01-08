#include <list>
#include <rpc.h>
#include <string>

#include "chroma_texture.h"
#include "game_object.h"

#ifndef __PERSON_H_INCLUDED__
#define __PERSON_H_INCLUDED__

class Person: public GameObject{
    public:
        Person();
        Person(Person*, Person*);
        
        void update(std::list<Person>&, std::list<Food>&, int);
        void updatePosition(int, int);
        void deriveFromStats();
        int statsCombined();
        std::string toString();

        GameObject* target;   // The target of whatever task is at hand
        std::string task;   // The task that will get carried out at the target

        // Required inputs
        int vigor;
        int attunement;
        int endurance;
        int vitality;
        int strength;
        int dexterity;
        int intelligence;
        int faith;
        int luck;
        
        // Derived stats
        int maxHealth;
        int health;
        float speed;
        int damage;
        int defense;

        // Food stats
        float maxFood;
        float currentFood;
        float foodDepletionRate;

        // Age stats
        int lifespan;
        int age;
};

#endif