#include <iostream>
#include <list>
#include <random>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <sstream>
#include <stdio.h>
#include <string>

#include "common/chroma_texture.h"
#include "common/enums.h"
#include "common/food.h"
#include "common/HUD.h"
#include "common/person.h"
#include "common/timer.h"
#include "common/world.h"
using namespace std;

const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1080;

bool init();
bool loadMedia();
void close();
void setWindow();

SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;
ChromaTexture personTexture;
ChromaTexture foodTexture;
ChromaTexture templeTexture;


bool init(){
    bool success = true;

    if (SDL_Init(SDL_INIT_VIDEO) < 0){
        printf("SDL could not initialize. SDL_Error: %s\n", SDL_GetError());
        success = false;
    }
    else{
        // Create Window
        gWindow = SDL_CreateWindow("Person Simulation", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

        if (gWindow == NULL){
            printf("Window could not be created. SDL_Error: %s\n", SDL_GetError());
            success = false;
        }
        else{
            // Create Renderer
            gRenderer = SDL_CreateRenderer(gWindow, 01, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
            //gRenderer = SDL_CreateRenderer(gWindow, 01, SDL_RENDERER_ACCELERATED);
            if (gRenderer == NULL){
                printf("Renderer could not be created. SDL Error: %s\n", SDL_GetError());
                success = false;
            }
            else{
                SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

                // PNG Loading
                int imgFlags = IMG_INIT_PNG;
                if (!(IMG_Init(imgFlags) & imgFlags)){
                    printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
                    success = false;
                }
            }
        }
    }

    if (TTF_Init() == -1){
        printf("TTF_Init: %s\n", TTF_GetError());
        success = false;
    }

    return success;
}

bool loadMedia(){
    bool success = true;

    //Load default surface
    if (!personTexture.loadFromFile("images/person.png", gRenderer)){
        printf("Failed to load texture image (person).");
        success = false;
    }

    if (!foodTexture.loadFromFile("images/food.png", gRenderer)){
        printf("Failed to load texture image (food).");
        success = false;
    }

    if (!templeTexture.loadFromFile("images/temple.png", gRenderer)){
        printf("Failed to load texture image (temple).");
        success = false;
    }

    return success;
}

void close(){
    // Deallocate surfaces
    personTexture.free();

    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    gWindow = NULL;
    gRenderer = NULL;

    IMG_Quit();
    SDL_Quit();
}

int main(int argc, char* argv[]){
    if (!init()){
        printf("Failed to initialize.\n");
    }
    else{
        if (!loadMedia()){
            printf("Failed to load media.\n");
        }
        else{
            bool quit = false;

            SDL_Event e;

            Timer fpsTimer;

            int xOffset = 0;
            int yOffset = 0;

            SDL_Point mousePos;

            World world(1000, 100, 3);
            world.initialize(&personTexture, &foodTexture, &templeTexture);

            HUD hud(gRenderer, &world);

            int countedFrames = 0;
            fpsTimer.start();

            bool paused = false;

            while (!quit){
                while (SDL_PollEvent(&e) != 0){
                    switch (e.type){
                        case SDL_QUIT: quit = true; break;
                        case SDL_KEYDOWN:
                            if (e.key.keysym.sym == SDLK_ESCAPE)
                                quit = true;
                            if (e.key.keysym.sym == SDLK_SPACE)
                                paused = !paused;
                            if (e.key.keysym.sym == SDLK_UP)
                                SDL_SetWindowSize(gWindow, 1920, 1080);
                            if (e.key.keysym.sym == SDLK_DOWN){
                                SDL_SetWindowSize(gWindow, 640, 480);
                            }
                            break;
                        case SDL_MOUSEMOTION:
                            mousePos.x = e.motion.x;
                            mousePos.y = e.motion.y;
                    }
                }
                // Keyboard input
                const Uint8* keystate = SDL_GetKeyboardState(NULL);

                if (keystate[SDL_SCANCODE_A]){
                    xOffset += 10;
                }
                if (keystate[SDL_SCANCODE_D]){
                    xOffset -= 10;
                }
                if (keystate[SDL_SCANCODE_W]){
                    yOffset += 10;
                }
                if (keystate[SDL_SCANCODE_S]){
                    yOffset -= 10;
                }

                // Update world if not paused
                if (!paused){
                    world.update();
                }

                // Clear screen
                SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0xFF);
                SDL_RenderClear(gRenderer);

                // Render objects to screen
                world.render(gRenderer, xOffset, yOffset);

                // Render UI
                hud.render(mousePos, xOffset, yOffset);

                // Calculate FPS
                float avgFPS = countedFrames / (fpsTimer.getTicks() / 1000.f);
                if (world.age % 60 == 0){
                    cout << "Average FPS: " << avgFPS << endl;
                }

                SDL_RenderPresent(gRenderer);
                ++countedFrames;
            }
        }
    }

    close();
    return 0;
}