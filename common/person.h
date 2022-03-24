#include <list>
#include <rpc.h>
#include <string>

#include "chroma_texture.h"
#include "game_object.h"
#include "stat.h"
#include "temple.h"

#ifndef __PERSON_H_INCLUDED__
#define __PERSON_H_INCLUDED__

class Person: public GameObject{
    public:
        Person();
        Person(Person*, Person*);
        
        void update(std::list<Person>&, std::list<Food>&, std::list<Temple>&, int);
        void updatePosition(int, int);
        void deriveFromStats();
        int statsCombined();
        std::string toString();

        GameObject* target;   // The target of whatever task is at hand
        std::string task;   // The task that will get carried out at the target

        // Required inputs
        Stat vigorStat;
        Stat attunementStat;
        Stat enduranceStat;
        Stat vitalityStat;
        Stat strengthStat;
        Stat dexterityStat;
        Stat intelligenceStat;
        Stat faithStat;
        Stat luckStat;
        
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

        // XP
        int XPvigor;
        int XPattunement;
        int XPendurance;
        int XPvitality;
        int XPstrength;
        int XPdexterity;
        int XPintelligence;
        int XPfaith;
        int XPluck;

        int XPLevelMultiplier = 100;

        // Training personality
        std::string trainingFocus;
        std::string trainingArea;
};

#endif