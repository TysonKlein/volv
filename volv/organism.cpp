#include "header.hpp"

Organism::Organism(sf::Vector2f LOC, int newDNA[], SimVars* newSimVars)
{
	simVars = newSimVars;

	//Set DNA for organism
	for (int i = 0; i < DNA_SIZE; i++)
	{
		DNA[i] = newDNA[i];
	}

	//set all DNA-dictated variables
	radius = valFromDNA(DNA, 1.0f, 3.f, 628745.1386f)*valFromDNA(DNA, 2.0f, 4.f, 52323.3523f)*valFromDNA(DNA, 2.0f, 4.f, 8965245.345f);
	maxSpeed = 1.9f * sqrt(sqrt((valFromDNA(DNA, 8.f, 140.f, 12051.9862f))) / (radius*radius));
	attack = (5.f + valFromDNA(DNA, 0.f, 40.f, 161205.f))*radius*radius / 1420.f;
	immunity = valFromDNA(DNA, 1500.f, 10000.f, 7862387.234);
	scaredness = valFromDNA(DNA, 0.f, 20.f, 896134.423f);
	hungerLevel = valFromDNA(DNA, 10.f, 40.f, 6712876.f);
	maxVitality = (valFromDNA(DNA, 0.f, 40.f, 123785.532f) + 10.f) * radius * radius / 15.f;
	metabolism = (valFromDNA(DNA, 0.f, 10.f, 58726.1235f)*0.004f)*sqrt(sqrt(maxSpeed));
	mateWait = valFromDNA(DNA, 80.f, 250.f, 815623.5196f);
	LIFESPAN = 5.f * radius / metabolism * (valFromDNA(DNA, 0.f, 30.f, 862375.123f) + 10.f);
	vision = std::min(valFromDNA(DNA, 2.f*radius, 10.f*radius, 815623.5196f), float(simVars->COLLIDE_SQUARE_SIZE));

	bodyColor.b = valFromDNA(DNA, 0.f, 255.f, 642624.6643f);
	bodyColor.g = valFromDNA(DNA, 0.f, 255.f, 236506.5472f);
	bodyColor.r = valFromDNA(DNA, 0.f, 255.f, 819637.0018f);
	bodyColor.a = 160;

	outlineColor.r = valFromDNA(DNA, 0.f, 255.f, 926.16413f);
	outlineColor.g = valFromDNA(DNA, 0.f, 255.f, 1810.35235f);
	outlineColor.b = valFromDNA(DNA, 0.f, 255.f, 52566.102365f);
	outlineColor.a = 160;

	//Set other variables
	BORN = simVars->TIME;
	NEXT_MATE = BORN;
	BREED = false;
	LEADER = false;
	maxBreed = DNA_SIZE;
	LIFESTAGE = 0;
	roamAngle = float(rand() % 10000) / 10000.f / 3.14f * 180.f;
	incHap = false;
	virus = false;
	maxBreedingDiff = DNA_SIZE;
	location = LOC;
	desiredLocation = location;
	rotation = 0;
	repel.x = 0;
	repel.y = 0;
	happiness = 0.f;
	killed = false;
	vitality = maxVitality;
	maxEnergy = maxVitality;
	energy = 0.5f*maxEnergy;
	producer = false;
	
	//Determine organism characteristics from these variables
	if (scaredness > 5.f)
	{
		DEFENSIVE = true;
		attack *= 1.5f;
		maxVitality *= 1.3f;
	}
	else
		DEFENSIVE = false;

	if (valFromDNA(DNA, 0.f, 25.f, 78183.43f) > 20.f) //Set Aggro
	{
		Aggro = true;
		displayType = 1;
		maxVitality *= 1.3f;
		maxSpeed *= 1.3f;
		immunity /= 5.f;
	}
	else
	{
		Aggro = false;
		attack /= 1.5f;
		displayType = 0;
	}

	if (DNA[DNA_SIZE-1] == 0)
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
}

void Organism::makeSick(Organism* other)
{
	for (int i = 0; i < DNA_SIZE; i++)
	{
		other->virusDNA[i] = virusDNA[i];
	}

	other->circ.setFillColor(sf::Color(valFromDNA(DNA, 0.f, 255.f, 642624.6643f),valFromDNA(DNA, 0.f, 255.f, 236506.5472f),valFromDNA(DNA, 0.f, 255.f, 819637.0018f)));
	other->circ.setRadius(other->radius*0.25f);
	other->circ.setOutlineThickness(2);
	other->circ.setOutlineColor(sf::Color(valFromDNA(DNA, 0.f, 255.f, 2934.f), valFromDNA(DNA, 0.f, 255.f, 10983784.f), valFromDNA(DNA, 0.f, 255.f, 1234.f)));
	other->circ.setOrigin(sf::Vector2f(other->radius*0.25f, other->radius*0.25f));

	other->virus = true;
}

void Organism::setBody()
{
	// resize it to 5 points
	float h, w;

	w = radius * 1.7f;
	h = w;

	int N = int(valFromDNA(DNA, 0.f, 6.f, 3122.f)) + 2;

	if (displayType == 0)
	{
		basicBody.setFillColor(bodyColor);
		basicBody.setRadius(radius);
		if (!producer)
		{
			basicBody.setPointCount(int(valFromDNA(DNA, 0.f, 50.f, 62341.f)) / 10 + 3);
		}
		basicBody.setOrigin(radius, radius);
		basicBody.setPosition(location);
		basicBody.setOutlineColor(outlineColor);
		basicBody.setOutlineThickness(valFromDNA(DNA, 0.f, 6.f, 9237.f) / 2);
	}
	else
	{
		convexBody.setPointCount(2 * N);

		// define the points

		for (int i = 0; i < N; i++)
		{
			convexBody.setPoint(i, sf::Vector2f(i*h / float(N - 1), w / 2.f + float(DNA[i % DNA_SIZE] + 0.5*DNA_SIZE) / (1.5f*DNA_SIZE) * w / 2.f));
			convexBody.setPoint(2 * N - i - 1, sf::Vector2f(i*h / float(N - 1), w / 2.f - float(DNA[i % DNA_SIZE] + 0.5*DNA_SIZE) / (1.5f*DNA_SIZE) * w / 2.f));
		}

		convexBody.setOrigin(h / 2.f, w / 2.f);
		convexBody.setFillColor(bodyColor);
		convexBody.setPosition(location);
		convexBody.setOutlineColor(outlineColor);
		convexBody.setOutlineThickness(valFromDNA(DNA, 0.f, 6.f, 9237.f) / 2);
	}
}

float Organism::getLikability(int DNA2[])
{
	int total = 0;
	for (int i = 0; i < 0.4f*DNA_SIZE; i++)
	{
		if (abs(DNA2[i] - DNA[i]) > 0.3f*DNA_SIZE)
		{
			total -= abs(DNA2[i] - DNA[i]);
		}
		else
		{
			total+= 0.3f*float(DNA_SIZE);
		}
	}
	return float(total) * 10.f / 0.4f*float(DNA_SIZE);
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
	if (vectorDistance(pos1, pos2) < 0.8f*radius)
	{
		float m = (0.8f*radius - vectorDistance(pos1, pos2)) / (0.8f*radius);
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
	for (int i = 0; i < DNA_SIZE; i++)
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

	if (!(energy > maxEnergy*hungerLevel / 45.f))
	{
		HUNGRY = true;
	}
	int randChange = rand() % 360, breedSum = 0, diff, virusSum;

	Organism* org;
	org_scariest = nullptr;
	org_youngest = nullptr;
	org_sexiest = nullptr;
	org_tastiest = nullptr;
	org_weirdest = nullptr;
	foo_closest = nullptr;

	repel = sf::Vector2f(0, 0);
	sf::Vector2f foodDir, GO_TO = location;

	energy -= metabolism * timeFactor;
	if (virus)
	{
		energy -= 1000.f*metabolism*timeFactor;
	}
	if (energy < 0.f)
	{
		energy = 0.f;
	}

	if (energy < 0.0001f)
	{
		vitality -= 20 * metabolism*timeFactor;
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

	happiness -= 0.5f*timeFactor;
	if (happiness < 0)
	{
		happiness = 0;
	}

	if (producer) {
		for (int i = 0; i < maxVitality*0.5f / (15 + DNA[2]); i++)
		{
			if (rand() % (30) == 0)
			{
				float angle = float(rand() % 10000) / 10000.f / 3.14f * 180.f;
				float distance = 1.2f*radius + float(rand() % 100);

				sf::Vector2f pos(location.x + std::cosf(angle)*distance, location.y + std::sinf(angle)*distance);
				pos = buffer(pos, simVars);

				Food* food = new Food(pos, simVars);
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
			bodyColor.b = valFromDNA(DNA, 0.f, 215.f, 642624.6643f) + 40.f;
			bodyColor.g = valFromDNA(DNA, 0.f, 215.f, 236506.5472f) + 40.f;
			bodyColor.r = valFromDNA(DNA, 0.f, 215.f, 819637.0018f) + 40.f;

			outlineColor.r = valFromDNA(DNA, 0.f, 215.f, 926.16413f) + 40.f;
			outlineColor.g = valFromDNA(DNA, 0.f, 215.f, 1810.35235f) + 40.f;
			outlineColor.b = valFromDNA(DNA, 0.f, 215.f, 52566.102365f) + 40.f;

			radius = radius * 1.5f;

			maxSpeed = maxSpeed / 1.2f;

			setBody();
		}
		else if (LIFESTAGE == 1 && simVars->TIME - BORN > LIFESPAN*0.8f)
		{
			LIFESTAGE = 2;
			bodyColor.b = valFromDNA(DNA, 0.f, 100.f, 642624.6643f) + 140.f;
			bodyColor.g = valFromDNA(DNA, 0.f, 100.f, 236506.5472f) + 140.f;
			bodyColor.r = valFromDNA(DNA, 0.f, 100.f, 819637.0018f) + 140.f;

			outlineColor.r = valFromDNA(DNA, 0.f, 100.f, 926.16413f) + 140.f;
			outlineColor.g = valFromDNA(DNA, 0.f, 100.f, 1810.35235f) + 140.f;
			outlineColor.b = valFromDNA(DNA, 0.f, 100.f, 52566.102365f) + 140.f;

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
					if (vectorDistance((*it)->location, location) < vision && ID != (*it)->ID)//If it is a different organism
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
								for (int i = 0; i < DNA_SIZE; i++)
								{
									if (virusDNA[i] == (*it)->DNA[i])
									{
										virusSum++;
									}
								}

								if (virusSum > 0.5f*DNA_SIZE) //if DNA is similar enough
								{
									virusSum += 1;
									if (rand() % ((DNA_SIZE - virusSum)*(DNA_SIZE - virusSum) / 2 + 1) == 0)
									{
										makeSick(*it);
									}
								}
							}

							//Repel from close organism
							float radFac = (*it)->radius / radius;
							radFac = radFac*radFac*radFac*radFac;

							if (radFac > 1.f)
								radFac = 1.f;

							repel.x -= collideFactor*((*it)->location.x - location.x)*radFac;
							repel.y -= collideFactor*((*it)->location.y - location.y)*radFac;

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
						if (weirdest(org_weirdest, (*it)))
						{
							org_weirdest = (*it);
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

	//IS OFFSPRING SAFE AND AM I NOT OFFSPRING?
	if (org_youngest != nullptr && org_scariest != nullptr && org_youngest != org_scariest && LIFESTAGE != 0 && int(scaredness)%2 == 0)
	{
		//NO
		state = PROTECT;
	}
	else
	{
		//YES
		//AM I DEFENSIVE?
		if (DEFENSIVE && org_weirdest != nullptr)
			state = DEFEND;
		else if (org_scariest != nullptr)
		{
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
					state = WANDER;
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
						state = WANDER;
					}
					else
					{
						state = FOOD;
					}
				}
				else
				{
					//NO
					state = WANDER;

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
			if (!Aggro && !foo_closest->MEAT)
			{
				GO_TO = foo_closest->location;
			}

			//EAT AGGRO
			else if (Aggro && foo_closest->MEAT)
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

	//DEFENDING
	if (state == DEFEND)
	{
		collideFactor = Collides(org_weirdest, sf::Vector2f(location.x, location.y));

		if (collideFactor > 0.0001f)
		{
			if (LIFESTAGE == 1)
			{
				org_weirdest->vitality -= attack;
				org_weirdest->killed = true;
			}
			else
			{
				org_weirdest->vitality -= 0.3f*attack;
				org_weirdest->killed = true;
			}
		}

		float distance = 2.f*(radius+ org_weirdest->radius);
		sf::Vector2f vec = sf::Vector2f(org_weirdest->location - location);
		float between = sqrt(vec.x*vec.x + vec.y*vec.y);

		GO_TO = org_weirdest->location- vec * 1.1f;
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

			if (happiness > 100)
			{
				NEXT_MATE = simVars->TIME + mateWait*timeFactor;
				BREED = true;
				energy *= 0.4f;
				for (int n = 0; n < DNA_SIZE; n++)
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
		if (foo_closest != nullptr)
		{
			collideFactor = CollidesFood(sf::Vector2f(location.x, location.y), foo_closest->location);
			eatFood(collideFactor, LL);
		}

		GO_TO = org_sexiest->location;
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
		int k = 0;
		///////////////////////////////////////////////////////////////////
		//DETERMINING THE ACTIONABLE FOOD, IF ANY
		///////////////////////////////////////////////////////////////////
		if (Aggro)
		{
			for (foodIt = LL[int(location.y / simVars->COLLIDE_SQUARE_SIZE + y)][int(location.x / simVars->COLLIDE_SQUARE_SIZE) + x].meatFoodList.begin(); foodIt != LL[int(location.y / simVars->COLLIDE_SQUARE_SIZE + y)][int(location.x / simVars->COLLIDE_SQUARE_SIZE) + x].meatFoodList.end(); foodIt++)
			{
				if (vectorDistance((*foodIt)->location, location) < vision)
				{
					if (foo_closest == nullptr || closest(foo_closest, (*foodIt)))
					{
						foo_closest = (*foodIt);
					}
					if (k > 5)
					{
						return;
					}
					k++;
				}
			}
		}
		else
		{
			for (foodIt = LL[int(location.y / simVars->COLLIDE_SQUARE_SIZE + y)][int(location.x / simVars->COLLIDE_SQUARE_SIZE) + x].plantFoodList.begin(); foodIt != LL[int(location.y / simVars->COLLIDE_SQUARE_SIZE + y)][int(location.x / simVars->COLLIDE_SQUARE_SIZE) + x].plantFoodList.end(); foodIt++)
			{
				if (vectorDistance((*foodIt)->location, location) < vision)
				{
					if (foo_closest == nullptr || closest(foo_closest, (*foodIt)))
					{
						foo_closest = (*foodIt);
					}
					if (k > 5)
					{
						return;
					}
					k++;
				}
			}
		}
		///////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////
	}
}

bool Organism::eatFood(float collideFactor, linkedList** LL)
{
	if (collideFactor > 0.01f)
	{
		if (maxEnergy - energy > foo_closest->value || state == FOOD)
		{
			if (!producer && !virus)
			{
				if (Aggro && foo_closest->MEAT)
				{
					energy += foo_closest->value;
					foo_closest->Eat(LL);
					if (rand() % int(immunity) == 0)
					{
						virus = true;
						for (int i = 0; i < DNA_SIZE; i++)
						{
							virusDNA[i] = DNA[i];
						}
						circ.setFillColor(sf::Color(valFromDNA(DNA, 0.f, 255.f, 642624.6643f), valFromDNA(DNA, 0.f, 255.f, 236506.5472f), valFromDNA(DNA, 0.f, 255.f, 819637.0018f)));
						circ.setRadius(radius*0.25f);
						circ.setOutlineThickness(2);
						circ.setOutlineColor(sf::Color(valFromDNA(DNA, 0.f, 255.f, 2934.f), valFromDNA(DNA, 0.f, 255.f, 10983784.f), valFromDNA(DNA, 0.f, 255.f, 1234.f)));
						circ.setOrigin(sf::Vector2f(radius*0.25f, radius*0.25f));
					}
				}
				else if (!Aggro && !foo_closest->MEAT)
				{
					energy += foo_closest->value;
					foo_closest->Eat(LL);
					if (rand() % int(immunity) == 0)
					{
						virus = true;
						for (int i = 0; i < DNA_SIZE; i++)
						{
							virusDNA[i] = DNA[i];
						}
						circ.setFillColor(sf::Color(valFromDNA(DNA, 0.f, 255.f, 642624.6643f), valFromDNA(DNA, 0.f, 255.f, 236506.5472f), valFromDNA(DNA, 0.f, 255.f, 819637.0018f)));
						circ.setRadius(radius*0.25f);
						circ.setOutlineThickness(2);
						circ.setOutlineColor(sf::Color(valFromDNA(DNA, 0.f, 255.f, 2934.f), valFromDNA(DNA, 0.f, 255.f, 10983784.f), valFromDNA(DNA, 0.f, 255.f, 1234.f)));
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
	if (breedSum < simVars->BREED_BASE && breedSum <= maxBreed + 0.1f*DNA_SIZE && breedSum >= 0)
	{
		maxBreed = breedSum + 0.1f*DNA_SIZE;
	}
	return breedSum;
}

//Returns value between 0(low mate factor) and 1(High mate factor)
bool Organism::mateFac(Organism* other)
{
	int breedSum = breedDiff(other);

	if (other->radius*0.66f > radius || other->radius*1.5f < radius)
	{
		//return false;
	}

	if (breedSum < simVars->BREED_BASE && breedSum <= maxBreed)
	{
		return true;
	}

	return false;
}

bool Organism::attackFac(Organism* other)
{
	int breedSum = breedDiff(other);

	if (energy < 0.4f*maxEnergy)
	{
		breedSum += 0.5f*DNA_SIZE;
	}

	if (other->radius*0.66f > radius || other->radius*1.5f < radius)
	{
		//return false;
	}

	if (breedSum > simVars->BREED_BASE && breedSum > maxBreed)
	{
		return true;
	}

	return false;
}

bool Organism::scaryFac(Organism* other)
{
	int breedSum = breedDiff(other);

	if (other->Aggro || other->DEFENSIVE || other->virus)
	{
		breedSum += 0.2f*DNA_SIZE;
	}

	if (other->state == ATTACK || other->state == PROTECT)
	{
		breedSum += 0.3f*DNA_SIZE;
	}

	if (other->radius*0.66f > radius || other->radius*1.5f < radius)
	{
		//return false;
	}

	if (breedSum > simVars->BREED_BASE && breedSum > maxBreed)
	{
		return true;
	}

	return false;
}

bool Organism::weirdFac(Organism* other)
{
	int breedSum = breedDiff(other);

	if (other->Aggro != Aggro)
	{
		breedSum += 0.1f*DNA_SIZE;
	}

	if (other->state == ATTACK || other->state == PROTECT)
	{
		breedSum -= 0.3f*DNA_SIZE;
	}

	if (breedSum > maxBreed)
	{
		return true;
	}

	return false;
}


bool Organism::weirdest(Organism* incumbent, Organism* challenger)
{
	if (!challenger->producer && !producer)//if you can see weidness
	{
		if (incumbent == nullptr)
		{
			bool fac = weirdFac(challenger);
			//And if you think they are weird
			if (fac)
			{
				return true;
			}
		}
		//And if some factor is biggger
		else if (vectorDistance(location, challenger->location) < vectorDistance(location, incumbent->location))
		{
			bool fac = weirdFac(challenger);

			//And if you think they are weird
			if (fac)
			{
				return true;
			}
		}
	}
	return false;
}

bool Organism::tastiest(Organism* incumbent, Organism* challenger)
{
	if (Aggro && !challenger->producer && !producer)//if you can attack
	{
		if (incumbent == nullptr)
		{
			bool fac = attackFac(challenger);
			//And if you can eat them based on similarity
			if (fac)
			{
				return true;
			}
		}
		//And if some factor is biggger
		else if (vectorDistance(location, challenger->location) < vectorDistance(location, incumbent->location))
		{
			bool fac = attackFac(challenger);

			//And if you can eat them based on similarity
			if (fac)
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
		bool fac = mateFac(challenger);
		//With other organism
		if (fac && challenger->LIFESTAGE == 1)
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
		bool fac = mateFac(challenger);
		//the other organism
		if (fac && (challenger->LIFESTAGE == 0))
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
	if (!challenger->producer && !producer)
	{
		bool fac = scaryFac(challenger);
		//to you
		if (fac)
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
	float nudge = 0.07f, edge = 0.1f;
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

	newLoc.x += velocity.x*timefactor + std::min(nudge * repel.x, radius / 2.f);
	newLoc.y += velocity.y*timefactor + std::min(nudge * repel.y, radius / 2.f);

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
	else
	{
		if (vectorDistance(desiredLocation, location) > (maxSpeed)*(1.f - 0.5f*virus))
		{
			tempVel.y *= (maxSpeed)*(1.f - 0.5f*virus) / vectorDistance(desiredLocation, location);
			tempVel.x *= (maxSpeed)*(1.f - 0.5f*virus) / vectorDistance(desiredLocation, location);
		}
	}
	float diff = 0.015f*timefactor;

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
	default:
		break;
	}

	window->draw(lines);
}