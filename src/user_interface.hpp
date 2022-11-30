#ifndef USER_INTERFACE_HPP
#define USER_INTERFACE_HPP

#include <SFML/Graphics.hpp>

#include "line.hpp"
#include "toolbar.hpp"

class UserInterface : public sf::RectangleShape
{
private:

	int toolbar_A_height = 30;
	int toolbar_B_height = 60;

	ToolBar_top toolbar_A;
	ToolBar_tab toolbar_B;
	ToolBar_textures toolbar_textures;

public:
	UserInterface(int width, int height) : 
		sf::RectangleShape(sf::Vector2f(width, height)),
		toolbar_A(sf::Vector2f(width, toolbar_A_height)),
		toolbar_B(sf::Vector2f(width, toolbar_B_height), sf::Vector2f(0, toolbar_A.getSize().y)),
		toolbar_textures(sf::Vector2f(534, 500), sf::Vector2f( sf::Vector2f(10, toolbar_B.getPosition().y + toolbar_B.getSize().y + 10)) )
	{
		
	}
	
	void update()
	{
		toolbar_A.update();
		toolbar_B.update();
		toolbar_textures.update();
	}
	
	std::vector<Object *> getObjects()
	{
		std::vector<Object *> objects;
		
		objects.push_back((Object*)(&toolbar_A));
		objects.push_back((Object*)(&toolbar_B));
		objects.push_back((Object*)(&toolbar_textures));
		
		if(toolbar_A.options.size())
		{
			for(Option & option : toolbar_A.options)
			{
				objects.push_back(&option);
			}
			for(ToolBar_dropdown & ddMenu : toolbar_A.dropdownMenus)
			{
				if(ddMenu.options.size())
				{
					for(Option & option : ddMenu.options)
					{
						objects.push_back(&option);
					}
				}
			}
		}
		return objects;
	}
	
	void render(sf::RenderWindow & window)
	{
		toolbar_B.render(window);
		toolbar_textures.render(window);
		toolbar_A.render(window);
	}
};

#endif // USER_INTERFACE_HPP