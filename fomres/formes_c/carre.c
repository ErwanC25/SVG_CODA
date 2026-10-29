#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "creation_formes.h"

int creat_carre(float x, float y, float side_length, const char *fill_color, const char *stroke_color, float stroke_width) {
    Formes_t *forme;

    if (!isfinite(side_length) || side_length <= 0.0f) {
        printf("Erreur : côté du carré invalide");
        return 0;
    }
    forme = calloc(1, sizeof(*forme));

    if (forme == NULL) {
        printf("Erreur : impossible d'allouer de la mémoire pour le carré");
        return 0;
    }
    forme -> data.carre.side_length = side_length;

    if (!ajouter_forme(forme, FORME_CARRE, x, y, fill_color, stroke_color, stroke_width)) {
        free(forme);
        return 0;
    }
    return 1;
}