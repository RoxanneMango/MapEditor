#ifndef TOOLBAR_TOP
#define TOOLBAR_TOP

#include "object.hpp"

#include "toolbar_dropdown.hpp"
#include "file_explorer.hpp"

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

	ToolBar_top(sf::Vector2f size) : Object(size)
	{
		setFillColor(sf::Color(180, 190, 200));
		
		// add options
		addOption("File", [](){ printf("File...\n"); });
		addOption("Edit", [](){	printf("Edit...\n"); });
		addOption("Map",  [](){ printf("Map...\n");	 });
		addOption("View", [](){	printf("View...\n"); });
		addOption("Help", [](){ printf("Help...\n"); });
		
		// add drop down menus
		for(Option & option : options)
		{
			dropdownMenus.push_back(
				ToolBar_dropdown
				(
					sf::Vector2f(
					option.getPosition().x, 
					option.getPosition().y + option.getSize().y
				)
			));
		}
		
		// start new map item for editor to use and possibly export for frankenpatch to use
		dropdownMenus[DropDownMenus::File].addOption("New...", [&]()
		{
			printf("New!\n");
		});
		
		// open saved map item for editor to use
		dropdownMenus[DropDownMenus::File].addOption("Open...", [&]()
		{
			printf("Open!\n");
			FILE_EXPLORER->open();
		});
		
		dropdownMenus[DropDownMenus::File].addOption("----------------", [](){}, false);
		
		// save map into map save file for map editor to use
		dropdownMenus[DropDownMenus::File].addOption("Save...", [&]()
		{
			printf("Save!\n");
		});
		
		// export map into map object for frankenpatch game to use
		dropdownMenus[DropDownMenus::File].addOption("Export...", [&]()
		{
			printf("Export!\n");
		});
		
		// close current tab
		dropdownMenus[DropDownMenus::File].addOption("Close...", [&]()
		{
			printf("Close!\n");
		});


		dropdownMenus[DropDownMenus::File].addOption("----------------", [](){}, false);	

		// exit program
		dropdownMenus[DropDownMenus::File].addOption("Exit...", []()
		{ 
			WINDOW->close();
		});
	
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
			for(ToolBar_dropdown & ddMenu : dropdownMenus)
			{
				ddMenu.select(false);
			}
		}
		
		if(selectedIndex >= 0)
		{
			if(Collision::AABB(*CURSOR, dropdownMenus[selectedIndex])  && CURSOR->isPressed())
			{
				for(Option & option : dropdownMenus[selectedIndex].options)
				{
					if(Collision::AABB(*CURSOR, option))
					{
						option.action();
						break;
					}
				}
			}
		}
		if(Collision::AABB(*CURSOR, *this) && CURSOR->isPressed())
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