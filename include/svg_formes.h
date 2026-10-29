#ifndef SVG_FORMES_H
#define SVG_FORMES_H

#include "svg_struct.h"
#include <stddef.h>

int create_circle(float x, float y, float rayon, const char *fill_color, const char *stroke_color, float stroke_width);
int create_ellipse(float x, float y, float rayon_x, float rayon_y, const char *fill_color, const char *stroke_color, float stroke_width);

int create_carre(float x, float y, float side_length, const char *fill_color, const char *stroke_color, float stroke_width);
int create_rectangle(float x, float y, float width, float height, const char *fill_color, const char *stroke_color, float stroke_width);

int create_ligne(float x1, float y1, float x2, float y2, const char *fill_color, float stroke_width);

int create_polyline(const Point_t *points, size_t point_count, const char *stroke_color, float stroke_width);
int create_polygon(const Point_t *points, size_t point_count, const char *fill_color, const char *stroke_color, float stroke_width);
int ceate_path(const char *data, const char *fill_color, const char *stroke_color, float stroke_width);
int valider_path_data(const char *data);
int create_group(const int *ids, size_t, size_t id_count);
int ungroup_forme(int id);

int display_forme(void);

int forme_exists(int id);
FormeType_t forme_type(int id);
int delete_forme(int id);

int modifier_position_forme(int id, int x, int y);

int modifier_rayon_cercle(int id, float rayon);
int modifier_dimensions_forme(int id, float dimension_x, float dimension_y);
int modifier_points_ligne(int id, float x1, float y1, float x2, float y2);
int modifier_points_polygon(int id, const Point_t *points, size_t point_count);
int modifier_path(int id, const char *data);

int modifier_color_forme(int id, const char *color, int stroke);

int set_transformations_forme(int id, int rotation, int flip_x, int flip_y);
int modifier_transformations_forme(int id, int axis, int value);

void free_formes(void);
void reset_id_counter(void);
void sync_id_counter(void);

#endif