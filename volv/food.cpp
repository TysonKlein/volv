#include "header.hpp"

Food::Food(sf::Vector2f pos, SimVars* newSimVars)
{
	simVars = newSimVars;

	location = pos;
	value = 20;

	rect.setSize(sf::Vector2f(4, 4));
	rect.setOrigin(sf::Vector2f(2, 2));
	rect.setFillColor(sf::Color(255, 255, 255, 70));
	rect.setPosition(location);

	MEAT = false;
	ID = simVars->CURRENT_FOOD;
	simVars->CURRENT_FOOD++;
}
void Food::setKilled()
{
	MEAT = true;
	rect.setFillColor(sf::Color(255, 150, 150, 110));
	rect.setSize(sf::Vector2f(6, 6));
	rect.setOrigin(sf::Vector2f(3, 3));
	value = 50;
}
void Food::Draw(sf::RenderWindow* window)
{
	window->draw(rect);
}
void Food::Eat(linkedList** LL)
{
	LL[int(location.y / simVars->COLLIDE_SQUARE_SIZE)][int(location.x / simVars->COLLIDE_SQUARE_SIZE)].removeFood(this);
	delete this;
}