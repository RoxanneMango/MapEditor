#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include <SFML/Graphics.hpp>

#include "toolbar_textures.hpp"
#include "editor_grid.hpp"

class Context;
Context * CURRENT_CONTEXT = NULL;

class Context : public sf::RectangleShape
{
public:
	ToolBar_textures toolbar_textures;
	EditorGrid editorGrid;
	
	Context(sf::Vector2f pos) : 
		sf::RectangleShape(sf::Vector2f(sf::VideoMode::getDesktopMode().width * WINDOW_SIZE_MULTIPLIER, (sf::VideoMode::getDesktopMode().height * WINDOW_SIZE_MULTIPLIER)-pos.y)),
		toolbar_textures(sf::Vector2f(540, 600), sf::Vector2f(pos.x + 10, pos.y + 10)),
		editorGrid(sf::Vector2f(800, 600), sf::Vector2f(pos.x + 600, pos.y + 10))
	{
		setPosition(pos);
		setFillColor(sf::Color::Blue);
	}
	
	void updateEditorGrid()
	{
		// if clicked, deselect tile
		if(sf::Mouse::isButtonPressed(sf::Mouse::Left))
		{
			editorGrid.selectedTile = NULL;
		}
		
		// Go through all tiles in editor grid, if there are any ...
		if(editorGrid.tiles.size())
		{
			int indexInGrid = 0;
			for(Tile & tile : editorGrid.tiles)
			{
				// if the cursor is on top of a tile...
				if(Collision::AABB(*CURSOR, tile))
				{
					// set that tile's hoverbox to hover color
					tile.hoverBox.setFillColor(sf::Color(tile.hoverColor.r, tile.hoverColor.g, tile.hoverColor.b, tile.transparency));

					// handle mouse click depending on the cursor mode and whether
					// the cursor is on top of a tile or not
					if(WINDOW->hasFocus() && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && Collision::AABB(*CURSOR, editorGrid))
					{
						if(CURSOR->cursorMode == CursorMode::Default)
						{
							editorGrid.selectedTile = &tile;
						}

						if((CURSOR->cursorMode == CursorMode::Paint))
						{
							TexturePackPreview * preview = toolbar_textures.selectedTexturePreview;
							
							if(preview && preview->selectedTile)
							{
								tile.setTexture(preview->selectedTile->getTexture());
								tile.setTextureRect(preview->selectedTile->getTextureRect());
								
								tile.indexInTexturePack = preview->selectedTile->indexInTexturePack;
								tile.texturePack = preview->selectedTile->texturePack;

								sf::Color color = tile.getFillColor();
								color.a = 255;
								tile.setFillColor(color);

							}
						}
						else if(CURSOR->cursorMode == CursorMode::Delete)
						{
							if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
							{
								tile.texturePack = NULL;
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
				else if(&tile == editorGrid.selectedTile)
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
	
	void update()
	{
		toolbar_textures.update();
		updateEditorGrid();
	}
	
	void render(sf::RenderWindow & window)
	{
		window.draw(*this);
		editorGrid.render(window);		
		toolbar_textures.render(window);
	}
};

#endif // CONTEXT_HPP