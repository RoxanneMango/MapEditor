#ifndef NEW_PROJECT_PANEL_HPP
#define NEW_PROJECT_PANEL_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

#include "option.hpp"
#include "line.hpp"
#include "input_field.hpp"
#include "collision.hpp"

class NewProjectPanel : public sf::RectangleShape
{
private:
	sf::Text window_title = sf::Text("> Create New Project", *FONT, FONT_SIZE);
	sf::Text closeButton_x = sf::Text("X", *FONT, FONT_SIZE);
	sf::RectangleShape closeButton;
	sf::Line topDivider;
	
public:

	InputField inputFieldProjectName;
	InputField inputFieldMapName;
	InputField inputFieldTileSize;
	InputField inputFieldMapSize_width;
	InputField inputFieldMapSize_height;

	InputField * currentInput = nullptr;

	std::vector<Option> options;
	std::vector<InputField *> inputs;

	bool isVisible = false;

	NewProjectPanel() : sf::RectangleShape(sf::Vector2f(1000, 700)),
		inputFieldProjectName(sf::Vector2f(200, 40), InputField::Type::String, "Project name:"),
		inputFieldMapName(sf::Vector2f(200, 40), InputField::Type::String, "Map name:"),
		inputFieldTileSize(sf::Vector2f(200, 40), InputField::Type::UnsignedInteger, "Tile size:"),
		inputFieldMapSize_width(sf::Vector2f(200, 40), InputField::Type::UnsignedInteger, "Map width (tiles):"),
		inputFieldMapSize_height(sf::Vector2f(200, 40), InputField::Type::UnsignedInteger, "Map height (tiles):")
	{
		sf::Color color = sf::Color(230, 230, 230);
		setFillColor(color);
		setOutlineThickness(2);
		setOutlineColor(sf::Color::Black);
		setPosition(sf::Vector2f(WINDOW->getSize().x/2 - getSize().x/2, WINDOW->getSize().y/2 - getSize().y/2));

		window_title.setFillColor(sf::Color::Black);
		window_title.setPosition(sf::Vector2f(getPosition().x + 10, getPosition().y + 10));
		
		closeButton.setFillColor(sf::Color(200, 80, 80));
		closeButton.setOutlineThickness(2);
		closeButton.setOutlineColor(sf::Color::Black);
		closeButton.setSize(sf::Vector2f(30, 30));
		closeButton.setPosition(sf::Vector2f(getPosition().x + getSize().x - closeButton.getSize().x - 5, getPosition().y + 5));
		
		closeButton_x.setFillColor(sf::Color::White);
		closeButton_x.setPosition(sf::Vector2f(closeButton.getPosition().x + 10, closeButton.getPosition().y + 3));

		topDivider = sf::Line(sf::Vector2f(getPosition().x, getPosition().y + closeButton.getSize().y + 10), getSize().x, 1);		
		
		sf::Vector2f inputPos = sf::Vector2f(getPosition().x + 50, topDivider.getPosition().y + 60);
		int inputMargin = inputFieldProjectName.getSize().y + 50;
		int i = 0;
		
		inputFieldProjectName.changePosition(sf::Vector2f(inputPos.x, inputPos.y + inputMargin*i++));
		inputFieldMapName.changePosition(sf::Vector2f(inputPos.x,  inputPos.y + inputMargin*i++));
		inputFieldTileSize.changePosition(sf::Vector2f(inputPos.x,  inputPos.y + inputMargin*i++));
		inputFieldMapSize_width.changePosition(sf::Vector2f(inputPos.x,  inputPos.y + inputMargin*i++));
		inputFieldMapSize_height.changePosition(sf::Vector2f(inputPos.x,  inputPos.y + inputMargin*i++));
		
		inputFieldProjectName.text.setString("projName");
		inputFieldMapName.text.setString("hello");
		inputFieldTileSize.text.setString("64");
		inputFieldMapSize_width.text.setString("10");
		inputFieldMapSize_height.text.setString("5");		
			
		inputs.push_back(&inputFieldProjectName);
		inputs.push_back(&inputFieldMapName);
		inputs.push_back(&inputFieldTileSize);
		inputs.push_back(&inputFieldMapSize_width);
		inputs.push_back(&inputFieldMapSize_height);
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

	void addOption(std::string label, sf::Vector2f pos, std::function<void()> func, bool highLightOnHover = true)
	{
		Option option(label, func, pos, highLightOnHover, sf::Color(200, 200, 200));
		option.setOutlineThickness(2);
		option.setOutlineColor(sf::Color::Black);
		
		options.push_back(option);
	}

	bool isOpen()
	{
		return isVisible;
	}
	
	void open()
	{
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
				
				for(Option & option : options)
				{
					option.update();
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

			for(Option & option : options)
			{
				option.render(window);
			}
			
			for(InputField * inputField : inputs)
			{
				inputField->render(window);
			}
		}		
	}
};

#endif // NEW_PROJECT_PANEL