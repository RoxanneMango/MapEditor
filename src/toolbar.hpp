#ifndef TOOLBAR_HPP
#define TOOLBAR_HPP

#include <vector>
#include <string>
#include <functional>

#include <SFML/Graphics.hpp>

#include "globals.hpp"
#include "option.hpp"
#include "scroll_wheel.hpp"


std::vector<sf::Texture *> * TEXTURE_PACKS;

class TexturePackPreview : public Object
{
public:

	sf::Text title;

	std::vector<Tile> tiles;
	unsigned int tile_size = 64;
	int margin = 2;
	int textMargin = 10;
	
	bool isSelected = false;
	std::string ID = "";

	TexturePackPreview(sf::Vector2f size, sf::Vector2f pos, std::string PATH) : title(PATH, *FONT, FONT_SIZE), ID(PATH), Object(size)
	{
		title.setPosition(pos);
		pos.y += FONT_SIZE + textMargin;
		setPosition(pos);

		setFillColor(sf::Color::Green);
		setOutlineThickness(2);
		setOutlineColor(sf::Color::Black);

		sf::Texture * texture = new sf::Texture();
		texture->loadFromFile(PATH);

		TEXTURE_PACKS->push_back(texture);
		
		// number of tiles in the texturePack
		sf::Vector2u texturePackTileNum = sf::Vector2u(texture->getSize().x / tile_size, texture->getSize().y / tile_size);
		
		for(unsigned int y = 0; y < texturePackTileNum.y; y++)
		{
			for(unsigned int x = 0; x < texturePackTileNum.x; x++)
			{

				sf::Vector2f texturePos( x*tile_size, y*tile_size );

				Tile tile(sf::Vector2f(tile_size, tile_size), sf::Vector2f(pos.x + texturePos.x + margin*x, pos.y + texturePos.y + margin*y));
				tile.setTexture(texture);
				tile.index = x + (y*texturePackTileNum.x);
				tile.ID = PATH;
				tile.setTextureRect(sf::IntRect(texturePos.x, texturePos.y, tile_size, tile_size));
				tile.setOutlineThickness(1);
				tile.setOutlineColor(sf::Color(0, 0, 0, 150));
				tile.hoverColor = sf::Color::Black;
				tiles.push_back(tile);
			}
		}
		
		title.setStyle(sf::Text::Bold);
		title.setFillColor(sf::Color::White);
		
		
		setSize(sf::Vector2f(size.x, texturePackTileNum.y*tile_size + (margin*texturePackTileNum.y-1)));
		
	}
	
	void update()
	{		
		if(tiles.size())
		{
			for(Tile & tile : tiles)
			{
				if(tile.isVisible)
				{
					bool isPressed = CURSOR->isPressed;
					if(Collision::AABB(*CURSOR, tile) || ((tile.index == Tile::selectedIndex) && (tile.ID == Tile::selectedID)) )
					{
						tile.hoverBox.setFillColor(sf::Color(tile.hoverColor.r, tile.hoverColor.g, tile.hoverColor.b, tile.transparency));						
						if(isPressed)
						{
							Tile::selectedIndex = tile.index;
							Tile::selectedID = tile.ID;
							printf("Tile_index: %d\n", Tile::selectedIndex);
						}
					}
					else
					{
						tile.hoverBox.setFillColor(sf::Color::Transparent);
					}
					if((tile.index == Tile::selectedIndex) && (tile.ID == Tile::selectedID))
					{
						tile.hoverBox.setFillColor(sf::Color(tile.hoverColor.r, tile.hoverColor.g, tile.hoverColor.b, tile.transparency));
						tile.setOutlineColor(sf::Color::Red);
					}
					else
					{
						tile.setOutlineColor(sf::Color::Black);						
					}
				}
			}
		}
			
	}
	
	void render(sf::RenderWindow & window)
	{
		window.draw(*this);
		window.draw(title);
		if(tiles.size())
		{
			for(Tile & tile : tiles)
			{
				tile.render(window);
			}
		}
	}
};
TexturePackPreview * SELECTED_TEXTURE_PACK = NULL;

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
		if(options.size())
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
		setSize(sf::Vector2f(getSize().x, margin.y + ((margin.y + option.getSize().y) * (options.size()-1))));
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


class ToolBar_textures : public Object
{
	sf::Vector2f margin = sf::Vector2f(8, 8);
	sf::Vector2f textureSize;
	float texturesHeight = margin.y;
	
public:
	std::vector<TexturePackPreview> texturePreviews;

	ScrollWheel_vertical scrollWheel;


	ToolBar_textures(sf::Vector2f size, sf::Vector2f position) : scrollWheel(sf::Vector2f(30, size.y), sf::Vector2f(position.x + size.x, position.y)), Object(size)
	{		
		setPosition(position);
		setFillColor(sf::Color(125, 145, 160));
		setOutlineThickness(3);
		setOutlineColor(sf::Color::Black);		
		
		textureSize = sf::Vector2f(size.x - margin.x, size.x - margin.y);
		addTexturePreview("../assets/texturepack.png");		
		addTexturePreview("../assets/pumpkin.png");
	}
	
	void update()
	{
		scrollWheel.update();
		if(texturePreviews.size())
		{
			for(TexturePackPreview & preview : texturePreviews)
			{
				if(CURSOR->isPressed)
				{
					if(Collision::AABB(*CURSOR, preview))
					{
						preview.isSelected = true;
						SELECTED_TEXTURE_PACK = &preview;
						CURSOR->isPressed = true;
					}
					else
					{
						preview.isSelected = false;
					}					
				}
				preview.update();
			}
		}
	}
	
	void addTexturePreview(std::string PATH)
	{
		sf::Vector2f position(sf::Vector2f(getPosition().x + margin.x/2, getPosition().y + texturesHeight));
		TexturePackPreview tpp(textureSize, position, PATH);

		texturesHeight += tpp.getSize().y + margin.y + FONT_SIZE + tpp.textMargin;
		texturePreviews.push_back(tpp);
	}
	
	void render(sf::RenderWindow & window)
	{
		window.draw(*this);
		
		if(texturePreviews.size())
		{
			for(TexturePackPreview & object : texturePreviews)
			{
				object.render(window);
			}
		}
		scrollWheel.render(window);
		
	}
};

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
			dropdownMenus.push_back(ToolBar_dropdown(
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
		if(CURSOR->isPressed)
		{
			for(ToolBar_dropdown & ddMenu : dropdownMenus)
			{
				ddMenu.select(false);
			}
			
			if(selectedIndex >= 0)
			{
				if(Collision::AABB(*CURSOR, dropdownMenus[selectedIndex]))
				{
					for(Option & option : dropdownMenus[selectedIndex].options)
					{
						if(Collision::AABB(*CURSOR, option))
						{
							option.action();
						}
					}
				}
			}
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

	Tab(std::string label, sf::Vector2f pos, sf::Vector2f size, bool isSelected = false) : Option(label, [&]()->void
	{
	}, pos),
	closeButton("x", []()->void
	{
		printf("close tab\n");
	}, sf::Vector2f(pos.x + size.x - FONT_SIZE, pos.y), true),
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
				if(CURSOR->isPressed)
				{
					if(Collision::AABB(*CURSOR, tabs[i]))
					{
						selectedIndex = i;
					}
					if(Collision::AABB(*CURSOR, tabs[i].closeButton))
					{
						yeetIndex = i;
						wasSelected = tabs[i].isSelected;
					}
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


#endif // TOOLBAR_HPP