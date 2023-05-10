#ifndef TOOLBAR_DROPDOWN_HPP
#define TOOLBAR_DROPDOWN_HPP

#include <SFML/Graphics.hpp>

#include "object.hpp"

class ToolBar_dropdown : public Object
{
	sf::Vector2f margin = sf::Vector2f(5, 5);
public:
	std::vector<Option> options;
	bool isVisible = false;

	ToolBar_dropdown(sf::Vector2f position) : Object(sf::Vector2f(190, FONT_SIZE))
	{
		setPosition(position);
		setFillColor(sf::Color(180, 190, 200));
		
		setOutlineThickness(2);
		setOutlineColor(sf::Color::Black);
	}
	
	virtual void update()
	{
		if(options.size() && isVisible)
		{
			for(Option & option : options)
			{
				option.update();
			}
		}
	}
	
	void addOption(std::string label, std::function<void()> func, bool highLightOnHover = true)
	{
		sf::Vector2f pos = sf::Vector2f(getPosition().x + margin.x, getPosition().y + margin.y/2 + (((margin.y) + FONT_SIZE)*options.size()));
		
		Option option(label, func, pos, highLightOnHover);
		option.isVisible = false;
		
		option.setSize(sf::Vector2f(getSize().x, option.getSize().y));
		
		options.push_back(option);
		setSize(sf::Vector2f(getSize().x, ((margin.y + option.getSize().y) * (options.size() > 1 ? options.size()-1 : 1))));
	}
	
	void select(bool value)
	{
		isVisible = value;
		if(options.size())
		{
			for(Option & option : options)
			{
				option.isVisible = value;
			}
		}		
	}
	
	void render(sf::RenderWindow & window)
	{
		if(isVisible)
		{
			window.draw(*this);
			if(options.size())
			{
				for(Option & option : options)
				{
					option.render(window);
				}
			}
		}
	}
};
std::vector<ToolBar_dropdown> * DROP_DOWN_MENUS;

#endif // TOOLBAR_DROPDOWN_HPP