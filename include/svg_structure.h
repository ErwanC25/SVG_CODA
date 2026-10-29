#ifndef SVG_STRUCT_H
#define SVG_STRUCT_H

#include <stddef.h>

typedef struct Formes_s Formes_t;

typedef struct Point_s {
    int x;
    int y;
} Point_t;

typedef enum TypeForme_e {
    FORME_INVALIDE = -1,
    FORME_CERLCE,
    FORME_ELLIPSE,
    FORME_RECTANGLE,
    FORME_CARRE,
    FORME_LIGNE,
    FORME_POLYLIGNE,
    FORME_POLYGON,
    FORME_PATH,
    FORME_GROUP,
} TypeForme_t;

struct Shape_s {
    int id;
    FormeType_t type;
    float x;
    float y;
    char color[32];

    float stroke_color[32];
    float stroke_width;
    int rotation;
    int flip x;
    int flip y;
    ShapeData_t data;

    struct Forme_s *next;
};

extern Forme_t *head_formes;

