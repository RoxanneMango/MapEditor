#ifndef TOOLBAR_TEXTURES_HP
#define TOOLBAR_TEXTURES_HP

#include "object.hpp"
#include "texture_pack_preview.hpp"
#include "scroll_wheel.hpp"

#include <vector>
#include <SFML/Graphics.hpp>

class ToolBar_textures : public Object
{
	sf::Vector2f textureSize;
	float texturesHeight = margin.y;
	
	float scrollScale = 1;
	
public:
	sf::Vector2f margin = sf::Vector2f(8, 8);
	
	TexturePackPreview * selectedTexturePreview;
	std::vector<TexturePackPreview> texturePreviews;

	ScrollWheel_vertical scrollWheel;

	ToolBar_textures(sf::Vector2f size, sf::Vector2f position) : scrollWheel(sf::Vector2f(30, size.y), sf::Vector2f(position.x + size.x, position.y))
	{		
		setSize(size);

		scrollWheel.wheeliePiece.setSize(sf::Vector2f(scrollWheel.wheeliePiece.getSize().x, size.y));

		setPosition(position);
		setFillColor(sf::Color(125, 145, 160));
		setOutlineThickness(3);
		setOutlineColor(sf::Color::Black);
		
		textureSize = sf::Vector2f(size.x - margin.x, size.x - margin.y);
		addTexturePreview("C:\\Users\\Roxanne\\Desktop\\Programming\\MapEditor\\assets/texturepack.png");
//		addTexturePreview(R"(C:\Users\Roxanne\Desktop\Programming\MapEditor\assets/texturepack.png)");		
//		addTexturePreview("C:/Users/Roxanne/Desktop/Programming/MapEditor/assets/texturepack.png");		
//		addTexturePreview("C:/Users/Roxanne/Desktop/Programming/MapEditor/assets/texturepack.png");		
		
	}
	
	void alignPreviews()
	{
		for(unsigned int i = 0; i < texturePreviews.size(); i++)
		{
			unsigned int offset = 0;

			for(unsigned int o = 0; o < i; o++)
			{
				offset += texturePreviews[o].getHeight() + texturePreviews[o].margin*2;
			}

			texturePreviews[i].changePosition(sf::Vector2f(texturePreviews[i].getPosition().x, 
				scrollWheel.getPosition().y + offset + margin.y - abs(scrollWheel.getPosition().y - scrollWheel.wheeliePiece.getPosition().y)*scrollScale
			));
		}
	}
	
	void update()
	{
		
	}
	
	void adjustScrollWheel()
	{
		scrollScale = scrollWheel.adjustSize(texturePreviews[texturePreviews.size()-1].getPosition().y + texturePreviews[texturePreviews.size()-1].getSize().y + margin.y*2);
		if(scrollScale == 1)
		{
			if(texturePreviews.size())
			{
				scrollWheel.wheeliePiece.setSize(sf::Vector2f(scrollWheel.wheeliePiece.getSize().x, scrollWheel.getSize().y));
				scrollWheel.wheeliePiece.setPosition(sf::Vector2f(scrollWheel.wheeliePiece.getPosition().x, scrollWheel.getPosition().y));
				
				alignPreviews();
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
			
			adjustScrollWheel();
			
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

ToolBar_textures * TOOLBAR_TEXTURES = nullptr;

#endif // TOOLBAR_TEXTURES_HPP