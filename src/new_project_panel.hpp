#ifndef NEW_PROJECT_PANEL_HPP
#define NEW_PROJECT_PANEL_HPP

class NewProjectPanel : public sf::RectangleShape
{
private:
	sf::Text window_title = sf::Text("> Create New Project", *FONT, FONT_SIZE);
	sf::Text closeButton_x = sf::Text("X", *FONT, FONT_SIZE);
	sf::RectangleShape closeButton;
	sf::Line topDivider;
public:
	std::vector<Option> options;

	bool isVisible = false;

	NewProjectPanel() : sf::RectangleShape(sf::Vector2f(1000, 700))
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
		}		
	}
};

#endif // NEW_PROJECT_PANEL