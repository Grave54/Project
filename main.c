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
	    int level; 	
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
	
	// Déclarer la nouvelle structure pour stocker les données de l'obstacle à partir du fichier texte
	typedef struct {
	    int x, y, w, h;
	} ObstacleFromFile;

    // Déclarer la nouvelle structure pour stocker les données de l'obstacle à partir du fichier texte
	typedef struct {
	    int x, y, w, h;
	} PlatformFromFile;


	// Prototyper la nouvelle fonction
	ObstacleFromFile* readObstaclesFromFile(const char* filename, int* obstacleCount) {
	    FILE* file = fopen(filename, "r");
	    if (file == NULL) {
		fprintf(stderr, "Unable to open file: %s\n", filename);
		exit(EXIT_FAILURE);
	    }

	    fscanf(file, "%d", obstacleCount);

	    // Allouer de la mémoire pour stocker les obstacles
	    ObstacleFromFile* obstacles = (ObstacleFromFile*)malloc(*obstacleCount * sizeof(ObstacleFromFile));

	    // Lire les données des obstacles à partir du fichier
	    for (int i = 0; i < *obstacleCount; ++i) {
		fscanf(file, "%d %d %d %d", &obstacles[i].x, &obstacles[i].y, &obstacles[i].w, &obstacles[i].h);
	    }

	    fclose(file);

	    return obstacles;

	}

    PlatformFromFile* readPlatformsFromFile(const char* filename, int* platformCount) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Unable to open file: %s\n", filename);
        exit(EXIT_FAILURE);
    }

    fscanf(file, "%d", platformCount);

    // Allouer de la mémoire pour stocker les plateformes
    PlatformFromFile* platforms = (PlatformFromFile*)malloc(*platformCount * sizeof(PlatformFromFile));

    // Lire les données des plateformes à partir du fichier
    for (int i = 0; i < *platformCount; ++i) {
        fscanf(file, "%d %d %d %d", &platforms[i].x, &platforms[i].y, &platforms[i].w, &platforms[i].h);
    }

    fclose(file);

    return platforms;
}

	
	void drawObstacles(SDL_Renderer* renderer, ObstacleFromFile* obstacles, int obstacleCount) {
	    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 0);  // Couleur bleu
	    for (int i = 0; i < obstacleCount; ++i) {
		SDL_Rect obstacleRect = { obstacles[i].x, obstacles[i].y, obstacles[i].w, obstacles[i].h };
		SDL_RenderFillRect(renderer, &obstacleRect);
	    }
	}

    void drawPlatforms(SDL_Renderer* renderer, PlatformFromFile* platforms, int platformCount) {
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Couleur verte pour les plateformes
    for (int i = 0; i < platformCount; ++i) {
        SDL_Rect platformRect = { platforms[i].x, platforms[i].y, platforms[i].w, platforms[i].h };
        SDL_RenderFillRect(renderer, &platformRect);
    }
}

	
	Mix_Chunk* jumpSound;
	Mix_Chunk* levelSound;
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



void updatePlayer(Player *player, PlatformFromFile* platforms, int platformCount, ExitDoor exitDoor, ObstacleFromFile* obstacles, int obstacleCount) {
    // Gérer la hauteur du joueur en fonction de son état (debout, accroupi, sautant)
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

    // Mettre à jour la position du joueur en fonction de son état (sautant ou non)
    if (player->isJumping) { // Si le joueur saute
        player->y += player->yVelocity;
        player->yVelocity += GRAVITY;

        // Gérer les collisions avec le sol
        if (player->y >= SCREEN_HEIGHT - player->h) {
            player->y = SCREEN_HEIGHT - player->h;
            player->isJumping = 0;
            player->jumpCount = 0;
        }

        // Vérifier la collision avec la porte de sortie
        if (checkCollision((SDL_Rect){player->x, player->y, player->w, player->h},
                           (SDL_Rect){exitDoor.x, exitDoor.y, exitDoor.w, exitDoor.h})) {
            SDL_Delay(2000);
            resetPlayer(player);
            player->level++;
        }

        // Si le joueur n'est plus en train de sauter, réinitialiser le nombre de sauts effectués
        if (!player->isJumping) {
            player->jumpCount = 0;
        }
    } else {
        // Si le joueur ne saute pas, appliquer la gravité
        player->y += GRAVITY * 10;

        // Gérer les collisions avec le sol
        if (player->y >= SCREEN_HEIGHT - player->h) {
            player->y = SCREEN_HEIGHT - player->h;
        }

        // Vérifier la collision avec la porte de sortie
        if (checkCollision((SDL_Rect){player->x, player->y, player->w, player->h},
                           (SDL_Rect){exitDoor.x, exitDoor.y, exitDoor.w, exitDoor.h})) {
            SDL_Delay(2000);
            resetPlayer(player);
            player->level++;
        }
    }

    // Vérifier les collisions avec les obstacles
    for (int i = 0; i < obstacleCount; ++i) {
        if (checkCollision((SDL_Rect){player->x, player->y, player->w, player->h},
                           (SDL_Rect){obstacles[i].x, obstacles[i].y, obstacles[i].w, obstacles[i].h})) {
            Mix_PlayChannel(-1, collisionSound, 0);
            resetPlayer(player);
            player->level = 1;
        }
    }

    // Vérifier la collision avec les plateformes lues à partir du fichier
    for (int i = 0; i < platformCount; ++i) {
        if (checkCollision((SDL_Rect){player->x, player->y, player->w, player->h},
                           (SDL_Rect){platforms[i].x, platforms[i].y, platforms[i].w, platforms[i].h})) {
            player->isJumping = 0;
            player->jumpCount = 0;
            player->y = platforms[i].y - player->h;
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
	    Player player = { 0, SCREEN_HEIGHT - 65, 35, 65, 0, 0, 0, 0, 1 };
	    ExitDoor exitDoor = { 700, 200, 50, 100 };
	    Menu menu = { { SCREEN_WIDTH - 100, 0, 100, 50 }, 0 };
	    
	    // Charger les obstacles à partir du fichier
	    int obstacleCount;
	    ObstacleFromFile* obstacles = readObstaclesFromFile("obstacles1.txt", &obstacleCount);

            // Charger les plateformes à partir du fichier
            int platformCount;
            PlatformFromFile* platforms = readPlatformsFromFile("platforms1.txt", &platformCount);

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
	    levelSound = Mix_LoadWAV("level.wav");
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

		updatePlayer(&player, platforms, platformCount, exitDoor, obstacles, obstacleCount);

		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderClear(renderer);

		SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);				
		
		drawObstacles(renderer, obstacles, obstacleCount);


        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
       drawPlatforms(renderer, platforms, platformCount);

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


		// Afficher le level a lecran
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		TTF_Font* font = TTF_OpenFont("arial.ttf", 72);
		if (font == NULL) {
		    printf("Failed to load font! SDL_ttf Error: %s\n", TTF_GetError());
		} else {
		    SDL_Color textColor = { 0, 0, 0, 0 };
		    char levelText[50];
		    sprintf(levelText, "Score: %d", player.level);
		    SDL_Surface* levelSurface = TTF_RenderText_Solid(font, levelText, textColor);
		    SDL_Texture* levelTexture = SDL_CreateTextureFromSurface(renderer, levelSurface);
		    SDL_Rect levelRect = { 10, 10, levelSurface->w, levelSurface->h };
		    SDL_RenderCopy(renderer, levelTexture, NULL, &levelRect);
		    SDL_FreeSurface(levelSurface);
		    SDL_DestroyTexture(levelTexture);
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
	    free(obstacles);
	    Mix_FreeChunk(jumpSound);
	    Mix_FreeChunk(levelSound);
	    Mix_FreeChunk(collisionSound);
	    TTF_CloseFont(continueButtonFont);
	    SDL_DestroyRenderer(renderer);
	    SDL_DestroyWindow(window);
	    Mix_Quit();
	    TTF_Quit();
	    SDL_Quit();
	    return 0;
	}
