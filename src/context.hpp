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
	int updateDistance = 4;
	sf::RectangleShape updateDistanceBox;

	ConfirmationPrompt confirmationPrompt;
	TextPrompt textPrompt;

	ToolBar_textures toolbar_textures;

	LayerMenu layerMenu;

	std::string mapName = "";
	std::string saveFile = "";
	
	int hoverIndex = 0;
	bool isHovered = false;
	bool isSelected = false;
	
	sf::Vector2u gridSize = sf::Vector2u(0,0);
	
	sf::Clock tileObjectPlaceTimer;
	float tileObjectPlaceTimeout_ms = 250;
	
	Context(std::string name, sf::Vector2u gridSize, sf::Vector2f pos) : 
		sf::RectangleShape(sf::Vector2f(sf::VideoMode::getDesktopMode().width * WINDOW_SIZE_MULTIPLIER, (sf::VideoMode::getDesktopMode().height * WINDOW_SIZE_MULTIPLIER)-pos.y)),
		toolbar_textures(sf::Vector2f(540, 600), sf::Vector2f(pos.x + 10, pos.y + 10)),
		mapName(name),
		gridSize(gridSize),
		layerMenu(sf::Vector2f(400, 100), sf::Vector2f(pos.x + 600, pos.y + 600 + 25))
	{
		setPosition(pos);
		
		layerMenu.selectedLayer = nullptr;
		layerMenu.layerRect = sf::FloatRect(pos.x + 600, pos.y + 10, 800, 600);
		
		updateDistanceBox.setFillColor(sf::Color::Transparent);
		updateDistanceBox.setSize(sf::Vector2f(updateDistance * TILE_SIZE, updateDistance * TILE_SIZE));
		updateDistanceBox.setOutlineThickness(5);
		updateDistanceBox.setOutlineColor(sf::Color::Red);
		updateDistanceBox.setOrigin(updateDistanceBox.getSize().x/2, updateDistanceBox.getSize().y/2);
		
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
							for(Layer & layer : layerMenu.layers)
							{
								for(Tile & tile : layer.tiles)
								{
									if(tile.texturePATH == preview.PATH)
									{
										textureIsInUse = true;
										break;
									}
								}
							}
							
							if(layerMenu.selectedLayer && textureIsInUse && !confirmationPrompt.isOpen())
							{
								confirmationPrompt.open
								(
									"This will remove the texture\npack from the list and delete\nall tiles that reference it\nin the editor grid!",
									[&]()
									{
										
										// delete all tiles that reference the removed texture pack
										for(Layer & layer : layerMenu.layers)
										{
											for(Tile & tile : layer.tiles)
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
										}
										
										int i = 0;
										for(TexturePackPreview & p : toolbar_textures.texturePreviews)
										{
											if(&p == &preview)
											{
												toolbar_textures.adjustScrollWheel();
												
												toolbar_textures.texturePreviews.erase(toolbar_textures.texturePreviews.begin() + i);
												toolbar_textures.selectedTexturePreview = NULL;

												//toolbar_textures.alignPreviews();
												
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
										toolbar_textures.adjustScrollWheel();
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
							if(WINDOW->hasFocus() && CURSOR->isPressed())//sf::Mouse::isButtonPressed(sf::Mouse::Left))
							{
								toolbar_textures.selectedTexturePreview = &preview;		// set this preview to be the selected preview
								preview.selectedTile = &tile;			// set this tile to be the selected tile
								
								if(CURSOR->cursorMode != CursorMode::AddTileObject)
								{
									CURSOR->setMode(CursorMode::Paint);		// set cursor to painting mode									
								}
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
		// 1. -- Don't update if there is no editor grid to update
		// 2. -- if clicked, deselect tile
		// 3. -- If there are no tiles there is no need to update them
		if(layerMenu.selectedLayer == nullptr) return;
		if(!layerMenu.selectedLayer->tiles.size()) return;

		// make sure that no double hovers occur
		if((hoverIndex < layerMenu.selectedLayer->tiles.size()) && (hoverIndex >= 0))
		{
			Tile & tile = layerMenu.selectedLayer->tiles[hoverIndex];
			if(&tile != layerMenu.selectedTile)
			{
				tile.hoverBox.setFillColor(sf::Color::Transparent);
			}
		}
		if(sf::Mouse::isButtonPressed(sf::Mouse::Left))
		{
			if(layerMenu.selectedTile)
			{
				layerMenu.selectedTile->hoverBox.setFillColor(sf::Color::Transparent);
				layerMenu.selectedTile = NULL;
			}
		}

		// Whenever a zoom in/out action is done, recalculate the size and origin of the updateDistanceBox
		// so it always updates a constant O(n) number of tiles
		if(updateDistanceBox.getSize().x != (updateDistance*TILE_SIZE*layerMenu.selectedLayer->currentScale))
		{
			// Set size of updateDistanceBox according to the grid tile scaling
			updateDistanceBox.setSize(sf::Vector2f(updateDistance*TILE_SIZE*layerMenu.selectedLayer->currentScale, 
												   updateDistance*TILE_SIZE*layerMenu.selectedLayer->currentScale));
			// clamp size of updateDistanceBox to the size of the editorGrid view;
			if(updateDistanceBox.getSize().x > layerMenu.selectedLayer->getSize().x)
			{	updateDistanceBox.setSize(sf::Vector2f(layerMenu.selectedLayer->getSize().x, updateDistanceBox.getSize().y)); }
			if(updateDistanceBox.getSize().y > layerMenu.selectedLayer->getSize().y)
			{	updateDistanceBox.setSize(sf::Vector2f(updateDistanceBox.getSize().x, layerMenu.selectedLayer->getSize().y)); }
			// adjust the origin of the updateDistanceBox after resizing it
			updateDistanceBox.setOrigin(updateDistanceBox.getSize().x/2, updateDistanceBox.getSize().y/2);
		}

		// Calculate the update bounding rect (only update tiles within that rect)
		// 1. -- Set updateDistanceBox position to cursor position
		// 2. -- Get global bounds rect of updateDistanceBox
		// 3. -- Get global bounds rect of the first tile in the tile grid of the selected layer
		// 4. -- Calculate the starting point of the update bounding rect as a relative index in the tiles grid
		updateDistanceBox.setPosition(CURSOR->getPosition());

		sf::FloatRect distanceRect = updateDistanceBox.getGlobalBounds();
		sf::FloatRect firstTileRect = layerMenu.selectedLayer->tiles[0].getGlobalBounds();
		sf::Vector2f div = sf::Vector2f(distanceRect.left - firstTileRect.left,
										distanceRect.top - firstTileRect.top);
		
		// calculate bounding rect to update using the updateDistance
		sf::Vector2i PA = sf::Vector2i(div.x/(TILE_SIZE * layerMenu.selectedLayer->currentScale), 	// [A]  O---O  [B]
									   div.y/(TILE_SIZE * layerMenu.selectedLayer->currentScale));	//		|	|
		sf::Vector2i PB = sf::Vector2i(PA.x + updateDistance, PA.y);								//	    |	|
		sf::Vector2i PC = sf::Vector2i(PA.x, PA.y + updateDistance);								//	    |	|
		sf::Vector2i PD = sf::Vector2i(PA.x + updateDistance, PA.y + updateDistance);				// [C]	O---O  [D]
		
		// only proceed to update if the bounding rect falls within the tile grid
		if( (((PD.x) < 0) || ((PD.y) < 0)) || ( ((PA.x > 0) && ((PA.x) > gridSize.x)) || ((PA.y > 0) && ((PA.y) > gridSize.y))) )
		{
			return;
		}

		// Make sure the bounding rect point indexes do not fall outside of acceptable array index values
		if(PA.x < 0)			PA.x = PC.x = 0;
		if(PA.y < 0)			PA.y = PB.y = 0;
		if(PD.x > gridSize.x)	PD.x = PB.x = gridSize.x;
		if(PD.y > gridSize.y)	PD.y = PC.y = gridSize.y;

		isHovered = false;
		for(int y = PA.y; y < PD.y; y++)
		{
			for(int x = PA.x; x < PD.x; x++)
			{
				Tile & tile = layerMenu.selectedLayer->tiles[y*gridSize.x + x];

				// if the cursor is on top of a tile...
				if(Collision::AABB(*CURSOR, tile) && Collision::AABB(*CURSOR, *layerMenu.selectedLayer) )
				{
					isHovered = true;
					int hoverObjectIndex = -1;
					if(layerMenu.selectedLayer->tileObjects.size())
					{
						for(int i = 0; i < layerMenu.selectedLayer->tileObjects.size(); i++)
						{
							TileObject & object = layerMenu.selectedLayer->tileObjects[i];
							if(Collision::AABB(*CURSOR, object))
							{
								hoverObjectIndex = i;
								object.hoverBox.setFillColor(sf::Color(object.hoverColor.r, object.hoverColor.g, object.hoverColor.b, object.transparency));					
							}
							else
							{
								object.hoverBox.setFillColor(sf::Color::Transparent);												
							}							
						}
					}
					hoverIndex = (hoverObjectIndex >= 0) ? -1 : y*gridSize.x + x;
					
					// handle mouse click depending on the cursor mode and whether
					// the cursor is on top of a tile or not
					if(WINDOW->hasFocus() && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
					{
//						if(CURSOR->isPressed() && (CURSOR->cursorMode == CursorMode::Default))
						if((CURSOR->cursorMode == CursorMode::Default))
						{
//							if(layerMenu.selectedTile && layerMenu.selectedTile != &tile)
//							{
//								layerMenu.selectedTile->hoverBox.setFillColor(sf::Color::Transparent);							
//							}
							tile.hoverBox.setFillColor(sf::Color(tile.hoverColor.r, tile.hoverColor.g, tile.hoverColor.b, tile.transparency));
							layerMenu.selectedTile = &tile;
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
						else if((CURSOR->cursorMode == CursorMode::AddTileObject) && (tileObjectPlaceTimer.getElapsedTime().asMilliseconds() >= tileObjectPlaceTimeout_ms))
						{
							TexturePackPreview * preview = toolbar_textures.selectedTexturePreview;
							Layer * layer = layerMenu.selectedLayer;
							if(preview && preview->selectedTile)
							{
								sf::Vector2f tileObjectPos = tile.getPosition();
								if(layerMenu.selectedOption && !layerMenu.selectedOption->freeMovementToggleButton.isToggled)
								{								
									tileObjectPos = sf::Vector2f(
										CURSOR->getPosition().x - CURSOR->body.getSize().x/2,
										CURSOR->getPosition().y - CURSOR->body.getSize().y/2);
								}
								
								TileObject object(tile.getSize(), 
									tileObjectPos, 
									tile.index, 
									preview->selectedTile->indexInTexturePack, 
									preview->selectedTile->texturePATH);								
								
								bool isValidPos = true;
								for(TileObject & obj : layer->tileObjects)
								{
									if(Collision::AABB(obj, object))
									{
										isValidPos = false;
									}
								}

								if(isValidPos)
								{
									object.setTexture(preview->selectedTile->getTexture());
									object.setTextureRect(preview->selectedTile->getTextureRect());
									
									sf::Vector2f relativeTileObjectPos = sf::Vector2f(
										abs(layer->getPosition().x - tileObjectPos.x), 
										abs(layer->getPosition().y - tileObjectPos.y));
									object.setMapPosition(relativeTileObjectPos);
									
									layer->tileObjects.push_back(object);
									
									tileObjectPlaceTimer.restart();
								}
								
							}								
						}
						else if(CURSOR->cursorMode == CursorMode::Delete && CURSOR->isPressed())
						{
							if(hoverObjectIndex >= 0)
							{
								layerMenu.selectedLayer->tileObjects.erase(layerMenu.selectedLayer->tileObjects.begin() + hoverObjectIndex);
							}
							else
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
						else if(CURSOR->cursorMode == CursorMode::Collision)
						{
							tile.setCollision(true);
						}
						else if(CURSOR->cursorMode == CursorMode::NoCollision)
						{
							tile.setCollision(false);
						}

					}
				}
			}			
		}
		
		// set the hover tile
		if((hoverIndex < layerMenu.selectedLayer->tiles.size()) && (hoverIndex >= 0))
		{
			Tile & tile = layerMenu.selectedLayer->tiles[hoverIndex];
			if(!isHovered && (layerMenu.selectedTile != &tile))
			{
				tile.hoverBox.setFillColor(sf::Color::Transparent);
				hoverIndex = -1;
			}
			else
			{				
				tile.hoverBox.setFillColor(sf::Color(tile.hoverColor.r, tile.hoverColor.g, tile.hoverColor.b, tile.transparency));						
			}
		}
		
	}
	
	bool save(std::string fileName)
	{
//		printf("mapname: %s", mapName.c_str());
		if(mapName.length())
		{			
			FILE * fp = fopen(fileName.c_str(), "w");

			if(fp == NULL) return false;
			
			fprintf(fp, "%s\n", mapName.c_str());	// save file name
			fprintf(fp, "%s\n", mapName.c_str());	// map name
			fprintf(fp, "%d\n", (int)layerMenu.selectedLayer->tile_size);	// tile width
			fprintf(fp, "%d\n", (int)layerMenu.selectedLayer->tile_size);	// tile height
			fprintf(fp, "%d\n", (int)layerMenu.selectedLayer->gridSize.x);	// map width
			fprintf(fp, "%d\n", (int)layerMenu.selectedLayer->gridSize.y);	// map height
			fprintf(fp, "%d\n", toolbar_textures.texturePreviews.size());	// number of texture packs
			fprintf(fp, "%d\n", layerMenu.layers.size());					// number of layers

			fprintf(fp, "\n");
			if(toolbar_textures.texturePreviews.size())
			{
				for(int i = 0; i < toolbar_textures.texturePreviews.size(); i++)
				{
					fprintf(fp, "%s\n", toolbar_textures.texturePreviews[i].PATH.c_str());	// texture pack PATH(s)
				}
			}
			fprintf(fp, "\n");

			if(layerMenu.layers.size())
			{
				for(Layer & layer : layerMenu.layers)
				{
					fprintf(fp, "%s\n", layer.name.c_str());
					if(layer.tiles.size())
					{
						std::string texturePATH = "";
						int textureIndex = 0;
						
//						fprintf(fp, "\t");
						
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
//										printf("tp: %s ; tp: %s\n", preview.PATH.c_str(), tile.texturePATH.c_str());
//										printf("ti: %d\n", textureIndex);
										texturePATH = preview.PATH;
										break;
									}
									textureIndex += 1;
								}
							}
							
							if(tile.indexInTexturePack >= 0)
							{
								fprintf(fp, "%d;%d;%d", textureIndex, tile.indexInTexturePack, tile.hasCollision);
							}
							else
							{
								fprintf(fp, "x;%d", tile.hasCollision);
							}
							
							fprintf(fp, "%s", (tile.index && ((tile.index+1) != (layerMenu.selectedLayer->gridSize.x * layerMenu.selectedLayer->gridSize.y)) && !((tile.index+1) % layerMenu.selectedLayer->gridSize.x) ? "\n" : " " ));
							
						}
					}
					fprintf(fp, "\n\n");
				}
			}
						
			fclose(fp);
			
			saveFile = fileName;			
//			printf("Save file: %s\n", saveFile.c_str());
			printf("saved!\n");
			
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
//		window.draw(updateDistanceBox); 	// 'quad tree' implementation
		toolbar_textures.render(window);		
	}
};

#endif // CONTEXT_HPP