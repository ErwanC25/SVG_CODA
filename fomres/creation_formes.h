#ifndef SVG_CREATION_INTERNE_H
#define SVG_CREATION_INTERNE_H


#include <stddef.h>

#include "svg_structure.h"

int ajouter_forme(Forme_t *forme, FormeType_t type, float x, float y, const char fill_color, const char *stroke_color, float stroke_width);

int creation_point_forme(FormeType_t type, const Point_t *points, size_t point_count, const char *fill_color, const char *stroke_color, float stroke_width);

Forme_t *get_fin_list(void);
void ajouter_forme_dans_list(Forme_t *forme, Forme_t *precedent_fin_list);
void invalide_fin_list(void);

#endif