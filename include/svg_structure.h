#ifndef SVG_STRUCT_H
#define SVG_STRUCT_H

#include <stddef.h>

typedef struct Forme_s Forme_t;

typedef struct Point_s {
    int x;
    int y;
} Point_t;

typedef enum FormeType_e {
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
} FormeType_t;

typedef union DataForme_u {
    Cerlce_t cercle;
    Ellipse_t ellipse;
    Rectangle_t rectangle;
    Carre_t carre;
    Ligne_t ligne;
    Polyligne polyligne;
    Polygon_t polygon;
    Path_t path;
    Group_t group;
} DataForme_t;

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
    DataForme_t data;

    struct Forme_s *next;
};

extern Forme_t *head_forme;

#endif