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
	
	Layer(sf::Vector2f size, sf::Vector2f pos, std::string name, sf::Vector2u gridSize, Option option, sf::Color color) : 
		EditorGrid(size, pos, gridSize, color), 
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
	
	void renderOption(sf::RenderWindow & window)
	{
		option.render(window);
		window.draw(line);
	}
};

class LayerMenu : public sf::RectangleShape
{
public:
	ScrollWheel_vertical scrollWheel;
	float scrollScale = 1;

	std::vector<Layer> layers;
	Layer * selectedLayer = nullptr;

	LayerMenu(sf::Vector2f size, sf::Vector2f pos) : scrollWheel(sf::Vector2f(20, size.y), sf::Vector2f(pos.x + size.x, pos.y))
	{
		setSize(size);
		setPosition(pos);
		setFillColor(sf::Color(125, 145, 160));
		
//		scrollWheel.isEnabled = false;
	}

	~LayerMenu()
	{ }

	void adjustScrollWheel()
	{
		scrollScale = scrollWheel.adjustSize(layers[layers.size()-1].option.getPosition().y + layers[layers.size()-1].option.getSize().y);
	}

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

	void addLayer(sf::Vector2f size, sf::Vector2f pos, std::string name, sf::Vector2u gridSize)
	{
		sf::Vector2f oPos = sf::Vector2f(getPosition().x, getPosition().y + layers.size()*30);		
		
		layers.push_back(Layer(size, pos, name, gridSize, Option(name, [](){}, oPos), sf::Color::Black));
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

		alignLayers();
		adjustScrollWheel();
	}
	
	void alignLayers()
	{
		int i = 0;
		
		for(Layer & layer : layers)
		{
			unsigned int offset = 0;
			for(unsigned int o = 0; o < i; o++) offset += layer.option.getSize().y + layer.line.thickness;

			layer.option.changePosition(sf::Vector2f(getPosition().x, 
				scrollWheel.getPosition().y + offset - abs(scrollWheel.getPosition().y - scrollWheel.wheeliePiece.getPosition().y)*scrollScale
			));

			layer.line.setPosition(sf::Vector2f(getPosition().x,
					scrollWheel.getPosition().y + offset - abs(scrollWheel.getPosition().y - scrollWheel.wheeliePiece.getPosition().y) * scrollScale
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
//		window.draw(*this);
		for(Layer & layer : layers)
		{
			if(layer.option.getPosition().y >= (scrollWheel.getPosition().y - (layer.option.getSize().y)))
			{
				layer.renderOption(window);
			}
		}
		for(Layer & layer : layers)
		{
			layer.renderGrid(window);
		}
		scrollWheel.render(window);
	}
};

#endif // LAYER_MENU_HPP