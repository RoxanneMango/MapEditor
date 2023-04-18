#ifndef CURSOR_HPP
#define CURSOR_HPP

#include <SFML/Graphics.hpp>

#include "tile.hpp"

class Cursor;
Cursor * CURSOR;

enum CursorMode
{
	Default = 0,
	Paint,
	Select,
	Delete,
	Text
};

class Cursor : public sf::RectangleShape
{
	sf::RectangleShape body;
	float transparency = 0.66;
	float scale = 0.8;
public:

	CursorMode cursorMode = CursorMode::Default;
	bool isBodyVisible = true;
	bool isClicked = false;
	
	Cursor() : sf::RectangleShape(sf::Vector2f(1, 1)), body(sf::Vector2f(TILE_SIZE, TILE_SIZE))
	{
		setFillColor(sf::Color::Red);
		
		body.setOrigin(body.getSize().x/2, body.getSize().y/2);
		body.setScale(scale, scale);
		sf::Color c = body.getFillColor();
		body.setFillColor(sf::Color(c.r, c.g, c.b, c.a*transparency));
	}
	
	void update(sf::Vector2f pos)
	{
		setPosition(pos);
		body.setPosition(pos);
	}
	
	bool isPressed()
	{
		if(isClicked)
		{
			isClicked = false;
			return true;
		}
		return false;
	}
	
	void render(sf::RenderWindow & window)
	{
		window.draw(*this);
		if(isBodyVisible && body.getTexture() != NULL) window.draw(body);
	}
	
	void setMode(CursorMode mode)
	{
		cursorMode = mode;
		switch(mode)
		{
			case CursorMode::Default:
			{
				fp::SetCursor(sf::Cursor::Arrow);
				resetBody();
				break;
			}
			case CursorMode::Select:
			{
				fp::SetCursor(sf::Cursor::Cross);
				break;
			}
			case CursorMode::Paint:
			{
				fp::SetCursor(sf::Cursor::Arrow);
//				setBody(SELECTED_TEXTURE_PACK->tiles[Tile::selectedIndex]);
//				SELECTED_TEXTURE_PACK->isSelected = true;
				break;
			}
			case CursorMode::Delete:
			{
				fp::SetCursor(sf::Cursor::NotAllowed);
				resetBody();
//				SELECTED_TEXTURE_PACK->isSelected = false;
				break;
			}
			case CursorMode::Text:
			{
				fp::SetCursor(sf::Cursor::Text);
				break;
			}
			default:
			{
				fp::SetCursor(sf::Cursor::Arrow);
				break;
			}
		}
	}
	
	void setBody(Tile & tile)
	{
		body.setTexture(tile.getTexture());
		body.setTextureRect(tile.getTextureRect());
		sf::Color c = body.getFillColor();
		body.setFillColor(sf::Color(c.r, c.g, c.b, 255*transparency));		
	}
	void resetBody()
	{
		body.setTexture(NULL);
		body.setTextureRect(sf::IntRect(0,0,0,0));
		sf::Color c = body.getFillColor();
		body.setFillColor(sf::Color(c.r, c.g, c.b, 0));		
	}
	
};

#endif // CURSOR_HPP