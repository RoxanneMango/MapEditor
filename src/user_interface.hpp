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
#include "save_file_explorer.hpp"
#include "resize_grid_panel.hpp"

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

	Context * currentContext = CURRENT_CONTEXT;

	bool loadFile(std::string fileName)
	{
		printf("fileName: %s\n", fileName.c_str());
		
		FILE * fp = fopen(fileName.c_str(), "r");
		if(fp == NULL) return false;

		std::vector<std::string> lines;
		
		char c;		
		std::string line = "";
		while(!feof(fp))
		{
			if((c = fgetc(fp)))
			{
				if(c == '\n')
				{
					lines.push_back(line);
					line.clear();
				}
				else
				{
					line += c;
				}
			}
		}
		
		int index = 0;
		
		std::string saveName 				= lines[index++];	// save file name
		std::string mapName 				= lines[index++];	// map name
		//
		unsigned int tileWidth 				= strtoul(&lines[index++][0], NULL, 10);	// width of tiles
		unsigned int tileHeight				= strtoul(&lines[index++][0], NULL, 10);	// heigth of tiles
		//
		unsigned int mapWidth			 	= strtoul(&lines[index++][0], NULL, 10);	// width of the map (in tiles)
		unsigned int mapHeight 				= strtoul(&lines[index++][0], NULL, 10);	// height of the map (in tiles)
		//
		unsigned int numberOfTexturePacks 	= strtoul(&lines[index++][0], NULL, 10);	// number of texture pack PATHs
		unsigned int numberOfLayers 		= strtoul(&lines[index++][0], NULL, 10);	// number of layers
		
		sf::Vector2u gridSize = sf::Vector2u(mapWidth, mapHeight);
		sf::Vector2u tileSize = sf::Vector2u(tileWidth, tileHeight);

		// create new project
		Context * cc = new Context(fileName, gridSize, sf::Vector2f(0, toolbar_tab.getPosition().y + toolbar_tab.getSize().y + toolbar_tab.getOutlineThickness()*2));
		contextList.push_back(cc);
		currentContext = cc;
		TOOLBAR_TEXTURES = &currentContext->toolbar_textures;

		TOOLBAR_TEXTURES->texturePreviews.clear();

		toolbar_tab.addOption(mapName, currentContext);
		toolbar_tab.selectedIndex = toolbar_tab.tabs.size()-1;
		for(Tab & tab : toolbar_tab.tabs)
		{
			tab.deselect();
		}
		toolbar_tab.tabs[toolbar_tab.tabs.size()-1].select();
				
		// load in the texturepacks
		index++;
		for(unsigned int i = index; i < (index+numberOfTexturePacks); i++)
		{
			currentContext->toolbar_textures.addTexturePreview(lines[i]);
//			printf("texture: %s\n", lines[i].c_str());
		}
		index += numberOfTexturePacks;
		
		// load in the layers
		for(unsigned int i = 0; i < numberOfLayers; i++)
		{
			index++;
			
			std::string layerName = lines[index++];			

			currentContext->layerMenu.addLayer(
				sf::Vector2f(800, 600), 
				sf::Vector2f(currentContext->getPosition().x + 600, currentContext->getPosition().y + 10), 
				layerName, gridSize
			);			
//			printf("%s\n", layerName.c_str());
			
			sf::Color gridColor = sf::Color::Black;
			Layer & layer = currentContext->layerMenu.layers[currentContext->layerMenu.layers.size()-1];
			
			layer.tiles.clear();
			for(unsigned int y = 0; y < gridSize.y; y++)
			{
				unsigned int x = 0;
				
				std::string line = lines[index++];
				line += "\n";
								
				std::string coord[2] = {"", ""};
				bool isTextureCoord = true;
				bool canMakeTile = false;
				
//				printf("\n");
				
				for(char & c : line)
				{								
					if(c == 'x')
					{
						isTextureCoord = true;
						canMakeTile = true;
					}
					else if(c == ';')
					{
						isTextureCoord = !isTextureCoord;
					}
					else
					{
						if(!isTextureCoord && ((c == ' ') || (c == '\n')))
						{
							canMakeTile = true;
						}
						else
						{
							if( (c != ' ') && (c != '\n') )
							{
								coord[1-isTextureCoord] += c;
							}
						}
					}

					if(canMakeTile)
					{
//						if(isTextureCoord)
//							printf("x ", coord[0].c_str(), coord[1].c_str());						
//						else
//							printf("%s;%s ", coord[0].c_str(), coord[1].c_str());						
						
						Tile tile(
							sf::Vector2f(tileSize.x, tileSize.y),
							sf::Vector2f(
								layer.getPosition().x + x*tileSize.x + x*layer.margin, 
								layer.getPosition().y + y*tileSize.y + y*layer.margin),
							(y*gridSize.x) + x // index
						);

						tile.hoverBox.setOutlineColor(gridColor);
						tile.hoverColor = sf::Color::Black;

						if(isTextureCoord)
						{
							sf::Color c = tile.getFillColor();
							c.a = 0;
							tile.setFillColor(c);							
						}
						else
						{
							unsigned int texture_i 	= strtoul(&coord[0][0], NULL, 10);
							unsigned int tile_i 	= strtoul(&coord[1][0], NULL, 10);
							
							// get the texturePack at index (texture_i)
							TexturePackPreview & preview = currentContext->toolbar_textures.texturePreviews[texture_i];
							
							// number of tiles in the texturePack
							sf::Vector2u packSize = sf::Vector2u(
								preview.texture->getSize().x / tileSize.x, 
								preview.texture->getSize().y / tileSize.y
							);							
							
							sf::Vector2u ti = sf::Vector2u((tile_i % gridSize.x), (tile_i / gridSize.x));
							
							// set the texture and the texture rect
							tile.setTexture(preview.texture);
							tile.setTextureRect(sf::IntRect(
								ti.x * tileSize.x,
								ti.y * tileSize.y, 
								tileSize.x, 
								tileSize.y)
							);							
							
							tile.indexInTexturePack = tile_i;
							tile.texturePATH = preview.PATH;

							sf::Color color = tile.getFillColor();
							color.a = 255;
							tile.setFillColor(color);
						}

						layer.tiles.push_back(tile);

						coord[0].clear();
						coord[1].clear();
						
						canMakeTile = false;					
						isTextureCoord = true;
						
						x++;
					}
				}

			}
//			printf("\n");
		}
		if(numberOfLayers)
		{
			currentContext->layerMenu.selectedLayer = &currentContext->layerMenu.layers[0];
		}
		
		printf("Loaded!\n");
		
		return true;
	}
	
	bool saveFile(std::string fileName)
	{
		if(currentContext)
		{
			return currentContext->save(fileName);
		}		
		return false;
	}	

	void createFloatRectMask(sf::FloatRect & rect)
	{
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

	void createFloatRectMask(sf::RectangleShape & shape)
	{
		sf::FloatRect rect = sf::FloatRect(
			shape.getPosition().x,
			shape.getPosition().y,
			shape.getSize().x, 
			shape.getSize().y);

		createFloatRectMask(rect);
	}

	void initUiElements()
	{
		// option position
		int opoff = 35;
		int margin = 20;
		sf::Color buttonColor = sf::Color(180, 180, 180);
		
		if(currentContext == nullptr) return;
		sf::Vector2f op = sf::Vector2f(currentContext->layerMenu.layerRect.left + currentContext->layerMenu.layerRect.width + opoff + margin, currentContext->layerMenu.layerRect.top + opoff);
		
		uiElements.push_back(moveLeft = Option("<", [&]()
			{
				if(currentContext && currentContext->layerMenu.layers.size())
				{
					for(Layer & layer : currentContext->layerMenu.layers)
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
				if(currentContext && currentContext->layerMenu.layers.size())
				{
					for(Layer & layer : currentContext->layerMenu.layers)
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
				if(currentContext && currentContext->layerMenu.layers.size())
				{
					for(Layer & layer : currentContext->layerMenu.layers)
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
				if(currentContext && currentContext->layerMenu.layers.size())
				{
					for(Layer & layer : currentContext->layerMenu.layers)
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
				if(currentContext && currentContext->layerMenu.layers.size())
				{
					for(Layer & layer : currentContext->layerMenu.layers)
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
				if(currentContext && currentContext->layerMenu.layers.size())
				{
					for(Layer & layer : currentContext->layerMenu.layers)
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
				if(currentContext && currentContext->layerMenu.layers.size())
				{
					for(Layer & layer : currentContext->layerMenu.layers)
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
//				currentContext->layerMenu.selectedLayer->isSelected = true;
			}, 
			sf::Vector2f(op.x - opoff, op.y + opoff*++y), true, buttonColor )
		);
		
		uiElements.push_back(paintTile = Option("Paint", [&]()
			{
				CURSOR->setMode(CursorMode::Paint);
				if(currentContext && currentContext->toolbar_textures.selectedTexturePreview)
				{
					CURSOR->setBody(*currentContext->toolbar_textures.selectedTexturePreview->selectedTile);
//					currentContext->toolbar_textures.selectedTexturePreview->isSelected = true;
//					currentContext->layerMenu.selectedLayer->isSelected = false;
				}
			}, 
			sf::Vector2f(op.x - opoff, op.y + opoff*++y), true, buttonColor )
		);
		
		uiElements.push_back(eraseTile = Option("Erase", [&]()
		{
			CURSOR->setMode(CursorMode::Delete);
//			currentContext->layerMenu.selectedLayer->isSelected = false;
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
			printf("Load!\n");
			fileExplorer.open([&](std::string PATH)->bool{
				return loadFile(PATH);
			});
		});
		
		// open saved map item for editor to use
		toolbar_top.dropdownMenus[ToolBar_top::DropDownMenus::File].addOption("Open texture...", [&]()
		{
			printf("Open!\n");
			fileExplorer.open([&](std::string PATH)->bool{
				return TOOLBAR_TEXTURES->addTexturePreview(PATH);
			});
		});		
		
		
		toolbar_top.dropdownMenus[ToolBar_top::DropDownMenus::File].addOption("----------------", [](){}, false);
		
		// save map into map save file for map editor to use
		toolbar_top.dropdownMenus[ToolBar_top::DropDownMenus::File].addOption("Save...", [&]()
		{
			printf("Save!\n");
			saveFile(savePATH + currentContext->mapName + saveFileExtension);
		});

		// save map into map save file for map editor to use as ...
		toolbar_top.dropdownMenus[ToolBar_top::DropDownMenus::File].addOption("Save As...", [&]()
		{
			printf("Save As!\n");
			selectedInput = &saveFileExplorer.input;
			saveFileExplorer.open([&](std::string str){
				currentContext->save(str);
			});
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
		
		////////////////////////////////////////////////////////////////////////////////////////////////
		
		toolbar_top.dropdownMenus[ToolBar_top::DropDownMenus::Map].addOption("Resize Map...", [&]()
		{
			if(currentContext && currentContext->layerMenu.selectedLayer)
			{
				resizeGridPanel.open(&currentContext->layerMenu.layers);
			}
		});
	}

	void initResizeGridPanel()
	{
		
	}

	void initNewProjectPanel()
	{
		sf::Vector2f pos = sf::Vector2f(newProjectPanel.getPosition().x/2 + newProjectPanel.getSize().x/2, newProjectPanel.getPosition().y + newProjectPanel.getSize().y - 70);		
		newProjectPanel.addOption("Create new Project", pos, [&]()
			{

				if(newProjectPanel.inputIsCorrrect())
				{					
					std::string mapName =  newProjectPanel.inputFieldMapName.getString();//+ std::to_string(toolbar_tab.tabs.size());
					sf::Vector2u gridSize = sf::Vector2u(
						newProjectPanel.inputFieldMapSize_width.getUnsignedInteger(),
						newProjectPanel.inputFieldMapSize_height.getUnsignedInteger()					
					);

					if(!gridSize.x || !gridSize.y)
					{
						if(!gridSize.x)
						{
							newProjectPanel.inputFieldMapSize_width.setFillColor(newProjectPanel.inputFieldMapSize_width.errorColor);
						}
						if(!gridSize.y)
						{
							newProjectPanel.inputFieldMapSize_height.setFillColor(newProjectPanel.inputFieldMapSize_height.errorColor);
						}						
						return;
					}

					Context * cc = new Context(mapName, gridSize, sf::Vector2f(0, toolbar_tab.getPosition().y + toolbar_tab.getSize().y + toolbar_tab.getOutlineThickness()*2));
					currentContext = cc;
					
					TOOLBAR_TEXTURES = &currentContext->toolbar_textures;
					
					toolbar_tab.addOption(mapName, currentContext);
					toolbar_tab.selectedIndex = toolbar_tab.tabs.size()-1;
					for(Tab & tab : toolbar_tab.tabs)
					{
						tab.deselect();
					}
					toolbar_tab.tabs[toolbar_tab.tabs.size()-1].select();
					
					currentContext->layerMenu.addLayer(sf::Vector2f(800, 600), sf::Vector2f(currentContext->getPosition().x + 600, currentContext->getPosition().y + 10), "Layer_0", currentContext->gridSize);
					currentContext->layerMenu.selectedLayer = &currentContext->layerMenu.layers[0];

					printf("Create new project!...\n");
					
					contextList.push_back(currentContext);
					
					newProjectPanel.close();
				}
			}
		);
	}

	void initLayerMenu()
	{
		// option position
		int opoff = 90;
		sf::Color buttonColor = sf::Color(180, 180, 180);
		
		if(currentContext == nullptr) return;
//		if(currentContext->layerMenu.selectedLayer == nullptr) return;
		
		sf::Vector2f op = sf::Vector2f(currentContext->layerMenu.getPosition().x + currentContext->layerMenu.getSize().x + currentContext->layerMenu.scrollWheel.getSize().x + 20, currentContext->layerMenu.getPosition().y);

		int i = 0;
		int marginY = 35;

		uiElements.push_back(addLayer = Option("+ NEW ", [&]()
			{
				if(currentContext)
				{
					sf::Vector2f pos = sf::Vector2f(currentContext->getPosition().x + 600, 
													currentContext->getPosition().y + 10);	
					currentContext->layerMenu.addLayer(
						sf::Vector2f(800, 600),
						pos,
						"Layer_" + std::to_string(currentContext->layerMenu.layers.size()), 
						currentContext->gridSize
					);
					moveReset.action();
				}
			}, 
			sf::Vector2f(op.x, op.y + marginY * i), true, buttonColor)
		);
		uiElements.push_back(deleteLayer = Option("- DEL ", [&]()
			{
				if(currentContext && currentContext->layerMenu.selectedLayer && currentContext->layerMenu.selectedOption)
				{
					int i = -1;
					for(Layer & layer : currentContext->layerMenu.layers)
					{
						if(&layer == currentContext->layerMenu.selectedLayer)
						{
							currentContext->layerMenu.options.erase(currentContext->layerMenu.options.begin() + i+1);
							currentContext->layerMenu.layers.erase(currentContext->layerMenu.layers.begin() + i+1);
							currentContext->layerMenu.alignOptions();
							
							i++;
							break;
						}
						i++;
					}
					
					if(!currentContext->layerMenu.layers.size())
					{
						currentContext->layerMenu.selectedLayer = NULL;
						currentContext->layerMenu.selectedOption = NULL;						
					}
					else
					{
						if( i >= (currentContext->layerMenu.layers.size()-1))
						{
							i = (currentContext->layerMenu.layers.size()-1);
						}
						
						currentContext->layerMenu.selectedLayer = &currentContext->layerMenu.layers[i];
						currentContext->layerMenu.selectedOption = &currentContext->layerMenu.options[i];
						currentContext->layerMenu.selectedOption->isSelected = true;
						
						currentContext->layerMenu.adjustScrollWheel();
					}
				}
			}, 
			sf::Vector2f(op.x + opoff, op.y + marginY * i++), true, buttonColor)
		);
		uiElements.push_back(moveUpLayer = Option("^ UP  ", [&]()
			{
				if(currentContext && currentContext->layerMenu.selectedOption)
				{
					int i = 0;
					for(LayerOption & option : currentContext->layerMenu.options)
					{
						if(&option == currentContext->layerMenu.selectedOption)
						{
							if(i > 0)
							{												
								LayerOption option1 = currentContext->layerMenu.options[i];
								LayerOption option2 = currentContext->layerMenu.options[i-1];
								//
								currentContext->layerMenu.options[i-1] = option1;
								currentContext->layerMenu.options[i] = option2;
								//
								currentContext->layerMenu.selectedOption->isSelected = false;								
								currentContext->layerMenu.selectedOption = &currentContext->layerMenu.options[i-1];
								currentContext->layerMenu.selectedOption->isSelected = true;
								//
								currentContext->layerMenu.alignOptions();

								Layer layer1 = currentContext->layerMenu.layers[i];
								Layer layer2 = currentContext->layerMenu.layers[i-1];
								//
								currentContext->layerMenu.layers[i-1] = layer1;
								currentContext->layerMenu.layers[i] = layer2;
								//
								currentContext->layerMenu.selectedLayer = &currentContext->layerMenu.layers[i-1];
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
				if(currentContext && currentContext->layerMenu.selectedOption)
				{
					int i = 0;
					for(LayerOption & option : currentContext->layerMenu.options)
					{
						if(&option == currentContext->layerMenu.selectedOption)
						{
							if(i < (currentContext->layerMenu.options.size()-1))
							{
								LayerOption option1 = currentContext->layerMenu.options[i];
								LayerOption option2 = currentContext->layerMenu.options[i+1];
								//
								currentContext->layerMenu.options[i+1] = option1;
								currentContext->layerMenu.options[i] = option2;
								//
								currentContext->layerMenu.selectedOption->isSelected = false;								
								currentContext->layerMenu.selectedOption = &currentContext->layerMenu.options[i+1];
								currentContext->layerMenu.selectedOption->isSelected = true;
								//
								currentContext->layerMenu.alignOptions();
								
								Layer layer1 = currentContext->layerMenu.layers[i];
								Layer layer2 = currentContext->layerMenu.layers[i+1];
								//
								currentContext->layerMenu.layers[i+1] = layer1;
								currentContext->layerMenu.layers[i] = layer2;
								//
								currentContext->layerMenu.selectedLayer = &currentContext->layerMenu.layers[i+1];
							}
							break;
						}
						i++;
					}
				}
			}, 
			sf::Vector2f(op.x + opoff, op.y + marginY * i), true, buttonColor)
		);

		if(currentContext && currentContext->layerMenu.selectedLayer)
		{
			currentContext->layerMenu.selectedLayer->initGrid(sf::Vector2u(8,8));
		}
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
	ResizeGridPanel resizeGridPanel;

	FileExplorer fileExplorer;
	SaveFileExplorer saveFileExplorer;

	std::vector<Context *> contextList;
	
	InputField * selectedInput = nullptr;

	UserInterface(int width, int height) : 
		sf::RectangleShape(sf::Vector2f(width, height)),
		toolbar_top(sf::Vector2f(width, toolbar_top_height)),
		toolbar_tab(sf::Vector2f(width, toolbar_tab_height), sf::Vector2f(0, toolbar_top.getSize().y), &contextList),
		fileExplorer(sf::Vector2f(width*WINDOW_SIZE_MULTIPLIER, height*WINDOW_SIZE_MULTIPLIER), sf::Vector2f((width-width*WINDOW_SIZE_MULTIPLIER)/2, (height-height*WINDOW_SIZE_MULTIPLIER)/2)),
		saveFileExplorer(sf::Vector2f(width*WINDOW_SIZE_MULTIPLIER, height*WINDOW_SIZE_MULTIPLIER), sf::Vector2f((width-width*WINDOW_SIZE_MULTIPLIER)/2, (height-height*WINDOW_SIZE_MULTIPLIER)/2))	
	{
	
		std::string mapName = "Home";
	
		currentContext = new Context(mapName, sf::Vector2u(8,8), sf::Vector2f(0, toolbar_tab.getPosition().y + toolbar_tab.getSize().y + toolbar_tab.getOutlineThickness()*2));
		contextList.push_back(currentContext);
		
		TOOLBAR_TEXTURES = &currentContext->toolbar_textures;
		
		initUiElements();
		initToolbarTop();
		initLayerMenu();

		toolbar_tab.addOption(mapName, currentContext);

		initNewProjectPanel();
		initResizeGridPanel();
		
		foreground.setSize(sf::Vector2f(width, height));
//		foreground.setPosition(currentContext->getPosition());
		foregroundImage.create(width, height, sf::Color(125, 145, 160));
	
		createFloatRectMask(toolbar_top);
		createFloatRectMask(toolbar_tab);
		createFloatRectMask(currentContext->toolbar_textures);
		createFloatRectMask(currentContext->toolbar_textures.scrollWheel);
		createFloatRectMask(currentContext->layerMenu.layerRect);
		createFloatRectMask(currentContext->layerMenu);
		createFloatRectMask(currentContext->layerMenu.scrollWheel);
		
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
		else if(saveFileExplorer.isOpen())
		{
			saveFileExplorer.update();
		}
		else if(newProjectPanel.isOpen())
		{
			selectedInput = newProjectPanel.currentInput;
			newProjectPanel.update();
		}
		else if(resizeGridPanel.isOpen())
		{
			selectedInput = resizeGridPanel.currentInput;
			resizeGridPanel.update();
		}
		else if((currentContext != nullptr) && (currentContext->confirmationPrompt.isOpen()))
		{
			currentContext->confirmationPrompt.update();
		}
		else
		{
			toolbar_top.update();
			toolbar_tab.update();

			for(Option & option : uiElements) option.update();
			
			if(currentContext != nullptr)
			{
				// handle user input (only if the window is in focus)
				//
				// handle input every (updateTime) ms -- this is to prevent
				// unnecessary input spam and make it so stuff that is done
				// to the editorGrid is bottle necked at a certain speed
				//
				// If something like moveLeft() was performed every game cycle instead of
				// every x number of milliseconds the grid would fly away into the sunset...
				if(WINDOW->hasFocus() && (updateClock.getElapsedTime().asMilliseconds() > updateTime))
				{
					// Save selected context to .map file in ../saves
					if(sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) && sf::Keyboard::isKeyPressed(sf::Keyboard::S))
					{
						currentContext->save(savePATH + currentContext->mapName + saveFileExtension);
					}						
					
					// Perform actions on the editorGrid layer(s)
					if(!sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
					{
						// Move the editor grid around (left;right;up;down)
						if(sf::Keyboard::isKeyPressed(sf::Keyboard::A)) moveLeft.action();
						if(sf::Keyboard::isKeyPressed(sf::Keyboard::D)) moveRight.action();
						if(sf::Keyboard::isKeyPressed(sf::Keyboard::W)) moveUp.action();
						if(sf::Keyboard::isKeyPressed(sf::Keyboard::S)) moveDown.action();
						
						// set the size scale of the editor grid (zooming in/out)
						if(sf::Keyboard::isKeyPressed(sf::Keyboard::Z)) zoomIn.action();
						if(sf::Keyboard::isKeyPressed(sf::Keyboard::X)) zoomOut.action();
					}					
					updateClock.restart();
				}
				currentContext->update();
			}
		}
		
		CURSOR->isBodyVisible = (currentContext != nullptr) && ( currentContext->layerMenu.selectedLayer && Collision::AABB(*CURSOR, *currentContext->layerMenu.selectedLayer) || Collision::AABB(*CURSOR, currentContext->toolbar_textures) );			
		
	}
	
	std::vector<Object *> getObjects()
	{
		std::vector<Object *> objects;
		
		objects.push_back((Object*)(&toolbar_top));
		objects.push_back((Object*)(&toolbar_tab));
		if(currentContext != nullptr)
		{
			objects.push_back((Object*)(&currentContext->toolbar_textures));
			objects.push_back((Object*)(currentContext->layerMenu.selectedLayer));
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
		if(currentContext != nullptr) currentContext->render(window);
		
		for(Option & option : uiElements) option.render(window);
		
		window.draw(foreground);
		toolbar_tab.render(window);
		toolbar_top.render(window);
		
		if(fileExplorer.isOpen())
		{
			fileExplorer.render(window);
		}
		if(saveFileExplorer.isOpen())
		{
			saveFileExplorer.render(window);
		}
		if(newProjectPanel.isOpen())
		{
			newProjectPanel.render(window);
		}
		if(resizeGridPanel.isOpen())
		{
			resizeGridPanel.render(window);
		}
		if(currentContext && currentContext->confirmationPrompt.isOpen())
		{
			currentContext->confirmationPrompt.render(window);
		}
	}
};

#endif // USER_INTERFACE_HPP