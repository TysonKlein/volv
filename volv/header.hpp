#pragma once

#ifndef HEADER_HPP_DECLARED
#define HEADER_HPP_DECLARED

////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////

//SFML
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

//CXXOPTS
#include "cxxopts.hpp"

#define _USE_MATH_DEFINES
#include <math.h>
#include <ctime>
#include <cstdlib>
#include <iostream>

////////////////////////////////////////////////////////////
//Globals
////////////////////////////////////////////////////////////
#define DNA_SIZE 20 //macro for the total size of the DNA strand or the genome for an organism
#define ORG_VALUE_COUNT 14

////////////////////////////////////////////////////////////
//Structs and Classes
////////////////////////////////////////////////////////////
struct SimVars
{
	int nWidth, nHeight;
	float fXbuffer, fYbuffer;
	int nLineSize;
	int nFoodSpawnRate;
	int nInitialNumberOfOrganisms;
	int nTime;
	int nMimimumBreedFactor;
	int nCurrentOrganism;
	int nCurrentFood;
	int nCollisionSquareSize;
	int nSeed;
	bool bDevmode;
	bool bFullscreen;
	bool bUnlimitedFramerate;
};

enum AI_STATES { WANDER, FOOD, MATE, FLEE, ATTACK, PROTECT, FOLLOW, DEFEND };
enum ORG_VALUES { SPEED, ENERGY, VITALITY, HAPPINESS, METABOLISM, ROTATION, BREED, ATTACK, ROAM_ANGLE, IMMUNITY, SCAREDNESS, HUNGER, VISION, MATING_COOLDOWN};

class Collideable
{
public:
	Collideable();
	~Collideable();

	float getRadius();
	float getDirection();
	float getSpeed();

	sf::Vector2f getLocation();

protected:
	sf::Vector2f location;
	float fRadius, fDirection, fSpeed;
	SimVars* simVars;
};

class Organism;
class Food;
class Barrier;

class linkedList
{
public:
	linkedList();

	void insert(Organism* newOrg);
	void insert(Food* newFood);
	void insert(Barrier* newBarrier);

	bool remove(Organism* oldOrg);
	void remove(Food* oldFood);

	void kill(Organism* oldOrg, linkedList** LL);
	void breed(Organism* oldOrg, linkedList** LL);

	void draw(sf::RenderWindow * window);
	void drawOrganisms(sf::RenderWindow * window);
	void drawFood(sf::RenderWindow * window);
	void drawBarrier(sf::RenderWindow * window);

private:
	SimVars * simVars;

	std::vector<Organism*> list;
	std::vector<Food*> plantFoodList, meatFoodList;
	std::vector<Barrier*> barrierList;

	std::vector<Food*>::iterator foodIt;
	linkedList* drawList;
	
	int X, Y;

	sf::RectangleShape LLrect;
};

class Food : protected Collideable
{
public:
	Food(sf::Vector2f pos, SimVars* newSimVars);
	void setKilled();
	void Draw(sf::RenderWindow* window);
	void Eat(linkedList** LL);

	int value, ID;
	bool MEAT;
	SimVars* simVars;
private:
	sf::RectangleShape rect;
};

class Organism : protected Collideable
{
public:
	Organism(sf::Vector2f LOC, int newDNA[], SimVars* newSimVars);

	void makeSick(Organism* other);
	void setBody();

	float getLikability(int DNA2[]);
	int similarityUpdate(Organism* other);

	void checkFoodVicinity(int x, int y, linkedList** LL);
	bool eatFood(float collideFactor, linkedList** LL);

	sf::Vector2f getLocation();
	float getRadius();

	int breedDiff(Organism* other);

	void AI(int me, linkedList** LL, float timeFactor);

	bool mateFac(Organism* other);
	bool attackFac(Organism* other);
	bool scaryFac(Organism* other);
	bool weirdFac(Organism* other);

	bool weirdest(Organism* incumbent, Organism* challenger);
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

	sf::Vector2f newLoc, breedLoc;
	int nID, BORN, LIFESPAN, LIFESTAGE, DNA[100], maxBreedingDiff, generation, NEXT_MATE, breedVar;
	float fCurrentOrgValue[ORG_VALUE_COUNT];
	float fMaxOrgValue[ORG_VALUE_COUNT];
	bool BREED, producer, incHap, virus, Aggro, killed, HUNGRY, LEADER, DEFENSIVE;
	int breedDNA[100], virusDNA[100];
	sf::CircleShape circ;
	AI_STATES state = WANDER;
	Organism* org_scariest;
	Organism* org_youngest;
	Organism* org_sexiest;
	Organism* org_tastiest;
	Organism* org_weirdest;
	std::vector<Organism*>::iterator it;
	std::vector<Barrier*>::iterator bit;
	Food* foo_closest;
	std::vector<Food*>::iterator foodIt;
	SimVars* simVars;

private:
	int displayType;
	sf::CircleShape basicBody;
	sf::ConvexShape convexBody;
	sf::Color bodyColor, outlineColor;
	sf::Vector2f velocity, desiredLocation, repel, tempVel;
	bool fleeing;
};

class Barrier : protected Collideable
{
public:
	Barrier(float x, float y, float newRadius, SimVars* newSimVars);
	void removeFood(linkedList** LL);
	void Draw(sf::RenderWindow* window);

	SimVars* simVars;
private:
	std::vector<Food*>::iterator foodIt;
	sf::CircleShape circ;
};

////////////////////////////////////////////////////////////
//Function headers
////////////////////////////////////////////////////////////
float vectorDistance(sf::Vector2f V1, sf::Vector2f V2);
float vectorDistanceSQ(sf::Vector2f V1, sf::Vector2f V2);
float Collides(Collideable* c1, Collideable* c2);
sf::Vector2f buffer(sf::Vector2f, SimVars* simVars);
void initializeSimVars(int argc, char* argv[], SimVars* simVars);
float valFromDNA(int DNA[], float min, float max, float seed);

#endif // !HEADER_HPP_DECLARED