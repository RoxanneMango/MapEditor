#ifndef TOOLBAR_TOP
#define TOOLBAR_TOP

#include "object.hpp"

#include "toolbar_dropdown.hpp"
#include "file_explorer.hpp"

#include "stdio.h"

class ToolBar_top : public Object
{
private:
	int margin_x = 10;
	int margin_y = 2;
	int marginBetweenOptions = 70;
	
	bool isSelected = false;
	int selectedIndex = -1;
	
public:

	enum DropDownMenus
	{
		File = 0,
		Edit,
		Map,
		View,
		Help
	};

	std::vector<Option> options;
	std::vector<ToolBar_dropdown> & dropdownMenus = *DROP_DOWN_MENUS;

	ToolBar_top()
	{}

	ToolBar_top(sf::Vector2f size) : Object(size)
	{
		setFillColor(sf::Color(180, 190, 200));
		
		// add options	
	}
	
	void init(sf::Vector2f size)
	{
		setSize(size);
		setFillColor(sf::Color(180, 190, 200));
	}
	
	void addOption(std::string label, std::function<void()> func, bool highLightOnHover = true)
	{
		sf::Vector2f pos = sf::Vector2f(getPosition().x + margin_x + (marginBetweenOptions*options.size()), getPosition().y + margin_y);
		options.push_back(Option(label, func, pos, highLightOnHover));
	}
	
	void update()
	{
		if(CURSOR->isClicked)
		{
			if(Collision::AABB(*CURSOR, *this))
			{
				int index = -1;
				for(unsigned int i = 0; i < options.size(); i++)
				{
					if(Collision::AABB(*CURSOR, options[i]))
					{
						index = i;
						break;
					}
				}
				if(index >= 0)
				{
					for(ToolBar_dropdown & ddMenu : dropdownMenus)
					{
						ddMenu.select(false);
					}					
					if(index == selectedIndex)
					{
						isSelected = isSelected ? false : true;
						dropdownMenus[selectedIndex].select(isSelected);
					}
					else
					{
						dropdownMenus[index].select(true);
						selectedIndex = index;
						isSelected = true;
					}
				}
				else
				{
					selectedIndex = -1;
					isSelected = false;
				}
			}
			else
			{
				if((selectedIndex >= 0) && dropdownMenus[selectedIndex].isVisible)
				{
					for(Option & option : dropdownMenus[selectedIndex].options)
					{
						if(Collision::AABB(*CURSOR, option))
						{
							option.action();
							CURSOR->isPressed();
							break;
						}
					}

					for(ToolBar_dropdown & ddMenu : dropdownMenus)
					{
						ddMenu.select(false);
					}
					selectedIndex = -1;
					isSelected = false;
				}
			}
		}
		
		if(options.size())
		{
			for(Option & option : options)
			{
				option.update();
			}
		}

		if(dropdownMenus.size())
		{
			for(ToolBar_dropdown & ddMenu : dropdownMenus)
			{
				ddMenu.update();
			}
		}
		
	}
	
	void render(sf::RenderWindow & window)
	{
		window.draw(*this);
		if(options.size())
		{
			for(Option & option : options)
			{
				option.render(window);
			}
			for(ToolBar_dropdown & ddMenu : dropdownMenus)
			{
				ddMenu.render(window);
			}
		}
	}
	
};

#endif // TOOLBAR_TOP