#ifndef LAYER_MENU_HPP
#define LAYER_MENU_HPP

#include "editor_grid.hpp"
#include "option.hpp"
#include "line.hpp"
#include "tile.hpp"

class Layer : public EditorGrid
{
public:
	float opacity = 100.00;
	std::string name = "";
	
	Layer(sf::Vector2f size, sf::Vector2f pos, std::string name, sf::Vector2u gridSize, sf::Color color) : 
		EditorGrid(size, pos, gridSize, color),
		name(name)
	{ }
	
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
				if ( (((tile.getPosition().x + TILE_SIZE*currentScale) > getPosition().x) && ((tile.getPosition().y + TILE_SIZE*currentScale) > getPosition().y)) && 
					 ((tile.getPosition().x < (getPosition().x + getSize().x)) && (tile.getPosition().y < (getPosition().y + getSize().y)))	)
				{
					tile.render(window);
				}
			}
		}
	}
};

class LayerOption : public Option
{
private:
	bool isCollisionLayer = false;
	
	void initToggles()
	{
		collisionToggleButton = Option("Colission?", [&]()
		{
			printf("Colission %s!\n", (isCollisionLayer ? "On" : "Off"));
		},
		sf::Vector2f(getPosition().x + getSize().x/2, getPosition().y), sf::Vector2f(100, 30), 2);
	}	
	
public:	
	Option collisionToggleButton;

	sf::Line line;
	
	LayerOption(std::string name, sf::Vector2f pos) : 
		Option(name, pos, true, sf::Color(198, 198, 198))
	{
		hoverColor = sf::Color(140, 140, 140);
		line = sf::Line(sf::Vector2f(pos.x, pos.y + getSize().y), getSize().x);

		initToggles();
	}
	
	void changePosition(sf::Vector2f pos) override
	{
		setPosition(sf::Vector2f(pos.x - 5, pos.y));
		hoverBox.setPosition(getPosition());

		int yPos = getSize().y - FONT_SIZE*1.2;		
		text.setPosition(sf::Vector2f(pos.x + 5, pos.y + (yPos ? yPos/2 : 0) ));
		
		line = sf::Line(sf::Vector2f(pos.x, pos.y + getSize().y), getSize().x);
		initToggles();
		
	}
	
	void changeSize(sf::Vector2f size) override
	{		
		setSize(size);
		hoverBox.setSize(size);		

		int yPos = getSize().y - FONT_SIZE*1.2;		
		text.setPosition(sf::Vector2f(getPosition().x + 5, getPosition().y + (yPos ? yPos/2 : 0) ));
		
		line = sf::Line(sf::Vector2f(getPosition().x, getPosition().y + size.y), size.x);

		initToggles();
	}	
	
	virtual void update()
	{
		if(highLightOnHover && isVisible)
		{
			if(Collision::AABB(*CURSOR, *this))
			{
				hoverBox.setFillColor(sf::Color(hoverColor.r, hoverColor.g, hoverColor.b, transparency));
				if(CURSOR->isPressed())
				{
					doAction();
				}
			}
			else if(isSelected)
			{
				hoverBox.setFillColor(sf::Color(hoverColor.r, hoverColor.g, hoverColor.b, transparency));				
			}
			else
			{
				hoverBox.setFillColor(sf::Color::Transparent);
			}
		}
	}
	
	void render(sf::RenderWindow & window)
	{
		window.draw(*this);
		window.draw(hoverBox);
		if(text.getString().getSize())
		{
			window.draw(text);
		}
		window.draw(line);
	}
	
};

class LayerMenu : public sf::RectangleShape
{
public:
	ScrollWheel_vertical scrollWheel;
	float scrollScale = 1;

	sf::RectangleShape gridOutline = sf::RectangleShape();
	
	std::vector<Layer> layers;
	Layer * selectedLayer = nullptr;

	std::vector<LayerOption> options;
	LayerOption * selectedOption = nullptr;

	sf::FloatRect layerRect;

	LayerMenu(sf::Vector2f size, sf::Vector2f pos) : scrollWheel(sf::Vector2f(20, size.y), sf::Vector2f(pos.x + size.x, pos.y))
	{
		setSize(size);
		setPosition(pos);
		setFillColor(sf::Color(125, 145, 160));
		
		gridOutline.setFillColor(sf::Color::Transparent);
		gridOutline.setOutlineColor(sf::Color::Black);
		gridOutline.setOutlineThickness(1);
		
		scrollWheel.disable();
	}

	~LayerMenu()
	{ }

	void adjustScrollWheel()
	{
		scrollScale = scrollWheel.adjustSize(options[options.size()-1].getPosition().y + options[options.size()-1].getSize().y);
	}

	void addLayer(sf::Vector2f size, sf::Vector2f pos, std::string name, sf::Vector2u gridSize)
	{		
		layers.push_back(Layer(size, pos, name, gridSize, sf::Color::Black));
		selectedLayer = &layers[layers.size()-1];
		
		int uiElementTotalBorderSize = 8;
		sf::Vector2f oPos(getPosition().x, getPosition().y + options.size()*30);
		
		options.push_back(LayerOption(name, oPos));
		selectedOption = &options[options.size()-1];
		
		sf::Vector2f oSize(sf::Vector2f(getSize().x + uiElementTotalBorderSize, selectedOption->getSize().y));		
		selectedOption->changeSize(oSize);
		for(LayerOption & option : options)
		{
			option.isSelected = false;
		}
		selectedOption->isSelected = true;

		alignOptions();
		adjustScrollWheel();
		
	}
	
	void alignOptions()
	{
		int i = 0;
		
		for(LayerOption & option : options)
		{
			unsigned int offset = 0;
			for(unsigned int o = 0; o < i; o++) offset += option.getSize().y + option.line.thickness;

			option.changePosition(sf::Vector2f(getPosition().x, 
				scrollWheel.getPosition().y + offset - abs(scrollWheel.getPosition().y - scrollWheel.wheeliePiece.getPosition().y)*scrollScale
			));
			i++;
		}

	}	
	
	void update()
	{
		if(!options.size() || !layers.size()) return;
		
		for(int i = 0; i < layers.size(); i++)
		{
			LayerOption & option = options[i];
			Layer & layer = layers[i];
			
			if(Collision::AABB(*CURSOR, option))
			{
				if(CURSOR->isPressed())
				{
					for(LayerOption & o : options)
					{
						o.isSelected = false;
					}
					selectedLayer = &layer;
					selectedOption = &option;
					selectedOption->isSelected = true;
				}
			}
			layer.drawBackPanel = (layers.size()-1) == i;

			option.update();
			layer.update();
		}
		
		if(selectedLayer && selectedLayer->tiles.size())
		{
			Tile & firstTile = selectedLayer->tiles[0];
			Tile & lastTile  = selectedLayer->tiles[selectedLayer->tiles.size()-1];

			// set the gridOutline
			float tile_size = TILE_SIZE * selectedLayer->currentScale;
			float outlineThickness = firstTile.getOutlineThickness() * selectedLayer->currentScale;
			float margin = selectedLayer->margin * selectedLayer->currentScale;

			gridOutline.setPosition(firstTile.getPosition());
			gridOutline.setSize(sf::Vector2f(
				abs( gridOutline.getPosition().x - lastTile.getPosition().x ) + tile_size,
				abs( gridOutline.getPosition().y - lastTile.getPosition().y ) + tile_size));
		}
		
		scrollWheel.update();
		
		// if the scroll wheel is selected, make it so the previews follow the cursors y-position
		if(WINDOW->hasFocus() && scrollWheel.isSelected) alignOptions();
	}
	
	void render(sf::RenderWindow & window)
	{
		if(layers.size())
		{
			for(Layer & layer : layers)
			{
				layer.renderGrid(window);
			}
			window.draw(gridOutline);
		}

		window.draw(*this);

		if(options.size())
		{
			for(LayerOption & option : options)
			{
				if(option.getPosition().y >= (scrollWheel.getPosition().y - (option.getSize().y)))
				{
					option.render(window);
				}
			}
		}
		scrollWheel.render(window);		
	}
};

#endif // LAYER_MENU_HPP