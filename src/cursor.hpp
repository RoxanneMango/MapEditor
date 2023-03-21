#ifndef CURSOR_HPP
#define CURSOR_HPP

#include <SFML/Graphics.hpp>

#include "tile.hpp"

class Cursor;
Cursor * CURSOR;

class Cursor : public sf::RectangleShape
{
	sf::RectangleShape body;
	float transparency = 0.66;
	float scale = 0.8;
public:
	bool isClicked = false;
	
	Cursor() : sf::RectangleShape(sf::Vector2f(1, 1)), body(sf::Vector2f(TILE_SIZE, TILE_SIZE))
	{
		setFillColor(sf::Color::Red);
		
		body.setOrigin(body.getSize().x/2, body.getSize().y/2);
		body.setScale(scale, scale);
		sf::Color c = body.getFillColor();
		body.setFillColor(sf::Color(c.r, c.g, c.b, c.a*transparency));
	}
	
	void update(sf::Vector2f pos)
	{
		setPosition(pos);
		body.setPosition(pos);
	}
	
	bool isPressed()
	{
		if(isClicked)
		{
			isClicked = false;
			return true;
		}
		return false;
	}
	
	void render(sf::RenderWindow & window)
	{
		window.draw(*this);
		if(body.getTexture() != NULL) window.draw(body);
	}
	
	void setBody(Tile & tile)
	{
		body.setTexture(tile.getTexture());
		body.setTextureRect(tile.getTextureRect());
	}
	
};

#endif // CURSOR_HPP