#include "header.hpp"

Food::Food(int val, sf::Vector2f pos)
{
	location = pos;
	value = val;

	rect.setSize(sf::Vector2f(4, 4));
	rect.setOrigin(sf::Vector2f(2, 2));
	rect.setFillColor(sf::Color(255, 255, 255, 70));
	rect.setPosition(location);

	killed = false;
	ID = CURRENT_FOOD;
	CURRENT_FOOD++;
}
void Food::setKilled()
{
	killed = true;
	rect.setFillColor(sf::Color(255, 150, 150, 110));
	rect.setSize(sf::Vector2f(6, 6));
	rect.setOrigin(sf::Vector2f(3, 3));
	value *= 2.5f;
}
void Food::Draw(sf::RenderWindow* window)
{
	window->draw(rect);
}
void Food::Eat(linkedList** LL)
{
	LL[int(location.y / collideSquareSize)][int(location.x / collideSquareSize)].removeFood(this);
	delete this;
}