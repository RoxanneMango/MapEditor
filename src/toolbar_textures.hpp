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
		if(texturePreviews.size())
		{
			for(unsigned int i = 0; i < texturePreviews.size(); i++)
			{
				if(scrollWheel.isSelected)
				{
					alignPreviews();
				}
				
				if(Collision::AABB(*CURSOR, texturePreviews[i]) && CURSOR->isClicked)
				{
//					printf("texturePackPreview selected\n");
					texturePreviews[i].isSelected = true;
					SELECTED_TEXTURE_PACK = &texturePreviews[i];
				}
				else
				{
					texturePreviews[i].isSelected = false;
				}
				
				if(Collision::AABB(*CURSOR, texturePreviews[i].closeButton))
				{
					if(CURSOR->isPressed())
					{
						texturePreviews.erase(texturePreviews.begin() + i);
						SELECTED_TEXTURE_PACK = NULL;

						alignPreviews();

						break;
					}
				}
				texturePreviews[i].update();
			}
		}
	}
	
	int addTexturePreview(std::string PATH)
	{
		sf::Vector2f position(sf::Vector2f(getPosition().x + margin.x/2, getPosition().y + texturesHeight));
		TexturePackPreview tpp(texturePreviews.size(), textureSize, position, PATH);

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