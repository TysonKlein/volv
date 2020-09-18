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
struct Settings
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

class Organism;
class Food;

struct OrganismProperties
{
	float fMaxSpeed, fEnergy, fMaxEnergy, fMaxVitality, fVitality, fHappiness, fMetabolism, fRotation, fBreed, fAttack, fRoamAngle, fImmunity, fScaredness, fHunger, fVision, fMatingCooldown;
	bool bBreed, bProducer, bIncreaseHappiness, bVirus, bAggro, bKilled, bHungry, bLeader, bDefensive, bFleeing;
	int breedDNA[100], virusDNA[100], DNA[100], nID, nBirthday, nLifespan, nLifestage, nMaxBreedingDifference, nGeneration, nTimeOfNextMate, nDisplayType, breedVar;
	Organism* org_scariest;
	Organism* org_youngest;
	Organism* org_sexiest;
	Organism* org_tastiest;
	Organism* org_weirdest;
	Food* foo_closest;
};

struct AIproperties
{
	void reset() {
		max = 0;
		min = 0;
		maxGoTo = 0;

		randChange = rand() % 360;
		breedSum = 0;
		bChangeDirection = false;
		bFoundFood = false;
	}
	float max, min, tempMax, tempMin, collideFactor, likeability, maxGoTo, distFac;
	int randChange, breedSum, diff, virusSum;
	bool bChangeDirection;
	bool bFoundFood;
};

enum AI_STATES { WANDER, FOOD, MATE, FLEE, ATTACK, PROTECT, FOLLOW, DEFEND };

class Collideable
{
public:
	Collideable();
	~Collideable();

	float getRadius();
	float getDirection();
	float getSpeed();

	sf::Vector2f getLocation();
	sf::Vector2f getVelocity();

protected:
	sf::Vector2f location;
	sf::Vector2f velocity;
	float fRadius, fDirection, fSpeed;
	Settings* settings;
};

class Organism;
class Food;
class Barrier;

struct linkedList
{
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

	Settings* settings;

	std::vector<Organism*> organismList;
	std::vector<Food*> plantFoodList, meatFoodList;
	std::vector<Barrier*> barrierList;

	std::vector<Food*>::iterator foodIt;
	linkedList* drawList;
	
	int X, Y;

	sf::RectangleShape LLrect;
};

class Food : public Collideable
{
public:
	Food(sf::Vector2f pos, Settings* newSettings, bool newMeat);
	void Draw(sf::RenderWindow* window);
	void Eat(linkedList** LL);

	int value, ID;
	bool bMeat;
	Settings* settings;
private:
	sf::RectangleShape rect;
};

class Organism : public Collideable
{
public:
	Organism(sf::Vector2f LOC, int newDNA[], Settings* newSettings);

	void makeSick(Organism* other);
	void setBody();

	float getLikability(int DNA2[]);
	int similarityUpdate(Organism* other);

	void checkFoodVicinity(int x, int y, linkedList** LL);
	bool eatFood(float collideFactor, linkedList** LL);

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

	sf::Vector2f getBreedLocation();

	sf::CircleShape circ;
	AI_STATES state = WANDER;
	std::vector<Organism*>::iterator orgIT;
	std::vector<Barrier*>::iterator barrierIT;
	std::vector<Food*>::iterator foodIT;
	Settings* settings;
	OrganismProperties props;
	AIproperties AIprops;

private:
	sf::CircleShape basicBody;
	sf::ConvexShape convexBody;
	sf::Color bodyColor, outlineColor;
	sf::Vector2f desiredLocation, repel, tempVel;
	sf::Vector2f newLoc, breedLoc;
};

class Barrier : public Collideable
{
public:
	Barrier(float x, float y, float newRadius, Settings* newSettings);
	void removeFood(linkedList** LL);
	void Draw(sf::RenderWindow* window);

	Settings* settings;
private:
	std::vector<Food*>::iterator foodIt;
	sf::CircleShape circ;
};

////////////////////////////////////////////////////////////
//Function headers
////////////////////////////////////////////////////////////
linkedList* LLfromArray(linkedList** LL, sf::Vector2f vec, Settings* settings, int nTileOffsetX, int nTileOffsetY);
float vectorDistance(sf::Vector2f V1, sf::Vector2f V2);
float vectorDistanceSQ(sf::Vector2f V1, sf::Vector2f V2);
float Collides(Collideable* c1, Collideable* c2);
sf::Vector2f buffer(sf::Vector2f, Settings* settings);
void initializesettings(int argc, char* argv[], Settings* settings);
float valFromDNA(int DNA[], float min, float max, float seed);
bool isInBounds(sf::Vector2f vec, Settings* settings, int nTileOffsetX, int nTileOffsetY);

#endif // !HEADER_HPP_DECLARED