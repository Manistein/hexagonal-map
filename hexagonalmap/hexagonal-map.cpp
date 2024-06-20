#include "hexagonal-map.h"

struct HexagonalMap* hexagonal_create(unsigned int width, unsigned int height) {
    struct HexagonalMap* map = (struct HexagonalMap*)malloc(sizeof(struct HexagonalMap));
    if (map == NULL) {
        return NULL;
    }

    map->arr = (struct HexagonalGrid*)malloc(sizeof(struct HexagonalGrid) * width * height);
    if (map->arr == NULL) {
        free(map);
        return NULL;
    }

    map->width = width;
    map->height = height;
    map->inner_radius = 0.5f;
    map->origin_x = 0.0f;
    map->origin_y = 0.0f;
    map->outter_radius = sqrt(3.0f) * (1.0f / 3.0f);
    map->screen_width = 1280.0f;
    map->screen_height = 720.0f;
	map->scale = 1.0f;

    return map;
}

int hexagonal_destory(struct HexagonalMap* map) {
    if (map == NULL) {
        return HDESTROY_ERROR;
    }

    free(map->arr);
    free(map);

    return HDESTROY_SUCCESS;
}

struct HexagonalGrid* hexagonal_get(struct HexagonalMap* map, unsigned int x, unsigned int y) {
    if (map == NULL || x >= map->width || y >= map->height) {
        return NULL;
    }
    return &map->arr[y * map->width + x];
}

struct HexagonalGrid* hexagonal_get_neighbor(struct HexagonalMap* map, unsigned int x, unsigned int y, unsigned int direction) {
    if (map == NULL || x >= map->width || y >= map->height) {
        return NULL;
    }

    unsigned int right_offset = (y % 2 == 0) ? 0 : 1;
    unsigned int left_offset = (y % 2 == 0) ? 1 : 0;

    switch (direction) {
        case HEXAGONAL_DIRECTION_LEFT:
            if (x == 0) {
                return NULL;
            }
            return &map->arr[y * map->width + x - 1];
        case HEXAGONAL_DIRECTION_UP_RIGHT:
            if (y == map->height - 1 || x == map->width - 1) {
                return NULL;
            }
            return &map->arr[(y + 1) * map->width + x + right_offset];
        case HEXAGONAL_DIRECTION_DOWN_RIGHT:
            if (y == 0 || x == map->width - 1) {
                return NULL;
            }
            return &map->arr[(y - 1) * map->width + x + right_offset];
        case HEXAGONAL_DIRECTION_RIGHT:
            if (x == map->width - 1) {
                return NULL;
            }
            return &map->arr[y * map->width + x + 1];
        case HEXAGONAL_DIRECTION_DOWN_LEFT:
            if (y == 0 || x == 0) {
                return NULL;
            }
            return &map->arr[(y - 1) * map->width + x - left_offset];
        case HEXAGONAL_DIRECTION_UP_LEFT:
            if (y == map->height - 1 || x == 0) {
                return NULL;
            }
            return &map->arr[(y + 1) * map->width + x - left_offset];
        default:
            return NULL;
    }
}