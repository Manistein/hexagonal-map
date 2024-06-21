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
	map->origin_y = map->screen_height - map->outter_radius;
	return 0;
}

static void draw_hexagonal_edge(ImDrawList* draw_list, struct HexagonalMap* map, float x_pos, float y_pos, ImU32 color) {
	draw_list->AddLine(ImVec2(x_pos, y_pos + map->outter_radius), ImVec2(x_pos - map->inner_radius, y_pos + map->outter_radius / 2.0f), color);
	draw_list->AddLine(ImVec2(x_pos - map->inner_radius, y_pos + map->outter_radius / 2.0f), ImVec2(x_pos - map->inner_radius, y_pos - map->outter_radius / 2.0f), color);
	draw_list->AddLine(ImVec2(x_pos - map->inner_radius, y_pos - map->outter_radius / 2.0f), ImVec2(x_pos, y_pos - map->outter_radius), color);
	draw_list->AddLine(ImVec2(x_pos, y_pos - map->outter_radius), ImVec2(x_pos + map->inner_radius, y_pos - map->outter_radius / 2.0f), color);
	draw_list->AddLine(ImVec2(x_pos + map->inner_radius, y_pos - map->outter_radius / 2.0f), ImVec2(x_pos + map->inner_radius, y_pos + map->outter_radius / 2.0f), color);
	draw_list->AddLine(ImVec2(x_pos + map->inner_radius, y_pos + map->outter_radius / 2.0f), ImVec2(x_pos, y_pos + map->outter_radius), color);
}

static void draw_hexagonal_grid(ImDrawList* draw_list, struct HexagonalMap* map, float x_pos, float y_pos, float x_offset, unsigned int x, unsigned int y) {
	ImU32 color = IM_COL32(255, 255, 255, 255);
	draw_hexagonal_edge(draw_list, map, x_pos, y_pos, color);

	char buf[64];
	sprintf(buf, "x:%d", x);
	int len = strlen(buf);
	draw_list->AddText(ImVec2(x_pos - map->inner_radius / 2.0f, y_pos - map->outter_radius / 2.0f), IM_COL32(255, 255, 255, 255), buf);

	sprintf(buf, "y:%d", y);
	len = strlen(buf);
	draw_list->AddText(ImVec2(x_pos - map->inner_radius / 2.0f, y_pos), IM_COL32(255, 255, 255, 255), buf);
}

// cheat a hexagonal grid as six triangles, and check if the mouse is in the any of the triangles.
// use cross product to check if the mouse is in the triangle.
static bool is_in_grid(struct HexagonalMap* map, unsigned int x, unsigned int y, ImVec2 mouse_pos) {
	float x_offset = y % 2 == 0 ? 0.0f : map->inner_radius;
	float x_pos = map->origin_x + ((float)x * map->inner_radius * 2) + x_offset;
	float y_pos = map->origin_y - ((float)y * map->outter_radius * 1.5f);

	ImVec2 p0 = ImVec2(x_pos, y_pos + map->outter_radius);
	ImVec2 p1 = ImVec2(x_pos - map->inner_radius, y_pos + map->outter_radius / 2.0f);
	ImVec2 p2 = ImVec2(x_pos - map->inner_radius, y_pos - map->outter_radius / 2.0f);
	ImVec2 p3 = ImVec2(x_pos, y_pos - map->outter_radius);
	ImVec2 p4 = ImVec2(x_pos + map->inner_radius, y_pos - map->outter_radius / 2.0f);
	ImVec2 p5 = ImVec2(x_pos + map->inner_radius, y_pos + map->outter_radius / 2.0f);

	ImVec2 p[6] = { p0, p1, p2, p3, p4, p5 };

	for (int i = 0; i < 6; i++) {
		ImVec2 a = p[i];
		ImVec2 b = p[(i + 1) % 6];

		ImVec2 ab = ImVec2(b.x - a.x, b.y - a.y);
		ImVec2 ac = ImVec2(mouse_pos.x - a.x, mouse_pos.y - a.y);

		float cross_product = (ab.x * ac.y) - (ab.y * ac.x);
		if (cross_product < 0.0f) {
			return false;
		}
	}

	return true;
}

static void graphics_highlight_over_grid(struct HexagonalMap* map) {
	ImVec2 mouse_pos = ImGui::GetMousePos();

	//// test 
	//mouse_pos.x = 469.0f;
	//mouse_pos.y = 157.0f;

	if (mouse_pos.x < map->origin_x - map->inner_radius || mouse_pos.x > map->screen_width || mouse_pos.y < 0.0f || mouse_pos.y > map->screen_height) {
		return;
	}
	
	unsigned int y = (unsigned int)((map->origin_y + map->outter_radius - mouse_pos.y) / (1.5f * map->outter_radius));
	float x_offset = y % 2 == 0 ? 0.0f : map->inner_radius;
	unsigned int x = (unsigned int)((mouse_pos.x - (map->origin_x - map->inner_radius) - x_offset) / (2 * map->inner_radius));


	ImDrawList* draw_list = ImGui::GetForegroundDrawList();

	unsigned int right_offset = (y % 2 == 0) ? 0 : 1;
	unsigned int left_offset = (y % 2 == 0) ? 1 : 0;

	unsigned int neighbors[7][2] = {
		{y, x - 1}, // left
		{y + 1, x + right_offset}, // up right
		{y - 1, x + right_offset}, // down right
		{y, x + 1}, // right
		{y - 1, x - left_offset}, // down left
		{y + 1, x - left_offset}, // up left
		{y, x}  // center
	};

	for (int i = 0; i < 7; i++) {
		unsigned int nx = neighbors[i][1];
		unsigned int ny = neighbors[i][0];

		struct HexagonalGrid* grid = hexagonal_get(map, nx, ny);
		if (grid == NULL)
			continue;

		if (is_in_grid(map, nx, ny, mouse_pos)) {
			x_offset = ny % 2 == 0 ? 0.0f : map->inner_radius;
			float x_pos = map->origin_x + ((float)nx * map->inner_radius * 2) + x_offset;
			float y_pos = map->origin_y - ((float)ny * map->outter_radius * 1.5f);

			ImU32 color = IM_COL32(255, 0, 0, 255);
			draw_hexagonal_edge(draw_list, map, x_pos, y_pos, color);

			x = nx;
			y = ny;

			break;
		}
	}

	char buf[64];
	sprintf(buf, "(%d,%d) (%f,%f)", x, y, mouse_pos.x, mouse_pos.y);
	draw_list->AddText(ImVec2(mouse_pos.x, mouse_pos.y), IM_COL32(0, 0, 0, 255), buf);
}

void graphics_render(struct HexagonalMap* map) {
	ImDrawList* draw_list = ImGui::GetBackgroundDrawList();

	unsigned int start_x = (unsigned int)(-fmin(map->origin_x, 0.0f) / (2 * map->inner_radius));
	unsigned int end_x = start_x + (unsigned int)((map->screen_width - fmax(map->origin_x, 0.0f)) / (2 * map->inner_radius));
	end_x = end_x < map->width ? end_x : map->width;

	unsigned int start_y = (unsigned int)((fmax(map->origin_y, map->screen_height) - map->screen_height) / (1.5f * map->outter_radius));
	unsigned int end_y = (unsigned int)(fmin(fmax(map->origin_y, 0.0f), map->screen_height) / (1.5f * map->outter_radius)) + 1;
	end_y = end_y < map->height ? end_y : map->height;

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

	graphics_highlight_over_grid(map);
}

void graphics_shutdown() {
}