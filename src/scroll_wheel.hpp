#ifndef SCROLL_WHEEL_HPP
#define SCROLL_WHEEL_HPP

#include <SFML/Graphics.hpp>

#include "object.hpp"
#include "cursor.hpp"

class ScrollWheel_vertical : public Object
{
private:
	sf::Color selectedColor = sf::Color(220, 220, 220);
	sf::Color enabledColor = sf::Color(200, 200, 200);
	sf::Color disabledColor = sf::Color(127, 127, 127);
	
public:
	bool isEnabled = true;
	bool isSelected = false;

	Object wheeliePiece;

	ScrollWheel_vertical(sf::Vector2f size, sf::Vector2f pos) : Object(size), wheeliePiece(sf::Vector2f(size.x, size.y/4))
	{
		setPosition(pos);
		setFillColor(disabledColor);
		setOutlineThickness(2);
		setOutlineColor(sf::Color::Black);

		wheeliePiece.setPosition(pos);
		wheeliePiece.setFillColor(enabledColor);
		wheeliePiece.setOutlineThickness(2);
		wheeliePiece.setOutlineColor(sf::Color::Black);
	}
	
	
	virtual void update()
	{
		if(isEnabled)
		{
			wheeliePiece.setFillColor(enabledColor);
			bool isPressed = CURSOR->isPressed;
			if(isPressed)
			{
				if(Collision::AABB(*CURSOR, wheeliePiece))
				{
					isSelected = isSelected ? false : true;
				}
				else
				{
					isSelected = false;
				}
			}
			if(isSelected)
			{
				wheeliePiece.setFillColor(selectedColor);
				wheeliePiece.setPosition(sf::Vector2f(wheeliePiece.getPosition().x, CURSOR->getPosition().y - wheeliePiece.getSize().y/2));
				if( (wheeliePiece.getPosition().y + wheeliePiece.getSize().y) > (getPosition().y + getSize().y) )
				{
					wheeliePiece.setPosition(sf::Vector2f(wheeliePiece.getPosition().x, getPosition().y + getSize().y - wheeliePiece.getSize().y));
				}
				else if( (wheeliePiece.getPosition().y) < (getPosition().y) )
				{
					wheeliePiece.setPosition(sf::Vector2f(wheeliePiece.getPosition().x, getPosition().y));
				}
			}
		}
		else
		{
			wheeliePiece.setFillColor(disabledColor);			
		}
	}
	
	virtual void render(sf::RenderWindow & window)
	{
		window.draw(*this);
		window.draw(wheeliePiece);
	}
	
};

#endif // SCROLL_WHEEL_HPP