#ifndef USER_INTERFACE_HPP
#define USER_INTERFACE_HPP

#include <SFML/Graphics.hpp>

#include "line.hpp"
#include "toolbar_top.hpp"
#include "toolbar_tab.hpp"
#include "toolbar_textures.hpp"
#include "layer_menu.hpp"
#include "cursor.hpp"
#include "context.hpp"
#include "new_project_panel.hpp"

class UserInterface : public sf::RectangleShape
{
private:
	int toolbar_top_height = 30;
	int toolbar_tab_height = 60;

	sf::RectangleShape foreground;
	sf::Image foregroundImage;
	sf::Texture foregroundTexture;

	std::string savePATH = "../saves/";
	std::string saveFileExtension = ".map";

	sf::Clock updateClock;
	float updateTime = 60;

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
		if(CURRENT_CONTEXT->layerMenu.selectedLayer == nullptr) return;

		sf::Vector2f op = sf::Vector2f(CURRENT_CONTEXT->layerMenu.selectedLayer->getPosition().x + CURRENT_CONTEXT->layerMenu.selectedLayer->getSize().x + opoff + margin, CURRENT_CONTEXT->layerMenu.selectedLayer->getPosition().y + opoff);
		
		uiElements.push_back(moveLeft = Option("<", [&]()
			{
				if(CURRENT_CONTEXT && CURRENT_CONTEXT->layerMenu.layers.size())
				{
					for(Layer & layer : CURRENT_CONTEXT->layerMenu.layers)
					{
						sf::Vector2f pos = layer.tiles[0].getPosition();
						layer.changePosition(sf::Vector2f(pos.x + layer.tile_size, pos.y));							
					}
				}
			}, 
			sf::Vector2f(op.x - opoff, op.y), true, buttonColor)
		);
		//
		uiElements.push_back(moveRight = Option(">", [&]()
			{
				if(CURRENT_CONTEXT && CURRENT_CONTEXT->layerMenu.layers.size())
				{
					for(Layer & layer : CURRENT_CONTEXT->layerMenu.layers)
					{
						sf::Vector2f pos = layer.tiles[0].getPosition();
						layer.changePosition(sf::Vector2f(pos.x - layer.tile_size, pos.y));							
					}
				}
			}, 
			sf::Vector2f(op.x + opoff, op.y), true, buttonColor )
		);
		//
		uiElements.push_back(moveUp = Option("^", [&]()
			{
				if(CURRENT_CONTEXT && CURRENT_CONTEXT->layerMenu.layers.size())
				{
					for(Layer & layer : CURRENT_CONTEXT->layerMenu.layers)
					{
						sf::Vector2f pos = layer.tiles[0].getPosition();
						layer.changePosition(sf::Vector2f(pos.x, pos.y + layer.tile_size));							
					}
				}
			}, 
			sf::Vector2f(op.x, op.y - opoff), true, buttonColor )
		);
		//
		uiElements.push_back(moveDown = Option("v", [&]()
			{
				if(CURRENT_CONTEXT && CURRENT_CONTEXT->layerMenu.layers.size())
				{
					for(Layer & layer : CURRENT_CONTEXT->layerMenu.layers)
					{
						sf::Vector2f pos = layer.tiles[0].getPosition();
						layer.changePosition(sf::Vector2f(pos.x, pos.y - layer.tile_size));							
					}
				}
			}, 
			sf::Vector2f(op.x, op.y + opoff), true, buttonColor )
		);
		//
		uiElements.push_back(moveReset = Option("r", [&]()
			{
				if(CURRENT_CONTEXT && CURRENT_CONTEXT->layerMenu.layers.size())
				{
					for(Layer & layer : CURRENT_CONTEXT->layerMenu.layers)
					{
						layer.resetGrid();
					}
				}
			}, 
			sf::Vector2f(op.x, op.y), true, buttonColor )
		);

		// y-position of the ui elements, starting at the bottom of the position-changing-arrow-ui-elements
		int y = 3;
		
		uiElements.push_back(zoomIn = Option("+", [&]() 
			{ 
				if(CURRENT_CONTEXT && CURRENT_CONTEXT->layerMenu.layers.size())
				{
					for(Layer & layer : CURRENT_CONTEXT->layerMenu.layers)
					{
						layer.changeScale(0.1);
					}
				}
			}, 
			sf::Vector2f(op.x - opoff, op.y + opoff * y), true, buttonColor )
		);
		//
		uiElements.push_back(zoomOut = Option("-", [&]()
			{
				if(CURRENT_CONTEXT && CURRENT_CONTEXT->layerMenu.layers.size())
				{
					for(Layer & layer : CURRENT_CONTEXT->layerMenu.layers)
					{
						layer.changeScale(-0.1);
					}
				}
			}, 
			sf::Vector2f(op.x, op.y + opoff * y), true, buttonColor )
		);
		
		uiElements.push_back(clickTile = Option("Click", [&]()
			{
				CURSOR->setMode(CursorMode::Default);
//				CURRENT_CONTEXT->layerMenu.selectedLayer->isSelected = true;
			}, 
			sf::Vector2f(op.x - opoff, op.y + opoff*++y), true, buttonColor )
		);
		
		uiElements.push_back(paintTile = Option("Paint", [&]()
			{
				CURSOR->setMode(CursorMode::Paint);
				if(CURRENT_CONTEXT && CURRENT_CONTEXT->toolbar_textures.selectedTexturePreview)
				{
					CURSOR->setBody(*CURRENT_CONTEXT->toolbar_textures.selectedTexturePreview->selectedTile);
//					CURRENT_CONTEXT->toolbar_textures.selectedTexturePreview->isSelected = true;
//					CURRENT_CONTEXT->layerMenu.selectedLayer->isSelected = false;
				}
			}, 
			sf::Vector2f(op.x - opoff, op.y + opoff*++y), true, buttonColor )
		);
		
		uiElements.push_back(eraseTile = Option("Erase", [&]()
		{
			CURSOR->setMode(CursorMode::Delete);
//			CURRENT_CONTEXT->layerMenu.selectedLayer->isSelected = false;
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
			if(CURRENT_CONTEXT)
			{
				CURRENT_CONTEXT->save(savePATH + CURRENT_CONTEXT->mapName + saveFileExtension);
			}
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

				std::string mapName = "Unsaved " + std::to_string(toolbar_tab.tabs.size());

				Context * currentContext = new Context(mapName, sf::Vector2f(0, toolbar_tab.getPosition().y + toolbar_tab.getSize().y + toolbar_tab.getOutlineThickness()*2));
				contextList.push_back(currentContext);			
				CURRENT_CONTEXT = currentContext;
				TOOLBAR_TEXTURES = &currentContext->toolbar_textures;
				
				toolbar_tab.addOption(mapName, currentContext);
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

	void initLayerMenu()
	{
		// option position
		int opoff = 90;
		sf::Color buttonColor = sf::Color(180, 180, 180);
		
		if(CURRENT_CONTEXT == nullptr) return;
		if(CURRENT_CONTEXT->layerMenu.selectedLayer == nullptr) return;
		
		sf::Vector2f op = sf::Vector2f(CURRENT_CONTEXT->layerMenu.getPosition().x + CURRENT_CONTEXT->layerMenu.getSize().x + CURRENT_CONTEXT->layerMenu.scrollWheel.getSize().x + 20, CURRENT_CONTEXT->layerMenu.getPosition().y);

		int i = 0;
		int marginY = 35;

		uiElements.push_back(addLayer = Option("+ NEW ", [&]()
			{
				if(CURRENT_CONTEXT)
				{
					CURRENT_CONTEXT->layerMenu.addLayer(
						sf::Vector2f(800, 600),
						sf::Vector2f(CURRENT_CONTEXT->getPosition().x + 600, CURRENT_CONTEXT->getPosition().y + 10), 
						"Layer_" + std::to_string(CURRENT_CONTEXT->layerMenu.layers.size())
					);
					moveReset.action();
				}
			}, 
			sf::Vector2f(op.x, op.y + marginY * i), true, buttonColor)
		);
		uiElements.push_back(deleteLayer = Option("- DEL ", [&]()
			{
				if(CURRENT_CONTEXT && CURRENT_CONTEXT->layerMenu.selectedLayer)
				{
					int i = -1;
					for(Layer & layer : CURRENT_CONTEXT->layerMenu.layers)
					{
						if(&layer == CURRENT_CONTEXT->layerMenu.selectedLayer)
						{
							CURRENT_CONTEXT->layerMenu.layers.erase(CURRENT_CONTEXT->layerMenu.layers.begin() + i+1);
							CURRENT_CONTEXT->layerMenu.alignLayerLabels();
							break;
						}
						i++;
					}
					if(i >= 0)
					{
						CURRENT_CONTEXT->layerMenu.selectedLayer = &CURRENT_CONTEXT->layerMenu.layers[i];
						CURRENT_CONTEXT->layerMenu.selectedLayer->option.isSelected = true;
					}
					else
					{
						CURRENT_CONTEXT->layerMenu.selectedLayer = NULL;
					}
				}
			}, 
			sf::Vector2f(op.x + opoff, op.y + marginY * i++), true, buttonColor)
		);
		uiElements.push_back(moveUpLayer = Option("^ UP  ", [&]()
			{
				if(CURRENT_CONTEXT && CURRENT_CONTEXT->layerMenu.selectedLayer)
				{
					int i = 0;
					for(Layer & layer : CURRENT_CONTEXT->layerMenu.layers)
					{
						if(&layer == CURRENT_CONTEXT->layerMenu.selectedLayer)
						{
							if(i > 0)
							{
								Layer layer1 = CURRENT_CONTEXT->layerMenu.layers[i];
								Layer layer2 = CURRENT_CONTEXT->layerMenu.layers[i-1];
								
								CURRENT_CONTEXT->layerMenu.layers[i-1] = layer1;
								CURRENT_CONTEXT->layerMenu.layers[i] = layer2;
								
								CURRENT_CONTEXT->layerMenu.selectedLayer = &CURRENT_CONTEXT->layerMenu.layers[i-1];
								
								CURRENT_CONTEXT->layerMenu.alignLayerLabels();
							}
							break;
						}
						i++;
					}
				}
			}, 
			sf::Vector2f(op.x, op.y + marginY * i), true, buttonColor)
		);
		uiElements.push_back(moveDownLayer = Option("v DOWN", [&]()
			{
				if(CURRENT_CONTEXT && CURRENT_CONTEXT->layerMenu.selectedLayer)
				{
					int i = 0;
					for(Layer & layer : CURRENT_CONTEXT->layerMenu.layers)
					{
						if(&layer == CURRENT_CONTEXT->layerMenu.selectedLayer)
						{
							if(i < (CURRENT_CONTEXT->layerMenu.layers.size()-1))
							{
								Layer layer1 = CURRENT_CONTEXT->layerMenu.layers[i];
								Layer layer2 = CURRENT_CONTEXT->layerMenu.layers[i+1];
								
								CURRENT_CONTEXT->layerMenu.layers[i+1] = layer1;
								CURRENT_CONTEXT->layerMenu.layers[i] = layer2;

								CURRENT_CONTEXT->layerMenu.selectedLayer = &CURRENT_CONTEXT->layerMenu.layers[i+1];

								CURRENT_CONTEXT->layerMenu.alignLayerLabels();								
							}
							break;
						}
						i++;
					}
				}
			}, 
			sf::Vector2f(op.x + opoff, op.y + marginY * i), true, buttonColor)
		);


	}


public:

	// editor grid buttons
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
	// Layer toolbar buttons
	Option addLayer;
	Option deleteLayer;
	Option moveUpLayer;
	Option moveDownLayer;
	//
	// list of all ui elements
	std::vector<Option> uiElements;	

	ToolBar_top toolbar_top;
	ToolBar_tab toolbar_tab;

	NewProjectPanel newProjectPanel;

	FileExplorer fileExplorer;

	std::vector<Context *> contextList;
	
	InputField * selectedInput = nullptr;

	UserInterface(int width, int height) : 
		sf::RectangleShape(sf::Vector2f(width, height)),
		toolbar_top(sf::Vector2f(width, toolbar_top_height)),
		toolbar_tab(sf::Vector2f(width, toolbar_tab_height), sf::Vector2f(0, toolbar_top.getSize().y), &contextList),
		fileExplorer(sf::Vector2f(width*WINDOW_SIZE_MULTIPLIER, height*WINDOW_SIZE_MULTIPLIER), sf::Vector2f((width-width*WINDOW_SIZE_MULTIPLIER)/2, (height-height*WINDOW_SIZE_MULTIPLIER)/2))	
	{
	
		std::string mapName = "Home";
	
		CURRENT_CONTEXT = new Context(mapName, sf::Vector2f(0, toolbar_tab.getPosition().y + toolbar_tab.getSize().y + toolbar_tab.getOutlineThickness()*2));
		contextList.push_back(CURRENT_CONTEXT);
		
		TOOLBAR_TEXTURES = &CURRENT_CONTEXT->toolbar_textures;
		
		initUiElements();
		initToolbarTop();
		initLayerMenu();

		toolbar_tab.addOption(mapName, CURRENT_CONTEXT);

		initNewProjectPanel();
		
		foreground.setSize(sf::Vector2f(width, height));
//		foreground.setPosition(currentContext->getPosition());
		foregroundImage.create(width, height, sf::Color(125, 145, 160));
	
		createFloatRectMask(toolbar_top);
		createFloatRectMask(toolbar_tab);
		createFloatRectMask(CURRENT_CONTEXT->toolbar_textures);
		createFloatRectMask(CURRENT_CONTEXT->toolbar_textures.scrollWheel);
		createFloatRectMask(*CURRENT_CONTEXT->layerMenu.selectedLayer);
		createFloatRectMask(CURRENT_CONTEXT->layerMenu);
		createFloatRectMask(CURRENT_CONTEXT->layerMenu.scrollWheel);
		
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
			selectedInput = newProjectPanel.currentInput;
		}
		else if((CURRENT_CONTEXT != nullptr) && (CURRENT_CONTEXT->confirmationPrompt.isOpen()))
		{
			CURRENT_CONTEXT->confirmationPrompt.update();
		}
		else
		{
			toolbar_top.update();
			toolbar_tab.update();

			for(Option & option : uiElements) option.update();
			
			if(CURRENT_CONTEXT != nullptr)
			{
				// handle user input (only if the window is in focus)
				if(WINDOW->hasFocus())
				{
					if(sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) && sf::Keyboard::isKeyPressed(sf::Keyboard::S))
					{
						CURRENT_CONTEXT->save(savePATH + CURRENT_CONTEXT->mapName + saveFileExtension);
					}
					else if(updateClock.getElapsedTime().asMilliseconds() > updateTime)
					{
						if(!sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
						{
							if(sf::Keyboard::isKeyPressed(sf::Keyboard::A)) moveLeft.action();
							if(sf::Keyboard::isKeyPressed(sf::Keyboard::D)) moveRight.action();
							if(sf::Keyboard::isKeyPressed(sf::Keyboard::W)) moveUp.action();
							if(sf::Keyboard::isKeyPressed(sf::Keyboard::S)) moveDown.action();
							
							if(sf::Keyboard::isKeyPressed(sf::Keyboard::Z)) zoomIn.action();
							if(sf::Keyboard::isKeyPressed(sf::Keyboard::X)) zoomOut.action();
							
							updateClock.restart();
						}
					}					
				}
				CURRENT_CONTEXT->update();
			}
		}
		
		CURSOR->isBodyVisible = (CURRENT_CONTEXT != nullptr) && ( CURRENT_CONTEXT->layerMenu.selectedLayer && Collision::AABB(*CURSOR, *CURRENT_CONTEXT->layerMenu.selectedLayer) || Collision::AABB(*CURSOR, CURRENT_CONTEXT->toolbar_textures) );			
		
	}
	
	std::vector<Object *> getObjects()
	{
		std::vector<Object *> objects;
		
		objects.push_back((Object*)(&toolbar_top));
		objects.push_back((Object*)(&toolbar_tab));
		if(CURRENT_CONTEXT != nullptr)
		{
			objects.push_back((Object*)(&CURRENT_CONTEXT->toolbar_textures));
			objects.push_back((Object*)(CURRENT_CONTEXT->layerMenu.selectedLayer));
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
		if(CURRENT_CONTEXT && CURRENT_CONTEXT->confirmationPrompt.isOpen())
		{
			CURRENT_CONTEXT->confirmationPrompt.render(window);
		}
	}
};

#endif // USER_INTERFACE_HPP