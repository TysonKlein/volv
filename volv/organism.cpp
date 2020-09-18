#include "header.hpp"

Organism::Organism(sf::Vector2f LOC, int newDNA[], Settings* newsettings)
{
	settings = newsettings;

	//Set DNA for organism
	for (int i = 0; i < DNA_SIZE; i++)
	{
		props.DNA[i] = newDNA[i];
	}

	//set all DNA-dictated variables
	fRadius = valFromDNA(props.DNA, 2.0f, 4.f, 628745.1386f)*valFromDNA(props.DNA, 3.0f, 4.f, 52323.3523f)*valFromDNA(props.DNA, 2.0f, 5.f, 8965245.345f);
	props.fMaxSpeed = 1.3f * sqrt(sqrt((valFromDNA(props.DNA, 8.f, 140.f, 12051.9862f))) / (fRadius * fRadius));
	props.fAttack = (5.f + valFromDNA(props.DNA, 0.f, 40.f, 161205.f))*fRadius*fRadius / 1420.f;
	props.fImmunity = valFromDNA(props.DNA, 1500.f, 10000.f, 7862387.234);
	props.fScaredness = valFromDNA(props.DNA, 0.f, 20.f, 896134.423f);
	props.fHunger = valFromDNA(props.DNA, 10.f, 40.f, 6712876.f);
	props.fMaxVitality = (valFromDNA(props.DNA, 0.f, 40.f, 123785.532f) + 10.f) * fRadius * fRadius / 15.f;
	props.fMetabolism = (valFromDNA(props.DNA, 0.f, 10.f, 58726.1235f)*0.004f)*sqrt(sqrt(props.fMaxSpeed));
	props.fMatingCooldown = valFromDNA(props.DNA, 30.f, 100.f, 815623.5196f);
	props.nLifespan = 2.f * fRadius / props.fMetabolism * (valFromDNA(props.DNA, 0.f, 30.f, 862375.123f) + 10.f);
	props.fVision = std::min(valFromDNA(props.DNA, 2.f*fRadius, 10.f*fRadius, 815623.5196f), float(settings->nCollisionSquareSize));

	bodyColor.b = valFromDNA(props.DNA, 0.f, 255.f, 642624.6643f);
	bodyColor.g = valFromDNA(props.DNA, 0.f, 255.f, 236506.5472f);
	bodyColor.r = valFromDNA(props.DNA, 0.f, 255.f, 819637.0018f);
	bodyColor.a = 160;

	outlineColor.r = valFromDNA(props.DNA, 0.f, 255.f, 926.16413f);
	outlineColor.g = valFromDNA(props.DNA, 0.f, 255.f, 1810.35235f);
	outlineColor.b = valFromDNA(props.DNA, 0.f, 255.f, 52566.102365f);
	outlineColor.a = 160;

	//Set other variables
	props.nBirthday = settings->nTime;
	props.nTimeOfNextMate = props.nBirthday;
	props.bBreed = false;
	props.bLeader = false;
	props.fBreed = DNA_SIZE;
	props.nLifestage = 0;
	props.fRoamAngle = float(rand() % 10000) / 10000.f / 3.14f * 180.f;
	props.bIncreaseHappiness = false;
	props.bVirus = false;
	props.nMaxBreedingDifference = DNA_SIZE;
	location = LOC;
	desiredLocation = location;
	props.fRotation = 0.f;
	repel.x = 0.f;
	repel.y = 0.f;
	props.fHappiness = 0.f;
	props.bKilled = false;
	props.fVitality = props.fMaxVitality;
	props.fMaxEnergy = props.fMaxVitality;
	props.fEnergy = 0.5f* props.fMaxEnergy;
	props.bProducer = false;
	
	//Determine organism characteristics from these variables
	if (props.fScaredness > 5.f)
	{
		props.bDefensive = true;
		props.fAttack *= 1.5f;
		props.fMaxVitality *= 1.3f;
	}
	else
		props.bDefensive = false;

	if (valFromDNA(props.DNA, 0.f, 25.f, 78183.43f) > 20.f) //Set Aggro
	{
		props.bAggro = true;
		props.nDisplayType = 1;
		props.fMaxVitality *= 1.5f;
		props.fMaxSpeed *= 1.5f;
		props.fImmunity /= 5.f;
	}
	else
	{
		props.bAggro = false;
		props.fAttack /= 2.f;
		props.nDisplayType = 0;
	}

	if (props.DNA[DNA_SIZE-1] == 0)
	{
		fRadius *= 1.7;
		bodyColor.g = 230;
		bodyColor.r = 230;
		bodyColor.b = 230;
		props.fMetabolism = 0;
		props.fMaxSpeed *= 0.01f;
		props.bProducer = true;
		props.nDisplayType = 0;
	}
	if (props.bProducer)
	{
		props.nLifespan *= 10000;
	}

	setBody();

	props.nID = settings->nCurrentOrganism;
	settings->nCurrentOrganism++;
}

void Organism::makeSick(Organism* other)
{
	for (int i = 0; i < DNA_SIZE; i++)
	{
		other->props.virusDNA[i] = props.virusDNA[i];
	}

	other->circ.setFillColor(sf::Color(valFromDNA(props.DNA, 0.f, 255.f, 642624.6643f),valFromDNA(props.DNA, 0.f, 255.f, 236506.5472f),valFromDNA(props.DNA, 0.f, 255.f, 819637.0018f)));
	other->circ.setRadius(other->fRadius*0.25f);
	other->circ.setOutlineThickness(2);
	other->circ.setOutlineColor(sf::Color(valFromDNA(props.DNA, 0.f, 255.f, 2934.f), valFromDNA(props.DNA, 0.f, 255.f, 10983784.f), valFromDNA(props.DNA, 0.f, 255.f, 1234.f)));
	other->circ.setOrigin(sf::Vector2f(other->fRadius*0.25f, other->fRadius*0.25f));

	other->props.bVirus = true;
}

void Organism::setBody()
{
	// resize it to 5 points
	float h, w;

	w = fRadius * 1.7f;
	h = w;

	int N = int(valFromDNA(props.DNA, 0.f, 6.f, 3122.f)) + 2;

	if (props.nDisplayType == 0)
	{
		basicBody.setFillColor(bodyColor);
		basicBody.setRadius(fRadius);
		if (!props.bProducer)
		{
			basicBody.setPointCount(int(valFromDNA(props.DNA, 0.f, 50.f, 62341.f)) / 10 + 3);
		}
		basicBody.setOrigin(fRadius, fRadius);
		basicBody.setPosition(location);
		basicBody.setOutlineColor(outlineColor);
		basicBody.setOutlineThickness(valFromDNA(props.DNA, 0.f, 6.f, 9237.f) / 2 + 1.f);
	}
	else
	{
		convexBody.setPointCount(2 * N);

		// define the points

		for (int i = 0; i < N; i++)
		{
			convexBody.setPoint(i, sf::Vector2f(i*h / float(N - 1), w / 2.f + float(props.DNA[i % DNA_SIZE] + 0.5*DNA_SIZE) / (1.5f*DNA_SIZE) * w / 2.f));
			convexBody.setPoint(2 * N - i - 1, sf::Vector2f(i*h / float(N - 1), w / 2.f - float(props.DNA[i % DNA_SIZE] + 0.5*DNA_SIZE) / (1.5f*DNA_SIZE) * w / 2.f));
		}

		convexBody.setOrigin(h / 2.f, w / 2.f);
		convexBody.setFillColor(bodyColor);
		convexBody.setPosition(location);
		convexBody.setOutlineColor(outlineColor);
		convexBody.setOutlineThickness(valFromDNA(props.DNA, 0.f, 6.f, 9237.f) / 2 + 1.f);
	}
}

float Organism::getLikability(int DNA2[])
{
	int total = 0;
	for (int i = 0; i < 0.4f*DNA_SIZE; i++)
	{
		if (abs(DNA2[i] - props.DNA[i]) > 0.3f*DNA_SIZE)
		{
			total -= abs(DNA2[i] - props.DNA[i]);
		}
		else
		{
			total+= 0.3f*float(DNA_SIZE);
		}
	}
	return float(total) * 10.f / 0.4f*float(DNA_SIZE);
}

int Organism::breedDiff(Organism* other)
{
	int breedVar = 0;
	for (int i = 0; i < DNA_SIZE; i++)
	{
		if (props.DNA[i] != other->props.DNA[i])
			breedVar++;
	}
	return breedVar;
}

void Organism::AI(int me, linkedList** LL, float timeFactor)
{
	AIprops.reset();
	props.bIncreaseHappiness = false;
	props.bHungry = false;
	props.bKilled = false;

	if (!(props.fEnergy > props.fMaxEnergy* props.fHunger / 45.f))
	{
		props.bHungry = true;
	}

	Organism* org;
	props.org_scariest = nullptr;
	props.org_youngest = nullptr;
	props.org_sexiest = nullptr;
	props.org_tastiest = nullptr;
	props.org_weirdest = nullptr;
	props.foo_closest = nullptr;

	repel = sf::Vector2f(0, 0);
	sf::Vector2f foodDir, GO_TO = location;

	props.fEnergy -= props.fMetabolism * timeFactor;
	if (props.bVirus)
	{
		props.fEnergy -= 100.f* props.fMetabolism*timeFactor;
	}
	if (props.fEnergy < 0.f)
	{
		props.fEnergy = 0.f;
	}

	if (props.fEnergy < 0.0001f)
	{
		props.fVitality -= 20 * props.fMetabolism*timeFactor;
	}
	else
	{
		props.fVitality += props.fMetabolism * timeFactor;
	}
	if (props.fVitality > props.fMaxVitality)
	{
		props.fVitality = props.fMaxVitality;
	}

	props.fHappiness -= 0.5f*timeFactor;
	if (props.fHappiness < 0)
	{
		props.fHappiness = 0;
	}

	if (props.bProducer) {
		for (int i = 0; i < props.fMaxVitality*0.5f / (15 + props.DNA[2]); i++)
		{
			if (rand() % (400) == 0)
			{
				float angle = float(rand() % 10000) / 10000.f / 3.14f * 180.f;
				float distance = 2.2f*fRadius + float(rand() % 100);

				sf::Vector2f pos(location.x + std::cosf(angle)*distance, location.y + std::sinf(angle)*distance);
				pos = buffer(pos, settings);

				Food* food = new Food(pos, settings, false);
				food->settings = settings;

				LLfromArray(LL, pos, settings, 0, 0)->insert(food);
				props.fEnergy -= 0.03f*timeFactor;
				bodyColor.a = 255.f* props.fEnergy*2.f / props.fMaxEnergy;
				basicBody.setFillColor(bodyColor);
			}
		}
	}

	if (props.nLifestage < 2)
	{
		if (props.nLifestage == 0 && settings->nTime - props.nBirthday > props.nLifespan*0.05f)
		{
			props.nLifestage = 1;
			bodyColor.b = valFromDNA(props.DNA, 0.f, 215.f, 642624.6643f) + 40.f;
			bodyColor.g = valFromDNA(props.DNA, 0.f, 215.f, 236506.5472f) + 40.f;
			bodyColor.r = valFromDNA(props.DNA, 0.f, 215.f, 819637.0018f) + 40.f;

			outlineColor.r = valFromDNA(props.DNA, 0.f, 215.f, 926.16413f) + 40.f;
			outlineColor.g = valFromDNA(props.DNA, 0.f, 215.f, 1810.35235f) + 40.f;
			outlineColor.b = valFromDNA(props.DNA, 0.f, 215.f, 52566.102365f) + 40.f;

			fRadius = fRadius * 1.5f;

			props.fMaxSpeed = props.fMaxSpeed / 1.2f;

			setBody();
		}
		else if (props.nLifestage == 1 && settings->nTime - props.nBirthday > props.nLifespan*0.8f)
		{
			props.nLifestage = 2;
			bodyColor.b = valFromDNA(props.DNA, 0.f, 100.f, 642624.6643f) + 140.f;
			bodyColor.g = valFromDNA(props.DNA, 0.f, 100.f, 236506.5472f) + 140.f;
			bodyColor.r = valFromDNA(props.DNA, 0.f, 100.f, 819637.0018f) + 140.f;

			outlineColor.r = valFromDNA(props.DNA, 0.f, 100.f, 926.16413f) + 140.f;
			outlineColor.g = valFromDNA(props.DNA, 0.f, 100.f, 1810.35235f) + 140.f;
			outlineColor.b = valFromDNA(props.DNA, 0.f, 100.f, 52566.102365f) + 140.f;

			props.fMaxSpeed = props.fMaxSpeed / 1.2f;

			setBody();
		}
	}

	checkFoodVicinity(0, 0, LL);

	if (props.foo_closest != nullptr)
	{
		AIprops.bFoundFood = true;
	}
	linkedList* l;
	for (int x = -1; x < 2; x++)
	{
		for (int y = -1; y < 2; y++)
		{
			if (isInBounds(location, settings, x, y))
			{
				l = LLfromArray(LL, location, settings, x, y);
				for (orgIT = l->organismList.begin(); orgIT != l->organismList.end(); orgIT++)
				{
					if (vectorDistance((*orgIT)->location, location) < props.fVision && props.nID != (*orgIT)->props.nID)//If it is a different organism
					{
						///////////////////////////////////////////////////////////////////
						//INVOLUNTARY CONTACT CONSEQUENCES (viruses, repelling, breeding values)
						///////////////////////////////////////////////////////////////////
						AIprops.collideFactor = Collides((*orgIT), this);

						if (AIprops.collideFactor > 0.0001f) //If the organism is colliding
						{
							if (!(*orgIT)->props.bProducer && !props.bProducer && props.bVirus && !(*orgIT)->props.bVirus) //If I have a virus and other doesnt
							{
								AIprops.virusSum = 0;
								for (int i = 0; i < DNA_SIZE; i++)
								{
									if (props.virusDNA[i] == (*orgIT)->props.DNA[i])
									{
										AIprops.virusSum++;
									}
								}

								if (AIprops.virusSum > 0.5f*DNA_SIZE) //if DNA is similar enough
								{
									AIprops.virusSum += 1;
									if (rand() % ((DNA_SIZE - AIprops.virusSum)*(DNA_SIZE - AIprops.virusSum) / 2 + 1) == 0)
									{
										makeSick(*orgIT);
									}
								}
							}

							//Repel from close organism
							float radFac = (*orgIT)->fRadius / fRadius;
							radFac = radFac * radFac*radFac*radFac;

							if (radFac > 1.f)
								radFac = 1.f;

							repel.x -= AIprops.collideFactor * ((*orgIT)->location.x - location.x)*radFac;
							repel.y -= AIprops.collideFactor * ((*orgIT)->location.y - location.y)*radFac;

							similarityUpdate((*orgIT));
						}
						///////////////////////////////////////////////////////////////////
						///////////////////////////////////////////////////////////////////


						///////////////////////////////////////////////////////////////////
						//DETERMINING THE ACTIONABLE ORGANISMS, IF ANY
						///////////////////////////////////////////////////////////////////

						//Determine the tastiest organism
						if (tastiest(props.org_tastiest, (*orgIT)))
						{
							props.org_tastiest = (*orgIT);
						}
						//Determine the sexiest organism
						if (sexiest(props.org_sexiest, (*orgIT)))
						{
							props.org_sexiest = (*orgIT);
						}
						//Determine the youngest organism
						if (youngest(props.org_youngest, (*orgIT)))
						{
							props.org_youngest = (*orgIT);
						}
						//Determine the scariest organism
						if (scariest(props.org_scariest, (*orgIT)))
						{
							props.org_scariest = (*orgIT);
						}
						//Determine the weirdest organism
						if (weirdest(props.org_weirdest, (*orgIT)))
						{
							props.org_weirdest = (*orgIT);
						}
						///////////////////////////////////////////////////////////////////
						///////////////////////////////////////////////////////////////////

						if (!AIprops.bFoundFood && x != 0 && y != 0)
						{
							checkFoodVicinity(x, y, LL);

							if (props.foo_closest != nullptr)
							{
								AIprops.bFoundFood = true;
							}
						}
					}
				}
				for (barrierIT = LLfromArray(LL, location, settings, x, y)->barrierList.begin(); barrierIT != LLfromArray(LL, location, settings, x, y)->barrierList.end(); barrierIT++)
				{
					AIprops.collideFactor = Collides((*barrierIT), this);

					if (AIprops.collideFactor > 0.0001f) //If the organism is colliding
					{
						//Repel
						repel.x -= 2.f* AIprops.collideFactor * ((*barrierIT)->getLocation().x - location.x);
						repel.y -= 2.f* AIprops.collideFactor * ((*barrierIT)->getLocation().y - location.y);
					}
				}
			}
		}
	}

	///////////////////////////////////////////////////////////////////
	//DETERMINING THE BEHAVIOUR FOR THIS ORGANISM (STATE, see flow chart)
	///////////////////////////////////////////////////////////////////

	//IS OFFSPRING SAFE AND AM I NOT OFFSPRING?
	if (props.org_youngest != nullptr && props.org_scariest != nullptr && props.org_youngest != props.org_scariest && props.nLifestage != 0 && int(props.fScaredness)%2 == 0)
	{
		//NO
		state = PROTECT;
	}
	else
	{
		//YES
		//AM I DEFENSIVE?
		if (props.bDefensive && props.org_weirdest != nullptr)
			state = DEFEND;
		else if (props.org_scariest != nullptr)
		{
			state = FLEE;
		}
		else
		{
			//YES
			//AM I HUNGRY?
			if (!props.bHungry)
			{
				//NO
				//AM I NEAR SOMEONE SIMILAR?
				if (props.org_sexiest != nullptr && props.nTimeOfNextMate < settings->nTime)
				{
					//YES
					//AM I YOUNG?
					if (props.nLifestage == 1)
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
				if (props.foo_closest != nullptr)
				{
					//YES
					//AM I SICK?
					if (props.bVirus)
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
					if (props.bAggro && props.org_tastiest != nullptr && props.bHungry)
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
		AIprops.collideFactor = Collides(props.foo_closest, this);

		if (!eatFood(AIprops.collideFactor, LL))
		{
			//EAT HERBO
			if (!props.bAggro && !props.foo_closest->bMeat)
			{
				GO_TO = props.foo_closest->getLocation();
			}

			//EAT AGGRO
			else if (props.bAggro && props.foo_closest->bMeat)
			{
				GO_TO = props.foo_closest->getLocation();
			}
		}
	}

	//FLEEING
	if (state == FLEE)
	{
		GO_TO = sf::Vector2f(location - (props.org_scariest->getLocation() - location));
	}

	//DEFENDING
	if (state == DEFEND)
	{
		AIprops.collideFactor = Collides(props.org_weirdest, this);

		if (AIprops.collideFactor > 0.0001f)
		{
			if (props.nLifestage == 1)
			{
				props.org_weirdest->props.fVitality -= props.fAttack;
				props.org_weirdest->props.bKilled = true;
			}
			else
			{
				props.org_weirdest->props.fVitality -= 0.3f* props.fAttack;
				props.org_weirdest->props.bKilled = true;
			}
		}

		float distance = 2.f*(fRadius+ props.org_weirdest->getRadius());
		sf::Vector2f vec = sf::Vector2f(props.org_weirdest->getLocation() - location);
		float between = sqrt(vec.x*vec.x + vec.y*vec.y);

		GO_TO = props.org_weirdest->getLocation()- vec * 1.1f;
	}

	//PROTECTING
	if (state == PROTECT)
	{
		AIprops.collideFactor = Collides(props.org_scariest, this);

		if (AIprops.collideFactor > 0.0001f)
		{
			if (props.nLifestage == 1)
			{
				props.org_scariest->props.fVitality -= props.fAttack;
				props.org_scariest->props.bKilled = true;
			}
			else
			{
				props.org_scariest->props.fVitality -= 0.3f* props.fAttack;
				props.org_scariest->props.bKilled = true;
			}
		}

		GO_TO = sf::Vector2f((props.org_scariest->getLocation() + props.org_youngest->getLocation()) / 2.f);
	}

	//ATTACKING
	if (state == ATTACK)
	{
		AIprops.collideFactor = Collides(props.org_tastiest, this);

		if (AIprops.collideFactor > 0.0001f)
		{
			if (props.nLifestage == 1)
			{
				props.org_tastiest->props.fVitality -= props.fAttack * timeFactor;
				props.org_tastiest->props.bKilled = true;
			}
			else
			{
				props.org_tastiest->props.fVitality -= 0.3f* props.fAttack*timeFactor;
				props.org_tastiest->props.bKilled = true;
			}
		}

		GO_TO = props.org_tastiest->getLocation();
	}

	//MATING
	if (state == MATE)
	{
		GO_TO = props.org_sexiest->location;

		AIprops.collideFactor = Collides(props.org_sexiest, this);

		if (AIprops.collideFactor > 0.0001f)
		{
			if (!props.bIncreaseHappiness)
			{
				props.fHappiness += 6.5f*timeFactor;
				props.bIncreaseHappiness = true;
			}

			if (props.fHappiness < 0)
			{
				props.fHappiness = 0;
			}

			if (props.fHappiness > 110)
			{
				props.fHappiness = 110;
			}

			if (props.fHappiness > 100)
			{
				props.nTimeOfNextMate = settings->nTime + props.fMatingCooldown*timeFactor;
				props.bBreed = true;
				props.fEnergy *= 0.4f;
				for (int n = 0; n < DNA_SIZE; n++)
				{
					props.breedDNA[n] = props.org_sexiest->props.DNA[n];
				}
				breedLoc = props.org_sexiest->location;
				props.org_sexiest->props.fHappiness = 0;
				props.fHappiness = 0;
			}
		}
	}

	//WANDERING
	if (state == WANDER)
	{
		if (props.bLeader)
		{
			props.fRoamAngle += float((rand() % 9) - 4)*0.02f;
			if (props.fRoamAngle < 0.f)
			{
				props.fRoamAngle += 180.f / M_PI;
			}
			if (props.fRoamAngle > 180.f / M_PI)
			{
				props.fRoamAngle -= 180.f / M_PI;
			}

			if (props.foo_closest != nullptr)
			{
				AIprops.collideFactor = Collides(props.foo_closest, this);
				eatFood(AIprops.collideFactor, LL);
			}

			GO_TO = sf::Vector2f(location.x + std::cosf(props.fRoamAngle)*60.f / 2.f, location.y + std::sinf(props.fRoamAngle)*60.f / 2.f);
		}
		else
		{
			props.fRoamAngle += float((rand() % 9) - 4)*0.1f;
			if (props.fRoamAngle < 0.f)
			{
				props.fRoamAngle += 180.f / M_PI;
			}
			if (props.fRoamAngle > 180.f / M_PI)
			{
				props.fRoamAngle -= 180.f / M_PI;
			}
			GO_TO = sf::Vector2f(location.x + std::cosf(props.fRoamAngle)*100.f / 2.f, location.y + std::sinf(props.fRoamAngle)*100.f / 2.f);
		}
	}

	//FOLLOWING
	if (state == FOLLOW)
	{
		if (props.foo_closest != nullptr)
		{
			AIprops.collideFactor = Collides(props.foo_closest, this);
			eatFood(AIprops.collideFactor, LL);
		}

		GO_TO = props.org_sexiest->location;
	}

	///////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////
	props.bLeader = false;
	changeDesiredLocation(GO_TO);
}

void Organism::checkFoodVicinity(int x, int y, linkedList** LL)
{
	if (props.bHungry)
	{
		int k = 0;
		///////////////////////////////////////////////////////////////////
		//DETERMINING THE ACTIONABLE FOOD, IF ANY
		///////////////////////////////////////////////////////////////////
		if (props.bAggro)
		{
			for (foodIT = LLfromArray(LL, location, settings, 0, 0)->meatFoodList.begin(); foodIT != LLfromArray(LL, location, settings, 0, 0)->meatFoodList.end(); foodIT++)
			{
				if (vectorDistance((*foodIT)->getLocation(), location) < props.fVision)
				{
					if (props.foo_closest == nullptr || closest(props.foo_closest, (*foodIT)))
					{
						props.foo_closest = (*foodIT);
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
			for (foodIT = LLfromArray(LL, location, settings, 0, 0)->plantFoodList.begin(); foodIT != LLfromArray(LL, location, settings, 0, 0)->plantFoodList.end(); foodIT++)
			{
				if (vectorDistance((*foodIT)->getLocation(), location) < props.fVision)
				{
					if (props.foo_closest == nullptr || closest(props.foo_closest, (*foodIT)))
					{
						props.foo_closest = (*foodIT);
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
	if (collideFactor > 0.1f)
	{
		if (props.fMaxEnergy - props.fEnergy > props.foo_closest->value || state == FOOD)
		{
			if (!props.bProducer && !props.bVirus)
			{
				if (props.bAggro && props.foo_closest->bMeat)
				{
					props.fEnergy += props.foo_closest->value;
					props.foo_closest->Eat(LL);
					if (rand() % int(props.fImmunity) == 0)
					{
						props.bVirus = true;
						for (int i = 0; i < DNA_SIZE; i++)
						{
							props.virusDNA[i] = props.DNA[i];
						}
						circ.setFillColor(sf::Color(valFromDNA(props.DNA, 0.f, 255.f, 642624.6643f), valFromDNA(props.DNA, 0.f, 255.f, 236506.5472f), valFromDNA(props.DNA, 0.f, 255.f, 819637.0018f)));
						circ.setRadius(fRadius*0.25f);
						circ.setOutlineThickness(2);
						circ.setOutlineColor(sf::Color(valFromDNA(props.DNA, 0.f, 255.f, 2934.f), valFromDNA(props.DNA, 0.f, 255.f, 10983784.f), valFromDNA(props.DNA, 0.f, 255.f, 1234.f)));
						circ.setOrigin(sf::Vector2f(fRadius*0.25f, fRadius*0.25f));
					}
				}
				else if (!props.bAggro && !props.foo_closest->bMeat)
				{
					props.fEnergy += props.foo_closest->value;
					props.foo_closest->Eat(LL);
					if (rand() % int(props.fImmunity) == 0)
					{
						props.bVirus = true;
						for (int i = 0; i < DNA_SIZE; i++)
						{
							props.virusDNA[i] = props.DNA[i];
						}
						circ.setFillColor(sf::Color(valFromDNA(props.DNA, 0.f, 255.f, 642624.6643f), valFromDNA(props.DNA, 0.f, 255.f, 236506.5472f), valFromDNA(props.DNA, 0.f, 255.f, 819637.0018f)));
						circ.setRadius(fRadius*0.25f);
						circ.setOutlineThickness(2);
						circ.setOutlineColor(sf::Color(valFromDNA(props.DNA, 0.f, 255.f, 2934.f), valFromDNA(props.DNA, 0.f, 255.f, 10983784.f), valFromDNA(props.DNA, 0.f, 255.f, 1234.f)));
						circ.setOrigin(sf::Vector2f(fRadius*0.25f, fRadius*0.25f));
					}
				}
			}
			if (props.bProducer)
				props.foo_closest->Eat(LL);

			if (props.fEnergy > props.fMaxEnergy)
				props.fEnergy = props.fMaxEnergy;
			return true;
		}
	}
	return false;
}

int Organism::similarityUpdate(Organism* other)
{
	int breedSum = breedDiff(other);
	if (breedSum < settings->nMimimumBreedFactor && breedSum <= props.nMaxBreedingDifference + 0.1f*DNA_SIZE && breedSum >= 0)
	{
		props.nMaxBreedingDifference = breedSum + 0.1f*DNA_SIZE;
	}
	return breedSum;
}

bool Organism::mateFac(Organism* other)
{
	int breedSum = breedDiff(other);

	if (breedSum < settings->nMimimumBreedFactor && breedSum <= props.nMaxBreedingDifference)
	{
		return true;
	}

	return false;
}

bool Organism::attackFac(Organism* other)
{
	int breedSum = breedDiff(other);

	if (props.fEnergy < 0.4f* props.fMaxEnergy)
	{
		breedSum += 0.5f*DNA_SIZE;
	}

	if (breedSum > settings->nMimimumBreedFactor && breedSum > props.fBreed)
	{
		return true;
	}

	return false;
}

bool Organism::scaryFac(Organism* other)
{
	int breedSum = breedDiff(other);

	if (other->props.bAggro || other->props.bDefensive || other->props.bVirus)
	{
		breedSum += 0.2f*DNA_SIZE;
	}

	if (other->state == ATTACK || other->state == PROTECT)
	{
		breedSum += 0.3f*DNA_SIZE;
	}
	
	if (breedSum > settings->nMimimumBreedFactor && breedSum > props.fBreed && (!props.bAggro || props.fVitality/props.fMaxVitality < 0.7f))
	{
		return true;
	}

	return false;
}

bool Organism::weirdFac(Organism* other)
{
	int breedSum = breedDiff(other);

	if (other->props.bAggro != props.bAggro)
	{
		breedSum += 0.1f*DNA_SIZE;
	}

	if (other->state == ATTACK || other->state == PROTECT)
	{
		breedSum -= 0.3f*DNA_SIZE;
	}

	if (breedSum > props.fBreed)
	{
		return true;
	}

	return false;
}


bool Organism::weirdest(Organism* incumbent, Organism* challenger)
{
	if (!challenger->props.bProducer && !props.bProducer)//if you can see weidness
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
		else if (vectorDistanceSQ(location, challenger->location) < vectorDistanceSQ(location, incumbent->location))
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
	if (props.bAggro && !challenger->props.bProducer && !props.bProducer)//if you can attack
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
		else if (vectorDistanceSQ(location, challenger->location) < vectorDistanceSQ(location, incumbent->location))
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
	if (!challenger->props.bProducer && !props.bProducer && !props.bVirus)//if you can mate
	{
		bool fac = mateFac(challenger);
		//With other organism
		if (fac && challenger->props.nLifestage == 1)
		{
			if (incumbent == nullptr)
			{
				return true;
			}

			//And they are closest
			if (vectorDistanceSQ(location, challenger->location) < vectorDistanceSQ(location, incumbent->location))
			{
				return true;
			}
		}
	}
	return false;
}
bool Organism::youngest(Organism* incumbent, Organism* challenger)
{
	if (!challenger->props.bProducer && !props.bProducer)//if you are able to protect
	{
		bool fac = mateFac(challenger);
		//the other organism
		if (fac && (challenger->props.nLifestage == 0))
		{
			if (incumbent == nullptr)
			{
				return true;
			}
			//And they are closest
			else if (vectorDistanceSQ(location, challenger->location) < vectorDistanceSQ(location, incumbent->location))
			{
				return true;
			}
		}
	}
	return false;
}
bool Organism::scariest(Organism* incumbent, Organism* challenger)
{
	if (!challenger->props.bProducer && !props.bProducer)
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
			else if (vectorDistanceSQ(location, challenger->location) < vectorDistanceSQ(location, incumbent->location))
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
	if (vectorDistanceSQ(location, challenger->getLocation()) < vectorDistanceSQ(location, incumbent->getLocation()))
	{
		return true;
	}
	return false;
}

void Organism::move(linkedList** LL, float timefactor)
{
	float nudge = 0.02f, edge = 0.9f;
	bool edgeCase = false;
	if (props.bProducer)
	{
		nudge = 0.00007f;
	}

	if (location.x < settings->fXbuffer)
	{
		velocity.x += (settings->fXbuffer - location.x)*edge;
		edgeCase = true;
	}
	if (location.y < settings->fYbuffer)
	{
		velocity.y += (settings->fYbuffer - location.y)*edge;
		edgeCase = true;
	}
	if (location.x > settings->nWidth - settings->fXbuffer)
	{
		velocity.x -= (settings->fXbuffer - (settings->nWidth - location.x))*edge;
		edgeCase = true;
	}
	if (location.y > settings->nHeight - settings->fYbuffer)
	{
		velocity.y -= (settings->fYbuffer - (settings->nHeight - location.y))*edge;
		edgeCase = true;
	}

	if (edgeCase)
	{
		props.fRoamAngle = std::atan2f(velocity.y, velocity.x);
	}

	newLoc = location;

	newLoc.x += velocity.x*timefactor + std::min(nudge * repel.x, fRadius / 2.f);
	newLoc.y += velocity.y*timefactor + std::min(nudge * repel.y, fRadius / 2.f);

	if (newLoc.x - fRadius < 0)
	{
		newLoc.x = fRadius;
	}
	if (newLoc.y - fRadius < 0)
	{
		newLoc.y = fRadius;
	}
	if (newLoc.x + fRadius > settings->nWidth)
	{
		newLoc.x = settings->nWidth - fRadius;
	}
	if (newLoc.y + fRadius > settings->nHeight)
	{
		newLoc.y = settings->nHeight - fRadius;
	}

	if (location.x / settings->nCollisionSquareSize != (newLoc.x) / settings->nCollisionSquareSize || location.y / settings->nCollisionSquareSize != (newLoc.y) / settings->nCollisionSquareSize)
	{
		LL[int(location.y / settings->nCollisionSquareSize)][int(location.x / settings->nCollisionSquareSize)].remove(this);
		LL[int(newLoc.y / settings->nCollisionSquareSize)][int(newLoc.x / settings->nCollisionSquareSize)].insert(this);
	}

	location = newLoc;
	bool neg = false;
	if (velocity.x < 0)
	{
		neg = true;
	}

	props.fRotation = 90.f + 180.0f*atanf((velocity.y) / (velocity.x)) / M_PI + 180.f*neg;

	if (props.nDisplayType == 0)
	{
		basicBody.setRotation(props.fRotation);
		basicBody.setPosition(location);
	}
	else
	{
		convexBody.setRotation(props.fRotation - 90.f);
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
		if (vectorDistance(desiredLocation, location) > (props.fMaxSpeed*1.7f)*(1.f - 0.5f*props.bVirus))
		{
			tempVel.y *= (props.fMaxSpeed *1.7f)*(1.f - 0.5f* props.bVirus) / vectorDistance(desiredLocation, location);
			tempVel.x *= (props.fMaxSpeed *1.7f)*(1.f - 0.5f* props.bVirus) / vectorDistance(desiredLocation, location);
		}
	}
	else if (state == FLEE)
	{
		if (vectorDistance(desiredLocation, location) > (props.fMaxSpeed *2.f)*(1.f - 0.5f* props.bVirus))
		{
			tempVel.y *= (props.fMaxSpeed *2.f)*(1.f - 0.5f* props.bVirus) / vectorDistance(desiredLocation, location);
			tempVel.x *= (props.fMaxSpeed *2.f)*(1.f - 0.5f* props.bVirus) / vectorDistance(desiredLocation, location);
		}
	}
	else
	{
		if (vectorDistance(desiredLocation, location) > (props.fMaxSpeed)*(1.f - 0.5f* props.bVirus))
		{
			tempVel.y *= (props.fMaxSpeed)*(1.f - 0.5f* props.bVirus) / vectorDistance(desiredLocation, location);
			tempVel.x *= (props.fMaxSpeed)*(1.f - 0.5f* props.bVirus) / vectorDistance(desiredLocation, location);
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
	if (props.nDisplayType == 0)
	{
		window->draw(basicBody);
	}
	else
	{
		window->draw(convexBody);
	}

	if (props.bVirus)
	{
		circ.setPosition(location);
		window->draw(circ);
	}
	if (props.bLeader)
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

sf::Vector2f Organism::getBreedLocation()
{
	return breedLoc;
}
