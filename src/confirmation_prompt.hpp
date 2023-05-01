#ifndef CONFIRMATION_PROMPT_HPP
#define CONFIRMATION_PROMPT_HPP

#include <SFML/Graphics.hpp>

#include "line.hpp"
#include "option.hpp"
#include "cursor.hpp"

class ConfirmationPrompt : public sf::RectangleShape
{
private:
	sf::Text window_title = sf::Text("> Are you sure?", *FONT, FONT_SIZE);
	
	sf::Text description = sf::Text("", *FONT, FONT_SIZE);
	
	sf::Line topDivider;

	bool isVisible = false;
	
public:
	bool isConfirmed = false;

	std::function<void()> confirmationFunction = [](){};
	Option confirmButton;
	Option cancelButton;

	ConfirmationPrompt() : sf::RectangleShape(sf::Vector2f(200, 150))
	{
		sf::Color color = sf::Color(230, 230, 230);
		setFillColor(color);
		setOutlineThickness(4);
		setOutlineColor(sf::Color::Black);

//		setSize(sf::Vector2f(200, 150 + (description.getGlobalBounds().height)));
//		setPosition(sf::Vector2f(WINDOW->getSize().x/2 - getSize().x/2, WINDOW->getSize().y/2 - getSize().y/2));

		window_title.setFillColor(sf::Color::Black);
//		window_title.setPosition(sf::Vector2f(getPosition().x + 10, getPosition().y + 10));
		
		topDivider = sf::Line(sf::Vector2f(getPosition().x, getPosition().y + FONT_SIZE*2 + 10), getSize().x, 1);		

		description.setFillColor(sf::Color::Black);
//		description.setPosition(sf::Vector2f(getPosition().x, topDivider.getPosition().y + topDivider.getSize().y + 10));
		
		confirmButton = Option(" Yes  ", [&](){ isConfirmed = true; close(); }, sf::Vector2f(getPosition().x + 20, getPosition().y + getSize().y - FONT_SIZE - 10), true, sf::Color(200, 200, 200));
		confirmButton.setOutlineThickness(2);
		confirmButton.setOutlineColor(sf::Color::Black);

		cancelButton = Option("Cancel", [&](){ isConfirmed = false; close(); }, sf::Vector2f(getPosition().x + getSize().x - confirmButton.getSize().x - 20, getPosition().y + getSize().y - FONT_SIZE - 10), true, sf::Color(200, 200, 200));
		cancelButton.setOutlineThickness(2);
		cancelButton.setOutlineColor(sf::Color::Black);

	}
	
	void update()
	{
		confirmButton.update();
		cancelButton.update();
	}
	
	void render(sf::RenderWindow & window)
	{
		window.draw(*this);
		window.draw(window_title);

		window.draw(topDivider);

		window.draw(description);

		confirmButton.render(window);
		cancelButton.render(window);
	}
	
	void open(std::string desc, std::function<void()> confirmationFunction)
	{
		CURSOR->setMode(CursorMode::Default);
		
		this->description.setString(desc);
		this->confirmationFunction = confirmationFunction;

		float width = (desc.length() ? (description.getGlobalBounds().width + 40) : 200);

		setSize(sf::Vector2f( width >= 200 ? width : 200, 150 + (description.getGlobalBounds().height)));
		setPosition(sf::Vector2f(WINDOW->getSize().x/2 - getSize().x/2, WINDOW->getSize().y/2 - getSize().y/2));

		window_title.setPosition(sf::Vector2f(getPosition().x + 10, getPosition().y + 10));
		topDivider.setLength(getSize().x);
		topDivider.setPosition(sf::Vector2f(getPosition().x, getPosition().y + FONT_SIZE*2 + 10));
		description.setPosition(sf::Vector2f(getPosition().x + 10, topDivider.getPosition().y + topDivider.getSize().y + 10));
		
		confirmButton.changePosition(sf::Vector2f(getPosition().x + 40, getPosition().y + getSize().y - FONT_SIZE - 30));
		cancelButton.changePosition(sf::Vector2f(getPosition().x + getSize().x - confirmButton.getSize().x - 40, getPosition().y + getSize().y - FONT_SIZE - 30));

		isVisible = true;
		isConfirmed = false;
	}
		
	void close()
	{
		if(isConfirmed)
		{
			confirmationFunction();
		}
		isVisible = false;
	}
	
	bool isOpen()
	{
		return isVisible;
	}
};

#endif // CONFIRMATION_PROMPT_HPP