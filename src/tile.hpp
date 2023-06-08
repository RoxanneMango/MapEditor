#ifndef TILE_HPP
#define TILE_HPP

#include <SFML/Graphics.hpp>
#include "object.hpp"

class Tile : public Object
{
//private:
public:
	sf::RectangleShape hoverBox;
	bool isVisible = true;
	uint8_t transparency = 100;
	sf::Color hoverColor = sf::Color(220, 225, 230);
	sf::Color collisionColor = sf::Color::Red;
	
	int index = -1;
//	int indexInGrid = -1;
	int indexInTexturePack = -1;
	
	// cannot actually access any of texturePack's member variables
//	TexturePackPreview * texturePack = nullptr;
	
	bool hasCollision = false;
	
	std::string texturePATH = "";
	
	float scale = 1;
	
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
		hoverBox.setOutlineColor(sf::Color::Black);
	}	

	void setCollision(bool c)
	{
		hasCollision = c;
		
		sf::Color gridColor = sf::Color::Black;
		
		sf::Vector2f s = getSize();
		sf::Vector2f p = getPosition();
		
		if(c)
		{
			int t = 3;
			hoverBox.setOutlineThickness(t);


			gridColor = sf::Color::Red;
			gridColor.a = 200;
			hoverBox.setOutlineColor(gridColor);

			hoverBox.setPosition(sf::Vector2f(p.x + t*scale, p.y + t*scale));
			hoverBox.setSize(sf::Vector2f(s.x - t*2 + 1, s.y - t*2 + 1));
		}
		else
		{
			gridColor.a = 200;
			hoverBox.setOutlineColor(gridColor);			
			hoverBox.setOutlineThickness(1);
			hoverBox.setPosition(p);
			hoverBox.setSize(s);
		}
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

class TileObject : public Tile
{
public:
	sf::Vector2i mapPosition;

	TileObject(sf::Vector2f size, sf::Vector2f pos, int index, int indexInTexturePack = -1, std::string texturePATH = "") : 
		Tile(size, pos, index, indexInTexturePack, texturePATH)
	{
		this->hoverColor = sf::Color::Black;
	}
	
	void setMapPosition(sf::Vector2f pos)
	{
		mapPosition = sf::Vector2i((int)pos.x, (int)pos.y);
	}
	
	sf::Vector2i getMapPosition()
	{
		return mapPosition;
	}
	
};

#endif // TILE_HPP