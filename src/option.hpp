#ifndef OPTION_HPP
#define OPTION_HPP

#include <string>
#include <functional>

#include "object.hpp"
#include "collision.hpp"

class Option : public Object
{
protected:	
	sf::Font * font = FONT;
	bool highLightOnHover = true;
	sf::RectangleShape hoverBox;
public:
	bool isVisible = true;
	uint8_t transparency = 128;
	sf::Color hoverColor = sf::Color(220, 225, 230);

	sf::Text text;
	std::function<void()> action;

	Option()
	{}

	Option(std::string str, std::function<void()> a, sf::Vector2f pos, bool highLightOnHover = true) : text(str, *font, FONT_SIZE), action(a), highLightOnHover(highLightOnHover), hoverBox(sf::Vector2f((str.size()+1) * (FONT_SIZE/2) + 5, FONT_SIZE*1.2)), Object(sf::Vector2f((str.size()+1) * (FONT_SIZE/2) + 5, FONT_SIZE*1.2))
	{
		setPosition(sf::Vector2f(pos.x - 5, pos.y));
		setFillColor(sf::Color::Transparent);

		hoverBox.setPosition(getPosition());
		hoverBox.setFillColor(sf::Color::Transparent);

		text.setPosition(pos);
		text.setFillColor(sf::Color::Black);
	}
		
	void doAction()
	{
		action();
	}
		
	virtual void update()
	{
		if(highLightOnHover && isVisible)
		{
			if(Collision::AABB(*CURSOR, *this))
			{
				hoverBox.setFillColor(sf::Color(hoverColor.r, hoverColor.g, hoverColor.b, transparency));
				if(CURSOR->isPressed())
				{
					doAction();
				}
			}
			else
			{
				hoverBox.setFillColor(sf::Color::Transparent);
			}
		}
	}
	
	void render(sf::RenderWindow & window)
	{
		window.draw(*this);
		window.draw(hoverBox);
		if(text.getString().getSize())
		{
			window.draw(text);
		}
	}
};

#endif // OPTION_HPP