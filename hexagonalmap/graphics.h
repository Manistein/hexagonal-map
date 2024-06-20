#ifndef _graphics_h_
#define _graphics_h_

#include "hexagonal-map.h"

int graphics_init(struct HexagonalMap* map, float screen_width, float screen_height);
void graphics_render(struct HexagonalMap* map);
void graphics_shutdown();

#endif