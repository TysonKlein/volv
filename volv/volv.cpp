#include "header.hpp"

int main(int argc, char* argv[])
{
	//Setting vars from command line args
	Settings settings;
	initializesettings(argc, argv, &settings);
	
	std::srand(settings.nSeed);
	sf::Font font;
	font.loadFromFile("res/dpcomic.ttf");
	sf::Text timerTxt;
	timerTxt.setFont(font);
	timerTxt.setPosition(100.f, 100.f);
	timerTxt.scale(2.f, 2.f);
	timerTxt.setString("HELLO");

	// Define some constants
	int gameWidth = float(1280);
	int gameHeight = float(720);
	float simRatio = float(settings.nWidth) / float(settings.nHeight);
	float dispRatio = float(gameWidth) / float(gameHeight);
	int framesBetweenAI = 10, framesBetweenFood = 10, currentAIframes = 0, secAI = 0, secFRAMES = 0, currentFoodFrames = 0, drawSkipFrames = 0, currentDrawFrames = 0;
	sf::Vector2f lastBarrier(-1.f, -1.f);

	// Create the window of the application
	sf::RenderWindow window;

	if (settings.bFullscreen)
		window.create(sf::VideoMode::getFullscreenModes().front(), "", sf::Style::Fullscreen);	
	else
		window.create(sf::VideoMode(gameWidth, gameHeight, 32), "", sf::Style::Default);

	sf::View overView;
	overView.setSize(settings.nWidth, settings.nHeight);
	overView.setCenter(settings.nWidth / 2, settings.nHeight / 2);

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
	LL = new linkedList*[settings.nHeight / settings.nCollisionSquareSize + 1]();

	for (int i = 0; i < settings.nHeight / settings.nCollisionSquareSize + 1; i++)
	{
		LL[i] = new linkedList[settings.nWidth / settings.nCollisionSquareSize + 1]();

		for (int j = 0; j < settings.nWidth / settings.nCollisionSquareSize + 1; j++)
		{
			LL[i][j].X = j;
			LL[i][j].Y = i;
			LL[i][j].settings = &settings;
			LL[i][j].LLrect.setSize(sf::Vector2f(settings.nCollisionSquareSize, settings.nCollisionSquareSize));
			LL[i][j].LLrect.setPosition(sf::Vector2f(settings.nCollisionSquareSize*j, settings.nCollisionSquareSize*i));
			LL[i][j].LLrect.setOutlineThickness(2);
			LL[i][j].LLrect.setFillColor(sf::Color(rand() % 20, rand() % 20, rand() % 20));
			LL[i][j].LLrect.setOutlineColor(sf::Color(200, 0, 0, 50));
			LL[i][j].drawList = &drawList;
		}
	}

	drawList.X = 0;
	drawList.Y = 0;
	drawList.settings = &settings;
	
	//Add organisms
	for (int i = 0; i < settings.nInitialNumberOfOrganisms; i++)
	{
		int DNA[DNA_SIZE];
		for (int i = 0; i < DNA_SIZE; i++)
		{
			if (i == DNA_SIZE-1)
				DNA[i] = 1 + rand() % (DNA_SIZE-1);
			else
				DNA[i] = rand() % DNA_SIZE;
		}
		Organism* org = new Organism(sf::Vector2f((rand() % (settings.nWidth - 2 * int(settings.fXbuffer))) + settings.fXbuffer, (rand() % (settings.nHeight - 2 * int(settings.fYbuffer))) + settings.fYbuffer), DNA, &settings);
		org->props.nGeneration = 0;
		LL[int(org->getLocation().y / settings.nCollisionSquareSize)][int(org->getLocation().x / settings.nCollisionSquareSize)].insert(org);
		drawList.insert(org);
	}

	//Add initial food
	if (true)
	{
		for (int i = 0; i < int(float(settings.nWidth*settings.nHeight*settings.nFoodSpawnRate) / 150000.f); i++)
		{
			if (rand() % 20 == 0)
			{
				sf::Vector2f pos((rand() % (settings.nWidth - 2 * int(settings.fXbuffer))) + settings.fXbuffer, (rand() % (settings.nHeight - 2 * int(settings.fYbuffer))) + settings.fYbuffer);
				pos = buffer(pos, &settings);
				Food* food = new Food(pos, &settings, false);
				LL[int(pos.y / settings.nCollisionSquareSize)][int(pos.x / settings.nCollisionSquareSize)].insert(food);
			}
		}
	}

	//Clocks and Timers
	sf::Clock AITimer;
	const sf::Time AITime = sf::seconds(1.0f / 300.0f);
	sf::Clock secTimer;
	const sf::Time sec = sf::seconds(1.0f);
	sf::Clock drawTimer;
	const sf::Time drawTime = sf::seconds(1.0f / 144.0f);
	sf::Clock clock;
	bool isPlaying = true;
	bool mouseDown = false;

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
				settings.bDevmode = !settings.bDevmode;
			}

			if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::U))
			{
				settings.bUnlimitedFramerate = !settings.bUnlimitedFramerate;
			}

			if ((event.type == sf::Event::MouseButtonPressed) && (event.mouseButton.button == sf::Mouse::Left))
			{
				int DNA[DNA_SIZE];
				for (int i = 0; i < DNA_SIZE; i++)
				{
					DNA[i] = rand() % DNA_SIZE;
					if (i == DNA_SIZE-1)
					{
						DNA[i] = (rand() % (DNA_SIZE - 1)) + 1;
					}
				}
				org = new Organism(sf::Vector2f(sf::Mouse::getPosition(window).x*settings.nHeight / gameHeight, sf::Mouse::getPosition(window).y*settings.nHeight / gameHeight), DNA, &settings);
				LL[int(org->getLocation().y / settings.nCollisionSquareSize)][int(org->getLocation().x / settings.nCollisionSquareSize)].insert(org);
				drawList.insert(org);
			}
			if ((event.type == sf::Event::MouseButtonPressed) && (event.mouseButton.button == sf::Mouse::Right))
			{
				mouseDown = true;
			}
			if ((event.type == sf::Event::MouseButtonReleased) && (event.mouseButton.button == sf::Mouse::Right))
			{
				mouseDown = false;
			}
		}
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		if (mouseDown)
		{
			if (vectorDistance(lastBarrier, sf::Vector2f(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y)) > 10.f)
			{
				lastBarrier = sf::Vector2f(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y);
				Barrier* bar = new Barrier(sf::Mouse::getPosition(window).x*settings.nHeight / gameHeight, sf::Mouse::getPosition(window).y*settings.nHeight / gameHeight, float(settings.nCollisionSquareSize/2), &settings);
				LL[int(bar->getLocation().y / settings.nCollisionSquareSize)][int(bar->getLocation().x / settings.nCollisionSquareSize)].insert(bar);
			}
		}
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//AI, all calculations
		if (isPlaying)
		{
			float deltaTime = clock.restart().asSeconds();

			if (secTimer.getElapsedTime() > sec)
			{
				secTimer.restart();
				secAI = 0;
				secFRAMES = 0;
			}

			if (AITimer.getElapsedTime() > AITime || settings.bUnlimitedFramerate)
			{
				settings.nTime++;
				secFRAMES++;
				AITimer.restart();

				if (framesBetweenAI == currentAIframes)
				{
					secAI++;
					for (int i = 0; i < settings.nHeight / settings.nCollisionSquareSize + 1; i++)
					{
						for (int j = 0; j < settings.nWidth / settings.nCollisionSquareSize + 1; j++)
						{
							for (orgIt = LL[i][j].organismList.begin(); orgIt != LL[i][j].organismList.end(); orgIt++)
							{
								(*orgIt)->AI((*orgIt)->props.nID, LL, float(framesBetweenAI));
							}
							
							for (std::vector<Barrier*>::iterator bit = LL[i][j].barrierList.begin(); bit != LL[i][j].barrierList.end(); bit++)
							{
								(*bit)->removeFood(LL);
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
					for (int i = 0; i < settings.nWidth * settings.nHeight * settings.nFoodSpawnRate / 500000 + 1; i++)
					{
						if (rand() % 200 == 0)
						{
							sf::Vector2f pos(settings.fXbuffer + rand() % int(settings.nWidth - 2 * settings.fXbuffer), settings.fYbuffer + rand() % int(settings.nHeight - 2 * settings.fYbuffer));
							pos = buffer(pos, &settings);
							
							Food* food = new Food(pos, &settings, false);
							LL[int(pos.y / settings.nCollisionSquareSize)][int(pos.x / settings.nCollisionSquareSize)].insert(food);
						}
					}
					currentFoodFrames = 0;
				}
				else
				{
					currentFoodFrames++;
				}

				for (int i = 0; i < settings.nHeight / settings.nCollisionSquareSize + 1; i++)
				{
					for (int j = 0; j < settings.nWidth / settings.nCollisionSquareSize + 1; j++)
					{
						for (int k = 0; k < LL[i][j].organismList.size(); k++)
						{
							if (LL[i][j].organismList[k]->props.fVitality < 0.f || LL[i][j].organismList[k]->props.nLifespan <= settings.nTime - LL[i][j].organismList[k]->props.nBirthday)
							{
								LL[i][j].kill(LL[i][j].organismList[k], LL);
							}
							else {
								LL[i][j].organismList[k]->changeVelocity(1.f);
								LL[i][j].organismList[k]->move(LL, 1.f);
							}
						}
						for (int k = 0; k < LL[i][j].organismList.size(); k++)
						{
							if (LL[i][j].organismList[k]->props.bBreed)
							{
								LL[i][j].breed(LL[i][j].organismList[k], LL);
							}
						}
					}
				}
			}
		}
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//Draw
		if (drawTimer.getElapsedTime() > drawTime)
		{
			if (currentDrawFrames < drawSkipFrames)
			{
				currentDrawFrames++;
			}
			else
			{
				currentDrawFrames = 0;
				drawTimer.restart();
				// Clear the window
				window.clear(sf::Color(0, 0, 0));

				//Draw in order -> Dev_squares, food, organisms
				if (settings.bDevmode)
				{
					for (int i = 0; i < settings.nHeight / settings.nCollisionSquareSize + 1; i++)
					{
						for (int j = 0; j < settings.nWidth / settings.nCollisionSquareSize + 1; j++)
						{
							LL[i][j].draw(&window);
						}
					}
				}

				for (int i = 0; i < settings.nHeight / settings.nCollisionSquareSize + 1; i++)
				{
					for (int j = 0; j < settings.nWidth / settings.nCollisionSquareSize + 1; j++)
					{
						LL[i][j].drawFood(&window);
						LL[i][j].drawBarrier(&window);
					}
				}
				/**/

				drawList.drawOrganisms(&window);

				std::string s = std::to_string(int(float(settings.nTime)*AITime.asSeconds())%60);
				std::string m = std::to_string(int(float(settings.nTime /60)*AITime.asSeconds())%60);
				std::string h = std::to_string(int(float(settings.nTime /3600)*AITime.asSeconds()));

				if (s.length() == 1)
				{
					s = '0' + s;
				}
				if (m.length() == 1)
				{
					m = '0' + m;
				}
				if (h.length() == 1)
				{
					h = '0' + h;
				}

				timerTxt.setString(h + ':' + m + ':' + s);
				window.draw(timerTxt);

				// Display things on screen
				window.display();

				//Check if things are taking too long to draw and skip some frames to compensate
				if (drawTimer.getElapsedTime() > drawTime*0.5f)
				{
					drawSkipFrames++;
				}
				else if (drawSkipFrames != 0 && drawTimer.getElapsedTime() < drawTime*0.25f)
				{
					drawSkipFrames--;
					if (drawSkipFrames < 0)
					{
						drawSkipFrames = 0;
					}
				}
			}
		}
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	}
	return EXIT_SUCCESS;
}