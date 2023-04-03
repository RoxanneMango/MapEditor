#ifndef EDITOR_GRID_HPP
#define EDITOR_GRID_HPP

#include "object.hpp"
#include "cursor.hpp"

#include <SFML/Graphics.hpp>

class EditorGrid : public Object
{
public:

	std::vector<Tile> tiles;
	unsigned int tile_size = 64;
	int margin = 2;
	int textMargin = 10;
	
	int selectedIndex = -1;
	bool isSelected = false;
	std::string ID = "";

	sf::Vector2u gridSize = sf::Vector2u(40, 40);

	sf::Texture * texture = new sf::Texture();

	EditorGrid(sf::Vector2f size, sf::Vector2f pos, std::string PATH) : ID(PATH), Object(size)
	{
		setPosition(pos);
		setSize(size);

		setFillColor(sf::Color(200,200,200));
		setOutlineThickness(0.5);
		setOutlineColor(sf::Color::Black);
		
		// number of tiles in the texturePack
		sf::Vector2u texturePackTileNum = gridSize;
	//	sf::Vector2u texturePackTileNum = sf::Vector2u(texture->getSize().x / tile_size, texture->getSize().y / tile_size);
		
		for(unsigned int y = 0; y < texturePackTileNum.y; y++)
		{
			for(unsigned int x = 0; x < texturePackTileNum.x; x++)
			{
				sf::Vector2f texturePos( x*tile_size, y*tile_size );

				Tile tile(sf::Vector2f(tile_size, tile_size), sf::Vector2f(pos.x + texturePos.x + margin*x, pos.y + texturePos.y + margin*y));

				tile.index = x + (y*texturePackTileNum.x);
				tile.ID = PATH;

				sf::Color color = tile.getFillColor();
				color.a = 0;
				tile.setFillColor(color);

				tile.hoverBox.setOutlineColor(sf::Color(0,0,0,200));
				tile.hoverColor = sf::Color::Black;
				tiles.push_back(tile);
			}
		}

	}
	
	void changePosition(sf::Vector2f pos)
	{
		if(tiles.size())
		{
			// number of tiles in the texturePack			
			sf::Vector2u texturePackTileNum = gridSize;
			float s = tiles[0].getScale().x;
//			float s = 1;
						
			for(unsigned int y = 0; y < texturePackTileNum.y; y++)
			{
				for(unsigned int x = 0; x < texturePackTileNum.x; x++)
				{
					sf::Vector2f texturePos( x * tile_size * s, y * tile_size *s );
					Tile & t = tiles[((y*texturePackTileNum.x)+x)];

					t.setPosition(sf::Vector2f((pos.x + texturePos.x + margin*x*s), (pos.y + texturePos.y + margin*y*s)));
					t.hoverBox.setPosition(sf::Vector2f((pos.x + texturePos.x + margin*x*s), (pos.y + texturePos.y + margin*y*s)));
				}
			}
		}
	}

	void changeScale(float scale)
	{
		if(tiles.size())
		{
			for(Tile & tile : tiles)
			{
				double oldScale = tile.getScale().x;
				tile.setScale(oldScale + scale, oldScale + scale);
				tile.hoverBox.setScale(oldScale + scale, oldScale + scale);
			}
			changePosition(tiles[0].getPosition());
		}
	}
	void resetGrid()
	{
		if(tiles.size())
		{
			for(Tile & tile : tiles)
			{
				tile.setScale(1,1);
				tile.hoverBox.setScale(1,1);
			}
			changePosition(getPosition());
		}
	}
	
	void update()
	{		
		if(tiles.size())
		{
			int tileIndex = 0;
			for(Tile & tile : tiles)
			{
				if(tile.isVisible)
				{
					if(Collision::AABB(*CURSOR, tile) && Collision::AABB(*CURSOR, *this))// || (tile.index == selectedIndex) )
					{
						tile.hoverBox.setFillColor(sf::Color(tile.hoverColor.r, tile.hoverColor.g, tile.hoverColor.b, tile.transparency));						

						if(CURSOR->cursorMode == CursorMode::Default)
						{
							if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
							{
								selectedIndex = tileIndex;
								isSelected = true;
							}
						}
						else
						{
							isSelected = false;
							selectedIndex = -1;
						}
						if(CURSOR->cursorMode == CursorMode::Paint)
						{
							if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && SELECTED_TEXTURE_PACK)
							{
								tile.setTexture(SELECTED_TEXTURE_PACK->tiles[Tile::selectedIndex].getTexture());
								tile.setTextureRect(SELECTED_TEXTURE_PACK->tiles[Tile::selectedIndex].getTextureRect());

								tile.index = Tile::selectedIndex;
								tile.ID = Tile::selectedID;

								sf::Color color = tile.getFillColor();
								color.a = 255;
								tile.setFillColor(color);

								selectedIndex = tile.index;								
							}
						}
						else if(CURSOR->cursorMode == CursorMode::Delete)
						{
							if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && SELECTED_TEXTURE_PACK)
							{
								tile.setTexture(NULL);
								tile.setTextureRect(sf::IntRect(0,0,0,0));

					//			tile.index = Tile::selectedIndex;
								tile.ID = "";

								sf::Color color = tile.getFillColor();
								color.a = 0;
								tile.setFillColor(color);
							}							
						}
					}
					else if(isSelected && (selectedIndex == tileIndex))
					{
						tile.hoverBox.setFillColor(sf::Color(tile.hoverColor.r, tile.hoverColor.g, tile.hoverColor.b, 60));
					}
					else
					{
						tile.hoverBox.setFillColor(sf::Color::Transparent);
					}

//					if((selectedIndex == tile.index)
//					{
//						tile.hoverBox.setFillColor(sf::Color(tile.hoverColor.r, tile.hoverColor.g, tile.hoverColor.b, tile.transparency));
//					}
//					else
//					{
//						tile.setOutlineColor(sf::Color::Black);
//					}
				}
				tileIndex++;
			}
		}
	}
	
	void render(sf::RenderWindow & window)
	{
		window.draw(*this);
		if(tiles.size())
		{
			for(Tile & tile : tiles)
			{
				tile.render(window);
			}
		}
	}
};

#endif // EDITOR_GRID_HPP