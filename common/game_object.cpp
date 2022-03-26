#include "game_object.h"

int GameObject::currentID = 0;

GameObject::GameObject(){
    id = ++currentID;
    dead = false;
}