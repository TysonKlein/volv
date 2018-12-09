#pragma once

#ifndef HEADER_HPP_DECLARED
#define HEADER_HPP_DECLARED

////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include "cxxopts.hpp"

#define _USE_MATH_DEFINES
#include <math.h>
#include <ctime>
#include <cstdlib>
#include <iostream>

////////////////////////////////////////////////////////////
//Globals
////////////////////////////////////////////////////////////

struct SimVars
{
	int WIDTH, HEIGHT;
	float Xbuff, Ybuff;
	int LINE_SIZE;
	int FOODRATE;
	int INIT_NUM_ORGANISMS;
	int TIME;
	int BREED_BASE;
	int CURRENT_ORGANISM;
	int CURRENT_FOOD;
	int COLLIDE_SQUARE_SIZE;
	int SEED;
	bool DEVMODE;
	bool FULLSCREEN;
	bool UNLIMIED_FRAMERATE;
};

float vectorDistance(sf::Vector2f V1, sf::Vector2f V2);
sf::Vector2f buffer(sf::Vector2f, SimVars* simVars);
void initializeSimVars(int argc, char* argv[], SimVars* simVars);

enum AI_STATES { WANDER, FOOD, MATE, FLEE, ATTACK, PROTECT, FOLLOW, FLOCK };

class Organism;
class Food;

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

	SimVars * simVars;
	std::vector<Organism*> list;
	linkedList* drawList;
	std::vector<Food*> foodList;
	int X, Y;
	sf::RectangleShape rect;
};

class Food
{
public:
	Food(int val, sf::Vector2f pos, SimVars* newSimVars);
	void setKilled();
	void Draw(sf::RenderWindow* window);
	void Eat(linkedList** LL);

	int value, ID;
	sf::Vector2f location;
	bool killed;
	SimVars* simVars;
private:
	sf::RectangleShape rect;
};

class Organism
{
public:
	Organism(sf::Vector2f LOC, int newDNA[], SimVars* newSimVars);

	void makeSick(Organism* other);
	void setBody();

	float getLikability(int DNA2[]);
	int similarityUpdate(Organism* other);

	float Collides(Organism* other, sf::Vector2f pos);
	float CollidesFood(sf::Vector2f pos1, sf::Vector2f pos2);
	void checkFoodVicinity(int x, int y, linkedList** LL);
	bool eatFood(float collideFactor, linkedList** LL);

	sf::Vector2f getLocation();
	float getRadius();

	int breedDiff(Organism* other);

	void AI(int me, linkedList** LL, float timeFactor);

	int mateFac(Organism* other);
	int attackFac(Organism* other);
	int defendFac(Organism* other);

	bool tastiest(Organism* incumbent, Organism* challenger);
	bool sexiest(Organism* incumbent, Organism* challenger);
	bool youngest(Organism* incumbent, Organism* challenger);
	bool scariest(Organism* incumbent, Organism* challenger);
	bool closest(Food* incumbent, Food* challenger);

	void die();

	void move(linkedList** LL, float timefactor);
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
	SimVars* simVars;

private:
	int displayType;
	sf::CircleShape basicBody;
	sf::ConvexShape convexBody;
	sf::Color bodyColor, outlineColor;
	sf::Vector2f velocity, desiredLocation, repel, tempVel;
	bool fleeing;
};

#endif // !HEADER_HPP_DECLARED