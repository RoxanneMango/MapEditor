#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include <SFML/Graphics.hpp>

#include "toolbar_textures.hpp"
#include "editor_grid.hpp"
#include "confirmation_prompt.hpp"
#include "line.hpp"
#include "layer_menu.hpp"

class Context;
Context * CURRENT_CONTEXT = NULL;

class Context : public sf::RectangleShape
{
public:
	ConfirmationPrompt confirmationPrompt;

	ToolBar_textures toolbar_textures;

	LayerMenu layerMenu;

	std::string mapName = "";
	std::string saveFile = "";
	
	Context(std::string name, sf::Vector2f pos) : 
		sf::RectangleShape(sf::Vector2f(sf::VideoMode::getDesktopMode().width * WINDOW_SIZE_MULTIPLIER, (sf::VideoMode::getDesktopMode().height * WINDOW_SIZE_MULTIPLIER)-pos.y)),
		toolbar_textures(sf::Vector2f(540, 600), sf::Vector2f(pos.x + 10, pos.y + 10)),
		mapName(name),
		layerMenu(sf::Vector2f(400, 100), sf::Vector2f(pos.x + 600, pos.y + 600 + 25))
	{
		setPosition(pos);
		
		layerMenu.addLayer(sf::Vector2f(800, 600), sf::Vector2f(pos.x + 600, pos.y + 10), "Layer_0");
		layerMenu.selectedLayer = &layerMenu.layers[0];
	}
	
	void updateLayerMenu()
	{
		layerMenu.update();
	}
	
	void updateToolbarTextures()
	{
		toolbar_textures.scrollWheel.update();
		
		if(toolbar_textures.texturePreviews.size())
		{
			for(TexturePackPreview & preview : toolbar_textures.texturePreviews)
			{
				// if the scroll wheel is selected, make it so the previews follow the cursors y-position
				if(WINDOW->hasFocus() && toolbar_textures.scrollWheel.isSelected) toolbar_textures.alignPreviews();

				if(Collision::AABB(*CURSOR, preview.closeButton))
				{
					if(WINDOW->hasFocus() && CURSOR->isPressed())
					{
						if(layerMenu.selectedLayer && layerMenu.selectedLayer->tiles.size())
						{							
							bool textureIsInUse = false;
							for(Tile & tile : layerMenu.selectedLayer->tiles)
							{
								if(tile.texturePATH == preview.PATH)
								{
									textureIsInUse = true;
									break;
								}
							}
							
							if(layerMenu.selectedLayer && textureIsInUse && !confirmationPrompt.isOpen())
							{
								confirmationPrompt.open(
									"This will remove the texture\npack from the list and delete\nall tiles that reference it\nin the editor grid!",
									[&](){
										
										// delete all tiles that reference the removed texture pack
										for(Tile & tile : layerMenu.selectedLayer->tiles)
										{
											if(tile.texturePATH == preview.PATH)
											{
												tile.texturePATH = "";
												tile.indexInTexturePack = -1;
												
												tile.setTexture(NULL);
												tile.setTextureRect(sf::IntRect(0,0,0,0));

												sf::Color color = tile.getFillColor();
												color.a = 0;
												tile.setFillColor(color);	
											}												
										}
										
										int i = 0;
										for(TexturePackPreview & p : toolbar_textures.texturePreviews)
										{
											if(&p == &preview)
											{
												toolbar_textures.texturePreviews.erase(toolbar_textures.texturePreviews.begin() + i);
												toolbar_textures.selectedTexturePreview = NULL;

												toolbar_textures.alignPreviews();
												break;
											}
											i++;
										}
									}
								);
							}
							else
							{								
								int i = 0;
								for(TexturePackPreview & p : toolbar_textures.texturePreviews)
								{
									if(&p == &preview)
									{
										toolbar_textures.texturePreviews.erase(toolbar_textures.texturePreviews.begin() + i);
										toolbar_textures.selectedTexturePreview = NULL;

										toolbar_textures.alignPreviews();
										break;
									}
									i++;
								}								
							}
							break;
						}						
					}
				}				
				
				
				// go through all its tiles, if there are any
				if(preview.tiles.size())
				{
					for(Tile & tile : preview.tiles)
					{
						// if the cursor is on top of a tile...
						if(Collision::AABB(*CURSOR, tile))
						{
							// make the hoverbox of that tile darker
							tile.hoverBox.setFillColor(sf::Color(
								tile.hoverColor.r, 
								tile.hoverColor.g, 
								tile.hoverColor.b, 
								tile.transparency)
							);
							
							// if a tile is clicked ...
							if(WINDOW->hasFocus() && sf::Mouse::isButtonPressed(sf::Mouse::Left))
							{
								toolbar_textures.selectedTexturePreview = &preview;		// set this preview to be the selected preview
								preview.selectedTile = &tile;			// set this tile to be the selected tile
								
								CURSOR->setMode(CursorMode::Paint);		// set cursor to painting mode
								CURSOR->setBody(tile);					// set cursor body to this tile texture
							}
						}	// if the tile is selected
						else if((&preview == toolbar_textures.selectedTexturePreview) && (&tile == preview.selectedTile))
						{
							// make the hoverbox of that tile darker
							tile.hoverBox.setFillColor(sf::Color(
								tile.hoverColor.r, 
								tile.hoverColor.g, 
								tile.hoverColor.b, 
								tile.transparency)
							);
						}
						else
						{
							tile.hoverBox.setFillColor(sf::Color::Transparent);
						}
					}
				}
			}
		}
	}	
	
	void updateEditorGrid()
	{
		// Don't update if there is no editor grid to update
		if(layerMenu.selectedLayer == nullptr) return;
		
		// if clicked, deselect tile
		if(sf::Mouse::isButtonPressed(sf::Mouse::Left))
		{
			layerMenu.selectedLayer->selectedTile = NULL;
		}		
		
		// Go through all tiles in editor grid, if there are any ...
		if(layerMenu.selectedLayer->tiles.size())
		{
			int indexInGrid = 0;
			for(Tile & tile : layerMenu.selectedLayer->tiles)
			{
				// if the cursor is on top of a tile...
				if(Collision::AABB(*CURSOR, tile) && Collision::AABB(*CURSOR, *layerMenu.selectedLayer) )
				{
					// set that tile's hoverbox to hover color
					tile.hoverBox.setFillColor(sf::Color(tile.hoverColor.r, tile.hoverColor.g, tile.hoverColor.b, tile.transparency));

					// handle mouse click depending on the cursor mode and whether
					// the cursor is on top of a tile or not
					if(WINDOW->hasFocus() && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
					{
						if(CURSOR->isPressed() && (CURSOR->cursorMode == CursorMode::Default))
						{
							layerMenu.selectedLayer->selectedTile = &tile;
						}

						if((CURSOR->cursorMode == CursorMode::Paint))
						{
							TexturePackPreview * preview = toolbar_textures.selectedTexturePreview;
							
							if(preview && preview->selectedTile)
							{
								tile.setTexture(preview->selectedTile->getTexture());
								tile.setTextureRect(preview->selectedTile->getTextureRect());
								
								tile.indexInTexturePack = preview->selectedTile->indexInTexturePack;
								tile.texturePATH = preview->selectedTile->texturePATH;

								sf::Color color = tile.getFillColor();
								color.a = 255;
								tile.setFillColor(color);

							}
						}
						else if(CURSOR->cursorMode == CursorMode::Delete)
						{
							if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
							{
								tile.texturePATH = "";
								tile.indexInTexturePack = -1;
								
								tile.setTexture(NULL);
								tile.setTextureRect(sf::IntRect(0,0,0,0));

								sf::Color color = tile.getFillColor();
								color.a = 0;
								tile.setFillColor(color);
							}							
						}

					}
					
				}
				else if(&tile == layerMenu.selectedLayer->selectedTile)
				{
					// set that tile's hoverbox to hover color
					tile.hoverBox.setFillColor(sf::Color(tile.hoverColor.r, tile.hoverColor.g, tile.hoverColor.b, tile.transparency));					
				}
				else
				{
					// set that tile's hoverbox to hover color
					tile.hoverBox.setFillColor(sf::Color::Transparent);										
				}
				indexInGrid++;
			}
		}
		
	}
	
	
	bool save(std::string fileName)
	{
		printf("mapname: %s", mapName.c_str());
		if(mapName.length())
		{			
			FILE * fp = fopen(fileName.c_str(), "w");

			if(fp == NULL) return false;
			
			fprintf(fp, "Map name: %s;\n", mapName.c_str());
			fprintf(fp, "tile size: [%d,%d];\n", (int)layerMenu.selectedLayer->tile_size, (int)layerMenu.selectedLayer->tile_size);
			fprintf(fp, "Grid size: [%d,%d]; # (%d total);\n", (int)layerMenu.selectedLayer->gridSize.x, (int)layerMenu.selectedLayer->gridSize.y, (int)layerMenu.selectedLayer->tiles.size());

			fprintf(fp, "\n");

			fprintf(fp, "texture Packs:\n{\n");
			if(toolbar_textures.texturePreviews.size())
			{
				for(int i = 0; i < toolbar_textures.texturePreviews.size(); i++)
				{
					fprintf(fp, "\t%d: %s;\n", i, toolbar_textures.texturePreviews[i].PATH.c_str());
				}
			}
			fprintf(fp, "}\n\n");

			if(layerMenu.layers.size())
			{
				for(Layer & layer : layerMenu.layers)
				{
					fprintf(fp, "%s:\n{\n", layer.name.c_str());
					if(layer.tiles.size())
					{
						std::string texturePATH = "";
						int textureIndex = 0;
						
						fprintf(fp, "\t");
						
						for(Tile & tile : layer.tiles)
						{					
							if((!texturePATH.length()) || ((tile.texturePATH.length()) && (tile.texturePATH != texturePATH)))
							{
								texturePATH = "";
								textureIndex = 0;
								for(TexturePackPreview & preview : toolbar_textures.texturePreviews)
								{
									if(preview.PATH == tile.texturePATH)
									{
										texturePATH = preview.PATH;
										break;
									}
									textureIndex += 1;
								}
							}
							
							if(tile.indexInTexturePack >= 0)
							{
								fprintf(fp, "[%d;%d]", textureIndex, tile.indexInTexturePack);
							}
							else
							{
								fprintf(fp, "[ ]");
							}
							
							fprintf(fp, ",%s", (tile.index && ((tile.index+1) != (layerMenu.selectedLayer->gridSize.x * layerMenu.selectedLayer->gridSize.y)) && !((tile.index+1) % layerMenu.selectedLayer->gridSize.x) ? "\n\t" : " " ));
							
						}
					}
					fprintf(fp, "\n}\n\n");
				}
			}
						
			fclose(fp);
			
			saveFile = fileName;			
			printf("Save file: %s\n", saveFile.c_str());
			
			return true;
		}
		return false;
	}
	
	void update()
	{
		updateToolbarTextures();
		updateEditorGrid();
		updateLayerMenu();		
	}
	
	void render(sf::RenderWindow & window)
	{
//		window.draw(*this);

		layerMenu.render(window);
		toolbar_textures.render(window);
	}
};

#endif // CONTEXT_HPP