#ifndef TILE_HPP
#define TILE_HPP

#include <SFML/Graphics.hpp>

class Tile : public Object
{
//private:
public:
	sf::RectangleShape hoverBox;
	bool isVisible = true;
	uint8_t transparency = 100;
	sf::Color hoverColor = sf::Color(220, 225, 230);

	static std::string selectedID;
	static unsigned int selectedIndex;

	std::string ID;
	unsigned int index = 0;	
	
	Tile(sf::Vector2f size, sf::Vector2f pos) : hoverBox(size), Object(size)
	{
		setPosition(pos);
		hoverBox.setPosition(pos);
		hoverBox.setFillColor(sf::Color::Transparent);
		hoverBox.setOutlineThickness(1);
		hoverBox.setOutlineColor(sf::Color::Red);
	}	

	virtual void update()
	{

	}
	
	void render(sf::RenderWindow & window)
	{
		window.draw(*this);
		window.draw(hoverBox);
	}
	
};

unsigned int Tile::selectedIndex = 0;
std::string Tile::selectedID = "";

#endif // TILE_HPP