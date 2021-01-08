#include <string>

#ifndef __CHROMATEXTURE_H_INCLUDED__
#define __CHROMATEXTURE_H_INCLUDED__

class ChromaTexture{
    public:
        ChromaTexture();
        ~ChromaTexture();
        bool loadFromFile(std::string, SDL_Renderer*);
        void free();
        void render(int, int, SDL_Renderer*);

        int getWidth();
        int getHeight();

    private:
        SDL_Texture* mTexture;

        int mWidth;
        int mHeight;
};

#endif