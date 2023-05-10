#ifndef RESIZE_GRID_PANEL_HPP
#define RESIZE_GRID_PANEL_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

#include <functional>

#include "option.hpp"
#include "line.hpp"
#include "input_field.hpp"
#include "collision.hpp"
#include "context.hpp"

class ResizeGridPanel : public sf::RectangleShape
{
private:
	sf::Text window_title = sf::Text("> Resize Editor Grid", *FONT, FONT_SIZE);
	sf::Text closeButton_x = sf::Text("X", *FONT, FONT_SIZE);
	sf::RectangleShape closeButton;
	sf::Line topDivider;
	
	std::function<void(unsigned int, unsigned int, EditorGrid::ResizeDirection)> action = [](unsigned int, unsigned int, EditorGrid::ResizeDirection){};
	
	static constexpr int inputMargin = 30;	

	EditorGrid::ResizeDirection resizeDirection;
	std::vector<Option> resizeAlignmentOptions;

	Option previewSizeButton;
	Option resizeButton;

	std::vector<Tile> tiles;
	sf::Vector2u gridSize;

	
	void resetAlignmentOptions()
	{
		for(Option & option : resizeAlignmentOptions)
		{
			option.isSelected = (false);
		}
	}	
	
	void initGrid(sf::Vector2u gridSize, sf::Color color = sf::Color::Black)
	{
		// clear grid first
		if(tiles.size())
		{
			tiles.clear();
		}
		
		// check if gridSize is sane
		if(!gridSize.x || !gridSize.y)
		{
			printf("Cannot have a grid with a x or y dimension of 0!\n");
			return;
		}
		
		int margin = 1;
		int textMargin = 10;
		
		sf::Color gridColor = color;
		gridColor.a = 200;
		
		this->gridSize = gridSize;
		
		sf::Vector2f pos = sf::Vector2f(getPosition().x + 300, getPosition().y + 70);
		
		unsigned int tile_size_x = (getPosition().x + getSize().x - pos.x - 70)/gridSize.x;
		unsigned int tile_size_y = (getPosition().y + getSize().y - pos.y - 120)/gridSize.y;

		unsigned int tile_size = tile_size_x < tile_size_y ? tile_size_x : tile_size_y;
		
		for(unsigned int y = 0; y < gridSize.y; y++)
		{
			for(unsigned int x = 0; x < gridSize.x; x++)
			{				
				sf::Vector2f texturePos( x*tile_size, y*tile_size );

				Tile tile(
					sf::Vector2f(tile_size, tile_size), 
					sf::Vector2f(pos.x + texturePos.x + margin*x, pos.y + texturePos.y + margin*y),
					(y*gridSize.x) + x // index
				);

				sf::Color c = tile.getFillColor();
				c.a = 0;
				tile.setFillColor(c);

				tile.hoverBox.setOutlineThickness(1);
				tile.hoverBox.setOutlineColor(gridColor);
				tile.hoverColor = sf::Color::Black;
				tiles.push_back(tile);
			}
		}
		
	}	
	
	
public:
	InputField inputFieldMapSize_width;
	InputField inputFieldMapSize_height;

	InputField * currentInput = nullptr;
	std::vector<InputField *> inputs;

	bool isVisible = false;

	void addAlignmentOption(std::string str, std::function<void()> fun, unsigned int ix, unsigned int iy)
	{
		int aSize = 50;
		int aMargin = aSize + 8; // outlinethickness*4
		sf::Vector2f aPos = sf::Vector2f(getPosition().x + 50, topDivider.getPosition().y + 30);
		
		resizeAlignmentOptions.push_back( Option( str, fun, 
			sf::Vector2f(aPos.x + aMargin*ix , aPos.y + aMargin*iy), 
			sf::Vector2f(aSize, aSize), 2, true, sf::Color::White, 255
		));
	}

	ResizeGridPanel() : sf::RectangleShape(sf::Vector2f(1000, 700)),
		inputFieldMapSize_width(sf::Vector2f(200, 40), InputField::Type::UnsignedInteger, "Map width (tiles):"),
		inputFieldMapSize_height(sf::Vector2f(200, 40), InputField::Type::UnsignedInteger, "Map height (tiles):"),
		previewSizeButton("Preview", [&]()
		{
			if(inputIsCorrrect())
			{
				initGrid(sf::Vector2u(inputFieldMapSize_width.getUnsignedInteger(), inputFieldMapSize_height.getUnsignedInteger()));//, resizeDirection);
			}
		}, sf::Vector2f(0,0)),
		resizeButton("    Resize    ", [&]()
		{
			if(inputIsCorrrect())
			{			
				action(inputFieldMapSize_width.getUnsignedInteger(), inputFieldMapSize_height.getUnsignedInteger(), resizeDirection);
				close();
			}
		}, sf::Vector2f(0,0))
		
	{
		// Panel framework		
		sf::Color color = sf::Color(230, 230, 230);
		setFillColor(color);
		setOutlineThickness(2);
		setOutlineColor(sf::Color::Black);
		setPosition(sf::Vector2f(WINDOW->getSize().x/2 - getSize().x/2, WINDOW->getSize().y/2 - getSize().y/2));
		//
		window_title.setFillColor(sf::Color::Black);
		window_title.setPosition(sf::Vector2f(getPosition().x + 10, getPosition().y + 10));
		//
		closeButton.setFillColor(sf::Color(200, 80, 80));
		closeButton.setOutlineThickness(2);
		closeButton.setOutlineColor(sf::Color::Black);
		closeButton.setSize(sf::Vector2f(30, 30));
		closeButton.setPosition(sf::Vector2f(getPosition().x + getSize().x - closeButton.getSize().x - 5, getPosition().y + 5));
		//
		closeButton_x.setFillColor(sf::Color::White);
		closeButton_x.setPosition(sf::Vector2f(closeButton.getPosition().x + 10, closeButton.getPosition().y + 3));
		//
		topDivider = sf::Line(sf::Vector2f(getPosition().x, getPosition().y + closeButton.getSize().y + 10), getSize().x, 1);
		
		int ix = 0;
		int iy = 0;
		
		// options
		addAlignmentOption("\\", [&](){ resizeDirection = EditorGrid::ResizeDirection::NorthWest; }, ix, iy); ix++;
		addAlignmentOption("^", [&](){  resizeDirection = EditorGrid::ResizeDirection::North;	  }, ix, iy); ix++;
		addAlignmentOption("/", [&](){  resizeDirection = EditorGrid::ResizeDirection::NorthEast; }, ix, iy); ix=0; iy++;
		//
		addAlignmentOption("<", [&](){  resizeDirection = EditorGrid::ResizeDirection::West;      }, ix, iy); ix++;
		addAlignmentOption("#", [&](){  resizeDirection = EditorGrid::ResizeDirection::Center;	  }, ix, iy); ix++;
		addAlignmentOption(">", [&](){  resizeDirection = EditorGrid::ResizeDirection::East;      }, ix, iy); ix=0; iy++;
		//
		addAlignmentOption("/", [&](){  resizeDirection = EditorGrid::ResizeDirection::SouthWest; }, ix, iy); ix++;
		addAlignmentOption("v", [&](){  resizeDirection = EditorGrid::ResizeDirection::South;	  }, ix, iy); ix++;
		addAlignmentOption("\\", [&](){ resizeDirection = EditorGrid::ResizeDirection::SouthEast; }, ix, iy);
		//
		resizeDirection = EditorGrid::ResizeDirection::Center;
		resizeAlignmentOptions[resizeDirection].isSelected = true;
		
		// inputs
		sf::Vector2f inputPos = sf::Vector2f(getPosition().x + 30, resizeAlignmentOptions[EditorGrid::ResizeDirection::South].getPosition().y + resizeAlignmentOptions[EditorGrid::ResizeDirection::South].getSize().y + 50);
		int inputMargin = inputFieldMapSize_width.getSize().y + 50;
		int i = 0;
		//
		inputFieldMapSize_width.changePosition(sf::Vector2f(inputPos.x,  inputPos.y + inputMargin*i++));
		inputFieldMapSize_height.changePosition(sf::Vector2f(inputPos.x,  inputPos.y + inputMargin*i++));
		//
		inputFieldMapSize_width.setFillColor(sf::Color::White);
		inputFieldMapSize_height.setFillColor(sf::Color::White);		
		//
		inputs.push_back(&inputFieldMapSize_width);
		inputs.push_back(&inputFieldMapSize_height);
		
		previewSizeButton.changeSize(sf::Vector2f(previewSizeButton.getSize().x + 20, previewSizeButton.getSize().y + 20));		
		previewSizeButton.changePosition(sf::Vector2f(
			inputs[inputs.size()-1]->getPosition().x, 
			inputs[inputs.size()-1]->getPosition().y + 
			inputs[inputs.size()-1]->getSize().y + 30));
		previewSizeButton.setOutlineColor(sf::Color::Black);
		previewSizeButton.setOutlineThickness(2);
		previewSizeButton.setFillColor(sf::Color::White);
		previewSizeButton.alignCenter();
		
		// Resize button
		resizeButton.changeSize(sf::Vector2f(resizeButton.getSize().x, resizeButton.getSize().y + 20));
		resizeButton.changePosition(sf::Vector2f(
			getPosition().x + getSize().x/2 - resizeButton.getSize().x/2, 
			getPosition().y + getSize().y - resizeButton.getSize().y - 30));
		resizeButton.setOutlineColor(sf::Color::Black);
		resizeButton.setOutlineThickness(2);
		resizeButton.setFillColor(sf::Color::White);
//		resizeButton.text.setStyle(sf::Text::Bold);
		resizeButton.alignCenter();
		
		
		
		
		
		
	}

	bool inputIsCorrrect()
	{
		bool isCorrect = true;
		for(InputField * input : inputs)
		{
			if(!input->isValid())
			{
				input->setFillColor(input->errorColor);
				isCorrect = false;
			}
		}
		return isCorrect;
	}

	bool isOpen()
	{
		return isVisible;
	}
	
	void open(sf::Vector2u gridSize, std::function<void(unsigned int, unsigned int, EditorGrid::ResizeDirection)> action)
	{		
		inputFieldMapSize_width.text.setString(std::to_string(gridSize.x));
		inputFieldMapSize_height.text.setString(std::to_string(gridSize.y));
	
		this->action = action;
		this->gridSize = gridSize;		
			
		initGrid(gridSize);
		
		isVisible = true;
	}
	
	void close()
	{
		isVisible = false;
	}	
	
	void update()
	{
		try
		{
			if(isVisible)
			{
				if(Collision::AABB(*CURSOR, closeButton))
				{
					if(CURSOR->isPressed())
					{
						close();
					}
					closeButton.setFillColor(sf::Color(255, 80, 80));
				}
				else
				{
					closeButton.setFillColor(sf::Color(200, 80, 80));
				}
				
				bool textHover = false;
				for(InputField * inputField : inputs)
				{
					if(Collision::AABB(*CURSOR, *inputField))
					{
						textHover = true;
						if(sf::Mouse::isButtonPressed(sf::Mouse::Left))
						{
							inputField->isSelected = true;
							inputField->setFillColor(inputField->color);
							inputField->setOutlineThickness(3);
							currentInput = inputField;
						}
					}
					else
					{
						if(sf::Mouse::isButtonPressed(sf::Mouse::Left))
						{
							inputField->isSelected = false;
							inputField->setOutlineThickness(2);
						}
					}					
					inputField->update();
				}
				CURSOR->setMode(textHover ? CursorMode::Text : CursorMode::Default);

				for(Option & option : resizeAlignmentOptions)
				{
					if(Collision::AABB(*CURSOR, option) && CURSOR->isClicked)
					{
						resetAlignmentOptions();
						option.isSelected = true;
					}
					option.update();
				}
				previewSizeButton.update();
				resizeButton.update();
			}
		}
		catch(...)
		{
			printf("Something went wrong while creating new project!\n");
		}	
	}
	
	void render(sf::RenderWindow & window)
	{
		if(isVisible)
		{
			window.draw(*this);
			window.draw(window_title);
			window.draw(closeButton);
			window.draw(topDivider);
			window.draw(window_title);
			window.draw(closeButton_x);

			for(Option & option : resizeAlignmentOptions)
			{
				option.render(window);
			}

			previewSizeButton.render(window);
			resizeButton.render(window);

			for(InputField * inputField : inputs)
			{
				inputField->render(window);
			}
			
			for(Tile & tile : tiles)
			{
				tile.render(window);
			}
			
		}		
	}
};

#endif // RESIZE_GRID_PANEL_HPP