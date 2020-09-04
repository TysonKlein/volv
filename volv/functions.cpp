#include "header.hpp"

linkedList* LLfromArray(linkedList** LL, sf::Vector2f vec, Settings* settings, int nTileOffsetX, int nTileOffsetY)
{
	return &(LL[int(vec.y / settings->nCollisionSquareSize) + nTileOffsetY][int(vec.x / settings->nCollisionSquareSize) + nTileOffsetX]);
}

float vectorDistance(sf::Vector2f V1, sf::Vector2f V2)
{
	float x, y;
	x = V1.x - V2.x;
	y = V1.y - V2.y;
	return sqrt(x * x + y * y);
}

float vectorDistanceSQ(sf::Vector2f V1, sf::Vector2f V2)
{
	float x, y;
	x = V1.x - V2.x;
	y = V1.y - V2.y;
	return (x * x + y * y);
}

float valFromDNA(int DNA[], float min, float max, float seed)
{
	float retVal = min;
	int A, iterations = int(seed * 33) % 4 + 1;
	for (int i = 0; i < iterations; i++)
	{
		A = abs(int(seed*52934.2444f + float(int(i*seed*22.22f)%31)*seed*23.4f) % DNA_SIZE);

		if (int(seed*982.98f + float(i)*seed*89.14f) % 11 > 5)
		{
			if (int(seed*223.33f + i*938.f) % 21 > 10)
				retVal += (235523.957f)*1.f / (float(iterations)) * float(DNA[A])/float(DNA_SIZE-1);
			else
				retVal += (632543.6346f)*1.f / (float(iterations)) * float(DNA_SIZE - 1 - DNA[A]) / float(DNA_SIZE - 1);
		}
		else
		{
			if (int(seed*223.33f + i * 938.f) % 21 > 10)
				retVal += (235353.3664f)*1.f / (float(iterations)) * float(DNA[DNA[A]]) / float(DNA_SIZE - 1);
			else
				retVal += (91345.19991f)*1.f / (float(iterations)) * float(DNA_SIZE - 1 - DNA[DNA[A]]) / float(DNA_SIZE - 1);
		}
	}
	return abs(float(int(retVal) % int(max-min)) + min);
}

float Collides(sf::Vector2f p1, sf::Vector2f p2, float r1, float r2)
{
	if (vectorDistance(p1, p2) < r1 + r2)
	{
		float m = (r1 + r2 - vectorDistance(p1, p2)) / (r1 + r2);
		return m;
	}
	return 0.0f;
}

sf::Vector2f buffer(sf::Vector2f pos, Settings* settings)
{
	sf::Vector2f newPos = pos;
	if (pos.x < settings->fXbuffer)
	{
		newPos.x = settings->fXbuffer;
	}
	else if (pos.x > settings->nWidth - settings->fXbuffer)
	{
		newPos.x = settings->nWidth - settings->fXbuffer;
	}
	if (pos.y < settings->fYbuffer)
	{
		newPos.y = settings->fYbuffer;
	}
	else if (pos.y > settings->nHeight - settings->fYbuffer)
	{
		newPos.y = settings->nHeight - settings->fYbuffer;
	}
	return newPos;
}

void initializesettings(int argc, char* argv[], Settings* settings)
{
	settings->nMimimumBreedFactor = DNA_SIZE-1;
	settings->nCurrentFood = 0;
	settings->nCurrentOrganism = 0;
	settings->nLineSize = 50;
	settings->nTime = 0;

	//Parsing the command line options using cxxopts
	cxxopts::Options options("volv", "Evolution simulator");

	try {
		options
			.positional_help("[optional args]")
			.show_positional_help();

		options.add_options("Engine")
			("x_buff", "Horizontal edge buffer", cxxopts::value<float>(settings->fXbuffer)->default_value("30"))
			("y_buff", "Vertical edge buffer", cxxopts::value<float>(settings->fYbuffer)->default_value("30"))
			("collision_square_size", "Side length of each hash-table collision square", cxxopts::value<int>(settings->nCollisionSquareSize)->default_value("100"))
			("u,unlimited_framerate", "Unlock fame rate", cxxopts::value<bool>(settings->bUnlimitedFramerate))
			;

		options.add_options("Display")
			("f,fullscreen", "Fullscreen mode", cxxopts::value<bool>(settings->bFullscreen))
			("d,developer_mode", "Toggle developer mode - shows AI decision making and ", cxxopts::value<bool>(settings->bDevmode))
			;

		options.add_options("Simulation")
			("y,height", "Simulaton Height", cxxopts::value<int>(settings->nHeight)->default_value("1500"))
			("x,width", "Simulaton Width", cxxopts::value<int>(settings->nWidth)->default_value("1800"))
			("n,number_organisms", "Number of unique organisms to start simulation", cxxopts::value<int>(settings->nInitialNumberOfOrganisms)->default_value("100"))
			("s,seed", "Seeded simulation, random seed if not", cxxopts::value<int>(settings->nSeed)->default_value(std::to_string(static_cast<unsigned int>(std::time(NULL)))))
			("food_density", "Food spawn density", cxxopts::value<int>(settings->nFoodSpawnRate)->default_value("15"))
			;

		options.add_options() //Other options
			("h, help", "Print help")
			;

		auto result = options.parse(argc, argv);

		//If help is selected, do not continue to run the program and simply display the help text
		if (result.count("help")) {
			std::cout << options.help({}) << std::endl;
			exit(0);
		}
	}
	catch (const cxxopts::OptionException& e) {
		std::cout << "error parsing options: " << e.what() << std::endl;
		exit(-1);
	}
}

bool isInBounds(sf::Vector2f vec, Settings* settings, int nTileOffsetX, int nTileOffsetY)
{
	return (int(vec.y / settings->nCollisionSquareSize + nTileOffsetY) >= 0 && int(vec.x / settings->nCollisionSquareSize) + nTileOffsetX >= 0 && int(vec.y / settings->nCollisionSquareSize + nTileOffsetY) < settings->nHeight / settings->nCollisionSquareSize + 1 && int(vec.x / settings->nCollisionSquareSize + nTileOffsetX) < settings->nWidth / settings->nCollisionSquareSize + 1);
}