#include "header.hpp"

int main(int argc, char* argv[])
{
	//Setting vars from command line args
	SimVars simVars;
	initializeSimVars(argc, argv, &simVars);
	
	std::srand(simVars.SEED);

	// Define some constants
	int gameWidth = float(1280);
	int gameHeight = float(720);
	float simRatio = float(simVars.WIDTH) / float(simVars.HEIGHT);
	float dispRatio = float(gameWidth) / float(gameHeight);
	int framesBetweenAI = 5, framesBetweenFood = 10, currentAIframes = 0, secAI = 0, secFRAMES = 0, currentFoodFrames = 0;

	// Create the window of the application
	sf::RenderWindow window;

	if (simVars.FULLSCREEN)
		window.create(sf::VideoMode::getFullscreenModes().front(), "", sf::Style::Fullscreen);	
	else
		window.create(sf::VideoMode(gameWidth, gameHeight, 32), "", sf::Style::Default);

	sf::View overView;
	overView.setSize(simVars.WIDTH, simVars.HEIGHT);
	overView.setCenter(simVars.WIDTH / 2, simVars.HEIGHT / 2);

	if (simRatio < dispRatio)
	{
		//Too tall to fit
		overView.setViewport(sf::FloatRect(0.5f - 0.5f*(simRatio/dispRatio), 0.f, simRatio/dispRatio, 1.f));
	}
	else
	{
		//Too wide to fit
		overView.setViewport(sf::FloatRect(0.f, 0.5f - 0.5f*(dispRatio / simRatio), 1.f, dispRatio / simRatio));
	}
	window.setView(overView);

	//Organism pointer for each new organism addition
	Organism* org;

	//LL list for collisions and logic, drawList for drawing the organisms only
	linkedList** LL;
	linkedList drawList;

	//Iterators
	std::vector<Organism*>::iterator orgIt;

	//Init both lists
	LL = new linkedList*[simVars.HEIGHT / simVars.COLLIDE_SQUARE_SIZE + 1]();

	for (int i = 0; i < simVars.HEIGHT / simVars.COLLIDE_SQUARE_SIZE + 1; i++)
	{
		LL[i] = new linkedList[simVars.WIDTH / simVars.COLLIDE_SQUARE_SIZE + 1]();

		for (int j = 0; j < simVars.WIDTH / simVars.COLLIDE_SQUARE_SIZE + 1; j++)
		{
			LL[i][j].X = j;
			LL[i][j].Y = i;
			LL[i][j].simVars = &simVars;
			LL[i][j].rect.setSize(sf::Vector2f(simVars.COLLIDE_SQUARE_SIZE, simVars.COLLIDE_SQUARE_SIZE));
			LL[i][j].rect.setPosition(sf::Vector2f(simVars.COLLIDE_SQUARE_SIZE*j, simVars.COLLIDE_SQUARE_SIZE*i));
			LL[i][j].rect.setOutlineThickness(2);
			LL[i][j].rect.setFillColor(sf::Color(rand() % 20, rand() % 20, rand() % 20));
			LL[i][j].rect.setOutlineColor(sf::Color(200, 0, 0, 50));
			LL[i][j].drawList = &drawList;
		}
	}

	drawList.X = 0;
	drawList.Y = 0;
	drawList.simVars = &simVars;

	//Add organisms
	for (int i = 0; i < simVars.INIT_NUM_ORGANISMS; i++)
	{
		int DNA[DNA_SIZE];
		for (int i = 0; i < DNA_SIZE; i++)
		{
			if (i == DNA_SIZE-1)
				DNA[i] = 1 + rand() % (DNA_SIZE-1);
			else
				DNA[i] = rand() % DNA_SIZE;
		}
		Organism* org = new Organism(sf::Vector2f((rand() % (simVars.WIDTH - 2 * int(simVars.Xbuff))) + simVars.Xbuff, (rand() % (simVars.HEIGHT - 2 * int(simVars.Ybuff))) + simVars.Ybuff), DNA, &simVars);
		org->generation = 0;
		LL[int(org->location.y / simVars.COLLIDE_SQUARE_SIZE)][int(org->location.x / simVars.COLLIDE_SQUARE_SIZE)].insert(org);
		drawList.insert(org);
	}

	//Add initial food
	if (true)
	{
		for (int i = 0; i < int(float(simVars.WIDTH*simVars.HEIGHT*simVars.FOODRATE) / 150000.f); i++)
		{
			if (rand() % 20 == 0)
			{
				sf::Vector2f pos((rand() % (simVars.WIDTH - 2 * int(simVars.Xbuff))) + simVars.Xbuff, (rand() % (simVars.HEIGHT - 2 * int(simVars.Ybuff))) + simVars.Ybuff);
				pos = buffer(pos, &simVars);
				Food* food = new Food(pos, &simVars);
				LL[int(pos.y / simVars.COLLIDE_SQUARE_SIZE)][int(pos.x / simVars.COLLIDE_SQUARE_SIZE)].insertFood(food);
			}
		}
	}

	//Clocks and Timers
	sf::Clock AITimer;
	const sf::Time AITime = sf::seconds(1.0f / 30.0f);
	sf::Clock secTimer;
	const sf::Time sec = sf::seconds(1.0f);
	sf::Clock clock;
	bool isPlaying = true;

	//Event for event handler
	sf::Event event;

	while (window.isOpen())
	{
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Handle events
		while (window.pollEvent(event))
		{
			// Window closed or escape key pressed: exit
			if ((event.type == sf::Event::Closed) ||
				((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Escape)))
			{
				window.close();
				break;
			}

			// Space key pressed: play
			if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Space))
			{
				simVars.DEVMODE = !simVars.DEVMODE;
			}

			if ((event.type == sf::Event::MouseButtonPressed) && (event.mouseButton.button == sf::Mouse::Left))
			{
				int DNA[DNA_SIZE];
				for (int i = 0; i < DNA_SIZE; i++)
				{
					DNA[i] = rand() % DNA_SIZE;
					if (i == DNA_SIZE-1)
					{
						DNA[i] = 1 + rand() % (DNA_SIZE-1);
					}
				}
				org = new Organism(sf::Vector2f(sf::Mouse::getPosition(window).x*simVars.HEIGHT / gameHeight, sf::Mouse::getPosition(window).y*simVars.HEIGHT / gameHeight), DNA, &simVars);
				LL[int(org->location.y / simVars.COLLIDE_SQUARE_SIZE)][int(org->location.x / simVars.COLLIDE_SQUARE_SIZE)].insert(org);
				drawList.insert(org);
			}
			if ((event.type == sf::Event::MouseButtonPressed) && (event.mouseButton.button == sf::Mouse::Right))
			{
				int DNA[DNA_SIZE];
				for (int i = 0; i < DNA_SIZE; i++)
				{
					DNA[i] = rand() % DNA_SIZE;
					if (i == DNA_SIZE - 1)
					{
						DNA[i] = 0;
					}
				}
				org = new Organism(sf::Vector2f(sf::Mouse::getPosition(window).x*simVars.HEIGHT / gameHeight, sf::Mouse::getPosition(window).y*simVars.HEIGHT / gameHeight), DNA, &simVars);
				LL[int(org->location.y / simVars.COLLIDE_SQUARE_SIZE)][int(org->location.x / simVars.COLLIDE_SQUARE_SIZE)].insert(org);
				drawList.insert(org);
			}
		}
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//Draw, AI, all calculations
		if (isPlaying)
		{
			float deltaTime = clock.restart().asSeconds();

			if (secTimer.getElapsedTime() > sec)
			{
				secTimer.restart();
				secAI = 0;
				secFRAMES = 0;
			}

			if (AITimer.getElapsedTime() > AITime || simVars.UNLIMIED_FRAMERATE)
			{
				simVars.TIME++;
				secFRAMES++;
				AITimer.restart();

				if (framesBetweenAI == currentAIframes)
				{
					secAI++;
					for (int i = 0; i < simVars.HEIGHT / simVars.COLLIDE_SQUARE_SIZE + 1; i++)
					{
						for (int j = 0; j < simVars.WIDTH / simVars.COLLIDE_SQUARE_SIZE + 1; j++)
						{
							for (orgIt = LL[i][j].list.begin(); orgIt != LL[i][j].list.end(); orgIt++)
							{
								(*orgIt)->AI((*orgIt)->ID, LL, float(framesBetweenAI)*AITime.asSeconds() / (2.f / 60.f));
							}
						}
					}
					currentAIframes = 0;
				}
				else
				{
					currentAIframes++;
				}

				if (framesBetweenFood == currentFoodFrames)
				{
					for (int i = 0; i < simVars.WIDTH * simVars.HEIGHT * simVars.FOODRATE / 500000 + 1; i++)
					{
						if (rand() % 20 == 0)
						{
							sf::Vector2f pos(simVars.Xbuff + rand() % int(simVars.WIDTH - 2 * simVars.Xbuff), simVars.Ybuff + rand() % int(simVars.HEIGHT - 2 * simVars.Ybuff));
							pos = buffer(pos, &simVars);
							
							Food* food = new Food(pos, &simVars);
							LL[int(pos.y / simVars.COLLIDE_SQUARE_SIZE)][int(pos.x / simVars.COLLIDE_SQUARE_SIZE)].insertFood(food);
						}
					}
					currentFoodFrames = 0;
				}
				else
				{
					currentFoodFrames++;
				}

				for (int i = 0; i < simVars.HEIGHT / simVars.COLLIDE_SQUARE_SIZE + 1; i++)
				{
					for (int j = 0; j < simVars.WIDTH / simVars.COLLIDE_SQUARE_SIZE + 1; j++)
					{
						for (int k = 0; k < LL[i][j].list.size(); k++)
						{
							if (LL[i][j].list[k]->vitality < 0.f || LL[i][j].list[k]->LIFESPAN <= simVars.TIME - LL[i][j].list[k]->BORN)
							{
								LL[i][j].kill(LL[i][j].list[k], LL);
							}
							else {
								LL[i][j].list[k]->changeVelocity(AITime.asSeconds() * 60.f);
								LL[i][j].list[k]->move(LL, AITime.asSeconds() * 60.f);
							}
						}
						for (int k = 0; k < LL[i][j].list.size(); k++)
						{
							if (LL[i][j].list[k]->BREED)
							{
								LL[i][j].breed(LL[i][j].list[k], LL);
							}
						}
					}
				}
				// Clear the window
				window.clear(sf::Color(0, 0, 0));

				//Draw in order -> Dev_squares, food, organisms
				if (simVars.DEVMODE)
				{
					for (int i = 0; i < simVars.HEIGHT / simVars.COLLIDE_SQUARE_SIZE + 1; i++)
					{
						for (int j = 0; j < simVars.WIDTH / simVars.COLLIDE_SQUARE_SIZE + 1; j++)
						{
							LL[i][j].draw(&window);
						}
					}
				}

				for (int i = 0; i < simVars.HEIGHT / simVars.COLLIDE_SQUARE_SIZE + 1; i++)
				{
					for (int j = 0; j < simVars.WIDTH / simVars.COLLIDE_SQUARE_SIZE + 1; j++)
					{
						LL[i][j].drawFood(&window);
					}
				}

				drawList.drawOrganisms(&window);

				// Display things on screen
				window.display();
			}
		}
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	}
	return EXIT_SUCCESS;
}