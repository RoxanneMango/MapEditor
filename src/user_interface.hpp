#ifndef USER_INTERFACE_HPP
#define USER_INTERFACE_HPP

#include <SFML/Graphics.hpp>

#include "line.hpp"
#include "toolbar_top.hpp"
#include "toolbar_tab.hpp"
#include "toolbar_textures.hpp"
#include "editor_grid.hpp"

class UserInterface : public sf::RectangleShape
{
private:

	int toolbar_A_height = 30;
	int toolbar_B_height = 60;

	sf::RectangleShape foreground;
	sf::Image foregroundImage;
	sf::Texture foregroundTexture;

	ToolBar_top toolbar_A;
	ToolBar_tab toolbar_B;
	ToolBar_textures * toolbar_textures;
	
	EditorGrid editorGrid;

	void createFloatRectMask(sf::RectangleShape & shape)
	{
		sf::FloatRect rect = sf::FloatRect(
			shape.getPosition().x,
			shape.getPosition().y,
			shape.getSize().x, 
			shape.getSize().y);

		// borderSize
		unsigned short bT = 3;//shape.getOutlineThickness();
		
		for(unsigned int y = rect.top - bT; y < (rect.top+rect.height+bT); y++)
		{
			for(unsigned int x = rect.left - bT; x < (rect.left+rect.width+bT); x++)
			{
				if(((x < rect.left) || (x > (rect.left+rect.width-bT/2))) || ((y < rect.top) || (y > (rect.top+rect.height-bT/2))))
				{
					foregroundImage.setPixel(x, y, sf::Color::Black);//shape.getOutlineColor());
				}
				else
				{
					foregroundImage.setPixel(x, y, sf::Color::Transparent);
				}
			}
		}
	}

public:
	UserInterface(int width, int height) : 
		foreground(sf::Vector2f(width, height)),
		sf::RectangleShape(sf::Vector2f(width, height)),
		toolbar_A(sf::Vector2f(width, toolbar_A_height)),
		toolbar_B(sf::Vector2f(width, toolbar_B_height), sf::Vector2f(0, toolbar_A.getSize().y)),
		editorGrid(sf::Vector2f(600, 600), sf::Vector2f(600, 100), "Editor_Grid")
	{
		TOOLBAR_TEXTURES = new ToolBar_textures(sf::Vector2f(534, 600), sf::Vector2f(10, toolbar_B.getPosition().y + toolbar_B.getSize().y + 10));
		toolbar_textures = TOOLBAR_TEXTURES;
		
		foregroundImage.create(width, height, sf::Color(125, 145, 160));
	
		createFloatRectMask(*toolbar_textures);
		createFloatRectMask(toolbar_textures->scrollWheel);
	
		editorGrid.setSize(sf::Vector2f(800, 600));
		editorGrid.setFillColor(sf::Color::Blue);
		createFloatRectMask(editorGrid);

		foregroundTexture.loadFromImage(foregroundImage);
		foreground.setTexture(&foregroundTexture);
		
	}
	
	~UserInterface()
	{
		delete TOOLBAR_TEXTURES;
	}
	
	void update()
	{
		toolbar_A.update();
		toolbar_B.update();
		toolbar_textures->update();
		editorGrid.update();
	}
	
	std::vector<Object *> getObjects()
	{
		std::vector<Object *> objects;
		
		objects.push_back((Object*)(&toolbar_A));
		objects.push_back((Object*)(&toolbar_B));
		objects.push_back((Object*)(&toolbar_textures));
		objects.push_back((Object*)(&editorGrid));
		
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
		toolbar_textures->render(window);
		editorGrid.render(window);
		window.draw(foreground);

		toolbar_B.render(window);
		toolbar_A.render(window);
	}
};

#endif // USER_INTERFACE_HPP