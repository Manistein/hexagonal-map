#include "graphics.h"
#include "imgui.h"

const float font_shift = 0.0f;

int graphics_init(struct HexagonalMap* map, float screen_width, float screen_height) {
	map->screen_width = screen_width;
	map->screen_height = screen_height;

	map->inner_radius *= 50.0f;
	map->outter_radius *= 50.0f;
	map->scale = 50.0f;

	map->origin_x = map->inner_radius;
	map->origin_y = map->screen_height - map->inner_radius;
	return 0;
}

static void draw_hexagonal_grid(ImDrawList* draw_list, struct HexagonalMap* map, float x_pos, float y_pos, float x_offset, unsigned int x, unsigned int y) {
	ImU32 color = IM_COL32(255, 255, 255, 255);
	draw_list->AddLine(ImVec2(x_pos, y_pos + map->outter_radius), ImVec2(x_pos - map->inner_radius, y_pos + map->outter_radius / 2.0f), color);
	draw_list->AddLine(ImVec2(x_pos - map->inner_radius, y_pos + map->outter_radius / 2.0f), ImVec2(x_pos - map->inner_radius, y_pos - map->outter_radius / 2.0f), color);
	draw_list->AddLine(ImVec2(x_pos - map->inner_radius, y_pos - map->outter_radius / 2.0f), ImVec2(x_pos, y_pos - map->outter_radius), color);
	draw_list->AddLine(ImVec2(x_pos, y_pos - map->outter_radius), ImVec2(x_pos + map->inner_radius, y_pos - map->outter_radius / 2.0f), color);
	draw_list->AddLine(ImVec2(x_pos + map->inner_radius, y_pos - map->outter_radius / 2.0f), ImVec2(x_pos + map->inner_radius, y_pos + map->outter_radius / 2.0f), color);
	draw_list->AddLine(ImVec2(x_pos + map->inner_radius, y_pos + map->outter_radius / 2.0f), ImVec2(x_pos, y_pos + map->outter_radius), color);

	char buf[64];
	sprintf(buf, "x:%d", x);
	int len = strlen(buf);
	draw_list->AddText(ImVec2(x_pos - map->inner_radius / 2.0f, y_pos - map->outter_radius / 2.0f), IM_COL32(255, 255, 255, 255), buf);

	sprintf(buf, "y:%d", y);
	len = strlen(buf);
	draw_list->AddText(ImVec2(x_pos - map->inner_radius / 2.0f, y_pos), IM_COL32(255, 255, 255, 255), buf);
}

void graphics_render(struct HexagonalMap* map) {
	ImDrawList* draw_list = ImGui::GetBackgroundDrawList();

	unsigned int start_x = (unsigned int)(-fmin(map->origin_x, 0.0f) / (2 * map->inner_radius));
	unsigned int end_x = start_x + (unsigned int)((map->screen_width - fmax(map->origin_x, 0.0f)) / (2 * map->inner_radius));

	unsigned int start_y = (unsigned int)((fmax(map->origin_y, map->screen_height) - map->screen_height) / (1.5f * map->outter_radius));
	unsigned int end_y = (unsigned int)(fmin(fmax(map->origin_y, 0.0f), map->screen_height) / (1.5f * map->outter_radius));

	for (unsigned int y = start_y; y < end_y; y++) {
		float x_offset = y % 2 == 0 ? 0.0f : map->inner_radius;

		for (unsigned int x = start_x; x < end_x; x++) {
			struct HexagonalGrid* grid = hexagonal_get(map, x, y);
			if (grid == NULL) {
				continue;
			}
			// Draw hexagon
			float x_pos = map->origin_x + ((float)x * map->inner_radius * 2) + x_offset;
			float y_pos = map->origin_y - ((float)y * map->outter_radius * 1.5f);

			if (x_pos >= 0.0f && x_pos <= map->screen_width && y_pos >= 0.0f && y_pos <= map->screen_height) {
				draw_hexagonal_grid(draw_list, map, x_pos, y_pos, x_offset, x, y);
			}
		}
	}
}

void graphics_shutdown() {
}