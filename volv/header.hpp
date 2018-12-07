#pragma once

#ifndef HEADER_HPP_DECLARED
#define HEADER_HPP_DECLARED

////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <iostream>

////////////////////////////////////////////////////////////
//Globals
////////////////////////////////////////////////////////////

float vectorDistance(sf::Vector2f V1, sf::Vector2f V2);
sf::Vector2f buffer(sf::Vector2f);

const float pi = 3.14159f;
const int NUMOFORGANISMS = 50;
const int collideSquareSize = 60;
const float Xbuff = 30.f;
const float Ybuff = 30.f;
const int linesSize = 100;

extern bool DEVMODE;
extern int TIME;
extern int BREED_BASE;
extern int CURRENT_ORGANISM;
extern int CURRENT_FOOD;

const int HEIGHT = 500;
const int WIDTH = 2000;

class Organism;
class Food;
enum AI_STATES { WANDER, FOOD, MATE, FLEE, ATTACK, PROTECT, FOLLOW, FLOCK };

class linkedList
{
public:
	linkedList();

	void insert(Organism* newOrg);
	void insertFood(Food* newFood);
	bool remove(Organism* oldOrg);
	void kill(Organism* oldOrg, linkedList** LL);
	void breed(Organism* oldOrg, linkedList** LL);
	void removeFood(Food* oldFood);

	void draw(sf::RenderWindow * window);
	void drawOrganisms(sf::RenderWindow * window);
	void drawFood(sf::RenderWindow * window);

	std::vector<Organism*> list;
	std::vector<Food*> foodList;
	int X, Y;
	sf::RectangleShape rect;
};


class Food
{
public:
	Food(int val, sf::Vector2f pos);
	void setKilled();
	void Draw(sf::RenderWindow* window);
	void Eat(linkedList** LL);

	int value, ID;
	sf::Vector2f location;
	bool killed;
private:
	sf::RectangleShape rect;
};

class Organism
{
public:
	Organism(sf::Vector2f LOC, int newDNA[]);

	void makeSick(Organism* other);
	void setBody();

	float getLikability(int DNA2[]);

	float Collides(Organism* other, sf::Vector2f pos);
	float CollidesFood(sf::Vector2f pos1, sf::Vector2f pos2);

	sf::Vector2f getLocation();

	float getRadius();

	int breedDiff(Organism* other);

	void AI(int me, linkedList** LL, float timeFactor);
	void checkFoodVicinity(int x, int y, linkedList** LL);

	bool eatFood(float collideFactor, linkedList** LL);

	int similarityUpdate(Organism* other);

	int mateFac(Organism* other);
	int attackFac(Organism* other);
	int defendFac(Organism* other);

	bool tastiest(Organism* incumbent, Organism* challenger);
	bool sexiest(Organism* incumbent, Organism* challenger);
	bool youngest(Organism* incumbent, Organism* challenger);
	bool scariest(Organism* incumbent, Organism* challenger);
	bool closest(Food* incumbent, Food* challenger);

	void move(linkedList** LL, float timefactor);

	void die();

	void changeVelocity(float timefactor);

	void changeDesiredLocation(sf::Vector2f newLoc);

	void Draw(sf::RenderWindow* window);

	void displayCollisions(sf::RenderWindow* window);

	sf::Vector2f location, newLoc, breedLoc;
	int ID, BORN, LIFESPAN, LIFESTAGE, DNA[10], maxBreedingDiff, generation, NEXT_MATE;
	float maxSpeed, maxEnergy, energy, maxVitality, vitality, radius, vision, happiness, metabolism, rotation, maxBreed, attack, roamAngle;
	bool BREED, producer, incHap, virus, Aggro, killed, HUNGRY, LEADER;
	int breedDNA[10], virusDNA[10];
	sf::CircleShape circ;
	AI_STATES state = WANDER;
	Organism* org_scariest;
	Organism* org_youngest;
	Organism* org_sexiest;
	Organism* org_tastiest;
	Organism* org_flock;
	Food* foo_closest;

private:
	int displayType;
	sf::CircleShape basicBody;
	sf::ConvexShape convexBody;
	sf::Color bodyColor, outlineColor;
	sf::Vector2f velocity, desiredLocation, repel, tempVel;
	bool fleeing;
};

#endif // !HEADER_HPP_DECLARED