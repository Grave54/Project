#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int MOVEMENT_SPEED = 5;
const int JUMP_FORCE = 15;
const int GRAVITY = 1;
const int MAX_JUMP_COUNT = 2;

typedef struct {
    int x, y, w, h;
    int isJumping;
    int jumpCount;
    int yVelocity;
    int score;
} Player;

typedef struct {
    int x, y, w, h;
} Platform;

typedef struct {
    int x, y, w, h;
} Obstacle;

Mix_Chunk* jumpSound;
Mix_Chunk* scoreSound;
Mix_Chunk* collisionSound;

void resetPlayer(Player *player);

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
                        Mix_PlayChannel(-1, jumpSound, 0); // Play jump sound
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

void updatePlayer(Player *player, Platform platform, Platform secondPlatform, Obstacle obstacle1, Obstacle obstacle2) {
    if (player->isJumping) {
        player->y += player->yVelocity;
        player->yVelocity += GRAVITY;
        if (player->y >= SCREEN_HEIGHT - player->h) {
            player->y = SCREEN_HEIGHT - player->h;
            player->isJumping = 0;
            player->jumpCount = 0;
        }
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
        // Check collision with obstacles
        if (player->x < obstacle1.x + obstacle1.w && player->x + player->w > obstacle1.x &&
            player->y < obstacle1.y + obstacle1.h && player->y + player->h > obstacle1.y) {
            Mix_PlayChannel(-1, collisionSound, 0); // Play collision sound
            resetPlayer(player);
        }

        if (player->x < obstacle2.x + obstacle2.w && player->x + player->w > obstacle2.x &&
            player->y < obstacle2.y + obstacle2.h && player->y + player->h > obstacle2.y) {
            Mix_PlayChannel(-1, collisionSound, 0); // Play collision sound
            resetPlayer(player);
        }
    } else {
        player->y += GRAVITY * 10;

        if (player->y + player->h >= platform.y && player->y + player->h <= platform.y + platform.h
            && player->x + player->w >= platform.x && player->x <= platform.x + platform.w) {
            player->y = platform.y - player->h;
        } else if (player->y + player->h >= secondPlatform.y && player->y + player->h <= secondPlatform.y + secondPlatform.h
            && player->x + player->w >= secondPlatform.x && player->x <= secondPlatform.x + secondPlatform.w) {
            player->isJumping = 0;
            player->jumpCount = 0;
            player->y = secondPlatform.y - player->h;
        }

        if (player->y >= SCREEN_HEIGHT - player->h) {
            player->y = SCREEN_HEIGHT - player->h;
        }
        // Check collision with obstacles
        if (player->x < obstacle1.x + obstacle1.w && player->x + player->w > obstacle1.x &&
            player->y < obstacle1.y + obstacle1.h && player->y + player->h > obstacle1.y) {
            Mix_PlayChannel(-1, collisionSound, 0); // Play collision sound
            resetPlayer(player);
        }

        if (player->x < obstacle2.x + obstacle2.w && player->x + player->w > obstacle2.x &&
            player->y < obstacle2.y + obstacle2.h && player->y + player->h > obstacle2.y) {
            Mix_PlayChannel(-1, collisionSound, 0); // Play collision sound
            resetPlayer(player);
        }
    }
}

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
    Player player = { 0, SCREEN_HEIGHT - 65, 35, 65, 0, 0, 0, 0 }; // Modify x, y, w, and h values accordingly
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
    scoreSound = Mix_LoadWAV("score.wav");
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
    Mix_FreeChunk(scoreSound);
    Mix_FreeChunk(collisionSound);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    Mix_Quit();
    TTF_Quit();
    SDL_Quit();
    return 0;
}
