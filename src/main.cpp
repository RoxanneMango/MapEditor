#include <SFML/Graphics.hpp>

#include "globals.hpp"
#include "constants.hpp"
#include "object.hpp"

#include "cursor.hpp"
#include "option.hpp"
#include "user_interface.hpp"

void initResources();
void freeResources();

int
main(int argc, char ** argv)
{
	int screenWidth = sf::VideoMode::getDesktopMode().width * 0.8;
	int screenHeight = sf::VideoMode::getDesktopMode().height * 0.8;	

	initResources();
	
	UserInterface UI(screenWidth, screenHeight);
	
	WINDOW = new sf::RenderWindow;
	WINDOW->create(sf::VideoMode(screenWidth, screenHeight), WINDOW_TITLE, sf::Style::Default);
	WINDOW->setFramerateLimit(FRAME_RATE_LIMIT);
	WINDOW->setVerticalSyncEnabled(true);
	WINDOW->setActive(true);
	WINDOW->setView(sf::View(sf::FloatRect(VIEW_CENTER_X, VIEW_CENTER_Y, screenWidth, screenHeight)));
	WINDOW->setKeyRepeatEnabled(false);

	sf::Image window_icon;
	window_icon.loadFromFile("../assets/pumpkin.png");
	WINDOW->setIcon(window_icon.getSize().x, window_icon.getSize().y, window_icon.getPixelsPtr());	

	sf::Event event;

//	sf::Color clearColor = sf::Color(150, 150, 180);
	sf::Color clearColor = sf::Color(125, 145, 160);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* Main loop starts here ...																			 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	while (WINDOW->isOpen())
	{
		try
		{
			CURSOR->isPressed = false;
			while (WINDOW->pollEvent(event))
			{
				if(event.type == sf::Event::Closed) WINDOW->close();
				if(event.type == sf::Event::Resized)
				{
					WINDOW->setView(sf::View(sf::FloatRect(0, 0, event.size.width, event.size.height)));
				}
				if(event.type == sf::Event::KeyPressed)
				{
					if(event.key.code == sf::Keyboard::Q || event.key.code == sf::Keyboard::Space)
					{
						WINDOW->close();
					}
				}
				if(event.type == sf::Event::MouseButtonPressed)
				{
					if(event.mouseButton.button == sf::Mouse::Button::Left)
					{
						CURSOR->isPressed = true;
					}
				}
			}
			CURSOR->update(sf::Vector2f(sf::Mouse::getPosition(*WINDOW).x, sf::Mouse::getPosition(*WINDOW).y));
			
			UI.update();
			
			WINDOW->clear(clearColor);

			UI.render(*WINDOW);
			CURSOR->render(*WINDOW);

			WINDOW->display();
		}
		catch(...)
		{
			printf("Something went wrong!\n");
		}
	}
	
	freeResources();
	
	return 0;
}

void initResources()
{
	FONT = new sf::Font();
	CURSOR = new Cursor();
	DROP_DOWN_MENUS = new std::vector<ToolBar_dropdown>;
	
	TEXTURE_PACKS = new std::vector<sf::Texture *>;
	
	for(unsigned int i = 0; !FONT->loadFromFile("../assets/CONSOLA.TTF") || (i < 5); i++);
}
void freeResources()
{
	if(TEXTURE_PACKS->size())
	{
		for(sf::Texture * texture : *TEXTURE_PACKS)
		{
			delete texture;
		}
	}
	delete FONT;
}