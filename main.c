#include <SDL2/SDL.h>
#include <stdio.h>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int MOVEMENT_SPEED = 5;

typedef struct {
    int x, y, w, h;
} Player;

void handleEvents(SDL_Event *event, int *quit, Player *player) {
    while (SDL_PollEvent(event) != 0) {
        if (event->type == SDL_QUIT) {
            *quit = 1;
        }
    }
}

void updatePlayer(Player *player, const Uint8 *currentKeyStates) {
    if (currentKeyStates[SDL_SCANCODE_RIGHT] && player->x < SCREEN_WIDTH - player->w) {
        player->x += MOVEMENT_SPEED;
    }
    if (currentKeyStates[SDL_SCANCODE_LEFT] && player->x > 0) {
        player->x -= MOVEMENT_SPEED;
    }
}

int main(int argc, char *args[]) {
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    SDL_Event event;
    Player player = {SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 50, 50};
    int quit = 0;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    const int SCREEN_TICKS_PER_FRAME = 1000 / 60;

    while (!quit) {
        int startTicks = SDL_GetTicks();

        handleEvents(&event, &quit, &player);

        const Uint8 *currentKeyStates = SDL_GetKeyboardState(NULL);
        updatePlayer(&player, currentKeyStates);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_Rect playerRect = {player.x, player.y, player.w, player.h};
        SDL_RenderFillRect(renderer, &playerRect);

        SDL_RenderPresent(renderer);

        int frameTicks = SDL_GetTicks() - startTicks;
        if (frameTicks < SCREEN_TICKS_PER_FRAME) {
            SDL_Delay(SCREEN_TICKS_PER_FRAME - frameTicks);
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
