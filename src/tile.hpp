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
	
	int index = -1;
//	int indexInGrid = -1;
	int indexInTexturePack = -1;
	
	// cannot actually access any of texturePack's member variables
//	TexturePackPreview * texturePack = nullptr;
	
	std::string texturePATH = "";
	
//	Tile(sf::Vector2f size, sf::Vector2f pos, int index, int indexInTexturePack = -1, TexturePackPreview * texturePack = nullptr) : 
	Tile(sf::Vector2f size, sf::Vector2f pos, int index, int indexInTexturePack = -1, std::string texturePATH = "") : 
		Object(size),
		hoverBox(size),
		index(index),
		indexInTexturePack(indexInTexturePack),
		texturePATH(texturePATH)
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
	
//	operator == (const Tile & t)
//	{
//		return true;
//		return ( (texturePack == t.texturePack) && (indexInTexturePack == t.indexInTexturePack) && (index == t.index) );
//	}
	
};

#endif // TILE_HPP