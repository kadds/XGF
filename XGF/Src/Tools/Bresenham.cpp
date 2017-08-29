#include "..\..\Include\Bresenham.hpp"
void Bresenham_Circle(int x0, int y0, int radius, std::vector<std::pair<int, int>> & out)
{
	int x = radius - 1;
	int y = 0;
	int dx = 1;
	int dy = 1;
	int err = dx - (radius << 1);

	while (x >= y)
	{
		out.push_back(std::make_pair(x0 + x, y0 + y));
		out.push_back(std::make_pair(x0 + y, y0 + x));
		out.push_back(std::make_pair(x0 - y, y0 + x));
		out.push_back(std::make_pair(x0 - x, y0 + y));
		out.push_back(std::make_pair(x0 - x, y0 - y));
		out.push_back(std::make_pair(x0 - y, y0 - x));
		out.push_back(std::make_pair(x0 + y, y0 - x));
		out.push_back(std::make_pair(x0 + x, y0 - y));

		if (err <= 0)
		{
			y++;
			err += dy;
			dy += 2;
		}
		if (err > 0)
		{
			x--;
			dx += 2;
			err += (-radius << 1) + dx;
		}
	}
}