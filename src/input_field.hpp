#ifndef INPUT_FIELD_HPP
#define INPUT_FIELD_HPP

#include <cmath>

#include <SFML/Graphics.hpp>

#include "globals.hpp"
#include "constants.hpp"
#include "line.hpp"


class InputField : public sf::RectangleShape
{
private:
	int blinkTime_ms = 500;
	sf::Clock blinkClock;
	
public:

	enum Type 
	{
		String = 0,
		Integer,
		UnsignedInteger
	};

	Type type = Type::String;

	sf::Text title;

	bool isSelected = false;
	sf::Text text;

	sf::Color color = sf::Color(240, 240, 240);
//	sf::Color selectedColor = sf::Color(235, 235, 235);
	sf::Color outlineColor = sf::Color(50, 50, 50);
	sf::Color errorColor = sf::Color(240, 200, 200);

	sf::Line blinkLine;
	bool blinkOn = false;

	InputField(sf::Vector2f size, Type type, std::string titleText = "") : 
		type(type),
		title(titleText, *FONT, FONT_SIZE), 
		text("", *FONT, FONT_SIZE), 
		blinkLine(size.y*0.6, 2, sf::Line::Direction::Down)
	{
		setSize(size);
		setFillColor(color);
		setOutlineThickness(2);
		setOutlineColor(outlineColor);
		title.setFillColor(sf::Color::Black);
		text.setFillColor(sf::Color::Black);		
		blinkLine.setFillColor(sf::Color::Black);		
	}
	
	bool isValid()
	{		
		if(type == Type::String)
		{
			return text.getString().getSize();
		}
		else if(type == Type::Integer)
		{
			return isValidInteger();
		}
		else if(type == Type::UnsignedInteger)
		{
			return isValidUnsignedInteger();
		}
		return false;
	}

	unsigned int getUnsignedInteger()
	{
		if(isValidUnsignedInteger())
		{
			printf("It's valid!\n");
			std::string str = text.getString();
			unsigned int len = str.length();
			unsigned int uinteger = 0;
			for(unsigned int i = 0; i < len; i++)
			{
				int num = (str[i] - '0');
				int multiplier = fp::pow(10,((len-1)-i));
				if(multiplier <= 0) multiplier = 1;				
				uinteger += num * multiplier;
			}
			return uinteger;
		}
		return 0;
	}
	
	std::string getString()
	{
		return text.getString();
	}
	
	int getInteger()
	{
		if(isValidInteger())
		{
			std::string str = text.getString();
			unsigned int len = str.length();
			int integer = 0;
			bool isNegative = str[0] == '-';
			for(unsigned int i = isNegative; i < len; i++)
			{
				int num = (str[i] - '0');
				int multiplier = fp::pow(10,((len-1)-i));
				if(multiplier <= 0) multiplier = 1;				
				integer += num * multiplier;
			}
			
			integer = isNegative ? -integer : integer;

			return (integer);
		}
		return 0;
	}
	
	bool isValidInteger()
	{
		std::string str = text.getString();
		unsigned int len = str.length();
		bool isNegative = false;
		
		if((len <= 0) || ((isNegative = (str[0] == '-')) && len <= 1)) return false;		

		for(unsigned int i = isNegative; i < len; i++)
		{
			int x = str[i] - '0';
			if((x < 0) || (x > 9)) return false;
		}
		return true;
	}
	
	bool isValidUnsignedInteger()
	{
		return (isValidInteger() && (text.getString()[0] != '-'));
	}
	
	void changePosition(sf::Vector2f pos)
	{
		setPosition(pos);
		title.setPosition(sf::Vector2f(pos.x, pos.y - FONT_SIZE - 10));		
		text.setPosition(sf::Vector2f(pos.x + 5, pos.y + 5));
		
		float lineMargin = abs(getSize().y - blinkLine.length)/2;
		
		blinkLine.setPosition(sf::Vector2f(pos.x + 5, pos.y + lineMargin));
	}
	
	void update()
	{		
		if(isSelected && (blinkClock.getElapsedTime().asMilliseconds() >= blinkTime_ms))
		{
			blinkOn = blinkOn ? false : true;
			blinkClock.restart();
		}
		blinkLine.setPosition(sf::Vector2f(getPosition().x + 5 + text.getGlobalBounds().width + 5, blinkLine.getPosition().y));
	}
	
	void render(sf::RenderWindow & window)
	{
		window.draw(title);
		window.draw(*this);
		window.draw(text);
		
		if(isSelected && blinkOn)
		{
			window.draw(blinkLine);
		}
		
	}
};

#endif // INPUT_FIELD_HPP