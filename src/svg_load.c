#include <stddef.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>

#include "svg_io.h"
#include "svg_formes.h"
#include "svg_structure.h"
#include "ceation_formes.h"


static void free_temporary_list(Forme_t *forme) {
    Forme_t *precedent = Forme_head;
    Forme_head = forme;
    free_forme();
    Forme_head = precedent;
}

static int lire_attribue(const char *element, const char *name, char *value, size_t value_size) {
    size_t name_length = strlen(name);
    const char *position = element;

    while ((position = strstr(position, name)) != NULL) {
        const char *start;
        const char *end;
        size_t length;

        if (position != element && !isspace((unsigned char) position[-1])) {
            position += name_length;
            continue;
        }
        start = position + name_length;
        while (isspace((unsigned char)*start))
            start++;

        if (*start != '=') {
            position += name_length;
            continue;
        }
        start ++;

        while (isspace((unsigned char)*start))
            start++;
        if (*start != '"' && *start != '\'')
            return 0;
        end = strchr(start, *start);
        if (end == NULL)
            return 0;

        length = (size_t)(end - (start + 1));
        if (length == 0 || length >= value_size)
            return 0;
        memcpy(value, start + 1, length);
        value[length] = '\0';
        return 1;

    }
    return 0;
}

static int lire_nombre_attribue(const char *element, const char *name, float *count) {
    char texte[64];
    char *end;

    if (!lire_attribue(element, name, texte, sizeof(texte)))
        return 0;

    errno = 0;
    *count = strtof(texte, &end);
    if (end == texte || errno == ERANGE || !isfinite(*count))
        return 0;
    while (isspace((unsigned char)*end))
        end++;

    return *end == '\0';
}

static int lire_entier_attribue(const char *element, const char *name, int *count) {
    char texte[64];
    char *end;
    long value;

    if (!lire_attribue(element, name, texte, sizeof(texte)))
        return 0;

    errno = 0;
    value = strtol(texte, &end, 10);
    if (end == texte || errno == ERANGE || value < INT_MIN || value > INT_MAX)
        return 0;
    while (isspace((unsigned char)*end))
        end++;
    if (*end != '\0')
        return 0;
    *count = (int)value;

    return 1;
}

static int lire_svg_style(const char *element, char fill_color[32], char stroke_color[32], float *stroke_width, int *rotation, int *flip_x, int *flip_y) {
    char width[64];
    const char *element_end = strchr(element, '>');

    if (element_end == NULL)
        return 0;
    while (element_end > element && isspace((unsigned char)element_end[-1]))
        element_end--;
    if (element_end == element || element_end[-1] != '/' || !lire_attribue(element, "fill", fill_color, 32) || !lire_attribue(element, "stroke", stroke_color, 32))
        return 0;

    if (strstr(element, "stroke-width") != NULL) {
        char *end;
        if (!lire_attribue(element, "stroke-width", width, sizeof(width)))
            return 0;
        errno = 0;
        *stroke_width = strtof(width, &end);
        if (end == width || errno == ERANGE || !isfinite(*stroke_width))
            return 0;
        while (isspace((unsigned char)*end))
            end++;
        if (*end != '\0')
            return 0;
    }
    else {
        *stroke_width = 1.0f;
    }

    *rotation = 0;
    *flip_x = 0;
    *flip_y = 0;

    if ((strstr(element, "data-rotation") != NULL && !lire_entier_attribue(element, "data-rotation", rotation)) ||
        (strstr(element, "data-invert-x") != NULL && !lire_entier_attribue(element, "data-invert-x", flip_x)) ||
        (strstr(element, "data-invert-y") != NULL && !lire_entier_attribue(element, "data-invert-y", flip_y)) ||
        (*flip_x != 0 && *flip_x != 1) || (*flip_y != 0 && *flip_y != 1))
        return 0;
    return *stroke_width > 0.0f;
}

static int lire_svg_points(const char *element, Point_t **points, size_t *point_count) {
    char texte[65536];
    char *position;
    Point_t *result;
    size_t compte = 0;
    size_t capacity = 16;

    if (!lire_attribue(element, "points", texte, sizeof(texte)))
        return 0;
    result = malloc(capacity * sizeof(*result));
    if (result == NULL)
        return 0;
    position = texte;

    while (*position != '\0') {
        char *end;
        long x;
        long y;
        int espace;
        while (isspace((unsigned char)*position))
            position++;
        if (*position == '\0')
            break;
        if (*position == ',')
            goto invalid;
        errno = 0;
        x = strtol(position, &end, 10);
        if (end == position || errno == ERANGE || x < INT_MIN || x > INT_MAX)
            goto invalid;
        position = end;
        espace = 0;

        while (isspace((unsigned char)*position)) {
            espace = 1;
            position++;
        }

        if (*position != ',') {
            if (!espace)
                goto invalid;
        }
        else
            position++;
        while (isspace((unsigned char)*position))
            position++;
        if (*position == '\0' || *position == ',')
            goto invalid;
        errno = 0;
        y = strtol(position, &end, 10);
        if (end == position || errno == ERANGE || y < INT_MIN || y > INT_MAX)
            goto invalid;
        position = end;
        if (compte == capacity) {
            Point_t *agrandi;
            if (capacity >= 2048)
                goto invalid;
            capacity *= 2;
            agrandi = realloc(result, capacity * sizeof(*result));
            if (agrandi == NULL)
                goto invalid;
            result = agrandi;
        }

        result[compte].x = (int)x;
        result[compte].y = (int)y;
        compte++;
        espace = 0;

        while (isspace((unsigned char)*position)) {
            espace = 1;
            position++;
        }

        if (*position == ',') {
            position++;
            while (isspace((unsigned char)*position))
                position++;
            if (*position == '\0' || *position == ',')
                goto invalid;
        }
        else if (*position != '\0' && !espace)
            goto invalid;
    }
    if (compte < 2 || compte > 2048)
        goto invalid;
    *points = result;
    *point_count = compte;
    return 1;

invalid:
    free(result);
    return 0;
}

typedef struct GroupLoad_s {
    Forme_t *parent;
    Forme_t *parent_tail;
    float x;
    float y;
    int rotation;
    int flip_x;
    int flip_y;
} GroupLoad_t;

int load(const char *path) {
    FILE *f_in;
    char ligne[65536];
    Forme_t *previous_drawing =Forme_head;
    int root_found = 0;
    int closing_tag_found = 0;
    int error = 0;
    int shape_count = 0;
    GroupLoad_t groups[64];
    size_t depth = 0;

    if (path == NULL || path[0] == '\0') {
        fprintf(stderr, "Invalid load path.\n");
        return 0;
    }
    f_in = fopen(path, "r");

    if (f_in == NULL) {
        perror("Unable to open the SVG file");
        return 0;
    }


    Forme_head = NULL;
    invalidate_list_tail();
    reset_id_counter();

    while (fgets(ligne, sizeof(ligne), f_in) != NULL) {
        char *group_start = strstr(ligne, "<g");
        char *group_end = strstr(ligne, "</g>");

        if (group_start != NULL && group_start[2] != '>' &&
            !isspace((unsigned char)group_start[2]))
            group_start = NULL;
        if (group_start != NULL) {
            char *element_end = strchr(group_start, '>');
            GroupLoad_t *group;
            if (depth == 64 || element_end == NULL || (element_end > group_start && element_end[-1] == '/')) {
                error = 1;
                break;
            }
            group = &groups[depth++];
            group->parent = Forme_head;
            group->parent_tail = get_fin_list();
            Forme_head = NULL;
            invalidate_list_tail();
            group->x = 0.0f;
            group->y = 0.0f;
            group->rotation = 0;
            group->flip_x = 0;
            group->flip_y = 0;

            if (strstr(group_start, "data-x") != NULL && !lire_nombre_attribue(group_start, "data-x", &group->x))
                error = 1;

            if (strstr(group_start, "data-y") != NULL && !lire_nombre_attribue(group_start, "data-y", &group->y))
                error = 1;

            if (strstr(group_start, "data-rotation") != NULL && !lire_entier_attribue(group_start, "data-rotation", &group->rotation))
                error = 1;

            if (strstr(group_start, "data-invert-x") != NULL && !lire_entier_attribue(group_start, "data-invert-x", &group->flip_x))
                error = 1;

            if (strstr(group_start, "data-invert-y") != NULL &&
                !lire_entier_attribue(group_start, "data-invert-y", &group->flip_y))
                error = 1;

            if (group->flip_x < 0 || group->flip_x > 1 || group->flip_y < 0 || group->flip_y > 1)
                error = 1;

            if (error)
                break;

            if (strchr(ligne, '\n') == NULL && !feof(f_in)) {
                error = 1;
                break;
            }
            continue;
        }


        if (group_end != NULL) {
            Forme_t *child = Forme_head;
            size_t child_count = 0;
            size_t i = 0;
            int *ids;

            while (child != NULL) {
                child_count++;
                child = child->next;
            }

            if (depth == 0 || child_count == 0) {
                error = 1;
                break;
            }
            ids = malloc(child_count * sizeof(*ids));
            if (ids == NULL) {
                error = 1;
                break;
            }
            child = Forme_head;
            while (child != NULL) {
                ids[i++] = child->id;
                child = child->next;
            }
            if (!creer_group(ids, child_count)) {
                free(ids);
                error = 1;
                break;
            }

            free(ids);
            {
                GroupLoad_t group = groups[--depth];
                Forme_t *creation_Forme = get_fin_list();
                creation_Forme->x = group.x;
                creation_Forme->y = group.y;
                creation_Forme->rotation = group.rotation;
                creation_Forme->flip_x = group.flip_x;
                creation_Forme->flip_y = group.flip_y;
                Forme_head = group.parent;
                append_shape_to_list(creation_Forme, group.parent_tail);
            }

            shape_count++;
            if (strchr(ligne, '\n') == NULL && !feof(f_in)) {
                error = 1;
                break;
            }
            continue;
        }
        char *Forme_element = strstr(ligne, "<circle");
        FormeType type = FORME_CERCLE;

        if (Forme_element == NULL) {
            Forme_element = strstr(ligne, "<ellipse");
            type = FORME_ELLIPSE;
        }

        if (Forme_element == NULL) {
            Forme_element = strstr(ligne, "<rect");
            type = FORME_RECTANGLE;
            if (Forme_element != NULL) {
                char type_rectangle[16];
                if (lire_attribue(Forme_element, "data-shape", type_rectangle, sizeof(type_rectangle)) && strcmp(type_rectangle, "carré") == 0)
                    type = FORME_CARRE;
            }
        }
        if (Forme_element == NULL) {
            Forme_element = strstr(ligne, "<ligne");
            type = FORME_LIGNE;
        }
        if (Forme_element == NULL) {
            Forme_element = strstr(ligne, "<polyligne");
            type = FORME_POLYLIGNE;
        }
        if (Forme_element == NULL) {
            Forme_element = strstr(ligne, "<polygon");
            type = FORME_POLYGON;
        }
        if (Forme_element == NULL) {
            Forme_element = strstr(ligne, "<path");
            type = FORME_PATH;
        }

        if (Forme_element != NULL) {
            char *element_end = strchr(Forme_element, '>');
            float x;
            float y;
            float dimension_x;
            float dimension_y;
            float stroke_width;
            int rotation;
            int flip_x;
            int flip_y;
            char fill_color[32];
            char stroke_color[32];
            Point_t *points = NULL;
            size_t point_count = 0;
            int Forme_creer = 0;

            if (strchr(ligne, '<') != Forme_element || element_end == NULL || strchr(element_end + 1, '<') != NULL ||
                !lire_svg_style(Forme_element, fill_color, stroke_color, &stroke_width, &rotation, &flip_x, &flip_y)) {
                printf("Le fichier contient un élément SVG non valide ou non pris en charge.");
                error = 1;
                break;
            }

            if (type == FORME_CERCLE) {
                if (lire_nombre_attribue(Forme_element, "cy", &y) &&
                    lire_nombre_attribue(Forme_element, "cx", &x) &&
                    lire_nombre_attribue(Forme_element, "r", &dimension_x))
                    Forme_creer = creer_cercle(x, y, dimension_x, fill_color, stroke_color, stroke_width);
            }

            else if (type == FORME_ELLIPSE) {
                if (lire_nombre_attribue(Forme_element, "cx", &x) &&
                    lire_nombre_attribue(Forme_element, "cy", &y) &&
                    lire_nombre_attribue(Forme_element, "rx", &dimension_x) &&
                    lire_nombre_attribue(Forme_element, "ry", &dimension_y))
                    Forme_creer = creer_ellipse(x, y, dimension_x, dimension_y, fill_color, stroke_color, stroke_width);
            }

            else if (type == FORME_RECTANGLE || type == FORME_CARRE) {
                if (lire_nombre_attribue(Forme_element, "x", &x) &&
                    lire_nombre_attribue(Forme_element, "y", &y) &&
                    lire_nombre_attribue(Forme_element, "width", &dimension_x) &&
                    lire_nombre_attribue(Forme_element, "height", &dimension_y)) {
                    if (type == FORME_CARRE) {
                        if (dimension_x == dimension_y)
                            Forme_creer = creer_carre(x + dimension_x / 2.0f, y + dimension_y / 2.0f, dimension_x, fill_color, stroke_color, stroke_width);
                    }
                    else
                        Forme_creer = creer_rectangle(x + dimension_x / 2.0f, y + dimension_y / 2.0f, dimension_x, dimension_y, fill_color, stroke_color, stroke_width);
                }
            }
            else if (type == FORME_LIGNE &&
                    lire_nombre_attribue(Forme_element, "x1", &x) &&
                    lire_nombre_attribue(Forme_element, "y1", &y) &&
                    lire_nombre_attribue(Forme_element, "x2", &dimension_x) &&
                    lire_nombre_attribue(Forme_element, "y2", &dimension_y))
            {
                Forme_creer = creer_ligne(x, y, dimension_x, dimension_y,
                                          stroke_color, stroke_width);
            }
            else if ((type == FORME_POLYLINE || type == FORME_POLYGON) &&
                     lire_svg_points(Forme_element, &points, &point_count)) {
                Forme_creer = type == FORME_POLYLINE
                                  ? creer_polyline(points, point_count, stroke_color, stroke_width) : creer_polygon(points, point_count, fill_color, stroke_color, stroke_width);
                free(points);
            }
            else if (type == FORME_PATH) {
                char data[4096];
                if (lire_attribue(Forme_element, "d", data, sizeof(data)))
                    Forme_creer = creer_path(data, fill_color,
                                             stroke_color, stroke_width);
                if (Forme_creer) {
                    Forme_t *last_shape = get_fin_list();
                    if ((strstr(Forme_element, "data-x") != NULL &&
                         !lire_nombre_attribue(Forme_element, "data-x", &last_shape->x)) ||
                        (strstr(Forme_element, "data-y") != NULL &&
                         !lire_nombre_attribue(Forme_element, "data-y", &last_shape->y)))
                        Forme_creer = 0;
                }
            }
            if (!Forme_creer) {
                printf("Le fichier contient une forme non valide.");
                error = 1;
                break;
            }
            {
                Forme_t *last_shape = get_fin_list();
                last_shape->rotation = rotation;
                last_shape->flip_x = flip_x;
                last_shape->flip_y = flip_y;
            }
            shape_count++;
        }
        else if (strstr(ligne, "<svg") != NULL)
        {
            root_found = 1;
        }
        else if (strstr(ligne, "</svg>") != NULL)
        {
            closing_tag_found = 1;
        }
        else if (strchr(ligne, '<') != NULL &&
                 strstr(ligne, "<?xml") == NULL &&
                 strstr(ligne, "<!--") == NULL)
        {
            printf( "Cette version ne prend pas en charge cet élément SVG..\n");
            error = 1;
            break;
        }

        if (strchr(ligne, '\n') == NULL && !feof(f_in))
        {
            printf( "Ligne SVG trop longue ou élément non pris en charge.\n");
            error = 1;
            break;
        }
    }

    if (ferror(f_in))
    {
        perror("Erreur lors de la lecture du fichier SVG");
        error = 1;
    }
    if (fclose(f_in) != 0)
    {
        perror("Erreur lors de la fermeture du fichier SVG");
        error = 1;
    }
    if (!root_found || !closing_tag_found)
    {
        printf("Le fichier n’est pas un document SVG complet..\n");
        error = 1;
    }
    if (depth != 0)
    {
        printf("Le fichier SVG contient un groupe non fermé.\n");
        error = 1;
    }

    if (error)
    {
        free_temporary_list(Forme_head);
        Forme_head = NULL;
        while (depth > 0)
            free_temporary_list(groups[--depth].parent);
        Forme_head = previous_drawing;
        invalidate_list_tail();
        sync_id_counter();
        return 0;
    }

    free_temporary_list(previous_drawing);
    get_list_tail();
    sync_id_counter();
    printf("Dessin chargé depuis %s (%d forme(s)).\n", path, shape_count);
    return 1;
}

