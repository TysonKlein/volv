#include "header.hpp"

Barrier::Barrier(float x, float y, float newRadius, Settings* newsettings)
{
	settings = newsettings;
	fRadius = newRadius;
	location = buffer(sf::Vector2f(x, y), settings);

	circ.setFillColor(sf::Color(100, 100, 100));
	circ.setPosition(location);
	circ.setRadius(fRadius);
	circ.setOrigin(sf::Vector2f(fRadius, fRadius));
	circ.setPointCount(5 + rand() % 10);
	circ.rotate(rand() % 360);
}

void Barrier::removeFood(linkedList** LL)
{
	for (int x = -1; x < 2; x++)
	{
		for (int y = -1; y < 2; y++)
		{
			if (isInBounds(location, settings, x, y))
			{
				for (int k = 0; k < LLfromArray(LL, location, settings, x, y)->plantFoodList.size(); k++)
				{
					if (Collides(LLfromArray(LL, location, settings, x, y)->plantFoodList[k], this) > 0.001f)
					{
						LLfromArray(LL, location, settings, x, y)->remove(LLfromArray(LL, location, settings, x, y)->plantFoodList[k]);
					}
				}

				for (int k = 0; k < LLfromArray(LL, location, settings, x, y)->meatFoodList.size(); k++)
				{
					if (Collides(LLfromArray(LL, location, settings, x, y)->meatFoodList[k], this) > 0.001f)
					{
						LLfromArray(LL, location, settings, x, y)->remove(LLfromArray(LL, location, settings, x, y)->meatFoodList[k]);
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