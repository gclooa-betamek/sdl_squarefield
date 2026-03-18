#ifndef SQUAREFIELD_H
#define SQUAREFIELD_H

#include <SDL3/SDL.h>

/* Application state */
typedef struct App
{
    SDL_Rect *bounds;
    SDL_Window **windows;
    SDL_Renderer **renderers;
    SDL_Texture **textures;

    bool image_loaded;
    int texture_width;
    int texture_height;
    int display_count;
} App;

#endif /* SQUAREFIELD_H */