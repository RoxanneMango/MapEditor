#ifndef CURSOR_HPP
#define CURSOR_HPP

#include <SFML/Graphics.hpp>

class Cursor : public sf::RectangleShape
{
public:
	Cursor() : sf::RectangleShape(sf::Vector2f(1, 1))
	{
		setFillColor(sf::Color::Red);
	}
	
	bool isClicked = false;
	
	void update(sf::Vector2f pos)
	{
		setPosition(pos);
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
	}
};

Cursor * CURSOR;

#endif // CURSOR_HPP