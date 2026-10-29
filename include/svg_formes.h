#ifndef SVG_FORMES_H
#define SVG_FORMES_H

#include "svg_struct.h"
#include <stddef.h>

int create_circle(float x, float y, float radius, const char *fill_color, const char *stroke_color, float stroke_width);
int create_ellipse(float x, float y, float radius_x, float radius_y, const char *fill_color, const char *stroke_color, float stroke_width);

int creat_carre(float x, float y, float side_length, const char *fill_color, const char *stroke_color, float stroke_width);
int creat_rectangle();

void free_formes(void);
void reset_id_counter(void);
void sync_id_counter(void);

#endif