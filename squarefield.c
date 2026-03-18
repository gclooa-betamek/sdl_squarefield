/** SQUAREFIELD
 *  @file   squarefield.c
 *  @author looa.dev
 *  @date   2026-03-12
 *  @brief  A simple SDL3 demo that draws a field of squares.
 */

#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_stdinc.h>
#include <squarefield.h>
#include <utils.h>

/* Cursor state */
static Uint32 last_input_ticks = 0;
static bool cursor_hidden = false;
static bool window_focused = true;

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    SDL_SetAppMetadata("squarefield", "1.0", "dev.looa.squarefield");
    if (!SDL_Init(SDL_INIT_VIDEO)) SDL_AppFailure("Failed to initialize SDL: %s");

    App *state = SDL_malloc(sizeof(App));
    if (!state) SDL_AppFailure("Failed to allocate memory for app state: %s");
    *appstate = state;
    SDL_zero(*state);

    /* Get display dimensions */
    SDL_DisplayID *displays = SDL_GetDisplays(&state->display_count);

    /* Allocate arrays for windows, renderers, textures */
    state->bounds = (SDL_Rect*)malloc(state->display_count * sizeof(SDL_Rect));
    state->windows = (SDL_Window**)malloc(state->display_count * sizeof(SDL_Window*));
    state->renderers = (SDL_Renderer**)malloc(state->display_count * sizeof(SDL_Renderer*));
    state->textures = (SDL_Texture**)malloc(state->display_count * sizeof(SDL_Texture*));

    for (int i = 0; i < state->display_count; i++) {
        SDL_GetDisplayBounds(displays[i], &state->bounds[i]);

        state->windows[i] = SDL_CreateWindow( 
            "SCRSVR", 
            state->bounds[i].w, 
            state->bounds[i].h, 
            SDL_WINDOW_BORDERLESS
        );
        SDL_SetWindowPosition(state->windows[i], state->bounds[i].x, state->bounds[i].y);

        state->renderers[i] = SDL_CreateRenderer(state->windows[i], NULL);
        SDL_SetRenderLogicalPresentation(
            state->renderers[i],
            state->bounds[i].w,
            state->bounds[i].h,
            SDL_LOGICAL_PRESENTATION_LETTERBOX
        );
        SDL_SetRenderVSync(state->renderers[i], 1);

        SDL_Surface *surface = NULL;
        char *png_path = NULL;
        SDL_asprintf(&png_path, "%simage.png", SDL_GetBasePath());
        surface = SDL_LoadPNG(png_path);
        if (!surface) {
            state->image_loaded = false;
            SDL_Log("Failed to load image: %s", SDL_GetError());
            SDL_Log("Make sure 'image.png' is in the same directory as the executable.");
            SDL_Log("Continuing without image...");
        } else {
            state->image_loaded = true;
            state->textures[i] = SDL_CreateTextureFromSurface(state->renderers[i], surface);
            if (!state->textures[i]) SDL_AppFailure("Failed to create texture: %s");
            state->texture_width = surface->w;
            state->texture_height = surface->h;
        }

        SDL_DestroySurface(surface);
        SDL_free(displays);
        SDL_free(png_path);
    }

    /* Initialize cursor timers */
    last_input_ticks = SDL_GetTicks();
    cursor_hidden = false;
    window_focused = true;
    SDL_ShowCursor();
    
    return SDL_APP_CONTINUE;
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    if (event->type == SDL_EVENT_QUIT) return SDL_APP_SUCCESS;

    switch (event->type) {
    case SDL_EVENT_KEY_DOWN:
        if (event->key.scancode == SDL_SCANCODE_ESCAPE) return SDL_APP_SUCCESS;
        last_input_ticks = SDL_GetTicks();
        if (cursor_hidden && window_focused) {
            SDL_ShowCursor();
            cursor_hidden = false;
        }
        break;
    case SDL_EVENT_MOUSE_MOTION:
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
    case SDL_EVENT_MOUSE_BUTTON_UP:
    case SDL_EVENT_MOUSE_WHEEL:
        last_input_ticks = SDL_GetTicks();
        if (cursor_hidden && window_focused) {
            SDL_ShowCursor();
            cursor_hidden = false;
        }
        break;
    case SDL_EVENT_WINDOW_FOCUS_GAINED:
        window_focused = true;
        last_input_ticks = SDL_GetTicks();
        if (cursor_hidden) {
            SDL_ShowCursor();
            cursor_hidden = false;
        }
        break;
    case SDL_EVENT_WINDOW_FOCUS_LOST:
        window_focused = false;
        if (cursor_hidden) {
            SDL_ShowCursor();
            cursor_hidden = false;
        }
        break;
    default:
        break;
    }
    return SDL_APP_CONTINUE;
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void *appstate)
{
    App *state = (App *)appstate;

    for (int i = 0; i < state->display_count; i++) {
        SDL_SetRenderDrawColor(state->renderers[i], 0, 0, 0, 255);
        SDL_RenderClear(state->renderers[i]);
    }

    looa_Grid(state);
    looa_Image(state);
    
    for (int i = 0; i < state->display_count; i++) {
        SDL_RenderPresent(state->renderers[i]);
    }
    /* Hide cursor if idle for 1s and window is focused */
    bool cursor_idle = (SDL_GetTicks() - last_input_ticks) >= 1000;
    if (window_focused && !cursor_hidden && cursor_idle) {
        SDL_HideCursor();
        cursor_hidden = true;
    }
    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    App *state = (App *)appstate;
    for (int i = 0; i < state->display_count; i++) {
        SDL_DestroyTexture(state->textures[i]);
        SDL_DestroyRenderer(state->renderers[i]);
        SDL_DestroyWindow(state->windows[i]);
    }
    SDL_free(state->bounds);
    SDL_free(state->windows);
    SDL_free(state->renderers);
    SDL_free(state->textures);
    SDL_free(state);

    SDL_Quit();
}