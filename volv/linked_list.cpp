#include "header.hpp"

linkedList::linkedList()
{
	X = 0;
	Y = 0;
}
void linkedList::insert(Organism* newOrg)
{
	list.push_back(newOrg);
}
void linkedList::insertFood(Food* newFood)
{
	if (newFood->MEAT)
		meatFoodList.push_back(newFood);
	else
		plantFoodList.push_back(newFood);
}
void linkedList::insertBarrier(Barrier * newBarrier)
{
	barrierList.push_back(newBarrier);
}
bool linkedList::remove(Organism* oldOrg) //Remove orgnism from the SEGMENT
{
	for (int i = 0; i < list.size(); i++)//Iterate through the list of organisms for this segment
	{
		if (list[i]->ID == oldOrg->ID)//If the organism has the correct ID
		{
			list.erase(list.begin() + i);//Remove it
			return true;
		}
	}
	return false;
}
void linkedList::kill(Organism* oldOrg, linkedList** LL) //Kill a specific organism from a SEGMENT, spawn food in its place
{
	for (int i = 0; i < oldOrg->maxVitality*0.05f / (15 + 15 * int(oldOrg->virus) + oldOrg->DNA[2]); i++)
	{
		sf::Vector2f pos = oldOrg->location;
		pos.x += float(rand() % 100) / 50.0f*oldOrg->radius - oldOrg->radius;
		pos.y += float(rand() % 100) / 50.0f*oldOrg->radius - oldOrg->radius;
		pos = buffer(pos, simVars);
		Food* food = new Food(pos, simVars);
		if (oldOrg->killed || rand() % 8 == 0)
		{
			food->setKilled();
		}
		LL[int(pos.y / simVars->COLLIDE_SQUARE_SIZE)][int(pos.x / simVars->COLLIDE_SQUARE_SIZE)].insertFood(food);
	}
	if (LL[int(oldOrg->location.y / simVars->COLLIDE_SQUARE_SIZE)][int(oldOrg->location.x / simVars->COLLIDE_SQUARE_SIZE)].remove(oldOrg))
	{
		drawList->remove(oldOrg);
		oldOrg->die();
	}
}
void linkedList::breed(Organism* oldOrg, linkedList** LL)
{
	int newDNA[DNA_SIZE], tot = 0, newCreature = rand() % 50;
	float a, b, c, d;

	for (int i = 0; i < DNA_SIZE; i++)
	{
		tot += int(oldOrg->DNA[i] != oldOrg->breedDNA[i]);
	}

	for (int i = 0; i < DNA_SIZE; i++)
	{
		a = rand() % 2000;
		b = rand() % 2000;
		c = rand() % (200 + (tot<int(0.1f*float(DNA_SIZE)))*(int(0.1f*float(DNA_SIZE)) - tot) * 150);
		d = a + b + c;

		a = a / d;
		b = b / d;
		c = c / d;
		if ((c > a && c > b) || newCreature == 0)
		{
			if (i == DNA_SIZE-1)
			{
				newDNA[i] = 1 + rand() % (DNA_SIZE-1);
				if ((rand() % 1000) == 0)
				{
					newDNA[i] = 0;
				}
			}
			else
			{
				newDNA[i] = rand() % DNA_SIZE;
			}
		}
		else if (a > b)
		{
			newDNA[i] = oldOrg->DNA[i];
		}
		else
		{
			newDNA[i] = oldOrg->breedDNA[i];
		}
	}

	int parentA = 0, parentB = 0;
	for (int i = 0; i < DNA_SIZE; i++)
	{
		if (oldOrg->DNA[i] != newDNA[i])
		{
			parentA++;
		}
		if (oldOrg->breedDNA[i] != newDNA[i])
		{
			parentB++;
		}
	}
	if (parentA > parentB)
	{
		parentA = parentB;
	}

	oldOrg->BREED = false;

	Organism* org = new Organism(sf::Vector2f((oldOrg->breedLoc.x + oldOrg->location.x) / 2.f, (oldOrg->breedLoc.y + oldOrg->location.y) / 2.f), newDNA, simVars);
	org->maxBreed = parentA;
	org->generation = oldOrg->generation + 1;
	LL[int(org->location.y / simVars->COLLIDE_SQUARE_SIZE)][int(org->location.x / simVars->COLLIDE_SQUARE_SIZE)].insert(org);
	drawList->insert(org);
}
void linkedList::removeFood(Food* oldFood)
{
	if (oldFood->MEAT)
	{
		for (int i = 0; i < meatFoodList.size(); i++)
		{
			if (meatFoodList[i]->ID == oldFood->ID)
			{
				meatFoodList.erase(meatFoodList.begin() + i);
			}
		}
	}
	else
	{
		for (int i = 0; i < plantFoodList.size(); i++)
		{
			if (plantFoodList[i]->ID == oldFood->ID)
			{
				plantFoodList.erase(plantFoodList.begin() + i);
			}
		}
	}
}
void linkedList::draw(sf::RenderWindow * window)
{
	window->draw(rect);
}
void linkedList::drawFood(sf::RenderWindow * window)
{
	for (it = meatFoodList.begin(); it != meatFoodList.end(); it++)
	{
		(*it)->Draw(window);
	}
	for (it = plantFoodList.begin(); it != plantFoodList.end(); it++)
	{
		(*it)->Draw(window);
	}
}
void linkedList::drawBarrier(sf::RenderWindow * window)
{
	for (std::vector<Barrier*>::iterator bit = barrierList.begin(); bit != barrierList.end(); bit++)
	{
		(*bit)->Draw(window);
	}
}
void linkedList::drawOrganisms(sf::RenderWindow * window)
{
	for (std::vector<Organism*>::iterator it = list.begin(); it != list.end(); it++)
	{
		(*it)->Draw(window);
		if (simVars->DEVMODE)
			(*it)->displayCollisions(window);
	}
}