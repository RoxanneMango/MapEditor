#ifndef TOOLBAR_TAB_HPP
#define TOOLBAR_TAB_HPP

#include <vector>
#include <string>
#include <functional>

#include <SFML/Graphics.hpp>

#include "globals.hpp"
#include "option.hpp"
#include "scroll_wheel.hpp"
#include "file_explorer.hpp"


class Tab : public Option
{
public:
	Option closeButton;
	unsigned int index = 0;
	
	bool isSelected = false;
	sf::Color selectedColor = sf::Color(125, 145, 160);
	sf::Color unselectedColor = sf::Color(198, 198, 198);

	sf::Line selectedOverlay;

	sf::Vector2f margin = sf::Vector2f(5, 5);

	Tab()
	{}

	Tab(std::string label, sf::Vector2f pos, sf::Vector2f size, bool isSelected = false) : 
		Option(label, [&]()->void {}, pos),
		closeButton("x", []()->void	{printf("close tab\n");}, sf::Vector2f(pos.x + size.x - FONT_SIZE, pos.y), true),
		selectedOverlay(size.x, 2),
		isSelected(isSelected)
	{		
		setPosition(pos);
		setSize(size);
		setFillColor(sf::Color(125, 145, 160));
		setOutlineThickness(2);
		setOutlineColor(sf::Color::Black);
		
		selectedOverlay.setPosition(sf::Vector2f(pos.x, pos.y + size.y));
		selectedOverlay.setFillColor(sf::Color(125, 145, 160));
		
		text.setPosition(sf::Vector2f(pos.x + margin.x, pos.y));
		
		if(isSelected)
		{
			select();
		}
		else
		{
			deselect();
		}
		
	}
	
	virtual void update()
	{
		closeButton.update();
	}
	
	void select()
	{
		isSelected = true;
		setFillColor(selectedColor);
		text.setFillColor(sf::Color::White);
		closeButton.text.setFillColor(sf::Color::White);		
		
		closeButton.hoverColor = sf::Color(180, 185, 200);
	}
	
	void deselect()
	{
		isSelected = false;
		setFillColor(unselectedColor);
		text.setFillColor(sf::Color::Black);
		closeButton.text.setFillColor(sf::Color::Black);

		closeButton.hoverColor = sf::Color(210, 215, 230);		
	}
	
	virtual void render(sf::RenderWindow & window)
	{
		window.draw(*this);
		window.draw(text);
		closeButton.render(window);
		
		if(isSelected)
		{
			window.draw(selectedOverlay);
		}
	}
};

std::vector<Tab> * TAB_LIST;
unsigned int TAB_INDEX = 0;

class ToolBar_tab : public Object
{
private:
	int margin_x = 10;
	int margin_y = 5;
	int marginBetweenOptions = 70;

	sf::RectangleShape top;
	sf::Line line;
	
	sf::Vector2f tabSize = sf::Vector2f(150, 25);
	
public:
	std::vector<Tab> tabs;

	ToolBar_tab()
	{}

	ToolBar_tab(sf::Vector2f size, sf::Vector2f position) : top(sf::Vector2f(size.x, size.y/2)), 
	line(size.x, 2),
	Object(sf::Vector2f(size.x, size.y/2))
	{
		top.setPosition(position);
		setPosition(sf::Vector2f(position.x, position.y + top.getSize().y));
		line.setPosition(getPosition());
		
		top.setFillColor(sf::Color(180, 190, 200));
		setFillColor(sf::Color(125, 145, 160));
		
		addOption("Home");
		addOption("map_a");
		addOption("map_b");
	}
	
	void addOption(std::string label)
	{
		sf::Vector2f pos = sf::Vector2f(getPosition().x + margin_x + (margin_x*tabs.size()) + (tabSize.x*tabs.size()), top.getPosition().y + margin_y);
		tabs.push_back(Tab(label, pos, tabSize, tabs.size() == 0));
	}
	
	void update()
	{
		if(tabs.size())
		{
			int selectedIndex = -1;
			int yeetIndex = -1;
			bool wasSelected = false;
			for(unsigned int i = 0; i < tabs.size(); i++)
			{
				if(Collision::AABB(*CURSOR, tabs[i]) && CURSOR->isPressed())
				{
					selectedIndex = i;
					break;
				}
				if(Collision::AABB(*CURSOR, tabs[i].closeButton) && CURSOR->isPressed())
				{
					yeetIndex = i;
					wasSelected = tabs[i].isSelected;
					break;
				}
			}
			if(yeetIndex >= 0)
			{
				std::vector<Tab> newTabs;
				
				for(unsigned int i = 0; i < tabs.size(); i++)
				{
					if(i != yeetIndex)
					{
						newTabs.push_back(tabs[i]);
					}
				}
				tabs.clear();
				for(Tab tab : newTabs)
				{
					addOption(tab.text.getString());
				}
				if(wasSelected)
				{
					selectedIndex = yeetIndex - 1;
				}
				else
				{
					selectedIndex = -1;
				}
			}			
			if(selectedIndex >= 0)
			{
				if(tabs.size())
				{
					for(unsigned int i = 0; i < tabs.size(); i++)
					{
						if(i == selectedIndex)
						{
							tabs[i].select();
						}
						else
						{
							tabs[i].deselect();
						}
					}
				}
			}
		}
		for(Tab & tab : tabs)
		{
			tab.update();
		}
	}
	
	void render(sf::RenderWindow & window)
	{
		window.draw(*this);
		window.draw(top);
		window.draw(line);
		if(tabs.size())
		{
			for(Tab & tab : tabs)
			{
				tab.render(window);
			}
		}		
	}	
};

#endif // TOOLBAR_TAB_HPP