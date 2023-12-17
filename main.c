	#include <SDL2/SDL.h>
	#include <SDL2/SDL_ttf.h>
	#include <SDL2/SDL_mixer.h>
	#include <stdio.h>
	#include <stdbool.h>

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
	    int isDucking;
	    int score;
	} Player;

	typedef struct {
	    int x, y, w, h;
	} Platform;

	typedef struct {
	    int x, y, w, h;
	} Obstacle;

	typedef struct {
	    int x, y, w, h;
	} ExitDoor;

	typedef struct {
	    SDL_Rect buttonRect;
	    int isVisible;
	} Menu;

	Mix_Chunk* jumpSound;
	Mix_Chunk* scoreSound;
	Mix_Chunk* collisionSound;

	void resetPlayer(Player *player);

	int checkCollision(SDL_Rect a, SDL_Rect b);

	void handleEvents(SDL_Event *event, int *quit, Player *player, Menu *menu) {
		while (SDL_PollEvent(event) != 0) {
		if (event->type == SDL_QUIT) {
		    *quit = 1;
		} else if (event->type == SDL_KEYDOWN) {
		    if (!menu->isVisible) {  // Ignorer les touches si le menu est ouvert
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
		    }
		} else if (event->type == SDL_KEYUP) {
		    if (!menu->isVisible) {  // Ignorer les touches si le menu est ouvert
		        switch (event->key.keysym.scancode) {
		            case SDL_SCANCODE_DOWN:
		                player->isDucking = 0;
		                break;
		            default:
		                break;
		        }
		    }
		} else if (event->type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_LEFT) {
		    // Créer un objet SDL_Point avec les coordonnées du clic de la souris
		    SDL_Point mousePoint = { event->button.x, event->button.y };
		    // Vérifier si le clic de la souris est dans le rectangle du bouton du menu
		    if (SDL_PointInRect(&mousePoint, &menu->buttonRect)) {
		        menu->isVisible = 1;
		    }

		    // Ajouter une condition pour vérifier si le menu est visible et gérer le clic sur le bouton "Continuer"
		    if (menu->isVisible) {
		        SDL_Rect continueButtonRect = { SCREEN_WIDTH / 4 + 20, SCREEN_HEIGHT / 4 + 20, SCREEN_WIDTH / 2 - 40, 50 };
		        if (SDL_PointInRect(&mousePoint, &continueButtonRect)) {
		            // Si le bouton "Continuer" est cliqué
		            menu->isVisible = 0; // Cacher le menu
		        }
		    }
		}
	    }
	}



	void updatePlayer(Player *player, Platform platform, Platform secondPlatform, Obstacle obstacle1, Obstacle obstacle2, ExitDoor exitDoor) {
	    if (player->isDucking) {
		player->h = 30;
	    } else {
		// Rétablir la taille du joueur à la valeur normale
		if (player->isJumping) {
		    player->h = 65;
		} else {
		    // Si le joueur ne saute pas, ajuster la position en fonction de la nouvelle hauteur
		    int previousHeight = player->h;
		    player->h = 65;
		    player->y -= (player->h - previousHeight);
		}
	    }
	    if (player->isJumping) { // Si le joueur saute
		player->y += player->yVelocity;
		player->yVelocity += GRAVITY;

		if (player->y >= SCREEN_HEIGHT - player->h) { // Si le joueur touche la bordure de l'écran
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

		if (checkCollision((SDL_Rect){player->x, player->y, player->w, player->h}, (SDL_Rect){exitDoor.x, exitDoor.y, exitDoor.w, exitDoor.h})) {
		    printf("Congratulations! You passed the level!\n");
		    SDL_Delay(2000);
		    resetPlayer(player);
		    player->score++;
		}

		// Si le joueur n'est plus en train de sauter, réinitialiser le nombre de sauts effectués
		if (!player->isJumping) {
		    player->jumpCount = 0;
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

		if (checkCollision((SDL_Rect){player->x, player->y, player->w, player->h}, (SDL_Rect){exitDoor.x, exitDoor.y, exitDoor.w, exitDoor.h})) {
		    printf("Congratulations! You passed the level!\n");
		    SDL_Delay(2000);
		    resetPlayer(player);
		    player->score++;
		}

		if (checkCollision((SDL_Rect){player->x, player->y + 1, player->w, player->h}, (SDL_Rect){platform.x, platform.y, platform.w, platform.h}) ||
		    checkCollision((SDL_Rect){player->x, player->y + 1, player->w, player->h}, (SDL_Rect){secondPlatform.x, secondPlatform.y, secondPlatform.w, secondPlatform.h})) {
		    player->jumpCount = 0;
		}
	    }
	}

	int checkCollision(SDL_Rect a, SDL_Rect b) {
	    return a.x < b.x + b.w && a.x + a.w > b.x && a.y < b.y + b.h && a.y + a.h > b.y;
	}

	void resetPlayer(Player *player) {
	    player->x = 0;
	    player->y = SCREEN_HEIGHT - 65;
	    player->isJumping = 0;
	    player->jumpCount = 0;
	    player->yVelocity = 0;
	}

	TTF_Font* continueButtonFont;
	SDL_Color textColor = { 255, 255, 255, 0 };  // Blanc

	int main(void) {
	    SDL_Window* window = NULL;
	    SDL_Renderer* renderer = NULL;
	    SDL_Event event;
	    Player player = { 0, SCREEN_HEIGHT - 65, 35, 65, 0, 0, 0, 0, 0 };
	    Platform platform = { 200, 400, 400, 20 };
	    Platform secondPlatform = { 500, 300, 300, 20 };
	    Obstacle obstacle1 = { 400, 300, 50, 50 };
	    Obstacle obstacle2 = { 600, 450, 50, 50 };
	    ExitDoor exitDoor = { 700, 200, 50, 100 };
	    Menu menu = { { SCREEN_WIDTH - 100, 0, 100, 50 }, 0 };


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
	    
	    // Charger la police pour le bouton "Continue"
	    continueButtonFont = TTF_OpenFont("arial.ttf", 24);
	    if (continueButtonFont == NULL) {
		printf("Failed to load font for continue button! SDL_ttf Error: %s\n", TTF_GetError());
		return 1;
	    }

	    jumpSound = Mix_LoadWAV("jump.wav");
	    scoreSound = Mix_LoadWAV("score.wav");
	    collisionSound = Mix_LoadWAV("collision.wav");

	    const int SCREEN_TICKS_PER_FRAME = 1000 / 60;

	    while (!quit) {
		int startTicks = SDL_GetTicks();

		handleEvents(&event, &quit, &player, &menu);

		const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);
		if (currentKeyStates[SDL_SCANCODE_RIGHT] && player.x < SCREEN_WIDTH - player.w) {
		    if (!menu.isVisible){
		    player.x += MOVEMENT_SPEED;
		}}
		if (currentKeyStates[SDL_SCANCODE_LEFT] && player.x > 0) {
		if (!menu.isVisible){
		    player.x -= MOVEMENT_SPEED;
		}}

		updatePlayer(&player, platform, secondPlatform, obstacle1, obstacle2, exitDoor);

		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderClear(renderer);

		SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
		SDL_Rect platformRect = { platform.x, platform.y, platform.w, platform.h };
		SDL_Rect secondPlatformRect = { secondPlatform.x, secondPlatform.y, secondPlatform.w, secondPlatform.h };
		SDL_RenderFillRect(renderer, &platformRect);
		SDL_RenderFillRect(renderer, &secondPlatformRect);

		SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);			
		SDL_Rect obstacleRect1 = { obstacle1.x, obstacle1.y, obstacle1.w, obstacle1.h };
		SDL_Rect obstacleRect2 = { obstacle2.x, obstacle2.y, obstacle2.w, obstacle2.h };
		SDL_RenderFillRect(renderer, &obstacleRect1);
		SDL_RenderFillRect(renderer, &obstacleRect2);

		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
		SDL_Rect exitDoorRect = { exitDoor.x, exitDoor.y, exitDoor.w, exitDoor.h };
		SDL_RenderFillRect(renderer, &exitDoorRect);

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
		SDL_Rect playerRect = { player.x, player.y, player.w, player.h };
		SDL_RenderFillRect(renderer, &playerRect);

		// Afficher le bouton noir
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderFillRect(renderer, &menu.buttonRect);

		// Vérifier si la souris est sur le bouton
		int mouseX, mouseY;
		SDL_GetMouseState(&mouseX, &mouseY);
		SDL_Rect mouseRect = { mouseX, mouseY, 1, 1 };

		if (SDL_HasIntersection(&mouseRect, &menu.buttonRect)) {
		    // Afficher un effet visuel pour indiquer que le bouton est survolé
		    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
		    SDL_RenderFillRect(renderer, &menu.buttonRect);

		    // Si le bouton est cliqué
		    if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
		        menu.isVisible = 1; // Afficher le menu
		    }
		}

		if (menu.isVisible) {
		    // Afficher le menu
		    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
		    SDL_Rect menuRect = { SCREEN_WIDTH / 4, SCREEN_HEIGHT / 4, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
		    SDL_RenderFillRect(renderer, &menuRect);

		    // Ajouter le bouton "Continuer"
		    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		    SDL_Rect continueButtonRect = { menuRect.x + 20, menuRect.y + 20, menuRect.w - 40, 50 };
		    SDL_RenderFillRect(renderer, &continueButtonRect);

		    // Vérifier si la souris est sur le bouton "Continuer"
		    if (SDL_HasIntersection(&mouseRect, &continueButtonRect)) {
			// Afficher un effet visuel pour indiquer que le bouton est survolé
			SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
			SDL_RenderFillRect(renderer, &continueButtonRect);

			// Si le bouton "Continuer" est cliqué
			if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
			    menu.isVisible = 0; // Cacher le menu
			}
		    }
		    // Ajouter le texte "Continue" sur le bouton "Continue"
		    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0); // Blanc
		    char continueButtonText[] = "Continue";
		    SDL_Surface* continueButtonSurface = TTF_RenderText_Solid(continueButtonFont, continueButtonText, textColor);
		    SDL_Texture* continueButtonTexture = SDL_CreateTextureFromSurface(renderer, continueButtonSurface);

		    // Positionner le texte au centre du bouton
		    int buttonTextX = continueButtonRect.x + (continueButtonRect.w - continueButtonSurface->w) / 2;
		    int buttonTextY = continueButtonRect.y + (continueButtonRect.h - continueButtonSurface->h) / 2;

		    SDL_Rect continueButtonTextRect = { buttonTextX, buttonTextY, continueButtonSurface->w, continueButtonSurface->h };
		    SDL_RenderCopy(renderer, continueButtonTexture, NULL, &continueButtonTextRect);
		    SDL_FreeSurface(continueButtonSurface);
		    SDL_DestroyTexture(continueButtonTexture);
		}


		// Afficher le score a lecran
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		TTF_Font* font = TTF_OpenFont("arial.ttf", 72);
		if (font == NULL) {
		    printf("Failed to load font! SDL_ttf Error: %s\n", TTF_GetError());
		} else {
		    SDL_Color textColor = { 0, 0, 0, 0 };
		    char scoreText[50];
		    sprintf(scoreText, "Score: %d", player.score);
		    SDL_Surface* scoreSurface = TTF_RenderText_Solid(font, scoreText, textColor);
		    SDL_Texture* scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);
		    SDL_Rect scoreRect = { 10, 10, scoreSurface->w, scoreSurface->h };
		    SDL_RenderCopy(renderer, scoreTexture, NULL, &scoreRect);
		    SDL_FreeSurface(scoreSurface);
		    SDL_DestroyTexture(scoreTexture);
		    TTF_CloseFont(font);
		}
		
		// Afficher le texte "MENU" sur le rectangle du menu
		TTF_Font* menuFont = TTF_OpenFont("arial.ttf", 24);
		
		if (menuFont == NULL) {
		    printf("Failed to load font for menu! SDL_ttf Error: %s\n", TTF_GetError());
		} else {
		    SDL_Color menuTextColor = { 255, 255, 255, 0 };  // Blanc
		    char menuText[] = "MENU";
		    SDL_Surface* menuSurface = TTF_RenderText_Solid(menuFont, menuText, menuTextColor);
		    SDL_Texture* menuTexture = SDL_CreateTextureFromSurface(renderer, menuSurface);

		    // Positionner le texte au centre du bouton
		    int textX = menu.buttonRect.x + (menu.buttonRect.w - menuSurface->w) / 2;
		    int textY = menu.buttonRect.y + (menu.buttonRect.h - menuSurface->h) / 2;

		    SDL_Rect menuTextRect = { textX, textY, menuSurface->w, menuSurface->h };
		    SDL_RenderCopy(renderer, menuTexture, NULL, &menuTextRect);
		    SDL_FreeSurface(menuSurface);
		    SDL_DestroyTexture(menuTexture);
		    TTF_CloseFont(menuFont);
		    
		}	
		SDL_RenderPresent(renderer);

		int frameTicks = SDL_GetTicks() - startTicks;
		if (frameTicks < SCREEN_TICKS_PER_FRAME) {
		    SDL_Delay(SCREEN_TICKS_PER_FRAME - frameTicks);
		}
	    }

	    // Libérer les ressources
	    Mix_FreeChunk(jumpSound);
	    Mix_FreeChunk(scoreSound);
	    Mix_FreeChunk(collisionSound);
	    TTF_CloseFont(continueButtonFont);
	    SDL_DestroyRenderer(renderer);
	    SDL_DestroyWindow(window);
	    Mix_Quit();
	    TTF_Quit();
	    SDL_Quit();
	    return 0;
	}

