#include <SDL.h>
#include <SDL_ttf.h>
#include <string>

#include "HUD.h"
#include "world.h"

using namespace std;

HUD::HUD(SDL_Renderer* rend, World* w){
    gRenderer = rend;
    world = w;

    joystixFont = TTF_OpenFont("fonts/joystix monospace.ttf", 24);
}

void HUD::render(SDL_Point mousePos, int xOffset, int yOffset){
    SDL_Color white = {255, 255, 255};
    mousePos.x -= xOffset;
    mousePos.y -= yOffset;
    
    // Add current year
    string s = "year: " + to_string(world->age / 60);
    addText(s, 10, 10, white);
    
    // Add current population amount
    s = "population: " + to_string(world->peopleList.size());
    addText(s, 10, 10 + 24 * 1 + 10 * 1, white);

    // Display stats of person currently hovered over
    for (Person &person : world->peopleList){
        if (SDL_PointInRect(&mousePos, &person.position)){
            int x = 1100;
            // Display relevant information
            s = "Person:" + to_string(person.id);
            addText(s, x, 10, white);
            s = "Health:" + to_string(person.health) + "/" + to_string(person.maxHealth);
            addText(s, x, 10 + 24 * 1 + 10 * 1, white);
            s = "SPD:" + to_string(person.speed);
            addText(s, x, 10 + 24 * 2 + 10 * 2, white);
            s = "DMG:" + to_string(person.damage) + "|DEF:" + to_string(person.defense);
            addText(s, x, 10 + 24 * 3 + 10 * 3, white);
            s = "Food:" + to_string(int(person.currentFood)) + "/" + to_string(int(person.maxFood));
            addText(s, x, 10 + 24 * 4 + 10 * 4, white);
            s = "Age:" + to_string(person.age / 60) + "/" + to_string(person.lifespan / 60);
            addText(s, x, 10 + 24 * 5 + 10 * 5, white);
            s = "VIG:" + to_string(person.vigorStat) + "|ATT:" + to_string(person.attunementStat) + "|END:" + to_string(person.enduranceStat);
            addText(s, x, 10 + 24 * 6 + 10 * 6, white);
            s = "VIT:" + to_string(person.vitalityStat) + "|STR:" + to_string(person.strengthStat) + "|DEX:" + to_string(person.dexterityStat);
            addText(s, x, 10 + 24 * 7 + 10 * 7, white);
            s = "INT:" + to_string(person.intelligenceStat) + "|FTH:" + to_string(person.faithStat) + "|LCK:" + to_string(person.luckStat);
            addText(s, x, 10 + 24 * 8 + 10 * 8, white);
            s = person.trainingFocus + " " + person.trainingArea;
            addText(s, x, 10 + 24 * 9 + 10 * 9, white);

            break;
        }
    }

    addText(to_string(mousePos.x) + " " + to_string(mousePos.y), 10, 1000, white);
}

void HUD::addText(string text, int x, int y, SDL_Color color){
    SDL_Surface* surfaceMessage = TTF_RenderText_Solid(joystixFont, text.c_str(), color);
    SDL_Texture* message = SDL_CreateTextureFromSurface(gRenderer, surfaceMessage);

    SDL_Rect messageRect;
    messageRect.x = x;
    messageRect.y = y;
    messageRect.w = text.length() * 24;
    messageRect.h = 24;

    SDL_RenderCopy(gRenderer, message, NULL, &messageRect);

    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(message);
}