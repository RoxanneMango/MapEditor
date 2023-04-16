#ifndef EDITOR_GRID_HPP
#define EDITOR_GRID_HPP

#include "object.hpp"
#include "tile.hpp"
#include "cursor.hpp"

#include <SFML/Graphics.hpp>

class EditorGrid : public Object
{
public:
	int margin = 2;
	int textMargin = 10;
	unsigned int tile_size = 64;

	sf::Texture * texture = new sf::Texture();
	sf::Vector2u gridSize = sf::Vector2u(8, 8);

	Tile * selectedTile = NULL;
	std::vector<Tile> tiles;

	bool drawBackPanel = false;

	EditorGrid(sf::Vector2f size, sf::Vector2f pos, sf::Color gridColor = sf::Color::Black)
	{		
		setPosition(pos);
		setSize(size);

//		setFillColor(sf::Color(200,200,200));
		setFillColor(sf::Color::Transparent);
		setOutlineThickness(0.5);
		setOutlineColor(sf::Color::Black);
		
		// number of tiles in the texturePack
		sf::Vector2u texturePackTileNum = gridSize;
		
		gridColor.a = 200;
		
		for(unsigned int y = 0; y < texturePackTileNum.y; y++)
		{
			for(unsigned int x = 0; x < texturePackTileNum.x; x++)
			{				
				sf::Vector2f texturePos( x*tile_size, y*tile_size );

				Tile tile(
					sf::Vector2f(tile_size, tile_size), 
					sf::Vector2f(pos.x + texturePos.x + margin*x, pos.y + texturePos.y + margin*y),
					(y*texturePackTileNum.x) + x // index
				);

				sf::Color color = tile.getFillColor();
				color.a = 0;
				tile.setFillColor(color);

				tile.hoverBox.setOutlineColor(gridColor);
				tile.hoverColor = sf::Color::Black;
				tiles.push_back(tile);
			}
		}

	}
	
	~EditorGrid()
	{
		
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
	
	void render(sf::RenderWindow & window)
	{
		if(drawBackPanel)
		{
			window.draw(*this);
		}
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