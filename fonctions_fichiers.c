#include "fonctions_fichiers.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define tailleW (512/16)
#define tailleH (320/10)
#define taille_persoW (285/3)
#define taille_persoH (250/2)

char** allouer_tab_2D(int nbLig, int nbCol) {
    char **res = malloc(nbLig * sizeof (char*));
    for (int i = 0; i < nbLig; i++) {
        *(res+i) = malloc(nbCol * sizeof (char ));
    }
    for (int i = 0; i < nbLig; ++i) {
        for (int j = 0; j < nbCol; ++j) {
            *(*(res+i)+j)=' ';
        }
    }
    return res;
}

void desallouer_tab_2D(char** tab, int nbLig) {
    for (int i=0;i<nbLig;i++){
        free(*(tab+i));
    }
}

void afficher_tab_2D(char** tab, int nbLig, int nbCol){
    printf("\nAfficher Lancement\n");
    for (int i=0;i<nbLig;i++){
        for (int j=0;j<nbCol;j++) {
            printf("%c ", tab[i][j]);
        }
        printf("\n");
    }
}

void taille_fichier(const char* nomFichier, int* nbLig, int* nbCol){
    *nbLig=0;
    *nbCol=0;
    int NBcaractere=0;
    int nbrColMAX=0;
    FILE* file;
    file=fopen(nomFichier,"r");
    if ( file == NULL ) {
        printf( "Cannot open file\n");
        return;
    }
    int c=fgetc(file);
    while (c!=EOF){
        if (c=='\n' || c=='\r'){
            if (NBcaractere>*nbCol) {
                *nbCol = NBcaractere;
            }
            if (NBcaractere>0) {
                *nbLig += 1;
                NBcaractere=0;
            }
        }
        else{
            NBcaractere+=1;
        }
        c= fgetc(file);
    }
    fclose(file);
}

/*char** lire_fichier(const char* nomFichier) {
    int *nbLig = malloc(sizeof (int ));
    int *nbCol = malloc(sizeof (int ));
    taille_fichier(nomFichier, nbLig, nbCol);
    char **res = allouer_tab_2D(*nbLig, *nbCol);
    FILE *file = NULL;
    file = fopen(nomFichier, "r");
    if (file != NULL) {
        for (int i = 0; i < *nbLig; i++) {
            fgets((*(res + i)), *nbCol, file);
        }
    }
    else{
        printf("\nerreur 1\n");
    }
    fclose(file);
    return res;
}*/
char** lire_fichier(const char* nomFichier){
    int nbLig=0;
    int nbCol=0;
    taille_fichier(nomFichier,&nbLig,&nbCol);
    char ** res= allouer_tab_2D(nbLig,nbCol);
    //afficher_tab_2D(res,nbLig,nbCol);
    FILE* file;
    file=fopen(nomFichier,"r");
    if ( file == NULL ) {
        printf( "Cannot open file\n");
        return NULL;
    }
    char c=fgetc(file);
    int NBcaractere=0,NBligne=0;
    while (c!=EOF){
        if (c=='\n' || c=='\r'){

            if (NBcaractere>0) {
                NBligne += 1;
                NBcaractere=0;
            }
        }
        else{
            res[NBligne][NBcaractere]=c;
            NBcaractere+=1;
        }
        c= fgetc(file);
    }
    fclose(file);
    return res;
}
char** modifier_caractere(char** tab, int nbLig, int nbCol, char ancien, char nouveau){
    for (int i=0;i<nbLig;i++){
        for (int j=0;j<nbCol;j++) {
            if(tab[i][j]==ancien){
                tab[i][j]=nouveau;
            }
        }
    }
    return tab;
}

void ecrire_fichier(const char* nomFichier, char** tab, int nbLig, int nbCol){
    FILE* file= fopen(nomFichier,"w+");
    if (file != NULL){
        for (int i = 0; i < nbLig; ++i) {
            for(int j=0; j<nbCol;j++){
                fputc(tab[i][j],file);
            }
            fputc('\n',file);

        }
    }
    else{
        printf( "Cannot open file\n");
        return;
    }
    fclose(file);
}

SDL_Texture* charger_image (const char* nomfichier, SDL_Renderer* renderer){
    SDL_Surface* surface=SDL_LoadBMP(nomfichier);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface) ;//chargement de l'image
    return texture;
}

int affichage(char** tab,int nbLig,int nbCol) {
    SDL_Window *fenetre; // Déclaration de la fenêtre
    SDL_Event evenements; // Événements liés à la fenêtre
    bool terminer = false;
    if (SDL_Init(SDL_INIT_VIDEO) < 0) // Initialisation de la SDL
    {
        printf("Erreur d’initialisation de la SDL: %s", SDL_GetError());
        SDL_Quit();
        return EXIT_FAILURE;
    }
    // Créer la fenêtre
    fenetre = SDL_CreateWindow("Fenetre SDL", SDL_WINDOWPOS_CENTERED,
                               SDL_WINDOWPOS_CENTERED, 22 * tailleW, 10 * tailleH, SDL_WINDOW_RESIZABLE);
    if (fenetre == NULL) // En cas d’erreur
    {
        printf("Erreur de la creation d’une fenetre: %s", SDL_GetError());
        SDL_Quit();
        return EXIT_FAILURE;
    }
    // Mettre en place un contexte de rendu de l’écran
    SDL_Renderer *ecran;
    ecran = SDL_CreateRenderer(fenetre, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture *fond = charger_image("../fond.bmp", ecran);
    SDL_Surface *surface = SDL_LoadBMP("../sprites.bmp");
    // Récupérer la valeur (RGB) du pixel au format donné.
    Uint32 pixel_transparent = SDL_MapRGB(surface->format, 0, 255, 255);
    // Définir la couleur (pixel transparent) dans une surface.
    int test = SDL_SetColorKey(surface, 1, pixel_transparent);
    SDL_Texture *perso = SDL_CreateTextureFromSurface(ecran, surface);
    SDL_Rect Src_perso;
    Src_perso.x = taille_persoW;
    Src_perso.y = taille_persoH;
    Src_perso.w = taille_persoW;
    Src_perso.h = taille_persoH;
    SDL_Rect Destr_perso;
    Destr_perso.x = 0;
    Destr_perso.y = 0;
    Destr_perso.w = taille_persoW;
    Destr_perso.h = taille_persoH;
    SDL_Rect Src_sprite[nbLig][nbCol];
    for (int i = 0; i < nbLig; i++) {
        for (int j = 0; j < nbCol; ++j) {
            Src_sprite[i][j].x = tailleW * (tab[i][j] - 48);
            Src_sprite[i][j].y = 0;
            Src_sprite[i][j].w = tailleW; // Largeur du sprite
            Src_sprite[i][j].h = tailleH; // Hauteur du sprite
        }
    }
    printf("%d\n", tab[nbLig - 1][nbCol - 1]);
    SDL_Rect DestR_sprite[nbLig][nbCol];
    for (int i = 0; i < nbCol; i++) {
        for (int j = 0; j < nbLig; ++j) {
            DestR_sprite[j][i].x = tailleW * i;
            DestR_sprite[j][i].y = tailleH * j;
            DestR_sprite[j][i].w = tailleW; // Largeur du sprite
            DestR_sprite[j][i].h = tailleH; // Hauteur du sprite
        }
    }
    SDL_Texture *texture = SDL_CreateTextureFromSurface(ecran, SDL_LoadBMP("../pavage.bmp"));//chargement de l'image
    while (!terminer) {
        SDL_RenderClear(ecran);
        SDL_RenderCopy(ecran, fond, NULL, NULL);
        for (int i = 0; i < nbLig; i++) {
            for (int j = 0; j < nbCol; ++j) {
                SDL_RenderCopy(ecran, texture, &Src_sprite[i][j], &DestR_sprite[i][j]);
            }
        }
        SDL_RenderCopy(ecran, perso, &Src_perso, &Destr_perso);
        //SDL_PollEvent
        SDL_PollEvent(&evenements);
        switch (evenements.type) {
            case SDL_QUIT:
                terminer = true;
                break;
            case SDL_KEYDOWN:
                switch (evenements.key.keysym.sym) {
                    case SDLK_ESCAPE:
                    case SDLK_q:
                        terminer = true;
                        break;
                    case SDLK_UP:
                        Destr_perso.y -=taille_persoH;
                        break;
                    case SDLK_DOWN:
                        Destr_perso.y += taille_persoH;
                        break;
                    case SDLK_LEFT:
                        Destr_perso.x -= taille_persoW;
                        break;
                    case SDLK_RIGHT:
                        Destr_perso.x += taille_persoW;
                        break;
                }
                break;
        }
        SDL_RenderPresent(ecran);
    }
    // Libérer de la mémoire
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(ecran);
    // Quitter SDL
    SDL_DestroyWindow(fenetre);
    SDL_Quit();
    printf("%d\n",tailleW);
    return 0;
}