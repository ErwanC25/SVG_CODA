#include <stdio.h>
#include <stdlib.h>

#include "ui_interne.h"

const char *BRIGHT_RED = "\033[1;31m";
const char *BRIGHT_YELLOW = "\033[1;33m";
const char *BRIGHT_BLUE = "\033[1;34m";
const char *BOLD = "\033[1m";
const char *RESET = "\033[0m";

int main(void) {
        int choix = -1;
        char last_path[512] = "";

        if (atexit(free_shapes) != 0) {
            printf("Impossible d’enregistrer le nettoyage de la mémoire\n");
            return EXIT_FAILURE;
        }
        load_last_path(last_path);


        printf("\nEditeur SVG en ligne de commande");
        while (choix != 0) {
            printf("%sVotre choix\n", BOLD, RESET);
            printf("%s1 : Créer une forme\n", BRIGHT_BLUE, RESET);
            printf("%s2 : Editer forme / groupe\n");
            printf("%s3 : Gérer les fichiers SVG\n");
            printf("%s4 : Groupe de formes");
            printf("%s0 : Quitter\n", BRIGHT_RED, RESET);
            resultat = read_integer("\nVotre choix : ", 0, 4, &choix, NULL);
            if (resultat < 0 || resultat > 4) {
                printf("Choix invalide, veuillez entrer un chiffre valide. \n");
                contunue;
            }

            switch (choix) {
                case 1 :
                    if (menu_creation_forme() == 0)
                        return EXIT_SUCCESS;
                    break;
                case 2 :
                    if (menu_editer_forme() == 0)
                        return EXIT_SUCCESS;
                    break;
                case 3 :
                    if (menu_fichier(last_path) == 0)
                        return EXIT_SUCCESS;
                    break;
                case 0 :
                    printf("fin du programme.\n");
                    break;
            }

        }
    return EXIT_SUCCESS;
}