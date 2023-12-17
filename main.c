#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>

// Définition des constantes pour la taille de l'écran
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

// Paramètres du joueur
const int MOVEMENT_SPEED = 5;
const int JUMP_FORCE = 15;
const int GRAVITY = 1;
const int MAX_JUMP_COUNT = 2;

// Structure représentant le joueur
typedef struct {
    int x, y, w, h;         // Position et dimensions du joueur
    int isJumping;          // Indique si le joueur est en train de sauter
    int jumpCount;          // Nombre de sauts effectués
    int yVelocity;          // Vitesse verticale du joueur
    int score;              // Score du joueur (non utilisé dans cet exemple)
} Player;

// Structure représentant une plateforme
typedef struct {
    int x, y, w, h;         // Position et dimensions de la plateforme
} Platform;

// Structure représentant un obstacle
typedef struct {
    int x, y, w, h;         // Position et dimensions de l'obstacle
} Obstacle;

// Déclaration des sons
Mix_Chunk* jumpSound;
Mix_Chunk* collisionSound;

// Fonction pour réinitialiser les paramètres du joueur
void resetPlayer(Player *player);

// Fonction pour gérer les événements du jeu
void handleEvents(SDL_Event *event, int *quit, Player *player) {
    while (SDL_PollEvent(event) != 0) {
        if (event->type == SDL_QUIT) {
            *quit = 1;  // Quitte le jeu si la fenêtre est fermée
        } else if (event->type == SDL_KEYDOWN) {
            switch (event->key.keysym.scancode) {
                case SDL_SCANCODE_UP:
                    // Gestion du saut du joueur
                    if (player->jumpCount < MAX_JUMP_COUNT && !player->isJumping) {
                        player->isJumping = 1;
                        player->yVelocity = -JUMP_FORCE;
                        player->jumpCount++;
                        Mix_PlayChannel(-1, jumpSound, 0); // Joue le son du saut
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

// Fonction pour mettre à jour la position du joueur et gérer les collisions
void updatePlayer(Player *player, Platform platform, Platform secondPlatform, Obstacle obstacle1, Obstacle obstacle2) {
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

        // Vérification des collisions avec les obstacles
        if (player->x < obstacle1.x + obstacle1.w && player->x + player->w > obstacle1.x &&
            player->y < obstacle1.y + obstacle1.h && player->y + player->h > obstacle1.y) {
            Mix_PlayChannel(-1, collisionSound, 0); // Joue le son de la collision
            resetPlayer(player);
        }

        if (player->x < obstacle2.x + obstacle2.w && player->x + player->w > obstacle2.x &&
            player->y < obstacle2.y + obstacle2.h && player->y + player->h > obstacle2.y) {
            Mix_PlayChannel(-1, collisionSound, 0); // Joue le son de la collision
            resetPlayer(player);
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

        // Vérification des collisions avec les obstacles
        if (player->x < obstacle1.x + obstacle1.w && player->x + player->w > obstacle1.x &&
            player->y < obstacle1.y + obstacle1.h && player->y + player->h > obstacle1.y) {
            Mix_PlayChannel(-1, collisionSound, 0); // Joue le son de la collision
            resetPlayer(player);
        }

        if (player->x < obstacle2.x + obstacle2.w && player->x + player->w > obstacle2.x &&
            player->y < obstacle2.y + obstacle2.h && player->y + player->h > obstacle2.y) {
            Mix_PlayChannel(-1, collisionSound, 0); // Joue le son de la collision
            resetPlayer(player);
        }
    }
}

// Fonction pour réinitialiser les paramètres du joueur
void resetPlayer(Player *player) {
    player->x = 0;
    player->y = SCREEN_HEIGHT - 65;
    player->isJumping = 0;
    player->jumpCount = 0;
    player->yVelocity = 0;
}

int main(int argc, char* args[]) {
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    SDL_Event event;
    Player player = { 0, SCREEN_HEIGHT - 65, 35, 65, 0, 0, 0, 0 }; // Modifier les valeurs x, y, w, et h en conséquence
    Platform platform = { 200, 400, 400, 20 };
    Platform secondPlatform = { 500, 300, 300, 20 };
    Obstacle obstacle1 = { 400, 300, 50, 50 };
    Obstacle obstacle2 = { 600, 450, 50, 50 };
    int quit = 0;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
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

    if (TTF_Init() == -1) {
        printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
        return 1;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
        return 1;
    }

    jumpSound = Mix_LoadWAV("jump.wav");
    collisionSound = Mix_LoadWAV("collision.wav");

    const int SCREEN_TICKS_PER_FRAME = 1000 / 60;

    while (!quit) {
        int startTicks = SDL_GetTicks();

        handleEvents(&event, &quit, &player);

        const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);
        if (currentKeyStates[SDL_SCANCODE_RIGHT] && player.x < SCREEN_WIDTH - player.w) {
            player.x += MOVEMENT_SPEED;
        }
        if (currentKeyStates[SDL_SCANCODE_LEFT] && player.x > 0) {
            player.x -= MOVEMENT_SPEED;
        }

        updatePlayer(&player, platform, secondPlatform, obstacle1, obstacle2);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_Rect platformRect = { platform.x, platform.y, platform.w, platform.h };
        SDL_Rect secondPlatformRect = { secondPlatform.x, secondPlatform.y, secondPlatform.w, secondPlatform.h };
        SDL_Rect obstacleRect1 = { obstacle1.x, obstacle1.y, obstacle1.w, obstacle1.h };
        SDL_Rect obstacleRect2 = { obstacle2.x, obstacle2.y, obstacle2.w, obstacle2.h };
        SDL_RenderFillRect(renderer, &platformRect);
        SDL_RenderFillRect(renderer, &secondPlatformRect);
        SDL_RenderFillRect(renderer, &obstacleRect1);
        SDL_RenderFillRect(renderer, &obstacleRect2);

        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
        SDL_Rect playerRect = { player.x, player.y, player.w, player.h };
        SDL_RenderFillRect(renderer, &playerRect);

        SDL_RenderPresent(renderer);

        int frameTicks = SDL_GetTicks() - startTicks;
        if (frameTicks < SCREEN_TICKS_PER_FRAME) {
            SDL_Delay(SCREEN_TICKS_PER_FRAME - frameTicks);
        }
    }

    Mix_FreeChunk(jumpSound);
    Mix_FreeChunk(collisionSound);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    Mix_Quit();
    TTF_Quit();
    SDL_Quit();
    return 0;
}
