#include "header.hpp"

Collideable::Collideable()
{
	fRadius = 1;
	fDirection = 0;
	fSpeed = 0;

	location = sf::Vector2f(0.f, 0.f);
}

Collideable::~Collideable()
{
}

float Collideable::getRadius()
{
	return fRadius;
}

float Collideable::getDirection()
{
	return fDirection;
}

float Collideable::getSpeed()
{
	return fSpeed;
}

sf::Vector2f Collideable::getLocation()
{
	return location;
}
