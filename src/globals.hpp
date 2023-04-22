#ifndef GLOBAL_HPP
#define GLOBAL_HPP

#include <SFML/Graphics.hpp>

sf::RenderWindow * WINDOW;
std::vector<sf::Texture *> * TEXTURE_PACKS;

unsigned int TILE_SIZE = 64;

namespace fp {

	// wrapper function for changing the cursor so you only have to give the type
	void SetCursor(sf::Cursor::Type c)
	{
		sf::Cursor cursor;
		if (cursor.loadFromSystem(c)) {
			WINDOW->setMouseCursor(cursor);
		}
	}

	constexpr int pow(int b, int e)
	{
		return (e == 0) ? 1 : b * fp::pow(b, e - 1);
	}	
	
}

#endif // GLOBAL_HPP