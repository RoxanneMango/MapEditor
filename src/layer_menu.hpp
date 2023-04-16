#ifndef LAYER_MENU_HPP
#define LAYER_MENU_HPP

#include "editor_grid.hpp"
#include "option.hpp"
#include "line.hpp"
#include "tile.hpp"

class Layer : public EditorGrid
{
public:
	Option option;
	sf::Line line;
	
	std::string name = "";
	float opacity = 100.00;

	Layer(sf::Vector2f size, sf::Vector2f pos, std::string name) : 
		EditorGrid(size, pos),
		name(name)
	{
	
	}	
	
	Layer(sf::Vector2f size, sf::Vector2f pos, std::string name, Option option, sf::Color color) : 
		EditorGrid(size, pos, color), 
		option(option),
		line(sf::Vector2f(option.getPosition().x, option.getPosition().y + option.getSize().y), option.getSize().x),
		name(name)
	{ }
	
	void update()
	{
		option.update();
	}
	
	void renderGrid(sf::RenderWindow & window)
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
	
	void renderOptions(sf::RenderWindow & window)
	{
		option.render(window);
		window.draw(line);		
	}
};

class LayerMenu : public sf::RectangleShape
{
public:
	ScrollWheel_vertical scrollWheel;

	std::vector<Layer> layers;
	Layer * selectedLayer = nullptr;

	LayerMenu(sf::Vector2f size, sf::Vector2f pos) : scrollWheel(sf::Vector2f(20, size.y), sf::Vector2f(pos.x + size.x, pos.y))
	{
		setSize(size);
		setPosition(pos);
		setFillColor(sf::Color(125, 145, 160));
	}

	~LayerMenu()
	{ }

	void alignLayerLabels()
	{
		int i = 0;
		for(Layer & layer : layers)
		{
			layer.option.changePosition(sf::Vector2f(getPosition().x, getPosition().y + i*30));
			layer.line.setPosition(sf::Vector2f(layer.option.getPosition().x, layer.option.getPosition().y + layer.option.getSize().y));
			i++;
		}
	}

	void addLayer(sf::Vector2f size, sf::Vector2f pos, std::string name)
	{
		sf::Vector2f oPos = sf::Vector2f(getPosition().x, getPosition().y + layers.size()*30);		
		
		layers.push_back(Layer(size, pos, name, Option(name, [](){}, oPos), sf::Color::Black));
		Layer & layer = layers[layers.size()-1];

		int uiElementTotalBorderSize = 8;
		layer.option.changeSize(sf::Vector2f(getSize().x + uiElementTotalBorderSize, layer.option.getSize().y));
		layer.option.setFillColor(sf::Color(198, 198, 198));
		layer.option.hoverColor = sf::Color(140, 140, 140);

		layer.line = sf::Line(sf::Vector2f(layer.option.getPosition().x, layer.option.getPosition().y + layer.option.getSize().y), layer.option.getSize().x);		
	
		for(Layer & layer : layers)
		{
			layer.option.isSelected = false;
		}
		layer.option.isSelected = true;

		selectedLayer = &layer;	
		
		// Enable and resize the scrollbar once there are enough elements to warrant scrolling
//		sf::Vector2f wheelieSize = sf::Vector2f();
		scrollWheel.wheeliePiece.setSize(sf::Vector2f(scrollWheel.wheeliePiece.getSize().x, scrollWheel.getSize().y / layers.size()));
		scrollWheel.isEnabled = false;
		
//		return layer;
	}
	
	void alignLayers()
	{
		int i = 0;
		
		for(Layer & layer : layers)
		{
			unsigned int offset = 0;
			for(unsigned int o = 0; o < i; o++) offset += layer.option.getSize().y + layer.line.thickness;

			layer.option.changePosition(sf::Vector2f(getPosition().x, 
				scrollWheel.getPosition().y + offset - abs(scrollWheel.getPosition().y - scrollWheel.wheeliePiece.getPosition().y)
			));

			layer.line.setPosition(sf::Vector2f(getPosition().x,
					scrollWheel.getPosition().y + offset - abs(scrollWheel.getPosition().y - scrollWheel.wheeliePiece.getPosition().y)
				)
			);

			i++;
		}

	}	
	
	void update()
	{		
		int i = 0;
		for(Layer & layer : layers)
		{
			if(Collision::AABB(*CURSOR, layer.option))
			{
				if(CURSOR->isPressed())
				{
					for(Layer & layer : layers)
					{
						layer.option.isSelected = false;
					}
					selectedLayer = &layer;
					selectedLayer->option.isSelected = true;
				}
			}

			layer.drawBackPanel = (layers.size()-1) == i++;
			layer.update();
		}		
		
		scrollWheel.update();
		
		// if the scroll wheel is selected, make it so the previews follow the cursors y-position
		if(WINDOW->hasFocus() && scrollWheel.isSelected) alignLayers();
	}
	
	void render(sf::RenderWindow & window)
	{
		for(Layer & layer : layers)
		{
			layer.renderGrid(window);
		}
		window.draw(*this);
		for(Layer & layer : layers)
		{
			layer.renderOptions(window);
		}

		scrollWheel.render(window);
	}
};

#endif // LAYER_MENU_HPP