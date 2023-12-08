#include <SDL2/SDL.h>
#include <stdio.h>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int MOVEMENT_SPEED = 5;
const int JUMP_FORCE = 15;
const int GRAVITY = 1;
const int MAX_JUMP_COUNT = 2;

// Structure représentant le joueur
typedef struct {
    int x, y, w, h;         // Coordonnées et dimensions du joueur
    int isJumping;          // Indique si le joueur est en train de sauter
    int jumpCount;          // Nombre de sauts effectués
    int yVelocity;          // Vitesse verticale du joueur
} Player;

// Fonction pour gérer les événements
void handleEvents(SDL_Event *event, int *quit, Player *player) {
    while (SDL_PollEvent(event) != 0) {
        if (event->type == SDL_QUIT) {
            *quit = 1;
        }
    }
}

// Fonction pour mettre à jour la position du joueur
void updatePlayer(Player *player, const Uint8 *currentKeyStates) {
    if (currentKeyStates[SDL_SCANCODE_RIGHT] && player->x < SCREEN_WIDTH - player->w) {
        player->x += MOVEMENT_SPEED;
    }
    if (currentKeyStates[SDL_SCANCODE_LEFT] && player->x > 0) {
        player->x -= MOVEMENT_SPEED;
    }

    if (player->isJumping) {
        // Si le joueur est en train de sauter, ajuster la position en fonction de la vitesse verticale
        player->y += player->yVelocity;
        player->yVelocity += GRAVITY;

        // Gérer les collisions avec le sol
        if (player->y >= SCREEN_HEIGHT - player->h) {
            player->y = SCREEN_HEIGHT - player->h;
            player->isJumping = 0;
            player->jumpCount = 0;
        }
    } else {
        // Si le joueur n'est pas en train de sauter, appliquer la gravité
        player->y += GRAVITY * 10;

        // Gérer les collisions avec le sol
        if (player->y >= SCREEN_HEIGHT - player->h) {
            player->y = SCREEN_HEIGHT - player->h;
        }

        // Gérer le saut du joueur
        if (currentKeyStates[SDL_SCANCODE_UP] && player->jumpCount < MAX_JUMP_COUNT) {
            player->isJumping = 1;
            player->yVelocity = -JUMP_FORCE;
            player->jumpCount++;
        }
    }
}

int main(int argc, char *args[]) {
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    SDL_Event event;
    Player player = {SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 50, 50, 0, 0, 0};  // Initialisation du joueur
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
