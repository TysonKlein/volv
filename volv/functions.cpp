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

sf::Vector2f buffer(sf::Vector2f pos, SimVars* simVars)
{
	sf::Vector2f newPos = pos;
	if (pos.x < simVars->Xbuff)
	{
		newPos.x = simVars->Xbuff;
	}
	else if (pos.x > simVars->WIDTH - simVars->Xbuff)
	{
		newPos.x = simVars->WIDTH - simVars->Xbuff;
	}
	if (pos.y < simVars->Ybuff)
	{
		newPos.y = simVars->Ybuff;
	}
	else if (pos.y > simVars->HEIGHT - simVars->Ybuff)
	{
		newPos.y = simVars->HEIGHT - simVars->Ybuff;
	}
	return newPos;
}

void initializeSimVars(SimVars* simVars)
{
	simVars->HEIGHT = 1500;
	simVars->WIDTH = 3000;
	simVars->BREED_BASE = 9;
	simVars->COLLIDE_SQUARE_SIZE = 60;
	simVars->CURRENT_FOOD = 0;
	simVars->CURRENT_ORGANISM = 0;
	simVars->DEVMODE = false;
	simVars->INIT_NUM_ORGANISMS = 120;
	simVars->LINE_SIZE = 50;
	simVars->TIME = 0;
	simVars->Xbuff = 30;
	simVars->Ybuff = 30;
}