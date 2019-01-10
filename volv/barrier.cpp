#include "header.hpp"

Barrier::Barrier(float x, float y, float newRadius, SimVars* newSimvars)
{
	simVars = newSimvars;
	radius = newRadius;
	location = buffer(sf::Vector2f(x, y), simVars);

	circ.setFillColor(sf::Color(100, 100, 100));
	circ.setPosition(location);
	circ.setRadius(radius);
	circ.setOrigin(sf::Vector2f(radius, radius));
	circ.setPointCount(5 + rand() % 10);
	circ.rotate(rand() % 360);
}

void Barrier::removeFood(linkedList** LL)
{
	for (int x = -1; x < 2; x++)
	{
		for (int y = -1; y < 2; y++)
		{
			if (int(location.y / simVars->COLLIDE_SQUARE_SIZE + y) >= 0 && int(location.x / simVars->COLLIDE_SQUARE_SIZE) + x >= 0 && int(location.y / simVars->COLLIDE_SQUARE_SIZE + y) < simVars->HEIGHT / simVars->COLLIDE_SQUARE_SIZE + 1 && int(location.x / simVars->COLLIDE_SQUARE_SIZE + x) < simVars->WIDTH / simVars->COLLIDE_SQUARE_SIZE + 1)
			{
				for (int k = 0; k < LL[int(location.y / simVars->COLLIDE_SQUARE_SIZE + y)][int(location.x / simVars->COLLIDE_SQUARE_SIZE) + x].plantFoodList.size(); k++)
				{
					if (Collides(LL[int(location.y / simVars->COLLIDE_SQUARE_SIZE + y)][int(location.x / simVars->COLLIDE_SQUARE_SIZE) + x].plantFoodList[k]->location, location, 2.f, radius) > 0.001f)
					{
						LL[int(location.y / simVars->COLLIDE_SQUARE_SIZE + y)][int(location.x / simVars->COLLIDE_SQUARE_SIZE) + x].removeFood(LL[int(location.y / simVars->COLLIDE_SQUARE_SIZE + y)][int(location.x / simVars->COLLIDE_SQUARE_SIZE) + x].plantFoodList[k]);
					}
				}

				for (int k = 0; k < LL[int(location.y / simVars->COLLIDE_SQUARE_SIZE + y)][int(location.x / simVars->COLLIDE_SQUARE_SIZE) + x].meatFoodList.size(); k++)
				{
					if (Collides(LL[int(location.y / simVars->COLLIDE_SQUARE_SIZE + y)][int(location.x / simVars->COLLIDE_SQUARE_SIZE) + x].meatFoodList[k]->location, location, 2.f, radius) > 0.001f)
					{
						LL[int(location.y / simVars->COLLIDE_SQUARE_SIZE + y)][int(location.x / simVars->COLLIDE_SQUARE_SIZE) + x].removeFood(LL[int(location.y / simVars->COLLIDE_SQUARE_SIZE + y)][int(location.x / simVars->COLLIDE_SQUARE_SIZE) + x].meatFoodList[k]);
					}
				}
			}
		}
	}
}

void Barrier::Draw(sf::RenderWindow* window)
{
	window->draw(circ);
}