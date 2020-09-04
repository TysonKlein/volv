#include "header.hpp"

Food::Food(sf::Vector2f pos, Settings* newSettings, bool newMeat)
{
	settings = newSettings;

	bMeat = newMeat;
	location = pos;
	value = 15;

	fRadius = 2.f;
	rect.setSize(sf::Vector2f(4, 4));
	rect.setOrigin(sf::Vector2f(2, 2));
	rect.setFillColor(sf::Color(255, 255, 255, 70));
	rect.setPosition(location);

	if (bMeat)
	{
		fRadius = 3.f;
		rect.setFillColor(sf::Color(255, 150, 150, 110));
		rect.setSize(sf::Vector2f(6, 6));
		rect.setOrigin(sf::Vector2f(3, 3));
		value = 30;
	}
	ID = settings->nCurrentFood;
	settings->nCurrentFood++;
}
void Food::Draw(sf::RenderWindow* window)
{
	window->draw(rect);
}
void Food::Eat(linkedList** LL)
{
	LLfromArray(LL, location, settings, 0, 0)->remove(this);
	delete this;
}