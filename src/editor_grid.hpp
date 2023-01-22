#ifndef EDITOR_GRID_HPP
#define EDITOR_GRID_HPP

#include "object.hpp"

#include <SFML/Graphics.hpp>

class EditorGrid : public Object
{
public:

	std::vector<Tile> tiles;
	unsigned int tile_size = 64;
	int margin = 2;
	int textMargin = 10;
	
	bool isSelected = false;
	std::string ID = "";

	sf::Texture * texture = new sf::Texture();

	EditorGrid(sf::Vector2f size, sf::Vector2f pos, std::string PATH) : ID(PATH), Object(size)
	{
		setPosition(pos);

		setFillColor(sf::Color::Blue);
		setOutlineThickness(0.5);
		setOutlineColor(sf::Color::Black);
		
		// number of tiles in the texturePack
		sf::Vector2u texturePackTileNum = sf::Vector2u(40, 40);
	//	sf::Vector2u texturePackTileNum = sf::Vector2u(texture->getSize().x / tile_size, texture->getSize().y / tile_size);
		
		for(unsigned int y = 0; y < texturePackTileNum.y; y++)
		{
			for(unsigned int x = 0; x < texturePackTileNum.x; x++)
			{
				sf::Vector2f texturePos( x*tile_size, y*tile_size );

				Tile tile(sf::Vector2f(tile_size, tile_size), sf::Vector2f(pos.x + texturePos.x + margin*x, pos.y + texturePos.y + margin*y));

//				tile.setTexture(texture);
				tile.index = x + (y*texturePackTileNum.x);
				tile.ID = PATH;
//				tile.setTextureRect(sf::IntRect(texturePos.x, texturePos.y, tile_size, tile_size));
				tile.setOutlineThickness(1);
				tile.setOutlineColor(sf::Color(0, 0, 0, 150));
				tile.hoverColor = sf::Color::Black;
				tiles.push_back(tile);
			}
		}
		
		setSize(sf::Vector2f(size.x, texturePackTileNum.y*tile_size + (margin*texturePackTileNum.y-1)));
		
	}
	
	void changePosition(sf::Vector2f pos)
	{
		if(tiles.size())
		{
			pos.y += FONT_SIZE + textMargin;
			setPosition(pos);

			// number of tiles in the texturePack
			
			sf::Vector2u texturePackTileNum = sf::Vector2u(40, 40);
			
//			sf::Vector2u texturePackTileNum = sf::Vector2u(texture->getSize().x / tile_size, texture->getSize().y / tile_size);
			for(unsigned int y = 0; y < texturePackTileNum.y; y++)
			{
				for(unsigned int x = 0; x < texturePackTileNum.x; x++)
				{
					sf::Vector2f texturePos( x*tile_size, y*tile_size );
					tiles[((y*texturePackTileNum.x)+x)].setPosition(sf::Vector2f(pos.x + texturePos.x + margin*x, pos.y + texturePos.y + margin*y));
					tiles[((y*texturePackTileNum.x)+x)].hoverBox.setPosition(sf::Vector2f(pos.x + texturePos.x + margin*x, pos.y + texturePos.y + margin*y));
				}
			}

		}
	}
	
	void update()
	{		
		if(tiles.size())
		{
			for(Tile & tile : tiles)
			{
				if(tile.isVisible)
				{
					if(Collision::AABB(*CURSOR, tile) || ((tile.index == Tile::selectedIndex) && (tile.ID == Tile::selectedID)) )
					{
						tile.hoverBox.setFillColor(sf::Color(tile.hoverColor.r, tile.hoverColor.g, tile.hoverColor.b, tile.transparency));						
						if(tile.index != Tile::selectedIndex)
						{
							if(CURSOR->isPressed())
							{
								Tile::selectedIndex = tile.index;
								Tile::selectedID = tile.ID;
								printf("Tile_index: %d\n", Tile::selectedIndex);
							}
						}
					}
					else
					{
						tile.hoverBox.setFillColor(sf::Color::Transparent);
					}
					if((tile.index == Tile::selectedIndex) && (tile.ID == Tile::selectedID))
					{
						tile.hoverBox.setFillColor(sf::Color(tile.hoverColor.r, tile.hoverColor.g, tile.hoverColor.b, tile.transparency));
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
		if(tiles.size())
		{
			for(Tile & tile : tiles)
			{
				tile.render(window);
			}
		}
	}
};
EditorGrid * SELECTED_EDITOR_GRID = NULL;

#endif // EDITOR_GRID_HPP