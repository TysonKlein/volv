#include "header.hpp"

bool DEVMODE = false;
int TIME = 0;
int BREED_BASE = 9;
int CURRENT_ORGANISM = 0;
int CURRENT_FOOD = 0;

float vectorDistance(sf::Vector2f V1, sf::Vector2f V2)
{
	float x, y;
	x = V1.x - V2.x;
	y = V1.y - V2.y;
	return sqrt(x*x + y * y);
}

sf::Vector2f buffer(sf::Vector2f pos)
{
	sf::Vector2f newPos = pos;
	if (pos.x < Xbuff)
	{
		newPos.x = Xbuff;
	}
	else if (pos.x > WIDTH - Xbuff)
	{
		newPos.x = WIDTH - Xbuff;
	}
	if (pos.y < Ybuff)
	{
		newPos.y = Ybuff;
	}
	else if (pos.y > HEIGHT - Ybuff)
	{
		newPos.y = HEIGHT - Ybuff;
	}
	return newPos;
}