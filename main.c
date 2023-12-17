#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>

// Dimensions de l'écran
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

// Constantes de mouvement du joueur
const int MOVEMENT_SPEED = 5;
const int JUMP_FORCE = 15;
const int GRAVITY = 1;
const int MAX_JUMP_COUNT = 2;

// Structure décrivant le joueur
typedef struct {
    int x, y, w, h;         // Position et dimensions du joueur
    int isJumping;          // Indique si le joueur est en train de sauter
    int jumpCount;          // Nombre de sauts effectués
    int yVelocity;          // Vitesse verticale du joueur
    int isDucking;          // Indique si le joueur est en position accroupie
    int score;              // Score du joueur
} Player;

// Structure décrivant une plateforme
typedef struct {
    int x, y, w, h;         // Position et dimensions de la plateforme
} Platform;

// Structure décrivant un obstacle
typedef struct {
    int x, y, w, h;         // Position et dimensions de l'obstacle
} Obstacle;

// Structure décrivant une porte de sortie
typedef struct {
    int x, y, w, h;         // Position et dimensions de la porte de sortie
} ExitDoor;

// Sons du jeu
Mix_Chunk* jumpSound;
Mix_Chunk* scoreSound;
Mix_Chunk* collisionSound;

// Fonction pour réinitialiser la position du joueur
void resetPlayer(Player *player);

// Fonction pour vérifier la collision entre deux rectangles
int checkCollision(SDL_Rect a, SDL_Rect b);

// Fonction pour gérer les événements du jeu
void handleEvents(SDL_Event *event, int *quit, Player *player) {
    while (SDL_PollEvent(event) != 0) {
        if (event->type == SDL_QUIT) {
            *quit = 1;
        } else if (event->type == SDL_KEYDOWN) {
            switch (event->key.keysym.scancode) {
                case SDL_SCANCODE_UP:
                    if (player->jumpCount < MAX_JUMP_COUNT) {
                        player->isJumping = 1;
                        player->yVelocity = -JUMP_FORCE;
                        player->jumpCount++;
                    }
                    break;
                case SDL_SCANCODE_DOWN:
                    player->isDucking = 1;
                    break;
                default:
                    break;
            }
        } else if (event->type == SDL_KEYUP) {
            switch (event->key.keysym.scancode) {
                case SDL_SCANCODE_DOWN:
                    player->isDucking = 0; // Le joueur arrête de se baisser
                    break;
                default:
                    break;
            }
        }
    }
}

// Fonction pour mettre à jour l'état du joueur et du jeu
void updatePlayer(Player *player, Platform platform, Platform secondPlatform, Obstacle obstacle1, Obstacle obstacle2, ExitDoor exitDoor) {
    // Gestion de l'accroupissement du joueur
    if (player->isDucking) {
        player->h = 30; // Reduire la hauteur du joueur pour l'accroupissement
    } else {
        player->h = 65; // Rétablir la taille normale du joueur
    }

    // Gestion du saut du joueur
    if (player->isJumping) {
        player->y += player->yVelocity;
        player->yVelocity += GRAVITY;

        // Gestion des collisions avec le sol
        if (player->y >= SCREEN_HEIGHT - player->h) {
            player->y = SCREEN_HEIGHT - player->h;
            player->isJumping = 0;
            player->jumpCount = 0;
        }

        // Gestion des collisions avec les plateformes
        if (player->y + player->h >= platform.y && player->y + player->h <= platform.y + platform.h
            && player->x + player->w >= platform.x && player->x <= platform.x + platform.w) {
            player->isJumping = 0;
            player->jumpCount = 0;
            player->y = platform.y - player->h;
        } else if (player->y + player->h >= secondPlatform.y && player->y + player->h <= secondPlatform.y + secondPlatform.h
            && player->x + player->w >= secondPlatform.x && player->x <= secondPlatform.x + secondPlatform.w) {
            player->isJumping = 0;
            player->jumpCount = 0;
            player->y = secondPlatform.y - player->h;
        }

        // Gestion des collisions avec les obstacles
        if (checkCollision((SDL_Rect){player->x, player->y, player->w, player->h}, (SDL_Rect){obstacle1.x, obstacle1.y, obstacle1.w, obstacle1.h})) {
            Mix_PlayChannel(-1, collisionSound, 0);
            resetPlayer(player);
            player->score = 0;
        }

        if (checkCollision((SDL_Rect){player->x, player->y, player->w, player->h}, (SDL_Rect){obstacle2.x, obstacle2.y, obstacle2.w, obstacle2.h})) {
            Mix_PlayChannel(-1, collisionSound, 0);
            resetPlayer(player);
            player->score = 0;
        }

        // Gestion des collisions avec la porte de sortie
        if (checkCollision((SDL_Rect){player->x, player->y, player->w, player->h}, (SDL_Rect){exitDoor.x, exitDoor.y, exitDoor.w, exitDoor.h})) {
            printf("Félicitations ! Vous avez passé le niveau !\n");
            SDL_Delay(2000);
            resetPlayer(player);
            player->score++;
        }

        // Réinitialiser le nombre de sauts effectués si le joueur n'est plus en train de sauter
        if (!player->isJumping) {
            player->jumpCount = 0;
        }

    } else {
        // Gestion du mouvement vers le bas du joueur
        player->y += GRAVITY * 10;

        // Gestion des collisions avec les plateformes
        if (player->y + player->h >= platform.y && player->y + player->h <= platform.y + platform.h
            && player->x + player->w >= platform.x && player->x <= platform.x + platform.w) {
            player->y = platform.y - player->h;
        } else if (player->y + player->h >= secondPlatform.y && player->y + player->h <= secondPlatform.y + secondPlatform.h
            && player->x + player->w >= secondPlatform.x && player->x <= secondPlatform.x + secondPlatform.w) {
            player->isJumping = 0;
            player->jumpCount = 0;
            player->y = secondPlatform.y - player->h;
        }

        // Gestion des collisions avec le sol
        if (player->y >= SCREEN_HEIGHT - player->h) {
            player->y = SCREEN_HEIGHT - player->h;
        }

        // Gestion des collisions avec les obstacles
        if (checkCollision((SDL_Rect){player->x, player->y, player->w, player->h}, (SDL_Rect){obstacle1.x, obstacle1.y, obstacle1.w, obstacle1.h})) {
            Mix_PlayChannel(-1, collisionSound, 0);
            resetPlayer(player);
            player->score = 0;
        }

        if (checkCollision((SDL_Rect){player->x, player->y, player->w, player->h}, (SDL_Rect){obstacle2.x, obstacle2.y, obstacle2.w, obstacle2.h})) {
            Mix_PlayChannel(-1, collisionSound, 0);
            resetPlayer(player);
            player->score = 0;
        }

        // Gestion des collisions avec la porte de sortie
        if (checkCollision((SDL_Rect){player->x, player->y, player->w, player->h}, (SDL_Rect){exitDoor.x, exitDoor.y, exitDoor.w, exitDoor.h})) {
            printf("Félicitations ! Vous avez passé le niveau !\n");
            SDL_Delay(2000);
            resetPlayer(player);
            player->score++;
        }

        // Réinitialiser le nombre de sauts effectués si le joueur est au-dessus d'une plateforme
        if (checkCollision((SDL_Rect){player->x, player->y + 1, player->w, player->h}, (SDL_Rect){platform.x, platform.y, platform.w, platform.h}) ||
            checkCollision((SDL_Rect){player->x, player->y + 1, player->w, player->h}, (SDL_Rect){secondPlatform.x, secondPlatform.y, secondPlatform.w, secondPlatform.h})) {
            player->jumpCount = 0;
        }
    }
}

// Fonction pour vérifier la collision entre deux rectangles
int checkCollision(SDL_Rect a, SDL_Rect b) {
    return a.x < b.x + b.w && a.x + a.w > b.x && a.y < b.y + b.h && a.y + a.h > b.y;
}

// Fonction pour réinitialiser la position du joueur
void resetPlayer(Player *player) {
    player->x = 0;
    player->y = SCREEN_HEIGHT - 65;
    player->isJumping = 0;
    player->jumpCount = 0;
    player->yVelocity = 0;
}

// Fonction principale du programme
int main(void) {
    // Initialisation des éléments SDL
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    SDL_Event event;
    Player player = { 0, SCREEN_HEIGHT - 65, 35, 65, 0, 0, 0, 0 ,0}; // Modifier les valeurs de x, y, w et h selon les besoins
    Platform platform = { 200, 400, 400, 20 };
    Platform secondPlatform = { 500, 300, 300, 20 };
    Obstacle obstacle1 = { 400, 300, 50, 50 };
    Obstacle obstacle2 = { 600, 450, 50, 50 };
    ExitDoor exitDoor = { 700, 200, 50, 100 };  // Ajuster la position et la taille de la porte de sortie

    int quit = 0;

    // Initialisation de SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
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

    // Initialisation de SDL_ttf
    if (TTF_Init() == -1) {
        printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
        return 1;
    }

    // Initialisation de SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
        return 1;
    }

    // Chargement des sons du jeu
    jumpSound = Mix_LoadWAV("jump.wav");
    scoreSound = Mix_LoadWAV("score.wav");
    collisionSound = Mix_LoadWAV("collision.wav");

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

        // Mise à jour de l'état du joueur
        updatePlayer(&player, platform, secondPlatform, obstacle1, obstacle2, exitDoor);

        // Effacement de l'écran
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        // Affichage des éléments du jeu
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_Rect platformRect = { platform.x, platform.y, platform.w, platform.h };
        SDL_Rect secondPlatformRect = { secondPlatform.x, secondPlatform.y, secondPlatform.w, secondPlatform.h };
        SDL_Rect obstacleRect1 = { obstacle1.x, obstacle1.y, obstacle1.w, obstacle1.h };
        SDL_Rect obstacleRect2 = { obstacle2.x, obstacle2.y, obstacle2.w, obstacle2.h };
        SDL_Rect exitDoorRect = { exitDoor.x, exitDoor.y, exitDoor.w, exitDoor.h };

        // Dessin des éléments
        SDL_RenderFillRect(renderer, &platformRect);
        SDL_RenderFillRect(renderer, &secondPlatformRect);
        SDL_RenderFillRect(renderer, &obstacleRect1);
        SDL_RenderFillRect(renderer, &obstacleRect2);
        SDL_RenderFillRect(renderer, &exitDoorRect);

        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
        SDL_Rect playerRect = { player.x, player.y, player.w, player.h };
        SDL_RenderFillRect(renderer, &playerRect);

        // Afficher le score à l'écran
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        TTF_Font* font = TTF_OpenFont("arial.ttf", 72); 
        if (font == NULL) {
            printf("Failed to load font! SDL_ttf Error: %s\n", TTF_GetError());
        } else {
            SDL_Color textColor = {0, 0, 0,0}; 
            char scoreText[50];
            sprintf(scoreText, "Score: %d", player.score);
            SDL_Surface* scoreSurface = TTF_RenderText_Solid(font, scoreText, textColor);
            SDL_Texture* scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);
            SDL_Rect scoreRect = {10, 10, scoreSurface->w, scoreSurface->h};
            SDL_RenderCopy(renderer, scoreTexture, NULL, &scoreRect);
            SDL_FreeSurface(scoreSurface);
            SDL_DestroyTexture(scoreTexture);
            TTF_CloseFont(font);
        }

        SDL_RenderPresent(renderer);

        int frameTicks = SDL_GetTicks() - startTicks;
        if (frameTicks < SCREEN_TICKS_PER_FRAME) {
            SDL_Delay(SCREEN_TICKS_PER_FRAME - frameTicks);
        }
    }

    Mix_FreeChunk(jumpSound);
    Mix_FreeChunk(scoreSound);
    Mix_FreeChunk(collisionSound);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    Mix_Quit();
    TTF_Quit();
    SDL_Quit();
    return 0;
}
