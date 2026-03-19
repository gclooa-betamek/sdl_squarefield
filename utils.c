#include <utils.h>

static SDL_FRect image_frame;

static float grid_scroll = 0.0f;

static SDL_FRect squares[19];

SDL_AppResult SDL_AppFailure(const char *error)
{
    SDL_Log(error, SDL_GetError());
    return SDL_APP_FAILURE;
}

void looa_Image(App *state)
{
    if (!state->image_loaded) return;
    for (int i = 0; i < state->display_count; i++) {
        image_frame.x = ((float) (state->bounds[i].w - state->texture_width)) / 2.0f;
        image_frame.y = ((float) (state->bounds[i].h - state->texture_height)) / 4.0f;
        image_frame.w = (float) state->texture_width;
        image_frame.h = (float) state->texture_height;
        SDL_RenderTexture(state->renderers[i], state->textures[i], NULL, &image_frame);
    }
}

void looa_Grid(App *state)
{
    for (int i = 0; i < state->display_count; i++) {
        SDL_SetRenderDrawColor(state->renderers[i], 192, 254, 4, 255);
        int center = state->bounds[i].w / 2;
        int horizon = state->bounds[i].h / 2;
        float spacing = 4.0f;
        grid_scroll += 0.005f;

        /* Perspective vertical lines */
        for (int j = -9; j <= 9; j++) {
            float x_top = center + j * 100;
            float x_bottom = center + j * 400;
            SDL_RenderLine(state->renderers[i], x_top, horizon, x_bottom, state->bounds[i].h);
        }
        SDL_RenderLine(state->renderers[i], 0, horizon, state->bounds[i].w, horizon);

        /* Moving horizontal depth lines */
        for (int j = 1; j < 11; j++) {
            float depth = fmodf(j * spacing + grid_scroll * 10, 40);
            float t = depth / 40.0f;
            float scale = t * t;
            float y = horizon + scale * (state->bounds[i].h - horizon);
            SDL_RenderLine(state->renderers[i], 0, y, state->bounds[i].w, y);
        }
    }
}

void looa_Squarefield(App *state)
{
    for (int i = 0; i < state->display_count; i++) {
        SDL_SetRenderDrawColor(state->renderers[i], 255, 153, 0, 255);
        int center = state->bounds[i].w / 2;
        int horizon = state->bounds[i].h / 2;
        float spacing = 4.0f;
        grid_scroll += 0.005f;

        for (int iy = 1; iy < 11; iy++) {
            float depth = fmodf(iy * spacing + grid_scroll * 10, 40);
            float t = depth / 40.0f;
            float scale = t * t;
            float y = horizon + scale * (state->bounds[i].h - horizon - 2);

            for (int ix = -9; ix <= 9; ix++) {
                float x_top = center + ix * 100;
                float x_bottom = center + ix * 400;
                squares[ix+9].x = x_top;
                squares[ix+9].y = y;
                squares[ix+9].w = (float) 2;
                squares[ix+9].h = (float) 2;
                SDL_RenderFillRects(state->renderers[i], &squares[ix+9], 1);
            }
        }
    }
}