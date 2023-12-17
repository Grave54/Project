#ifndef fonctions_fichiers_h
#define fonctions_fichiers_h
#include <SDL2/SDL.h>

char** allouer_tab_2D(int nbLig, int nbCol);
void desallouer_tab_2D(char** tab, int nbLig);
void afficher_tab_2D(char** tab, int nbLig, int nbCol);
void taille_fichier(const char* nomFichier, int* nbLig, int* nbCol);
char** lire_fichier(const char* nomFichier);
char** modifier_caractere(char** tab, int nbLig, int nbCol, char ancien, char nouveau);
void ecrire_fichier(const char* nomFichier, char** tab, int nbLig, int nbCol);
SDL_Texture* charger_image (const char* nomfichier, SDL_Renderer* renderer);
int affichage(char** tab,int nbLig,int nbCol);
#endif //fonctions_fichiers_h
