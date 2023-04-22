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

	sf::Color gridColor;

	EditorGrid(sf::Vector2f size, sf::Vector2f pos, sf::Color gridColor = sf::Color::Black) : gridColor(gridColor)
	{		
		setPosition(pos);
		setSize(size);

//		setFillColor(sf::Color(200,200,200));
		setFillColor(sf::Color::Transparent);
		setOutlineThickness(0.5);
		setOutlineColor(sf::Color::Black);

	}

	EditorGrid(sf::Vector2f size, sf::Vector2f pos, sf::Vector2u gridSize, sf::Color gridColor = sf::Color::Black) : gridColor(gridColor)
	{		
		setPosition(pos);
		setSize(size);

//		setFillColor(sf::Color(200,200,200));
		setFillColor(sf::Color::Transparent);
		setOutlineThickness(0.5);
		setOutlineColor(sf::Color::Black);

		initGrid(gridSize, gridColor);
	}
	

	void initGrid(sf::Vector2u gridSize, sf::Color color = sf::Color::Black)
	{
		// clear grid first
		if(tiles.size())
		{
			tiles.clear();
		}
		
		// check if gridSize is sane
		if(!gridSize.x || !gridSize.y)
		{
			printf("Cannot have a grid with a x or y dimension of 0!\n");
			return;
		}
		
		gridColor = color;
		gridColor.a = 200;
		
		this->gridSize = gridSize;
		
		sf::Vector2f pos = getPosition();
		
		for(unsigned int y = 0; y < gridSize.y; y++)
		{
			for(unsigned int x = 0; x < gridSize.x; x++)
			{				
				sf::Vector2f texturePos( x*tile_size, y*tile_size );

				Tile tile(
					sf::Vector2f(tile_size, tile_size), 
					sf::Vector2f(pos.x + texturePos.x + margin*x, pos.y + texturePos.y + margin*y),
					(y*gridSize.x) + x // index
				);

				sf::Color c = tile.getFillColor();
				c.a = 0;
				tile.setFillColor(c);

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