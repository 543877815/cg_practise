#include "map.h"
RENDERABLE_BEGIN

MapObj::MapObj()
{
}

void MapObj::SetUpData()
{
	float grid_left = -0.5;
	float grid_right = 0.5;
	float grid_bottom = -0.5;
	float grid_top = 0.5;
	uint32_t grid_size_width = 100;
	float grid_unit_width = (grid_right - grid_left) / grid_size_width;
	uint32_t grid_size_height = 100;
	float grid_unit_height = (grid_top - grid_bottom) / grid_size_height;

	uint32_t num_vertices_per_row = grid_size_width + 1;

	std::vector<Point>vertices;
	std::vector<uint32_t>indices;

	for (uint32_t i = 0; i < grid_size_width; i++)
	{
		for (uint32_t j = 0; j < grid_size_height; j++)
		{
			vertices.emplace_back(grid_left + i * grid_unit_width, grid_bottom + j * grid_unit_height, 0.0);
		}
	}
}


RENDERABLE_END

