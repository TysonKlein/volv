#include "header.hpp"

int main()
{
	std::srand(static_cast<unsigned int>(std::time(NULL)));

	// Define some constants

	const int gameWidth = float(400 * WIDTH / float(HEIGHT));
	const int gameHeight = float(400);
	int framesBetweenAI = 5;
	int FOODMULT = 1;
	int currentAIframes = 0, secAI = 0, secFRAMES = 0;

	Organism* org;
	linkedList** LL;

	LL = new linkedList*[HEIGHT / collideSquareSize + 1]();

	for (int i = 0; i < HEIGHT / collideSquareSize + 1; i++)
	{
		LL[i] = new linkedList[WIDTH / collideSquareSize + 1]();

		for (int j = 0; j < WIDTH / collideSquareSize + 1; j++)
		{
			LL[i][j].X = j;
			LL[i][j].Y = i;
			LL[i][j].rect.setSize(sf::Vector2f(collideSquareSize, collideSquareSize));
			LL[i][j].rect.setPosition(sf::Vector2f(collideSquareSize*j, collideSquareSize*i));
			LL[i][j].rect.setOutlineThickness(2);
			LL[i][j].rect.setFillColor(sf::Color(rand() % 20, rand() % 20, rand() % 20));
			LL[i][j].rect.setOutlineColor(sf::Color(200, 0, 0, 50));
		}
	}

	for (int i = 0; i < NUMOFORGANISMS; i++)
	{
		int DNA[10];
		for (int i = 0; i < 10; i++)
		{
			DNA[i] = rand() % 10;
		}
		Organism* org = new Organism(sf::Vector2f((rand() % (WIDTH - 2 * int(Xbuff))) + Xbuff, (rand() % (HEIGHT - 2 * int(Ybuff))) + Ybuff), DNA);
		org->generation = 0;
		LL[int(org->location.y / collideSquareSize)][int(org->location.x / collideSquareSize)].insert(org);
	}

	//FOOD
	if (true)
	{
		for (int i = 0; i < int(float(WIDTH*HEIGHT*FOODMULT) / 150000.f); i++)
		{
			sf::Vector2f pos((rand() % (WIDTH - 2 * int(Xbuff))) + Xbuff, (rand() % (HEIGHT - 2 * int(Ybuff))) + Ybuff);
			pos = buffer(pos);
			Food* food = new Food(15 + rand() % 5, pos);
			LL[int(pos.y / collideSquareSize)][int(pos.x / collideSquareSize)].insertFood(food);
		}
	}

	// Create the window of the application
	sf::RenderWindow window(sf::VideoMode(gameWidth, gameHeight, 32), "",
		sf::Style::Default);
	sf::View overView;
	overView.setSize(WIDTH, HEIGHT);
	overView.setCenter(WIDTH / 2, HEIGHT / 2);
	overView.setViewport(sf::FloatRect(0.f, 0.f, 1.f, 1.f));
	window.setView(overView);

	// Load the text font
	sf::Font font;
	font.loadFromFile("consola.ttf");

	// Define the paddles properties
	sf::Clock AITimer;
	const sf::Time AITime = sf::seconds(1.0f / 30.0f);
	sf::Clock secTimer;
	const sf::Time sec = sf::seconds(1.0f);
	sf::Clock clock;
	bool isPlaying = true;

	while (window.isOpen())
	{
		// Handle events
		sf::Event event;
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
				DEVMODE = !DEVMODE;
			}

			if ((event.type == sf::Event::MouseButtonPressed) && (event.mouseButton.button == sf::Mouse::Left))
			{
				int DNA[10];
				for (int i = 0; i < 10; i++)
				{
					DNA[i] = rand() % 10;
					if (i == 9)
					{
						DNA[i] = 1 + rand() % 9;
					}
				}
				org = new Organism(sf::Vector2f(sf::Mouse::getPosition(window).x*HEIGHT / gameHeight, sf::Mouse::getPosition(window).y*HEIGHT / gameHeight), DNA);
				LL[int(org->location.y / collideSquareSize)][int(org->location.x / collideSquareSize)].insert(org);
			}
			if ((event.type == sf::Event::MouseButtonPressed) && (event.mouseButton.button == sf::Mouse::Right))
			{
				int DNA[10];
				for (int i = 0; i < 10; i++)
				{
					DNA[i] = rand() % 10;
					if (i == 9)
					{
						DNA[i] = 0;
					}
				}
				org = new Organism(sf::Vector2f(sf::Mouse::getPosition(window).x*HEIGHT / gameHeight, sf::Mouse::getPosition(window).y*HEIGHT / gameHeight), DNA);
				LL[int(org->location.y / collideSquareSize)][int(org->location.x / collideSquareSize)].insert(org);
			}
		}

		if (isPlaying)
		{
			float deltaTime = clock.restart().asSeconds();

			if (secTimer.getElapsedTime() > sec)
			{
				std::cout << secAI << " - " << secFRAMES << std::endl;
				secTimer.restart();
				secAI = 0;
				secFRAMES = 0;
			}

			if (AITimer.getElapsedTime() > AITime || true)
			{
				TIME++;
				secFRAMES++;
				AITimer.restart();

				if (framesBetweenAI == currentAIframes)
				{
					secAI++;
					for (int i = 0; i < HEIGHT / collideSquareSize + 1; i++)
					{
						for (int j = 0; j < WIDTH / collideSquareSize + 1; j++)
						{
							for (std::vector<Organism*>::iterator it = LL[i][j].list.begin(); it != LL[i][j].list.end(); it++)
							{
								(*it)->AI((*it)->ID, LL, float(framesBetweenAI)*AITime.asSeconds() / (2.f / 60.f));
							}
						}
					}
					currentAIframes = 0;
				}
				else
				{
					currentAIframes++;
				}

				for (int i = 0; i < WIDTH * HEIGHT * FOODMULT / 4000000 + 1; i++)
				{
					sf::Vector2f pos(Xbuff + rand() % int(WIDTH - 2 * Xbuff), Ybuff + rand() % int(HEIGHT - 2 * Ybuff));
					pos = buffer(pos);
					bool spawn = true;

					if (spawn)
					{
						Food* food = new Food(5 + rand() % 4, pos);
						LL[int(pos.y / collideSquareSize)][int(pos.x / collideSquareSize)].insertFood(food);
					}
				}

				for (int i = 0; i < HEIGHT / collideSquareSize + 1; i++)
				{
					for (int j = 0; j < WIDTH / collideSquareSize + 1; j++)
					{
						for (int k = 0; k < LL[i][j].list.size(); k++)
						{
							if (LL[i][j].list[k]->vitality < 0.f || LL[i][j].list[k]->LIFESPAN <= TIME - LL[i][j].list[k]->BORN)
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

				if (DEVMODE)
				{
					for (int i = 0; i < HEIGHT / collideSquareSize + 1; i++)
					{
						for (int j = 0; j < WIDTH / collideSquareSize + 1; j++)
						{
							LL[i][j].draw(&window);
						}
					}
				}

				for (int i = 0; i < HEIGHT / collideSquareSize + 1; i++)
				{
					for (int j = 0; j < WIDTH / collideSquareSize + 1; j++)
					{
						LL[i][j].drawFood(&window);
						LL[i][j].drawOrganisms(&window);
					}
				}

				// Display things on screen
				window.display();
			}
		}
	}

	return EXIT_SUCCESS;
}