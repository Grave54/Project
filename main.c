#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>

// Dimensions de l'écran
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

// Paramètres du joueur
const int MOVEMENT_SPEED = 5;
const int JUMP_FORCE = 15;
const int GRAVITY = 1;
const int MAX_JUMP_COUNT = 2;

// Structure représentant le joueur
typedef struct {
    int x, y, w, h;           // Position et dimensions du joueur
    int isJumping;            // Indique si le joueur est en train de sauter
    int jumpCount;            // Nombre de sauts effectués
    int yVelocity;            // Vitesse verticale du joueur
    int score;                // Score du joueur
} Player;

// Structure représentant une plateforme
typedef struct {
    int x, y, w, h;           // Position et dimensions de la plateforme
} Platform;

// Structure représentant un obstacle
typedef struct {
    int x, y, w, h;           // Position et dimensions de l'obstacle
} Obstacle;

// Structure représentant une porte de sortie
typedef struct {
    int x, y, w, h;           // Position et dimensions de la porte de sortie
} ExitDoor;

// Sons du jeu
Mix_Chunk* jumpSound;
Mix_Chunk* scoreSound;
Mix_Chunk* collisionSound;

// Fonction pour réinitialiser la position du joueur
void resetPlayer(Player *player);

// Fonction pour vérifier la collision entre deux rectangles
int checkCollision(SDL_Rect a, SDL_Rect b);

// Fonction pour gérer les événements SDL
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
                        Mix_PlayChannel(-1, jumpSound, 0); // Jouer le son du saut
                    } else if (player->jumpCount < MAX_JUMP_COUNT && player->isJumping) {
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
void updatePlayer(Player *player, Platform platform, Platform secondPlatform, Obstacle obstacle1, Obstacle obstacle2, ExitDoor exitDoor) {
    // Gestion du saut du joueur
    if (player->isJumping) {
        player->y += player->yVelocity;
        player->yVelocity += GRAVITY;
        // Atterrissage du joueur
        if (player->y >= SCREEN_HEIGHT - player->h) {
            player->y = SCREEN_HEIGHT - player->h;
            player->isJumping = 0;
            player->jumpCount = 0;
        }
        // Collision avec la première plateforme
        if (player->y + player->h >= platform.y && player->y + player->h <= platform.y + platform.h
            && player->x + player->w >= platform.x && player->x <= platform.x + platform.w) {
            player->isJumping = 0;
            player->jumpCount = 0;
            player->y = platform.y - player->h;
        }
        // Collision avec la deuxième plateforme
        else if (player->y + player->h >= secondPlatform.y && player->y + player->h <= secondPlatform.y + secondPlatform.h
            && player->x + player->w >= secondPlatform.x && player->x <= secondPlatform.x + secondPlatform.w) {
            player->isJumping = 0;
            player->jumpCount = 0;
            player->y = secondPlatform.y - player->h;
        }
        // Collision avec les obstacles
        if (checkCollision((SDL_Rect){player->x, player->y, player->w, player->h}, (SDL_Rect){obstacle1.x, obstacle1.y, obstacle1.w, obstacle1.h})) {
            Mix_PlayChannel(-1, collisionSound, 0); // Jouer le son de la collision
            resetPlayer(player);
            player->score = 0; // Réinitialiser le score en cas de collision avec un obstacle
        }

        if (checkCollision((SDL_Rect){player->x, player->y, player->w, player->h}, (SDL_Rect){obstacle2.x, obstacle2.y, obstacle2.w, obstacle2.h})) {
            Mix_PlayChannel(-1, collisionSound, 0); // Jouer le son de la collision
            resetPlayer(player);
            player->score = 0; // Réinitialiser le score en cas de collision avec un obstacle
        }

        // Collision avec la porte de sortie
        if (checkCollision((SDL_Rect){player->x, player->y, player->w, player->h}, (SDL_Rect){exitDoor.x, exitDoor.y, exitDoor.w, exitDoor.h})) {
            // Le joueur a atteint la porte de sortie, passer au niveau suivant
            // Implémenter le code pour configurer le niveau suivant ou terminer le jeu
            printf("Félicitations ! Vous avez passé le niveau !\n");
            SDL_Delay(2000); // Attendre pendant 2 secondes (à des fins de démonstration)
            resetPlayer(player); // Réinitialiser la position du joueur
            player->score++; // Augmenter le score en passant au niveau suivant
        }
    }
    // Gestion du mouvement vers le bas lorsque le joueur n'est pas en train de sauter
    else {
        player->y += GRAVITY * 10;

        // Collision avec les plateformes
        if (player->y + player->h >= platform.y && player->y + player->h <= platform.y + platform.h
            && player->x + player->w >= platform.x && player->x <= platform.x + platform.w) {
            player->y = platform.y - player->h;
        } else if (player->y + player->h >= secondPlatform.y && player->y + player->h <= secondPlatform.y + secondPlatform.h
            && player->x + player->w >= secondPlatform.x && player->x <= secondPlatform.x + secondPlatform.w) {
            player->isJumping = 0;
            player->jumpCount = 0;
            player->y = secondPlatform.y - player->h;
        }

        // Limite inférieure de l'écran
        if (player->y >= SCREEN_HEIGHT - player->h) {
            player->y = SCREEN_HEIGHT - player->h;
        }
        // Collision avec les obstacles
        if (checkCollision((SDL_Rect){player->x, player->y, player->w, player->h}, (SDL_Rect){obstacle1.x, obstacle1.y, obstacle1.w, obstacle1.h})) {
            Mix_PlayChannel(-1, collisionSound, 0); // Jouer le son de la collision
            resetPlayer(player);
            player->score = 0; // Réinitialiser le score en cas de collision avec un obstacle
        }

        if (checkCollision((SDL_Rect){player->x, player->y, player->w, player->h}, (SDL_Rect){obstacle2.x, obstacle2.y, obstacle2.w, obstacle2.h})) {
            Mix_PlayChannel(-1, collisionSound, 0); // Jouer le son de la collision
            resetPlayer(player);
            player->score = 0; // Réinitialiser le score en cas de collision avec un obstacle
        }

        // Collision avec la porte de sortie
        if (checkCollision((SDL_Rect){player->x, player->y, player->w, player->h}, (SDL_Rect){exitDoor.x, exitDoor.y, exitDoor.w, exitDoor.h})) {
            // Le joueur a atteint la porte de sortie, passer au niveau suivant
            // Implémenter le code pour configurer le niveau suivant ou terminer le jeu
            printf("Félicitations ! Vous avez passé le niveau !\n");
            SDL_Delay(2000); // Attendre pendant 2 secondes (à des fins de démonstration)
            resetPlayer(player); // Réinitialiser la position du joueur
            player->score++; // Augmenter le score en passant au niveau suivant
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

int main(void) {
    // Initialisation SDL
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    SDL_Event event;
    Player player = { 0, SCREEN_HEIGHT - 65, 35, 65, 0, 0, 0, 0 }; // Modifier les valeurs x, y, w et h en conséquence
    Platform platform = { 200, 400, 400, 20 };
    Platform secondPlatform = { 500, 300, 300, 20 };
    Obstacle obstacle1 = { 400, 300, 50, 50 };
    Obstacle obstacle2 = { 600, 450, 50, 50 };
    ExitDoor exitDoor = { 700, 200, 50, 100 };  // Ajuster la position et la taille de la porte de sortie

    int quit = 0;

    // Initialisation SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        printf("SDL n'a pas pu s'initialiser ! SDL_Error : %s\n", SDL_GetError());
        return 1;
    }

    // Création de la fenêtre
    window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf("La fenêtre n'a pas pu être créée ! SDL_Error : %s\n", SDL_GetError());
        return 1;
    }

    // Création du rendu
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("Le rendu n'a pas pu être créé ! SDL_Error : %s\n", SDL_GetError());
        return 1;
    }

    // Initialisation de SDL_ttf
    if (TTF_Init() == -1) {
        printf("SDL_ttf n'a pas pu s'initialiser ! SDL_ttf Error: %s\n", TTF_GetError());
        return 1;
    }

    // Initialisation de SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("SDL_mixer n'a pas pu s'initialiser ! SDL_mixer Error: %s\n", Mix_GetError());
        return 1;
    }

    // Chargement des sons
    jumpSound = Mix_LoadWAV("jump.wav");
    scoreSound = Mix_LoadWAV("score.wav");
    collisionSound = Mix_LoadWAV("collision.wav");

    // Boucle principale du jeu
    const int SCREEN_TICKS_PER_FRAME = 1000 / 60;
    while (!quit) {
        int startTicks = SDL_GetTicks();

        // Gestion des événements
        handleEvents(&event, &quit, &player);

        // Gestion du mouvement du joueur
        const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);
        if (currentKeyStates[SDL_SCANCODE_RIGHT] && player.x < SCREEN_WIDTH - player.w) {
            player.x += MOVEMENT_SPEED;
        }
        if (currentKeyStates[SDL_SCANCODE_LEFT] && player.x > 0) {
            player.x -= MOVEMENT_SPEED;
        }

        // Mise à jour du joueur et rendu de la scène
        updatePlayer(&player, platform, secondPlatform, obstacle1, obstacle2, exitDoor);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        // Rendu des éléments du jeu
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_Rect platformRect = { platform.x, platform.y, platform.w, platform.h };
        SDL_Rect secondPlatformRect = { secondPlatform.x, secondPlatform.y, secondPlatform.w, secondPlatform.h };
        SDL_Rect obstacleRect1 = { obstacle1.x, obstacle1.y, obstacle1.w, obstacle1.h };
        SDL_Rect obstacleRect2 = { obstacle2.x, obstacle2.y, obstacle2.w, obstacle2.h };
        SDL_Rect exitDoorRect = { exitDoor.x, exitDoor.y, exitDoor.w, exitDoor.h };

        SDL_RenderFillRect(renderer, &platformRect);
        SDL_RenderFillRect(renderer, &secondPlatformRect);
        SDL_RenderFillRect(renderer, &obstacleRect1);
        SDL_RenderFillRect(renderer, &obstacleRect2);
        SDL_RenderFillRect(renderer, &exitDoorRect);

        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
        SDL_Rect playerRect = { player.x, player.y, player.w, player.h };
        SDL_RenderFillRect(renderer, &playerRect);

        // Afficher le score a l'écran
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Fixé la couleur à noir
        TTF_Font* font = TTF_OpenFont("arial.ttf", 72); // Augmenté la taille de la font
        if (font == NULL) {
            printf("Failed to load font! SDL_ttf Error: %s\n", TTF_GetError());
        } else {
            SDL_Color textColor = {0, 0, 0, 0}; // Fixé la couleur du texte
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
