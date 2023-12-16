#include <SDL2/SDL.h>
#include <stdio.h>

// Dimensions de la fenêtre
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

// Paramètres du joueur
const int MOVEMENT_SPEED = 5;
const int JUMP_FORCE = 15;
const int GRAVITY = 1;
const int MAX_JUMP_COUNT = 2;

// Structure représentant le joueur
typedef struct {
    int x, y, w, h;
    int isJumping;
    int jumpCount;
    int yVelocity;
} Player;

// Structure représentant une plateforme
typedef struct {
    int x, y, w, h;
} Platform;

// Gestion des événements
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
        } else if (event->type == SDL_KEYDOWN) {
            switch (event->key.keysym.scancode) {
                case SDL_SCANCODE_UP:
                    if (player->jumpCount < MAX_JUMP_COUNT && !player->isJumping) {
                        player->isJumping = 1;
                        player->yVelocity = -JUMP_FORCE;
                        player->jumpCount++;
                    } else if (player->jumpCount < MAX_JUMP_COUNT && player->isJumping) {
                        // Implémentation du double saut
                        player->yVelocity = -JUMP_FORCE;
                        player->jumpCount++;
                    }
                    break;
                default:
                    break;
            }
        }
    }
}

// Mise à jour de la position du joueur en fonction des collisions
void updatePlayer(Player *player, Platform platform, Platform secondPlatform) {
    if (player->isJumping) {
        // Mise à jour de la position verticale lors d'un saut
        player->y += player->yVelocity;
        player->yVelocity += GRAVITY;

        // Gestion des collisions avec le sol
        if (player->y >= SCREEN_HEIGHT - player->h) {
            player->y = SCREEN_HEIGHT - player->h;
            player->isJumping = 0;
            player->jumpCount = 0;
        }

        // Gestion des collisions avec la première plateforme
        if (player->y + player->h >= platform.y && player->y + player->h <= platform.y + platform.h
            && player->x + player->w >= platform.x && player->x <= platform.x + platform.w) {
            player->isJumping = 0;
            player->jumpCount = 0;
            player->y = platform.y - player->h;
        }

        // Gestion des collisions avec la deuxième plateforme
        else if (player->y + player->h >= secondPlatform.y && player->y + player->h <= secondPlatform.y + secondPlatform.h
            && player->x + player->w >= secondPlatform.x && player->x <= secondPlatform.x + secondPlatform.w) {
            player->isJumping = 0;
            player->jumpCount = 0;
            player->y = secondPlatform.y - player->h;
        }
    } else {
        // Appliquer la gravité lors du déplacement latéral
        player->y += GRAVITY * 10;

        // Gestion des collisions avec la première plateforme
        if (player->y + player->h >= platform.y && player->y + player->h <= platform.y + platform.h
            && player->x + player->w >= platform.x && player->x <= platform.x + platform.w) {
            player->y = platform.y - player->h;
        }

        // Gestion des collisions avec la deuxième plateforme
        else if (player->y + player->h >= secondPlatform.y && player->y + player->h <= secondPlatform.y + secondPlatform.h
            && player->x + player->w >= secondPlatform.x && player->x <= secondPlatform.x + secondPlatform.w) {
            player->isJumping = 0;
            player->jumpCount = 0;
            player->y = secondPlatform.y - player->h;
        }

        // Gestion des collisions avec la limite inférieure de l'écran
        if (player->y >= SCREEN_HEIGHT - player->h) {
            player->y = SCREEN_HEIGHT - player->h;
        }
    }
}

// Réinitialisation des paramètres du joueur
void resetPlayer(Player *player) {
    player->x = 0;
    player->y = SCREEN_HEIGHT - 65;
    player->isJumping = 0;
    player->jumpCount = 0;
    player->yVelocity = 0;
}

int main(void) {
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    SDL_Event event;
    Player player = { 0, SCREEN_HEIGHT - 65, 35, 65, 0, 0, 0 }; 
    Platform platform = { 200, 400, 400, 20 };
    Platform secondPlatform = { 500, 300, 300, 20 };
    int quit = 0;

    // Initialisation de SDL
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

    // Création de la fenêtre
    window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    // Création du renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    // Paramètres pour maintenir 60 images par seconde
    const int SCREEN_TICKS_PER_FRAME = 1000 / 60;

    while (!quit) {
        int startTicks = SDL_GetTicks();
        // Gestion des événements
        handleEvents(&event, &quit, &player);

        // Gestion des mouvements du joueur
        const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);
        if (currentKeyStates[SDL_SCANCODE_RIGHT] && player.x < SCREEN_WIDTH - player.w) {
            player.x += MOVEMENT_SPEED;
        }
        if (currentKeyStates[SDL_SCANCODE_LEFT] && player.x > 0) {
            player.x -= MOVEMENT_SPEED;
        }

        // Mise à jour de la position du joueur et gestion des collisions
        updatePlayer(&player, platform, secondPlatform);

        // Effacement de l'écran
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        // Dessin des plateformes
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_Rect platformRect = { platform.x, platform.y, platform.w, platform.h };
        SDL_Rect secondPlatformRect = { secondPlatform.x, secondPlatform.y, secondPlatform.w, secondPlatform.h };
        SDL_RenderFillRect(renderer, &platformRect);
        SDL_RenderFillRect(renderer, &secondPlatformRect);

        // Dessin du joueur (rectangle bleu)
        SDL_Rect playerRect = { player.x, player.y, player.w, player.h };
        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
        SDL_RenderFillRect(renderer, &playerRect);

        // Mise à jour de l'affichage
        SDL_RenderPresent(renderer);

        // Contrôle du taux d'images par seconde
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

    // Libération des ressources
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
