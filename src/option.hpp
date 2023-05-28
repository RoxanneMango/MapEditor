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
	bool isSelected = false;
	uint8_t transparency = 128;
	sf::Color hoverColor = sf::Color(220, 225, 230);

	sf::Text text;
	std::function<void()> action = [](){};

	Option()
	{}

//	Option(std::string str, std::function<void()> a, sf::Vector2f pos, bool highLightOnHover = true, sf::Color fillColor = sf::Color::Transparent) : text(str, *font, FONT_SIZE), action(a), highLightOnHover(highLightOnHover), hoverBox(sf::Vector2f((str.size()+1) * (FONT_SIZE*0.6) + 5, FONT_SIZE*1.2)), Object(sf::Vector2f((str.size()+1) * (FONT_SIZE*0.6) + 5, FONT_SIZE*1.2))
	Option(std::string str, std::function<void()> a, sf::Vector2f pos, bool highLightOnHover = true, sf::Color fillColor = sf::Color::Transparent) : text(str, *font, FONT_SIZE), action(a), highLightOnHover(highLightOnHover)//, hoverBox(), Object()
	{
		setPosition(sf::Vector2f(pos.x - 5, pos.y));
		setFillColor(fillColor);

		hoverBox.setPosition(getPosition());
		hoverBox.setFillColor(sf::Color::Transparent);

		text.setPosition(pos);
		text.setFillColor(sf::Color::Black);
		
		sf::Vector2f size = sf::Vector2f(text.getGlobalBounds().width + FONT_SIZE*0.5 + (str.size() ? 5 : 0), text.getGlobalBounds().height + FONT_SIZE*0.5);
		
		unsigned int minSize = FONT_SIZE*1.2;
		
		size.x = size.x < minSize ? minSize : size.x;
		size.y = size.y < minSize ? minSize : size.y;
		
		setSize(size);
		hoverBox.setSize(size);
		
	}

	Option(std::string str, sf::Vector2f pos, bool highLightOnHover = true, sf::Color fillColor = sf::Color::Transparent, uint8_t transparency = 128) :
			text(str, *font, FONT_SIZE), highLightOnHover(highLightOnHover), transparency(transparency)
	{
		changePosition(pos);

		setFillColor(fillColor);
		hoverBox.setFillColor(sf::Color::Transparent);
		text.setFillColor(sf::Color::Black);

		sf::Vector2f size = sf::Vector2f(text.getGlobalBounds().width + FONT_SIZE*0.5 + (str.size() ? 5 : 0), text.getGlobalBounds().height + FONT_SIZE*0.5);
		unsigned int minSize = FONT_SIZE*1.2;
		
		size.x = size.x < minSize ? minSize : size.x;
		size.y = size.y < minSize ? minSize : size.y;
		
		setSize(size);
		hoverBox.setSize(size);

		alignCenter();		
	}

	Option(std::string str, std::function<void()> a, sf::Vector2f pos, sf::Vector2f size, int outlineThickness, bool highLightOnHover = true, sf::Color fillColor = sf::Color::Transparent, uint8_t transparency = 128) : 
		text(str, *font, FONT_SIZE), action(a), highLightOnHover(highLightOnHover), transparency(transparency)
	{
		changePosition(pos);
		changeSize(size);

		setFillColor(fillColor);
		hoverBox.setFillColor(sf::Color::Transparent);
		text.setFillColor(sf::Color::Black);
		
		setOutlineThickness(outlineThickness);
		setOutlineColor(sf::Color::Black);

		alignCenter();
	}

	
	virtual void changePosition(sf::Vector2f pos)
	{
		setPosition(sf::Vector2f(pos.x - 5, pos.y));
		hoverBox.setPosition(getPosition());

		int yPos = getSize().y - FONT_SIZE*1.2;		
		text.setPosition(sf::Vector2f(pos.x + 5, pos.y + (yPos ? yPos/2 : 0) ));
//		text.setPosition(pos);
	}
	
	virtual void changeSize(sf::Vector2f size)
	{		
		setSize(size);
		hoverBox.setSize(size);		

		int yPos = getSize().y - FONT_SIZE*1.2;		
		text.setPosition(sf::Vector2f(getPosition().x + 5, getPosition().y + (yPos ? yPos/2 : 0) ));
	}
	
	void alignCenter()
	{
		text.setPosition(sf::Vector2f(getPosition().x + getSize().x/2 - text.getGlobalBounds().width/2, text.getPosition().y));
	}
	
	void doAction()
	{
		hoverBox.setFillColor(sf::Color(hoverColor.r, hoverColor.g, hoverColor.b, transparency));
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
			else if(isSelected)
			{
				hoverBox.setFillColor(sf::Color(hoverColor.r, hoverColor.g, hoverColor.b, transparency));				
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