#include "header.hpp"

Organism::Organism(sf::Vector2f LOC, int newDNA[], SimVars* newSimVars)
{
	simVars = newSimVars;

	for (int i = 0; i < 10; i++)
	{
		DNA[i] = newDNA[i];
	}

	BORN = simVars->TIME;
	NEXT_MATE = BORN;
	BREED = false;
	LEADER = false;
	rotation = 0;
	maxBreed = 10;

	bodyColor.b = (DNA[0] * 100 + DNA[0] * 10 + DNA[1]) / 4;
	bodyColor.g = ((9 - DNA[1]) * 100 + DNA[1] * 10 + DNA[8]) / 4;
	bodyColor.r = (DNA[2] * 100 + DNA[2] * 10 + DNA[1]) / 4;
	bodyColor.a = 160;

	outlineColor.r = (DNA[6] * 100 + DNA[3] * 10 + DNA[1]) / 4;
	outlineColor.g = (DNA[7] * 100 + DNA[6] * 10 + DNA[8]) / 4;
	outlineColor.b = (DNA[5] * 100 + DNA[4] * 10 + DNA[1]) / 4;
	outlineColor.a = 160;

	LIFESTAGE = 0;

	roamAngle = float(rand() % 10000) / 10000.f / 3.14f * 180.f;
	incHap = false;
	virus = false;

	maxBreedingDiff = 10;

	radius = 1.1f*sqrt(10 + DNA[3] * (9 - DNA[5]) + DNA[2] * DNA[4] * DNA[8]);

	maxSpeed = 3.f*float(DNA[2] + DNA[2] + (9 - DNA[7])) / (radius*radius);

	location = LOC;
	desiredLocation = location;
	rotation = 0;
	repel.x = 0;
	repel.y = 0;
	happiness = 0.f;

	attack = (35.f + DNA[2] + DNA[5] + DNA[6] - (9 - DNA[8]))*radius / 12.f;
	if (attack < 1)
	{
		attack = 1;
	}

	killed = false;

	maxVitality = (DNA[6] + DNA[3] + DNA[7] + DNA[8] + 10) * radius / 5.f;
	vitality = maxVitality;
	metabolism = 0.1f - float((DNA[9] + DNA[3] + DNA[5]) / 3)*0.007f;
	metabolism *= sqrt(sqrt(maxSpeed))*0.5f;
	maxEnergy = maxVitality;
	energy = 0.5*maxEnergy;
	producer = false;
	LIFESPAN = 50.f / metabolism * ((9 - DNA[1]) + DNA[3] + DNA[2] + 10);

	if ((DNA[DNA[5]] + DNA[DNA[7]] + DNA[DNA[2]]) > 20)
	{
		Aggro = true;
		displayType = 1;
		maxVitality *= 1.3f;
		energy = maxEnergy * 0.8f;
		maxSpeed *= 1.3f;
	}
	else
	{
		Aggro = false;
		attack /= 2.f;
		displayType = 0;
	}

	if (DNA[9] == 0)
	{
		radius *= 1.7;
		bodyColor.g = 230;
		bodyColor.r = 230;
		bodyColor.b = 230;
		metabolism = 0;
		maxSpeed *= 0.01f;
		producer = true;
		displayType = 0;
	}
	if (producer)
	{
		LIFESPAN *= 10000;
	}

	setBody();

	ID = simVars->CURRENT_ORGANISM;
	simVars->CURRENT_ORGANISM++;

	vision = float(DNA[0] + DNA[6] + DNA[8]) + 8 * radius;
}

void Organism::makeSick(Organism* other)
{
	for (int i = 0; i < 10; i++)
	{
		other->virusDNA[i] = virusDNA[i];
	}

	other->circ.setFillColor(sf::Color((virusDNA[2] * 100 + virusDNA[2] * 10 + virusDNA[1]) / 4, (virusDNA[1] * 100 + virusDNA[1] * 10 + virusDNA[8]) / 4, (virusDNA[0] * 100 + virusDNA[0] * 10 + virusDNA[1]) / 4));
	other->circ.setRadius(other->radius*0.25f);
	other->circ.setOutlineThickness(2);
	other->circ.setOutlineColor(sf::Color((virusDNA[6] * 100 + virusDNA[3] * 10 + virusDNA[1]) / 4, (virusDNA[7] * 100 + virusDNA[6] * 10 + virusDNA[8]) / 4, (virusDNA[5] * 100 + virusDNA[4] * 10 + virusDNA[1]) / 4));
	other->circ.setOrigin(sf::Vector2f(other->radius*0.25f, other->radius*0.25f));

	other->virus = true;
}

void Organism::setBody()
{
	// resize it to 5 points
	float h = DNA[0] * DNA[1] + DNA[7] + 20;
	float w = DNA[2] * DNA[1] + DNA[4] + 20;

	w = radius * 1.7f;
	h = w;
	int N = DNA[5] + 2;

	if (displayType == 0)
	{
		basicBody.setFillColor(bodyColor);
		basicBody.setRadius(radius);
		if (!producer)
		{
			basicBody.setPointCount((DNA[0] * DNA[3] + DNA[1]) / 10 + 3);
		}
		basicBody.setOrigin(radius, radius);
		basicBody.setPosition(location);
		basicBody.setOutlineColor(outlineColor);
		basicBody.setOutlineThickness(DNA[9] / 2);
	}
	else
	{
		convexBody.setPointCount(2 * N);

		// define the points

		for (int i = 0; i < N; i++)
		{
			convexBody.setPoint(i, sf::Vector2f(i*h / float(N - 1), w / 2.f + float(DNA[i % 10] + 5) / 15.f * w / 2.f));
			convexBody.setPoint(2 * N - i - 1, sf::Vector2f(i*h / float(N - 1), w / 2.f - float(DNA[i % 10] + 5) / 15.f*w / 2.f));
		}

		convexBody.setOrigin(h / 2.f, w / 2.f);
		convexBody.setFillColor(bodyColor);
		convexBody.setPosition(location);
		convexBody.setOutlineColor(outlineColor);
		convexBody.setOutlineThickness(DNA[9] / 2);
	}
}

float Organism::getLikability(int DNA2[])
{
	int total = 0;
	for (int i = 0; i < 4; i++)
	{
		if (abs(DNA2[i] - DNA[i]) > 3)
		{
			total -= abs(DNA2[i] - DNA[i]);
		}
		else
		{
			total++;
		}
	}
	return float(total) * 10.f / 4.f;
}

float Organism::Collides(Organism* other, sf::Vector2f pos)
{
	if (vectorDistance(other->getLocation(), pos) < other->getRadius() + radius)
	{
		float m = (other->getRadius() + radius - vectorDistance(other->getLocation(), pos)) / (other->getRadius() + radius);
		return m;
	}
	return 0.0f;
}

float Organism::CollidesFood(sf::Vector2f pos1, sf::Vector2f pos2)
{
	if (vectorDistance(pos1, pos2) < 3.f*radius)
	{
		float m = (3.f*radius - vectorDistance(pos1, pos2)) / (3.f*radius);
		return m;
	}
	return 0.0f;
}

sf::Vector2f Organism::getLocation()
{
	return location;
}

float Organism::getRadius()
{
	return radius;
}

int Organism::breedDiff(Organism* other)
{
	int breedSum = 0;
	for (int i = 0; i < 10; i++)
	{
		breedSum += int((DNA[i] != other->DNA[i]));
	}
	return breedSum;
}

void Organism::AI(int me, linkedList** LL, float timeFactor)
{
	float dir = 0, dir1 = 0, dir2 = 0, max = 0, min = 0, tempMax, tempMin, collideFactor, likeability, maxGoTo = 0, distFac;
	bool changeDir = false;
	incHap = false;
	HUNGRY = false;
	bool FOUND_FOOD = false;

	if (!(energy > maxEnergy*float(DNA[2] + DNA[9] + DNA[8] + (9 - DNA[1])) / 45.f))
	{
		HUNGRY = true;
	}
	int randChange = rand() % 360, breedSum = 0, diff, virusSum;

	Organism* org;
	org_scariest = nullptr;
	org_youngest = nullptr;
	org_sexiest = nullptr;
	org_tastiest = nullptr;
	org_flock = nullptr;
	foo_closest = nullptr;

	repel = sf::Vector2f(0, 0);
	sf::Vector2f foodDir, GO_TO = location;

	energy -= metabolism * timeFactor;
	if (virus)
	{
		energy -= 8.f*metabolism*timeFactor;
	}
	if (energy < 0.f)
	{
		energy = 0.f;
	}

	if (energy < 0.0001f)
	{
		vitality -= 4 * metabolism*timeFactor;
	}
	else
	{
		vitality += metabolism * timeFactor;
	}
	if (vitality > maxVitality)
	{
		vitality = maxVitality;
	}
	killed = false;

	float lookAhead = vision / 100.f*radius;

	happiness -= 0.5f*timeFactor;
	if (happiness < 0)
	{
		happiness = 0;
	}

	if (producer) {
		float a, b, c;

		for (int i = 0; i < maxVitality*0.5f / (15 + DNA[2]); i++)
		{
			if (rand() % (5 + DNA[3] + DNA[7] + DNA[5]) == 0)
			{
				float angle = float(rand() % 10000) / 10000.f / 3.14f * 180.f;
				float distance = 1.2f*radius + float(rand() % 100);

				sf::Vector2f pos(location.x + std::cosf(angle)*distance, location.y + std::sinf(angle)*distance);
				pos = buffer(pos, simVars);

				Food* food = new Food(15 + DNA[2], pos, simVars);
				food->simVars = simVars;

				LL[int(pos.y / simVars->COLLIDE_SQUARE_SIZE)][int(pos.x / simVars->COLLIDE_SQUARE_SIZE)].insertFood(food);
				energy -= 0.03f*timeFactor;
				bodyColor.a = 255.f*energy*2.f / maxEnergy;
				basicBody.setFillColor(bodyColor);
			}
		}
	}

	if (LIFESTAGE < 2)
	{
		if (LIFESTAGE == 0 && simVars->TIME - BORN > LIFESPAN*0.05f)
		{
			LIFESTAGE = 1;
			bodyColor.b = (DNA[0] * 100 + DNA[0] * 10 + DNA[1]) / 5 + 40;
			bodyColor.g = (DNA[1] * 100 + DNA[1] * 10 + DNA[8]) / 5 + 40;
			bodyColor.r = (DNA[2] * 100 + DNA[2] * 10 + DNA[1]) / 5 + 40;

			outlineColor.r = (DNA[6] * 100 + DNA[3] * 10 + DNA[1]) / 5 + 40;
			outlineColor.g = (DNA[7] * 100 + DNA[6] * 10 + DNA[8]) / 5 + 40;
			outlineColor.b = (DNA[5] * 100 + DNA[4] * 10 + DNA[1]) / 5 + 40;

			radius = radius * 1.5f;

			maxSpeed = maxSpeed / 1.2f;

			setBody();
		}
		else if (LIFESTAGE == 1 && simVars->TIME - BORN > LIFESPAN*0.8f)
		{
			LIFESTAGE = 2;
			bodyColor.b = (DNA[0] * 100 + DNA[0] * 10 + DNA[1]) / 10 + 150;
			bodyColor.g = (DNA[1] * 100 + DNA[1] * 10 + DNA[8]) / 10 + 150;
			bodyColor.r = (DNA[2] * 100 + DNA[2] * 10 + DNA[1]) / 10 + 150;

			outlineColor.r = (DNA[6] * 100 + DNA[3] * 10 + DNA[1]) / 5 + 50;
			outlineColor.g = (DNA[7] * 100 + DNA[6] * 10 + DNA[8]) / 5 + 50;
			outlineColor.b = (DNA[5] * 100 + DNA[4] * 10 + DNA[1]) / 5 + 50;

			maxSpeed = maxSpeed / 1.2f;

			setBody();
		}
	}

	checkFoodVicinity(0, 0, LL);
	if (foo_closest != nullptr)
	{
		FOUND_FOOD = true;
	}

	for (int x = -1; x < 2; x++)
	{
		for (int y = -1; y < 2; y++)
		{
			if (int(location.y / simVars->COLLIDE_SQUARE_SIZE + y) >= 0 && int(location.x / simVars->COLLIDE_SQUARE_SIZE) + x >= 0 && int(location.y / simVars->COLLIDE_SQUARE_SIZE + y) < simVars->HEIGHT / simVars->COLLIDE_SQUARE_SIZE + 1 && int(location.x / simVars->COLLIDE_SQUARE_SIZE + x) < simVars->WIDTH / simVars->COLLIDE_SQUARE_SIZE + 1)
			{
				for (std::vector<Organism*>::iterator it = LL[int(location.y / simVars->COLLIDE_SQUARE_SIZE + y)][int(location.x / simVars->COLLIDE_SQUARE_SIZE) + x].list.begin(); it != LL[int(location.y / simVars->COLLIDE_SQUARE_SIZE + y)][int(location.x / simVars->COLLIDE_SQUARE_SIZE) + x].list.end(); it++)
				{
					if (ID != (*it)->ID)//If it is a different organism
					{
						///////////////////////////////////////////////////////////////////
						//INVOLUNTARY CONTACT CONSEQUENCES (viruses, repelling, breeding values)
						///////////////////////////////////////////////////////////////////
						collideFactor = Collides((*it), sf::Vector2f(location.x, location.y));

						if (collideFactor > 0.0001f) //If the organism is colliding
						{
							if (!(*it)->producer && !producer && virus && !(*it)->virus) //If I have a virus and other doesnt
							{
								virusSum = 0;
								for (int i = 0; i < 10; i++)
								{
									if (virusDNA[i] != (*it)->DNA[i])
									{
										virusSum++;
									}
								}

								if (virusSum < 8) //if DNA is similar enough
								{
									virusSum += 1;
									if (rand() % (virusSum*virusSum*virusSum / 2 + 1) == 0)
									{
										makeSick(*it);
									}
								}
							}

							//Repel from close organism
							repel.x -= (radius + (*it)->radius - vectorDistance((*it)->location, location))*((*it)->location.x - location.x);
							repel.y -= (radius + (*it)->radius - vectorDistance((*it)->location, location))*((*it)->location.y - location.y);

							similarityUpdate((*it));
						}
						///////////////////////////////////////////////////////////////////
						///////////////////////////////////////////////////////////////////



						///////////////////////////////////////////////////////////////////
						//DETERMINING THE ACTIONABLE ORGANISMS, IF ANY
						///////////////////////////////////////////////////////////////////

						//Determine the tastiest organism
						if (tastiest(org_tastiest, (*it)))
						{
							org_tastiest = (*it);
						}
						//Determine the sexiest organism
						if (sexiest(org_sexiest, (*it)))
						{
							org_sexiest = (*it);
							if (org_sexiest->ID > ID)
							{
								if (org_flock == nullptr)
								{
									org_flock = org_sexiest;
								}
								else if (org_flock->ID > org_sexiest->ID)
								{
									org_flock = org_sexiest;
								}
							}
						}
						//Determine the youngest organism
						if (youngest(org_youngest, (*it)))
						{
							org_youngest = (*it);
						}
						//Determine the scariest organism
						if (scariest(org_scariest, (*it)))
						{
							org_scariest = (*it);
						}
						///////////////////////////////////////////////////////////////////
						///////////////////////////////////////////////////////////////////

						if (!FOUND_FOOD)
						{
							checkFoodVicinity(x, y, LL);
						}
					}
				}


			}
		}
	}

	///////////////////////////////////////////////////////////////////
	//DETERMINING THE BEHAVIOUR FOR THIS ORGANISM (STATE, see flow chart)
	///////////////////////////////////////////////////////////////////

	//IS OFFSPRING SAFE / AM I NOT OFFSPRING?
	if (org_youngest != nullptr && org_scariest != nullptr && org_youngest != org_scariest && LIFESTAGE != 0)
	{
		//NO
		state = PROTECT;
	}
	else
	{
		//YES
		//AM I SAFE?
		if (org_scariest != nullptr)
		{
			//NO
			state = FLEE;
		}
		else
		{
			//YES
			//AM I HUNGRY?
			if (!HUNGRY)
			{
				//NO
				//AM I NEAR SOMEONE SIMILAR?
				if (org_sexiest != nullptr && NEXT_MATE < simVars->TIME)
				{
					//YES
					//AM I YOUNG?
					if (LIFESTAGE == 1)
					{
						state = MATE;
					}
					else
					{
						state = FOLLOW;
					}
				}
				else
				{
					//NO
					if (DNA[1] >= 5 && org_flock != nullptr)
					{
						state = FLOCK;
					}
					else
					{
						state = WANDER;
					}
				}
			}
			else
			{
				//YES
				//FOOD NEARBY?
				if (foo_closest != nullptr)
				{
					//YES
					//AM I SICK?
					if (virus)
					{
						if (DNA[1] >= 5 && org_flock != nullptr)
						{
							state = FLOCK;
						}
						else
						{
							state = WANDER;
						}
					}
					else
					{
						if (DNA[1] >= 5 && org_flock != nullptr)
						{
							state = FLOCK;
						}
						else
						{
							if (LEADER)
							{
								state = WANDER;
							}
							else
							{
								state = FOOD;
							}
						}
					}
				}
				else
				{
					//NO
					if (DNA[1] >= 5 && org_flock != nullptr)
					{
						state = FLOCK;
					}
					else
					{
						state = WANDER;
					}

					//IF AGGRO, PREY NEARBY, VERY HUNGRY?
					if (Aggro && org_tastiest != nullptr && HUNGRY)
					{
						//YES
						state = ATTACK;
					}
				}
			}
		}
	}

	///////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////
	//CARRY OUT THE BEHAVIOUR
	///////////////////////////////////////////////////////////////////
	//EATING
	if (state == FOOD)
	{
		collideFactor = CollidesFood(sf::Vector2f(location.x, location.y), foo_closest->location);

		if (!eatFood(collideFactor, LL))
		{
			//EAT HERBO
			if (!Aggro && !foo_closest->killed)
			{
				GO_TO = foo_closest->location;
			}

			//EAT AGGRO
			else if (Aggro && foo_closest->killed)
			{
				GO_TO = foo_closest->location;
			}
		}
	}

	//FLEEING
	if (state == FLEE)
	{
		GO_TO = sf::Vector2f(location - (org_scariest->location - location));
	}

	//PROTECTING
	if (state == PROTECT)
	{
		collideFactor = Collides(org_scariest, sf::Vector2f(location.x, location.y));

		if (collideFactor > 0.0001f)
		{
			if (LIFESTAGE == 1)
			{
				org_scariest->vitality -= attack;
				org_scariest->killed = true;
			}
			else
			{
				org_scariest->vitality -= 0.3f*attack;
				org_scariest->killed = true;
			}
		}

		GO_TO = sf::Vector2f((org_scariest->location + org_youngest->location) / 2.f);
	}

	//ATTACKING
	if (state == ATTACK)
	{
		collideFactor = Collides(org_tastiest, sf::Vector2f(location.x, location.y));

		if (collideFactor > 0.0001f)
		{
			if (LIFESTAGE == 1)
			{
				org_tastiest->vitality -= attack * timeFactor;
				org_tastiest->killed = true;
			}
			else
			{
				org_tastiest->vitality -= 0.3f*attack*timeFactor;
				org_tastiest->killed = true;
			}
		}

		GO_TO = org_tastiest->location;
	}

	//MATING
	if (state == MATE)
	{
		GO_TO = org_sexiest->location;

		collideFactor = Collides(org_sexiest, sf::Vector2f(location.x, location.y));

		if (collideFactor > 0.0001f)
		{
			if (!incHap)
			{
				happiness += 6.5f*timeFactor;
				incHap = true;
			}

			if (happiness < 0)
			{
				happiness = 0;
			}

			if (happiness > 110)
			{
				happiness = 110;
			}

			if (happiness > 100)// && org_sexiest->energy > org_sexiest->maxEnergy*float(org_sexiest->DNA[2] + org_sexiest->DNA[9] + org_sexiest->DNA[8] + org_sexiest->DNA[1]) / 45.f)
			{
				NEXT_MATE = simVars->TIME + (DNA[5] + (1 - DNA[7]) + DNA[3] + 10)*timeFactor*10.f;
				BREED = true;
				energy *= 0.4f;
				for (int n = 0; n < 10; n++)
				{
					breedDNA[n] = org_sexiest->DNA[n];
				}
				breedLoc = org_sexiest->location;
				org_sexiest->happiness = 0;
				happiness = 0;
			}
		}
	}

	//WANDERING
	if (state == WANDER)
	{
		if (LEADER)
		{
			roamAngle += float((rand() % 9) - 4)*0.02f;
			if (roamAngle < 0.f)
			{
				roamAngle += 180.f / M_PI;
			}
			if (roamAngle > 180.f / M_PI)
			{
				roamAngle -= 180.f / M_PI;
			}
			if (foo_closest != nullptr)
			{
				collideFactor = CollidesFood(sf::Vector2f(location.x, location.y), foo_closest->location);

				eatFood(collideFactor, LL);
			}
			GO_TO = sf::Vector2f(location.x + std::cosf(roamAngle)*60.f / 2.f, location.y + std::sinf(roamAngle)*60.f / 2.f);
		}
		else
		{
			roamAngle += float((rand() % 9) - 4)*0.1f;
			if (roamAngle < 0.f)
			{
				roamAngle += 180.f / M_PI;
			}
			if (roamAngle > 180.f / M_PI)
			{
				roamAngle -= 180.f / M_PI;
			}
			GO_TO = sf::Vector2f(location.x + std::cosf(roamAngle)*100.f / 2.f, location.y + std::sinf(roamAngle)*100.f / 2.f);
		}
	}

	//FOLLOWING
	if (state == FOLLOW)
	{
		GO_TO = org_sexiest->location;

		collideFactor = Collides(org_sexiest, sf::Vector2f(location.x, location.y));
	}

	//LATTICEING
	if (state == FLOCK)
	{
		if (foo_closest != nullptr)
		{
			collideFactor = CollidesFood(sf::Vector2f(location.x, location.y), foo_closest->location);
			eatFood(collideFactor, LL);
		}

		if (org_flock->org_flock != nullptr)
		{
			GO_TO = org_flock->desiredLocation - (org_flock->org_flock->desiredLocation - org_flock->desiredLocation);
		}
		else
		{
			org_flock->LEADER = true;
			if (DNA[3] > 3 && DNA[6] < 7)
			{
				GO_TO = org_flock->location - (org_flock->desiredLocation - org_flock->location);
			}
			else
			{
				if ((BORN % 2) == 0)
				{
					GO_TO = org_flock->location + sf::Vector2f(-(org_flock->location.y - org_flock->desiredLocation.y), (org_flock->location.x - org_flock->desiredLocation.x)) - (org_flock->desiredLocation - org_flock->location);
				}
				else
				{
					GO_TO = org_flock->location + sf::Vector2f((org_flock->location.y - org_flock->desiredLocation.y), -(org_flock->location.x - org_flock->desiredLocation.x)) - (org_flock->desiredLocation - org_flock->location);
				}
			}
		}
	}

	///////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////
	LEADER = false;
	changeDesiredLocation(GO_TO);
}

void Organism::checkFoodVicinity(int x, int y, linkedList** LL)
{
	if (HUNGRY)
	{
		for (int k = 0; k < LL[int(location.y / simVars->COLLIDE_SQUARE_SIZE + y)][int(location.x / simVars->COLLIDE_SQUARE_SIZE) + x].foodList.size(); k++)
		{
			///////////////////////////////////////////////////////////////////
			//DETERMINING THE ACTIONABLE FOOD, IF ANY
			///////////////////////////////////////////////////////////////////

			//Determine the closest food
			if (foo_closest == nullptr || closest(foo_closest, LL[int(location.y / simVars->COLLIDE_SQUARE_SIZE + y)][int(location.x / simVars->COLLIDE_SQUARE_SIZE) + x].foodList[k]))
			{
				if (Aggro)
				{
					if (LL[int(location.y / simVars->COLLIDE_SQUARE_SIZE + y)][int(location.x / simVars->COLLIDE_SQUARE_SIZE) + x].foodList[k]->killed)
					{
						foo_closest = LL[int(location.y / simVars->COLLIDE_SQUARE_SIZE + y)][int(location.x / simVars->COLLIDE_SQUARE_SIZE) + x].foodList[k];
					}
				}
				else
				{
					if (!LL[int(location.y / simVars->COLLIDE_SQUARE_SIZE + y)][int(location.x / simVars->COLLIDE_SQUARE_SIZE) + x].foodList[k]->killed)
					{
						foo_closest = LL[int(location.y / simVars->COLLIDE_SQUARE_SIZE + y)][int(location.x / simVars->COLLIDE_SQUARE_SIZE) + x].foodList[k];
					}
				}
			}
			///////////////////////////////////////////////////////////////////
			///////////////////////////////////////////////////////////////////
			if (k > 5)
			{
				return;
			}
		}
	}
}

bool Organism::eatFood(float collideFactor, linkedList** LL)
{
	if (collideFactor > 2.f / 3.f)
	{
		if (maxEnergy - energy > foo_closest->value)
		{
			if (!producer && !virus)
			{
				if (Aggro && foo_closest->killed)
				{
					energy += foo_closest->value;
					foo_closest->Eat(LL);
					if (rand() % (55 * (DNA[6] * DNA[8] + 1)) == 0)
					{
						virus = true;
						for (int i = 0; i < 10; i++)
						{
							virusDNA[i] = DNA[i];
						}
						circ.setFillColor(sf::Color(25.f*virusDNA[2], 25.f*virusDNA[5], 25.f*virusDNA[7]));
						circ.setRadius(radius*0.25f);
						circ.setOutlineThickness(2);
						circ.setOutlineColor(sf::Color(25.f*virusDNA[1], 25.f*virusDNA[4], 25.f*virusDNA[6]));
						circ.setOrigin(sf::Vector2f(radius*0.25f, radius*0.25f));
					}
				}
				else if (!Aggro && !foo_closest->killed)
				{
					energy += foo_closest->value;
					foo_closest->Eat(LL);
					if (rand() % (150 * (DNA[6] * DNA[2] + 1)) == 0)
					{
						virus = true;
						for (int i = 0; i < 10; i++)
						{
							virusDNA[i] = DNA[i];
						}
						circ.setFillColor(sf::Color(25.f*virusDNA[2], 25.f*virusDNA[5], 25.f*virusDNA[7]));
						circ.setRadius(radius*0.25f);
						circ.setOutlineThickness(2);
						circ.setOutlineColor(sf::Color(25.f*virusDNA[1], 25.f*virusDNA[4], 25.f*virusDNA[6]));
						circ.setOrigin(sf::Vector2f(radius*0.25f, radius*0.25f));
					}
				}
			}
			if (producer)
				foo_closest->Eat(LL);

			if (energy > maxEnergy)
				energy = maxEnergy;
			return true;
		}
	}
	return false;
}

int Organism::similarityUpdate(Organism* other)
{
	int breedSum = breedDiff(other);
	if (breedSum < simVars->BREED_BASE && breedSum <= maxBreed - 1 && breedSum >= 0)
	{
		maxBreed = breedSum;
	}
	return breedSum;
}

int Organism::mateFac(Organism* other)
{
	int breedSum = breedDiff(other);
	if (breedSum < simVars->BREED_BASE && breedSum <= maxBreed && breedSum >= 0)
	{
		return (maxBreed - breedSum);
	}
	return -1;
}
int Organism::attackFac(Organism* other)
{
	int breedSum = breedDiff(other);
	if (breedSum > (maxBreed + int(energy < 0.4f*maxEnergy) * 2))
	{
		return (breedSum - (maxBreed + int(energy < 0.4f*maxEnergy) * 2));
	}
	return -1;
}
int Organism::defendFac(Organism* other)
{
	int breedSum = breedDiff(other);
	if (breedSum > maxBreed + 2 * int(other->Aggro) + 1)
	{
		return (breedSum - (maxBreed + 2 * int(other->Aggro) + 1));
	}
	return -1;
}

bool Organism::tastiest(Organism* incumbent, Organism* challenger)
{
	if (Aggro && !challenger->producer && !producer)//if you can attack
	{
		if (incumbent == nullptr)
		{
			int fac = attackFac(challenger);
			//And if you can eat them based on similarity
			if (fac >= 0)
			{
				return true;
			}
		}
		//And if some factor is biggger
		else if (vectorDistance(location, challenger->location) < vectorDistance(location, incumbent->location))
		{
			int fac = attackFac(challenger);

			//And if you can eat them based on similarity
			if (fac >= 0)
			{
				return true;
			}
		}
	}
	return false;
}
bool Organism::sexiest(Organism* incumbent, Organism* challenger)
{
	if (!challenger->producer && !producer && !virus)//if you can mate
	{
		int fac = mateFac(challenger);
		//With other organism
		if (fac >= 0 && challenger->LIFESTAGE == 1)
		{
			if (incumbent == nullptr)
			{
				return true;
			}
			//And they are closest
			if (vectorDistance(location, challenger->location) < vectorDistance(location, incumbent->location))
			{
				return true;
			}
		}
	}
	return false;
}
bool Organism::youngest(Organism* incumbent, Organism* challenger)
{
	if (!challenger->producer && !producer)//if you are able to protect
	{
		int fac = mateFac(challenger);
		//the other organism
		if (fac >= 0 && (challenger->LIFESTAGE == 0 || DNA[8] > 6))
		{
			if (incumbent == nullptr)
			{
				return true;
			}
			//And they are closest
			else if (vectorDistance(location, challenger->location) < vectorDistance(location, incumbent->location))
			{
				return true;
			}
		}
	}
	return false;
}

bool Organism::scariest(Organism* incumbent, Organism* challenger)
{
	if ((((vitality > 0.8f*maxVitality && Aggro) || !Aggro) && !challenger->producer && !producer && (challenger->Aggro || DNA[7] > 6)) || challenger->virus)//if they are dangerous
	{
		int BS2;
		BS2 = breedDiff(challenger);
		//to you
		if (BS2 > simVars->BREED_BASE || (BS2 > challenger->maxBreed && challenger->energy < 0.2f*challenger->maxEnergy) || challenger->state == ATTACK || challenger->virus)
		{
			if (incumbent == nullptr)
			{
				return true;
			}
			//and they are closest
			else if (vectorDistance(location, challenger->location) < vectorDistance(location, incumbent->location))
			{
				return true;
			}
		}
	}
	return false;
}

bool Organism::closest(Food* incumbent, Food* challenger)
{
	if (incumbent == nullptr)
	{
		return true;
	}
	//if they are closest
	if (vectorDistance(location, challenger->location) < vectorDistance(location, incumbent->location))
	{
		return true;
	}
	return false;
}

void Organism::move(linkedList** LL, float timefactor)
{
	float nudge = 0.0255f / radius, edge = 0.1f;
	bool edgeCase = false;
	if (producer)
	{
		nudge = 0.00007f;
	}

	if (location.x < simVars->Xbuff)
	{
		velocity.x += (simVars->Xbuff - location.x)*edge;
		edgeCase = true;
	}
	if (location.y < simVars->Ybuff)
	{
		velocity.y += (simVars->Ybuff - location.y)*edge;
		edgeCase = true;
	}
	if (location.x > simVars->WIDTH - simVars->Xbuff)
	{
		velocity.x -= (simVars->Xbuff - (simVars->WIDTH - location.x))*edge;
		edgeCase = true;
	}
	if (location.y > simVars->HEIGHT - simVars->Ybuff)
	{
		velocity.y -= (simVars->Ybuff - (simVars->HEIGHT - location.y))*edge;
		edgeCase = true;
	}

	if (edgeCase)
	{
		roamAngle = std::atan2f(velocity.y, velocity.x);
	}

	newLoc = location;

	newLoc.x += velocity.x*timefactor + nudge * repel.x;
	newLoc.y += velocity.y*timefactor + nudge * repel.y;

	if (newLoc.x - radius < 0)
	{
		newLoc.x = radius;
	}
	if (newLoc.y - radius < 0)
	{
		newLoc.y = radius;
	}
	if (newLoc.x + radius > simVars->WIDTH)
	{
		newLoc.x = simVars->WIDTH - radius;
	}
	if (newLoc.y + radius > simVars->HEIGHT)
	{
		newLoc.y = simVars->HEIGHT - radius;
	}

	if (location.x / simVars->COLLIDE_SQUARE_SIZE != (newLoc.x) / simVars->COLLIDE_SQUARE_SIZE || location.y / simVars->COLLIDE_SQUARE_SIZE != (newLoc.y) / simVars->COLLIDE_SQUARE_SIZE)
	{
		LL[int(location.y / simVars->COLLIDE_SQUARE_SIZE)][int(location.x / simVars->COLLIDE_SQUARE_SIZE)].remove(this);
		LL[int((newLoc.y) / simVars->COLLIDE_SQUARE_SIZE)][int((newLoc.x) / simVars->COLLIDE_SQUARE_SIZE)].insert(this);
	}

	location = newLoc;
	bool neg = false;
	if (velocity.x < 0)
	{
		neg = true;
	}

	rotation = 90.f + 180.0f*atanf((velocity.y) / (velocity.x)) / M_PI + 180.f*neg;

	if (displayType == 0)
	{
		basicBody.setRotation(90.f + 180.0f*atanf((velocity.y) / (velocity.x)) / M_PI + 180.f*neg);
		basicBody.setPosition(location);
	}
	else
	{
		convexBody.setRotation(180.0f*atanf((velocity.y) / (velocity.x)) / M_PI + 180.f*neg);
		convexBody.setPosition(location);
	}
}

void Organism::die()
{
	delete this;
}

void Organism::changeVelocity(float timefactor)
{
	tempVel = desiredLocation - location;
	if (state == ATTACK)
	{
		if (vectorDistance(desiredLocation, location) > (maxSpeed*1.7f)*(1.f - 0.5f*virus))
		{
			tempVel.y *= (maxSpeed*1.7f)*(1.f - 0.5f*virus) / vectorDistance(desiredLocation, location);
			tempVel.x *= (maxSpeed*1.7f)*(1.f - 0.5f*virus) / vectorDistance(desiredLocation, location);
		}
	}
	else if (state == FLEE)
	{
		if (vectorDistance(desiredLocation, location) > (maxSpeed*2.f)*(1.f - 0.5f*virus))
		{
			tempVel.y *= (maxSpeed*2.f)*(1.f - 0.5f*virus) / vectorDistance(desiredLocation, location);
			tempVel.x *= (maxSpeed*2.f)*(1.f - 0.5f*virus) / vectorDistance(desiredLocation, location);
		}
	}
	else if (state == FLOCK)
	{
		if (vectorDistance(desiredLocation, location) > (maxSpeed*1.5f)*(1.f - 0.5f*virus))
		{
			tempVel.y *= (maxSpeed*1.5f)*(1.f - 0.5f*virus) / vectorDistance(desiredLocation, location);
			tempVel.x *= (maxSpeed*1.5f)*(1.f - 0.5f*virus) / vectorDistance(desiredLocation, location);
		}
	}
	else
	{
		if (vectorDistance(desiredLocation, location) > (maxSpeed)*(1.f - 0.5f*virus))
		{
			tempVel.y *= (maxSpeed)*(1.f - 0.5f*virus) / vectorDistance(desiredLocation, location);
			tempVel.x *= (maxSpeed)*(1.f - 0.5f*virus) / vectorDistance(desiredLocation, location);
		}
	}
	float diff = 0.0015f*float(DNA[6] + DNA[2])*timefactor + float(int(state == FLOCK))*0.03f;

	velocity.x = (1.0f - diff)*velocity.x + diff * tempVel.x;
	velocity.y = (1.0f - diff)*velocity.y + diff * tempVel.y;
}

void Organism::changeDesiredLocation(sf::Vector2f newLoc)
{
	desiredLocation = newLoc;
}

void Organism::Draw(sf::RenderWindow* window)
{
	if (displayType == 0)
	{
		window->draw(basicBody);
	}
	else
	{
		window->draw(convexBody);
	}

	if (virus)
	{
		circ.setPosition(location);
		window->draw(circ);
	}
	if (LEADER)
	{
		circ.setPosition(location);
		window->draw(circ);
	}
};

void Organism::displayCollisions(sf::RenderWindow* window)
{
	sf::VertexArray lines(sf::LinesStrip, 2);
	Organism* org;

	lines[0].position = location;
	lines[1].position = desiredLocation;

	switch (state)
	{
	case WANDER:
		lines[1].color = sf::Color(100, 100, 100, 250);
		lines[0].color = sf::Color(100, 100, 100, 250);
		break;
	case FOLLOW:
		lines[1].color = sf::Color(255, 180, 30, 250);
		lines[0].color = sf::Color(255, 180, 30, 250);
		break;
	case FOOD:
		lines[1].color = sf::Color(0, 0, 255, 250);
		lines[0].color = sf::Color(0, 0, 255, 250);
		break;
	case MATE:
		lines[1].color = sf::Color(0, 255, 0, 250);
		lines[0].color = sf::Color(0, 255, 0, 250);
		break;
	case FLEE:
		lines[1].color = sf::Color(255, 255, 0, 250);
		lines[0].color = sf::Color(255, 255, 0, 250);
		break;
	case ATTACK:
		lines[1].color = sf::Color(255, 0, 0, 250);
		lines[0].color = sf::Color(255, 0, 0, 250);
		break;
	case PROTECT:
		lines[1].color = sf::Color::Cyan;
		lines[0].color = sf::Color::Cyan;
		break;
	case FLOCK:
		lines[1].color = sf::Color(255, 255, 255, 250);
		lines[0].color = sf::Color(255, 255, 255, 250);
		break;
	default:
		break;
	}

	window->draw(lines);
}