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

#include "context.hpp"

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

	Context * context = NULL;

	Tab()
	{}

	~Tab()
	{
//		delete context;
	}

	Tab(std::string label, sf::Vector2f pos, sf::Vector2f size, Context * context, bool isSelected = false) : 
		Option(label, [&]()->void {}, pos),
		closeButton("x", []()->void	{printf("close tab\n");}, sf::Vector2f(pos.x + size.x - FONT_SIZE, pos.y), true),
		selectedOverlay(size.x, 2),
		context(context),
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
		CURSOR->setMode(CursorMode::Default);
		
//		context->toolbar_textures
		
//		if(SELECTED_TEXTURE_PACK)
//		{
//			Tile::selectedIndex = 0;
//			Tile::selectedID = "";
//			SELECTED_TEXTURE_PACK->selectedIndex = false;
//			SELECTED_TEXTURE_PACK->isSelected = false;
//			SELECTED_TEXTURE_PACK = NULL;
//		}
//		if(CURRENT_CONTEXT)
//		{
//			CURRENT_CONTEXT->editorGrid.isSelected = false;
//			if(CURRENT_CONTEXT->toolbar_textures.selectedTexturePreview);
//				CURRENT_CONTEXT->toolbar_textures.selectedTexturePreview = NULL;
//		}
		
		
//		SELECTED_TEXTURE_PACK = NULL;
//		SELECTED_TEXTURE_PACK->isSelected = false;
//		Tile::selectedIndex = 999;
//		Tile::selectedID = "";
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

class ToolBar_tab : public Object
{
private:
	int margin_x = 10;
	int margin_y = 5;
	int marginBetweenOptions = 70;

	sf::RectangleShape top;
	sf::Line line;
	
	sf::Vector2f tabSize = sf::Vector2f(150, 25);
	
	std::vector<Context *> * contextList;
	
public:
	int selectedIndex = -1;
	
	Tab * selectedTab = NULL;
	std::vector<Tab> tabs;

	ToolBar_tab()
	{}

	ToolBar_tab(sf::Vector2f size, sf::Vector2f position, std::vector<Context *> * contextList) : 
		Object(sf::Vector2f(size.x, size.y/2)),
		top(sf::Vector2f(size.x, size.y/2)), 
		line(size.x, 2),
		contextList(contextList)
	{
		top.setPosition(position);
		setPosition(sf::Vector2f(position.x, position.y + top.getSize().y));
		line.setPosition(getPosition());
		
		top.setFillColor(sf::Color(180, 190, 200));
		setFillColor(sf::Color(125, 145, 160));
	}
	
	void init(sf::Vector2f size, sf::Vector2f position)
	{
		setSize(sf::Vector2f(size.x, size.y/2));
		setPosition(sf::Vector2f(position.x, position.y + top.getSize().y));
		
		top.setSize(sf::Vector2f(size.x, size.y/2));
		top.setPosition(position);

		line.length = size.x;
		line.thickness = 2;
		line.direction = sf::Line::Direction::Right;
		line.setPosition(getPosition());
		
		top.setFillColor(sf::Color(180, 190, 200));
		setFillColor(sf::Color(125, 145, 160));
	}
	
	void addOption(std::string label, Context * context)
	{
		sf::Vector2f pos = sf::Vector2f(getPosition().x + margin_x + (margin_x*tabs.size()) + (tabSize.x*tabs.size()), top.getPosition().y + margin_y);
		tabs.push_back(Tab(label, pos, tabSize, context, tabs.size() == 0));
	}
	
	void update()
	{
		if(tabs.size())
		{
			selectedIndex = -1;
			int yeetIndex = -1;
			bool wasSelected = false;
			if(WINDOW->hasFocus())
			{
				for(unsigned int i = 0; i < tabs.size(); i++)
				{
					if(Collision::AABB(*CURSOR, tabs[i].closeButton) && CURSOR->isPressed())
					{
						yeetIndex = i;
						wasSelected = tabs[i].isSelected;
						break;
					}
					if(Collision::AABB(*CURSOR, tabs[i]) && CURSOR->isPressed())
					{
						selectedIndex = i;
						CURRENT_CONTEXT = tabs[i].context;
						break;
					}
				}
			}
			if(yeetIndex >= 0)
			{
				for(unsigned int i = 0; i < contextList->size(); i++)
				{
					if((*contextList)[i] == tabs[yeetIndex].context)
					{
						contextList->erase(contextList->begin()+i);
						if(CURRENT_CONTEXT == tabs[yeetIndex].context)
						{
							CURRENT_CONTEXT =
								(yeetIndex+1) < tabs.size() ? 
									tabs[yeetIndex+1].context :
								(yeetIndex-1) > 0 ? 
									tabs[yeetIndex-1].context :
								nullptr;
						}
						delete tabs[yeetIndex].context;
						break;
					}
				}
				
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
					addOption(tab.text.getString(), tab.context);
				}
				
				if(wasSelected)
				{
					selectedIndex = yeetIndex < tabs.size() ? yeetIndex : yeetIndex-1;
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