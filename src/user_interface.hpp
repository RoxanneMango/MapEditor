#ifndef USER_INTERFACE_HPP
#define USER_INTERFACE_HPP

#include <SFML/Graphics.hpp>

#include "line.hpp"
#include "toolbar_top.hpp"
#include "toolbar_tab.hpp"
#include "toolbar_textures.hpp"
#include "editor_grid.hpp"

#include "cursor.hpp"

#include "context.hpp"

class NewProjectPanel : public sf::RectangleShape
{
private:
	sf::Text window_title = sf::Text("> Create New Project", *FONT, FONT_SIZE);
	sf::Text closeButton_x = sf::Text("X", *FONT, FONT_SIZE);
	sf::RectangleShape closeButton;
	sf::Line topDivider;
public:
	std::vector<Option> options;

	bool isVisible = true;

	NewProjectPanel() : sf::RectangleShape(sf::Vector2f(1000, 700))
	{
		sf::Color color = sf::Color(230, 230, 230);
		setFillColor(color);
		setOutlineThickness(2);
		setOutlineColor(sf::Color::Black);
		setPosition(sf::Vector2f(WINDOW->getSize().x/2 - getSize().x/2, WINDOW->getSize().y/2 - getSize().y/2));

		window_title.setFillColor(sf::Color::Black);
		window_title.setPosition(sf::Vector2f(getPosition().x + 10, getPosition().y + 10));
		
		closeButton.setFillColor(sf::Color(200, 80, 80));
		closeButton.setOutlineThickness(2);
		closeButton.setOutlineColor(sf::Color::Black);
		closeButton.setSize(sf::Vector2f(30, 30));
		closeButton.setPosition(sf::Vector2f(getPosition().x + getSize().x - closeButton.getSize().x - 5, getPosition().y + 5));
		
		closeButton_x.setFillColor(sf::Color::White);
		closeButton_x.setPosition(sf::Vector2f(closeButton.getPosition().x + 10, closeButton.getPosition().y + 3));

		topDivider = sf::Line(sf::Vector2f(getPosition().x, getPosition().y + closeButton.getSize().y + 10), getSize().x, 1);		
		
	}

	void addOption(std::string label, sf::Vector2f pos, std::function<void()> func, bool highLightOnHover = true)
	{
		Option option(label, func, pos, highLightOnHover, sf::Color(200, 200, 200));
		option.setOutlineThickness(2);
		option.setOutlineColor(sf::Color::Black);
		
		options.push_back(option);
	}

	bool isOpen()
	{
		return isVisible;
	}
	
	void open()
	{
		isVisible = true;
	}
	
	void close()
	{
		isVisible = false;
	}	
	
	void update()
	{
		try
		{
			if(isVisible)
			{
				if(Collision::AABB(*CURSOR, closeButton))
				{
					if(CURSOR->isPressed())
					{
						close();
					}
					closeButton.setFillColor(sf::Color(255, 80, 80));
				}
				else
				{
					closeButton.setFillColor(sf::Color(200, 80, 80));
				}
				
				for(Option & option : options)
				{
					option.update();
				}
			}
		}
		catch(...)
		{
			printf("Something went wrong while creating new project!\n");
		}	
	}
	
	void render(sf::RenderWindow & window)
	{
		if(isVisible)
		{
			window.draw(*this);
			window.draw(window_title);
			window.draw(closeButton);
			window.draw(topDivider);
			window.draw(window_title);
			window.draw(closeButton_x);

			for(Option & option : options)
			{
				option.render(window);
			}			
		}		
	}
};

class UserInterface : public sf::RectangleShape
{
private:
	int toolbar_top_height = 30;
	int toolbar_tab_height = 60;

	sf::RectangleShape foreground;
	sf::Image foregroundImage;
	sf::Texture foregroundTexture;

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

	void initUiElements()
	{
		// option position
		int opoff = 35;
		int margin = 20;
		sf::Color buttonColor = sf::Color(180, 180, 180);
		
		if(CURRENT_CONTEXT == nullptr) return;
		
		sf::Vector2f op = sf::Vector2f(CURRENT_CONTEXT->editorGrid.getPosition().x + CURRENT_CONTEXT->editorGrid.getSize().x + opoff + margin, CURRENT_CONTEXT->editorGrid.getPosition().y + opoff);
		
		uiElements.push_back(moveLeft = Option("<", [&]()
			{
				if(CURRENT_CONTEXT->editorGrid.tiles.size())
				{
					sf::Vector2f pos = CURRENT_CONTEXT->editorGrid.tiles[0].getPosition();
					CURRENT_CONTEXT->editorGrid.changePosition(sf::Vector2f(pos.x + CURRENT_CONTEXT->editorGrid.tile_size, pos.y));
				}
			}, 
			sf::Vector2f(op.x - opoff, op.y), true, buttonColor)
		);
		//
		uiElements.push_back(moveRight = Option(">", [&]()
			{
				if(CURRENT_CONTEXT->editorGrid.tiles.size())
				{
					sf::Vector2f pos = CURRENT_CONTEXT->editorGrid.tiles[0].getPosition();
					CURRENT_CONTEXT->editorGrid.changePosition(sf::Vector2f(pos.x - CURRENT_CONTEXT->editorGrid.tile_size, pos.y));
				}
			}, 
			sf::Vector2f(op.x + opoff, op.y), true, buttonColor )
		);
		//
		uiElements.push_back(moveUp = Option("^", [&]()
			{
				if(CURRENT_CONTEXT->editorGrid.tiles.size())
				{
					sf::Vector2f pos = CURRENT_CONTEXT->editorGrid.tiles[0].getPosition();
					CURRENT_CONTEXT->editorGrid.changePosition(sf::Vector2f(pos.x, pos.y + CURRENT_CONTEXT->editorGrid.tile_size));
				}
			}, 
			sf::Vector2f(op.x, op.y - opoff), true, buttonColor )
		);
		//
		uiElements.push_back(moveDown = Option("v", [&]()
			{
				if(CURRENT_CONTEXT->editorGrid.tiles.size())
				{
					sf::Vector2f pos = CURRENT_CONTEXT->editorGrid.tiles[0].getPosition();
					CURRENT_CONTEXT->editorGrid.changePosition(sf::Vector2f(pos.x, pos.y - CURRENT_CONTEXT->editorGrid.tile_size));
				}
			}, 
			sf::Vector2f(op.x, op.y + opoff), true, buttonColor )
		);
		//
		uiElements.push_back(moveReset = Option("r", [&]()
			{
				CURRENT_CONTEXT->editorGrid.resetGrid();
			}, 
			sf::Vector2f(op.x, op.y), true, buttonColor )
		);

		// y-position of the ui elements, starting at the bottom of the position-changing-arrow-ui-elements
		int y = 3;
		
		uiElements.push_back(zoomIn = Option("+", [&]() 
			{ 
				CURRENT_CONTEXT->editorGrid.changeScale(0.1); 
			}, 
			sf::Vector2f(op.x - opoff, op.y + opoff * y), true, buttonColor )
		);
		//
		uiElements.push_back(zoomOut = Option("-", [&]()
			{
				CURRENT_CONTEXT->editorGrid.changeScale(-0.1);
			}, 
			sf::Vector2f(op.x, op.y + opoff * y), true, buttonColor )
		);
		
		uiElements.push_back(clickTile = Option("Click", [&]()
			{
				CURSOR->setMode(CursorMode::Default);
//				CURRENT_CONTEXT->editorGrid.isSelected = true;
			}, 
			sf::Vector2f(op.x - opoff, op.y + opoff*++y), true, buttonColor )
		);
		
		uiElements.push_back(paintTile = Option("Paint", [&]()
			{
				CURSOR->setMode(CursorMode::Paint);
				if(CURRENT_CONTEXT->toolbar_textures.selectedTexturePreview)
				{
					CURSOR->setBody(*CURRENT_CONTEXT->toolbar_textures.selectedTexturePreview->selectedTile);
//					CURRENT_CONTEXT->toolbar_textures.selectedTexturePreview->isSelected = true;
//					CURRENT_CONTEXT->editorGrid.isSelected = false;
				}
			}, 
			sf::Vector2f(op.x - opoff, op.y + opoff*++y), true, buttonColor )
		);
		
		uiElements.push_back(eraseTile = Option("Erase", [&]()
		{
			CURSOR->setMode(CursorMode::Delete);
//			CURRENT_CONTEXT->editorGrid.isSelected = false;
		}, sf::Vector2f(op.x - opoff, op.y + opoff*++y), true, buttonColor ));
		
	}

	void initToolbarTop()
	{
		// initializing toolbar_top
		toolbar_top.addOption("File", [](){ printf("File...\n"); });
		toolbar_top.addOption("Edit", [](){ printf("Edit...\n"); });
		toolbar_top.addOption("Map",  [](){ printf("Map...\n");  });
		toolbar_top.addOption("View", [](){ printf("View...\n"); });
		toolbar_top.addOption("Help", [](){ printf("Help...\n"); });
		
		// add drop down menus
		for(Option & option : toolbar_top.options)
		{
			toolbar_top.dropdownMenus.push_back(
				ToolBar_dropdown
				(
					sf::Vector2f(
					option.getPosition().x, 
					option.getPosition().y + option.getSize().y
				)
			));
		}
		
		// start new map item for editor to use and possibly export for frankenpatch to use
		toolbar_top.dropdownMenus[ToolBar_top::DropDownMenus::File].addOption("New...", [&]()
		{
			printf("New!\n");
			newProjectPanel.open();
		});
		
		// open saved map item for editor to use
		toolbar_top.dropdownMenus[ToolBar_top::DropDownMenus::File].addOption("Open...", [&]()
		{
			printf("Open!\n");
			fileExplorer.open();
		});
		
		toolbar_top.dropdownMenus[ToolBar_top::DropDownMenus::File].addOption("----------------", [](){}, false);
		
		// save map into map save file for map editor to use
		toolbar_top.dropdownMenus[ToolBar_top::DropDownMenus::File].addOption("Save...", [&]()
		{
			printf("Save!\n");
		});

		// save map into map save file for map editor to use as ...
		toolbar_top.dropdownMenus[ToolBar_top::DropDownMenus::File].addOption("Save As...", [&]()
		{
			printf("Save As!\n");
		});
		
		// export map into map object for frankenpatch game to use
		toolbar_top.dropdownMenus[ToolBar_top::DropDownMenus::File].addOption("Export...", [&]()
		{
			printf("Export!\n");
			
			FILE * fp = fopen("../export/test.map", "w");
			
		});
		
		// close current tab
		toolbar_top.dropdownMenus[ToolBar_top::DropDownMenus::File].addOption("Close...", [&]()
		{
			printf("Close!\n");
		});

		toolbar_top.dropdownMenus[ToolBar_top::DropDownMenus::File].addOption("----------------", [](){}, false);	

		// exit program
		toolbar_top.dropdownMenus[ToolBar_top::DropDownMenus::File].addOption("Exit...", []()
		{ 
			WINDOW->close();
		});		
	}

	void initNewProjectPanel()
	{
		sf::Vector2f pos = sf::Vector2f(newProjectPanel.getPosition().x/2 + newProjectPanel.getSize().x/2, newProjectPanel.getPosition().y + newProjectPanel.getSize().y - 50);		
		newProjectPanel.addOption("Create new Project", pos, [&]()
			{

				Context * currentContext = new Context(sf::Vector2f(0, toolbar_tab.getPosition().y + toolbar_tab.getSize().y + toolbar_tab.getOutlineThickness()*2));
				contextList.push_back(currentContext);			
				CURRENT_CONTEXT = currentContext;
				TOOLBAR_TEXTURES = &currentContext->toolbar_textures;
				
				std::string tabName = "Unsaved " + std::to_string(toolbar_tab.tabs.size());
				
				toolbar_tab.addOption(tabName, currentContext);
				toolbar_tab.selectedIndex = toolbar_tab.tabs.size()-1;
				for(Tab & tab : toolbar_tab.tabs)
				{
					tab.deselect();
				}
				toolbar_tab.tabs[toolbar_tab.tabs.size()-1].select();
				
				printf("Create new project!...\n");
				
				newProjectPanel.close();
			}
		);		
	}

public:

	Option moveLeft;
	Option moveRight;
	Option moveUp;
	Option moveDown;
	Option moveReset;
	//
	Option zoomIn;
	Option zoomOut;
	//
	Option clickTile;
	Option paintTile;
	Option eraseTile;
	//
	std::vector<Option> uiElements;	

	ToolBar_top toolbar_top;
	ToolBar_tab toolbar_tab;

	NewProjectPanel newProjectPanel;

	FileExplorer fileExplorer;

	std::vector<Context *> contextList;

	UserInterface(int width, int height) : 
		sf::RectangleShape(sf::Vector2f(width, height)),
		toolbar_top(sf::Vector2f(width, toolbar_top_height)),
		toolbar_tab(sf::Vector2f(width, toolbar_tab_height), sf::Vector2f(0, toolbar_top.getSize().y), &contextList),
		fileExplorer(sf::Vector2f(width*WINDOW_SIZE_MULTIPLIER, height*WINDOW_SIZE_MULTIPLIER), sf::Vector2f((width-width*WINDOW_SIZE_MULTIPLIER)/2, (height-height*WINDOW_SIZE_MULTIPLIER)/2))	
	{
	
		CURRENT_CONTEXT = new Context(sf::Vector2f(0, toolbar_tab.getPosition().y + toolbar_tab.getSize().y + toolbar_tab.getOutlineThickness()*2));
		contextList.push_back(CURRENT_CONTEXT);
		
		TOOLBAR_TEXTURES = &CURRENT_CONTEXT->toolbar_textures;
		
		initUiElements();	
		initToolbarTop();

		toolbar_tab.addOption("Home", CURRENT_CONTEXT);

		initNewProjectPanel();
		
		foreground.setSize(sf::Vector2f(width, height));
//		foreground.setPosition(currentContext->getPosition());
		foregroundImage.create(width, height, sf::Color(125, 145, 160));
	
		createFloatRectMask(toolbar_top);
		createFloatRectMask(toolbar_tab);
		createFloatRectMask(CURRENT_CONTEXT->toolbar_textures);
		createFloatRectMask(CURRENT_CONTEXT->toolbar_textures.scrollWheel);
		createFloatRectMask(CURRENT_CONTEXT->editorGrid);
		
		for(Option & option : uiElements)
		{
			createFloatRectMask(option);
		}	

		foregroundTexture.loadFromImage(foregroundImage);
		foreground.setTexture(&foregroundTexture);
		
	}
	
	~UserInterface()
	{
		if(contextList.size())
		{
			for(Context * context : contextList)
			{
				delete context;
			}
		}
	}
	
	void update()
	{
		if(fileExplorer.isOpen())
		{
			fileExplorer.update();
		}
		else if(newProjectPanel.isOpen())
		{
			newProjectPanel.update();
		}
		else
		{
			toolbar_top.update();
			toolbar_tab.update();

			for(Option & option : uiElements) option.update();
			
			if(CURRENT_CONTEXT != nullptr) CURRENT_CONTEXT->update();			
		}
	}
	
	std::vector<Object *> getObjects()
	{
		std::vector<Object *> objects;
		
		objects.push_back((Object*)(&toolbar_top));
		objects.push_back((Object*)(&toolbar_tab));
		if(CURRENT_CONTEXT != nullptr)
		{
			objects.push_back((Object*)(&CURRENT_CONTEXT->toolbar_textures));
			objects.push_back((Object*)(&CURRENT_CONTEXT->editorGrid));
		}
		
		if(toolbar_top.options.size())
		{
			for(Option & option : toolbar_top.options)
			{
				objects.push_back(&option);
			}
			for(ToolBar_dropdown & ddMenu : toolbar_top.dropdownMenus)
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
		if(CURRENT_CONTEXT != nullptr) CURRENT_CONTEXT->render(window);
		
		for(Option & option : uiElements) option.render(window);
		
		window.draw(foreground);
		toolbar_tab.render(window);
		toolbar_top.render(window);
		
		if(fileExplorer.isOpen())
		{
			fileExplorer.render(window);
		}
		if(newProjectPanel.isOpen())
		{
			newProjectPanel.render(window);
		}
	}
};

#endif // USER_INTERFACE_HPP