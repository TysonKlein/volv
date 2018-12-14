#include "header.hpp"

float vectorDistance(sf::Vector2f V1, sf::Vector2f V2)
{
	float x, y;
	x = V1.x - V2.x;
	y = V1.y - V2.y;
	return sqrt(x*x + y * y);
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

sf::Vector2f buffer(sf::Vector2f pos, SimVars* simVars)
{
	sf::Vector2f newPos = pos;
	if (pos.x < simVars->Xbuff)
	{
		newPos.x = simVars->Xbuff;
	}
	else if (pos.x > simVars->WIDTH - simVars->Xbuff)
	{
		newPos.x = simVars->WIDTH - simVars->Xbuff;
	}
	if (pos.y < simVars->Ybuff)
	{
		newPos.y = simVars->Ybuff;
	}
	else if (pos.y > simVars->HEIGHT - simVars->Ybuff)
	{
		newPos.y = simVars->HEIGHT - simVars->Ybuff;
	}
	return newPos;
}

void initializeSimVars(int argc, char* argv[], SimVars* simVars)
{
	simVars->BREED_BASE = DNA_SIZE-1;
	simVars->CURRENT_FOOD = 0;
	simVars->CURRENT_ORGANISM = 0;
	simVars->LINE_SIZE = 50;
	simVars->TIME = 0;

	cxxopts::Options options("volv", "Evolution simulator");

	try {
		options
			.positional_help("[optional args]")
			.show_positional_help();

		options.add_options("Engine")
			("x_buff", "Horizontal edge buffer", cxxopts::value<float>(simVars->Xbuff)->default_value("30"))
			("y_buff", "Vertical edge buffer", cxxopts::value<float>(simVars->Ybuff)->default_value("30"))
			("collision_square_size", "Side length of each hash-table collision square", cxxopts::value<int>(simVars->COLLIDE_SQUARE_SIZE)->default_value("80"))
			("u,unlimited_framerate", "Unlock fame rate", cxxopts::value<bool>(simVars->UNLIMIED_FRAMERATE))
			;

		options.add_options("Display")
			("f,fullscreen", "Fullscreen mode", cxxopts::value<bool>(simVars->FULLSCREEN))
			("d,developer_mode", "Toggle developer mode - shows AI decision making and ", cxxopts::value<bool>(simVars->DEVMODE))
			;

		options.add_options("Simulation")
			("y,height", "Simulaton Height", cxxopts::value<int>(simVars->HEIGHT)->default_value("1000"))
			("x,width", "Simulaton Width", cxxopts::value<int>(simVars->WIDTH)->default_value("1500"))
			("n,number_organisms", "Number of unique organisms to start simulation", cxxopts::value<int>(simVars->INIT_NUM_ORGANISMS)->default_value("1500"))
			("s,seed", "Seeded simulation, random seed if not", cxxopts::value<int>(simVars->SEED)->default_value(std::to_string(static_cast<unsigned int>(std::time(NULL)))))
			("food_density", "Food spawn density", cxxopts::value<int>(simVars->FOODRATE)->default_value("20"))
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