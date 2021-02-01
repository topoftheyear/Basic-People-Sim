#include "enums.h"
#include "game_object.h"

#ifndef __TEMPLE_H_INCLUDED__
#define __TEMPLE_H_INCLUDED__

class Temple: public GameObject{
    public:
        Temple(StatType);

        void update();

        StatType statType;
};

#endif