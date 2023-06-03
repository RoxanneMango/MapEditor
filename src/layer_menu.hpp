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
	
	bool isVisible = true;
	
	Layer(sf::Vector2f size, sf::Vector2f pos, std::string name, sf::Vector2u gridSize, sf::Color color) : 
		EditorGrid(size, pos, gridSize, color),
		name(name)
	{ }
	
	void renderGrid(sf::RenderWindow & window)
	{
		if(isVisible)
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
	}
};

class Cross
{
private:
	sf::Line line1;
	sf::Line line2;
		
public:
	Cross(sf::Vector2f pos, sf::Vector2f size, int thickness) : line1(pos, size.x, thickness), line2(pos, size.x, thickness)
	{
		line1.setOrigin(sf::Vector2f(line1.getSize().x/2, line1.getSize().y/2));
		line2.setOrigin(sf::Vector2f(line2.getSize().x/2, line2.getSize().y/2));
		
		line1.setRotation(45);
		line2.setRotation(315);
	}
	
	void setPosition(sf::Vector2f pos)
	{		
		line1.setPosition(pos);
		line2.setPosition(pos);
	}
	
	void render(sf::RenderWindow & window)
	{
		line1.render(window);
		line2.render(window);
	}
};

class ToggleOption : public Option
{
public:
	bool isToggled = true;
	
	int toggleBoxMargin = 10;
	sf::RectangleShape toggleBox;

	sf::Color unCheckedColor  = sf::Color(200, 200, 200);
	sf::Color hoverColor = sf::Color(180, 180, 180);
	sf::Color checkedColor = sf::Color(150, 150, 150);

	Cross cross;

	ToggleOption(sf::Vector2f pos, std::string label = "") : 
		Option(label, pos, false, sf::Color::Transparent),
		toggleBox(sf::Vector2f(15, 15)),
		cross(pos, toggleBox.getSize(), 2)
	{		
		text.setCharacterSize(FONT_SIZE*0.8);
		changePosition(pos);

		toggleBox.setPosition(sf::Vector2f(pos.x + getSize().x - toggleBox.getSize().x - toggleBoxMargin, pos.y));
		toggleBox.setOutlineThickness(2);
		toggleBox.setOutlineColor(sf::Color::Black);
		toggleBox.setFillColor(checkedColor);		
	}

	void changePosition(sf::Vector2f pos) override
	{
		setPosition(pos);
		toggleBox.setPosition(sf::Vector2f(
			pos.x + getSize().x - toggleBox.getSize().x, 
			pos.y + (getSize().y - (toggleBox.getSize().y - toggleBox.getOutlineThickness()))/2));
		text.setPosition(sf::Vector2f(
			pos.x + getSize().x - toggleBox.getSize().x - text.getGlobalBounds().width - toggleBoxMargin,
			pos.y + (getSize().y - text.getCharacterSize())/2));
			
		cross.setPosition(sf::Vector2f(
			toggleBox.getPosition().x + toggleBox.getSize().x/2.0,
			toggleBox.getPosition().y + toggleBox.getSize().y/2.0
			));
	}
	
	void update() override
	{
		if(isVisible)
		{
			if(Collision::AABB(*CURSOR, toggleBox))
			{
				if(!isToggled)
				{
					toggleBox.setFillColor(hoverColor);
				}
				if(CURSOR->isPressed())
				{
					isToggled = isToggled ? false : true;
					if(isToggled)
					{
						toggleBox.setFillColor(checkedColor);
					}
					else
					{
						toggleBox.setFillColor(unCheckedColor);
					}
				}
			}
			else
			{
				if(!isToggled)
				{
					toggleBox.setFillColor(unCheckedColor);
				}				
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
		window.draw(toggleBox);
		
		if(isToggled)
		{
			cross.render(window);
		}
	}
};

class LayerOption : public Option
{
private:
	bool isCollisionLayer = false;

public:	
	ToggleOption visibilityToggleButton;

	sf::Line line;
	
	LayerOption(std::string name, sf::Vector2f pos) : 
		Option(name, pos, true, sf::Color(198, 198, 198)),
		visibilityToggleButton(pos, "Visible")
	{
		hoverColor = sf::Color(140, 140, 140);
		line = sf::Line(sf::Vector2f(pos.x, pos.y + getSize().y), getSize().x);
	}
	
	void changePosition(sf::Vector2f pos) override
	{
		setPosition(sf::Vector2f(pos.x - 5, pos.y));
		hoverBox.setPosition(getPosition());

		int yPos = getSize().y - FONT_SIZE*1.2;		
		text.setPosition(sf::Vector2f(pos.x + 5, pos.y + (yPos ? yPos/2 : 0) ));
		
		visibilityToggleButton.changePosition(sf::Vector2f(pos.x + getSize().x - visibilityToggleButton.getSize().x - 30, pos.y));
		
		line = sf::Line(sf::Vector2f(pos.x, pos.y + getSize().y), getSize().x);		
	}
	
	void changeSize(sf::Vector2f size) override
	{		
		setSize(size);
		hoverBox.setSize(size);		

		int yPos = getSize().y - FONT_SIZE*1.2;		
		text.setPosition(sf::Vector2f(getPosition().x + 5, getPosition().y + (yPos ? yPos/2 : 0) ));
		
		visibilityToggleButton.changePosition(sf::Vector2f(getPosition().x + getSize().x - visibilityToggleButton.getSize().x, visibilityToggleButton.getPosition().y));		
			
		line = sf::Line(sf::Vector2f(getPosition().x, getPosition().y + size.y), size.x);
	}	
	
	virtual void update()
	{
		if(highLightOnHover && isVisible)
		{
			if(Collision::AABB(*CURSOR, *this))
			{
				hoverBox.setFillColor(sf::Color(hoverColor.r, hoverColor.g, hoverColor.b, transparency));
				if(CURSOR->isClicked)
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
		visibilityToggleButton.update();
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
		visibilityToggleButton.render(window);
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
			
			layer.isVisible = option.visibilityToggleButton.isToggled;
			
			if(Collision::AABB(*CURSOR, option))
			{
				if(CURSOR->isClicked)
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