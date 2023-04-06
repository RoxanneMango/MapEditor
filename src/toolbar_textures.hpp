#ifndef TOOLBAR_TEXTURES_HP
#define TOOLBAR_TEXTURES_HP

#include "object.hpp"
#include "texture_pack_preview.hpp"
#include "scroll_wheel.hpp"

#include <vector>
#include <SFML/Graphics.hpp>

class ToolBar_textures : public Object
{
	sf::Vector2f margin = sf::Vector2f(8, 8);
	sf::Vector2f textureSize;
	float texturesHeight = margin.y;
	
public:
	TexturePackPreview * selectedTexturePreview;
	std::vector<TexturePackPreview> texturePreviews;

	ScrollWheel_vertical scrollWheel;

	ToolBar_textures(sf::Vector2f size, sf::Vector2f position) : scrollWheel(sf::Vector2f(30, size.y), sf::Vector2f(position.x + size.x, position.y))
	{		
		setSize(size);

		setPosition(position);
		setFillColor(sf::Color(125, 145, 160));
		setOutlineThickness(3);
		setOutlineColor(sf::Color::Black);
		
		textureSize = sf::Vector2f(size.x - margin.x, size.x - margin.y);
		addTexturePreview("../assets/texturepack.png");
		addTexturePreview("../assets/pumpkin.png");
		
	}
	
	void alignPreviews()
	{
		for(unsigned int i = 0; i < texturePreviews.size(); i++)
		{
			unsigned int offset = 0;
			for(unsigned int o = 0; o < i; o++) offset += texturePreviews[o].getHeight() + texturePreviews[o].margin*2;
			texturePreviews[i].changePosition(sf::Vector2f(texturePreviews[i].getPosition().x, 
				scrollWheel.getPosition().y + offset + margin.y - abs(scrollWheel.getPosition().y - scrollWheel.wheeliePiece.getPosition().y)
			));
		}		
	}
	
	void update()
	{
		scrollWheel.update();
/*
		if(sf::Mouse::isButtonPressed(sf::Mouse::Left))
		{
			if(selectedTexturePreview)
			{
				selectedTexturePreview->selectedTile = NULL;
				selectedTexturePreview = NULL;
			}
		}
*/
		
		if(texturePreviews.size())
		{
			unsigned int i = 0;
			for(TexturePackPreview & preview : texturePreviews)
			{
				// if the scroll wheel is selected, make it so the previews follow the cursors y-position
				if(WINDOW->hasFocus() && scrollWheel.isSelected) alignPreviews();

				if(Collision::AABB(*CURSOR, preview.closeButton))
				{
					if(WINDOW->hasFocus() && CURSOR->isPressed())
					{
						texturePreviews.erase(texturePreviews.begin() + i);
						selectedTexturePreview = NULL;

						alignPreviews();
						break;
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
								selectedTexturePreview = &preview;		// set this preview to be the selected preview
								preview.selectedTile = &tile;			// set this tile to be the selected tile
								
								CURSOR->setMode(CursorMode::Paint);		// set cursor to painting mode
								CURSOR->setBody(tile);					// set cursor body to this tile texture
							}
						}	// if the tile is selected
						else if((&preview == selectedTexturePreview) && (&tile == preview.selectedTile))
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
				
				i++;
			}
		}
	}
	
	int addTexturePreview(std::string PATH)
	{
		sf::Vector2f position(sf::Vector2f(getPosition().x + margin.x/2, getPosition().y + texturesHeight));
		TexturePackPreview tpp(textureSize, position, PATH);

		if(tpp.tiles.size())
		{
			texturesHeight += tpp.getSize().y + margin.y + FONT_SIZE + tpp.textMargin;
			texturePreviews.push_back(tpp);
			
			alignPreviews();
			
			return 1;
		}
		return 0;
	}
	
	void render(sf::RenderWindow & window)
	{
		window.draw(*this);
		
		if(texturePreviews.size())
		{
			for(TexturePackPreview & object : texturePreviews)
			{
				object.render(window);
			}
		}
		scrollWheel.render(window);
		
	}
};

ToolBar_textures * TOOLBAR_TEXTURES;

#endif // TOOLBAR_TEXTURES_HPP