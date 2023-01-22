#ifndef TEXTURE_PACK_PREVIEW_HPP
#define TEXTURE_PACK_PREVIEW_HPP

class TexturePackPreview : public Object
{
public:

	sf::RectangleShape closeButton;
	sf::Text closeButtonText;
	
	sf::Text title;

	std::vector<Tile> tiles;
	unsigned int tile_size = 64;
	int margin = 2;
	int textMargin = 10;
	
	bool isSelected = false;
	std::string ID = "";

	sf::Texture * texture = new sf::Texture();

	TexturePackPreview(sf::Vector2f size, sf::Vector2f pos, std::string PATH) : closeButtonText("x", *FONT, FONT_SIZE), title(PATH, *FONT, FONT_SIZE), ID(PATH), Object(size)
	{
		closeButton.setPosition(sf::Vector2f(pos.x+textMargin/2, pos.y+textMargin/2));
		closeButton.setSize(sf::Vector2f(FONT_SIZE, FONT_SIZE));
		closeButton.setFillColor(sf::Color::Red);
		closeButtonText.setPosition(sf::Vector2f(pos.x+textMargin, pos.y));
		
		title.setPosition(sf::Vector2f(pos.x + FONT_SIZE + textMargin, pos.y));
		pos.y += FONT_SIZE + textMargin;
		setPosition(pos);

		setFillColor(sf::Color::Green);
		setOutlineThickness(0.5);
		setOutlineColor(sf::Color::Black);

		if(!texture->loadFromFile(PATH))
		{
			printf("Could not load texture! (%s)\n", PATH.c_str());
			return;
		}


		TEXTURE_PACKS->push_back(texture);
		
		// number of tiles in the texturePack
		sf::Vector2u texturePackTileNum = sf::Vector2u(texture->getSize().x / tile_size, texture->getSize().y / tile_size);
		
		for(unsigned int y = 0; y < texturePackTileNum.y; y++)
		{
			for(unsigned int x = 0; x < texturePackTileNum.x; x++)
			{
				sf::Vector2f texturePos( x*tile_size, y*tile_size );

				Tile tile(sf::Vector2f(tile_size, tile_size), sf::Vector2f(pos.x + texturePos.x + margin*x, pos.y + texturePos.y + margin*y));
				tile.setTexture(texture);
				tile.index = x + (y*texturePackTileNum.x);
				tile.ID = PATH;
				tile.setTextureRect(sf::IntRect(texturePos.x, texturePos.y, tile_size, tile_size));
				tile.setOutlineThickness(1);
				tile.setOutlineColor(sf::Color(0, 0, 0, 150));
				tile.hoverColor = sf::Color::Black;
				tiles.push_back(tile);
			}
		}
		
		title.setStyle(sf::Text::Bold);
		title.setFillColor(sf::Color::White);
		
		setSize(sf::Vector2f(size.x, texturePackTileNum.y*tile_size + (margin*texturePackTileNum.y-1)));
		
	}
	
	unsigned int getHeight()
	{
		return ((FONT_SIZE + textMargin) + getSize().y + margin*2);
	}
	
	void changePosition(sf::Vector2f pos)
	{
		if(tiles.size())
		{
			closeButton.setPosition(sf::Vector2f(pos.x+textMargin/2, pos.y+textMargin/2));
			closeButtonText.setPosition(sf::Vector2f(pos.x+textMargin, pos.y));
			title.setPosition(sf::Vector2f(pos.x + FONT_SIZE + textMargin, pos.y));
			pos.y += FONT_SIZE + textMargin;
			setPosition(pos);


			// number of tiles in the texturePack
			sf::Vector2u texturePackTileNum = sf::Vector2u(texture->getSize().x / tile_size, texture->getSize().y / tile_size);
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
		window.draw(title);
		window.draw(closeButton);
		window.draw(closeButtonText);
		if(tiles.size())
		{
			for(Tile & tile : tiles)
			{
				tile.render(window);
			}
		}
	}
};
TexturePackPreview * SELECTED_TEXTURE_PACK = NULL;

#endif // TEXTURE_PACK_PREVIEW_HPP