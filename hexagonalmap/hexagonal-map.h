#ifndef _hexagonal_map_h_
#define _hexagonal_map_h_

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <math.h>

#define HDESTROY_SUCCESS 0
#define HDESTROY_ERROR 1

#define HEXAGONAL_DIRECTION_LEFT 0
#define HEXAGONAL_DIRECTION_UP_RIGHT 1
#define HEXAGONAL_DIRECTION_DOWN_RIGHT 2
#define HEXAGONAL_DIRECTION_RIGHT 3
#define HEXAGONAL_DIRECTION_DOWN_LEFT 4
#define HEXAGONAL_DIRECTION_UP_LEFT 5

struct HexagonalGrid {
	size_t tag;
};

// The origin position is at the bottom left conner.
struct HexagonalMap {
	struct HexagonalGrid* arr;
	unsigned int width;
	unsigned int height;
	float inner_radius;
	float origin_x;
	float origin_y;
	float outter_radius;	
	float screen_width;
	float screen_height;
	float scale;
};

struct HexagonalMap* hexagonal_create(unsigned int width, unsigned int height);
int hexagonal_destory(struct HexagonalMap* map);
struct HexagonalGrid* hexagonal_get(struct HexagonalMap* map, unsigned int x, unsigned int y);
struct HexagonalGrid* hexagonal_get_neighbor(struct HexagonalMap* map, unsigned int x, unsigned int y, unsigned int direction);

#endif
