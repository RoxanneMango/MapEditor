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
		editorGrid(sf::Vector2f(800, 600), sf::Vector2f(pos.x + 600, pos.y + 10), "Editor_Grid")
	{
		setPosition(pos);
		setFillColor(sf::Color::Blue);
	}
	
	void update()
	{
		toolbar_textures.update();
		editorGrid.update();
	}
	
	void render(sf::RenderWindow & window)
	{
		window.draw(*this);
		editorGrid.render(window);		
		toolbar_textures.render(window);
	}
};

#endif // CONTEXT_HPP