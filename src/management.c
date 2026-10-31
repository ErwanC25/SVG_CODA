#include <stddef.h>
#include <stdlib.h>

#include "svg_structure.h"
#include "svg_formes.h"
#include "creation_formes.h"


static const char *forme_type_nom(FormeType_t type)
{
    switch (type)
    {
        case FORME_CIRCLE:
            return "cercle";
        case FORME_ELLIPSE:
            return "ellipse";
        case FORME_RECTANGLE:
            return "rectangle";
        case FORME_SQUARE:
            return "carré";
        case FORME_LINE:
            return "ligne";
        case FORME_POLYLINE:
            return "polyligne";
        case FORME_POLYGON:
            return "polygon";
        case FORME_PATH:
            return "path";
        case FORME_GROUP:
            return "group";
        default:
            return "shape";
    }
}

static void free_forme(Forme_t *forme) {
    if (forme -> type == FORME_GROUP) {
        Forme_t *child = forme -> data.group.chlidren;
        while (child != NULL) {
            Forme_t *next = child -> next;
            child -> next = NULL;
            free_forme(child);
            child = next;
        }
    }

    else if (forme -> type == FORME_PATH)
        free(forme -> data.path.data);
    else if (forme -> type == FORME_POLYLIGNE)
        free(forme -> data.polyligne.points);
    else if (forme -> type == FORME_POLIGON)
        free(forme -> data.polygon.points);
    free(forme);
}