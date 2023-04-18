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
	int screenWidth = sf::VideoMode::getDesktopMode().width * WINDOW_SIZE_MULTIPLIER;
	int screenHeight = sf::VideoMode::getDesktopMode().height * WINDOW_SIZE_MULTIPLIER;	

	initResources();
	
	WINDOW = new sf::RenderWindow;
	WINDOW->create(sf::VideoMode(screenWidth, screenHeight), WINDOW_TITLE, sf::Style::Default);
	WINDOW->setFramerateLimit(FRAME_RATE_LIMIT);
	WINDOW->setVerticalSyncEnabled(false);
	WINDOW->setActive(true);
	WINDOW->setView(sf::View(sf::FloatRect(VIEW_CENTER_X, VIEW_CENTER_Y, screenWidth, screenHeight)));
//	WINDOW->setKeyRepeatEnabled(false);

	sf::Image window_icon;
	window_icon.loadFromFile("../assets/pumpkin.png");
	WINDOW->setIcon(window_icon.getSize().x, window_icon.getSize().y, window_icon.getPixelsPtr());	

	sf::Event event;

//	sf::Color clearColor = sf::Color(150, 150, 180);
//	sf::Color clearColor = sf::Color(125, 145, 160);

	sf::Color clearColor = sf::Color(200,200,200);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* Main loop starts here ...																			 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

//	printf("argv: %s", argv[0]);

	fp::SetCursor(sf::Cursor::Arrow);

	UserInterface UI(screenWidth, screenHeight);

	while (WINDOW->isOpen())
	{
		try
		{
			CURSOR->isClicked = false;
//			CURSOR->isClicked = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

			while (WINDOW->pollEvent(event))
			{
				if(event.type == sf::Event::Closed) WINDOW->close();
				if(event.type == sf::Event::Resized)
				{
					WINDOW->setView(sf::View(sf::FloatRect(0, 0, event.size.width, event.size.height)));
				}
				if(event.type == sf::Event::KeyPressed && WINDOW->hasFocus())
				{
					if(event.key.code == sf::Keyboard::Escape) WINDOW->close();
					
					if(event.key.code == sf::Keyboard::F) CURSOR->setMode(CursorMode::Default);
					if(event.key.code == sf::Keyboard::Q) CURSOR->setMode(CursorMode::Delete);					

					// echo back characters in the input field if one is selected
					// this part here only checks if the backspace key is pressed
					// if the backspace key is pressed, pop back 1 char from  the
					// text string in the input
					if(UI.selectedInput && UI.selectedInput->isSelected) 
					{
						std::string str = UI.selectedInput->text.getString();
						if((event.key.code == sf::Keyboard::BackSpace) && str.length())
						{							
							str.pop_back();
							UI.selectedInput->text.setString(str);
						}
					}

				}
				
				// echo back characters in the input field if one is selected
				// only echo back characters that are printable. Characters get
				// appended to the text string of the selected input
				if(UI.selectedInput && UI.selectedInput->isSelected && (event.type == sf::Event::TextEntered)) 
				{
					if(event.text.unicode < 128)
					{
						std::string str = UI.selectedInput->text.getString();
						if(std::isprint(event.text.unicode))
						{
							UI.selectedInput->text.setString(str + (char)(event.text.unicode));
						}
					}
				}

				if(event.type == sf::Event::MouseButtonPressed)
				{
					if(event.mouseButton.button == sf::Mouse::Button::Left)
					{
						CURSOR->isClicked = true;
					}
				}
				if(event.type == sf::Event::MouseWheelScrolled && WINDOW->hasFocus())
				{
					if(event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel)
					{
						printf("wheel type: vertical\n");
						if(sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
						{
							printf("Zooming!\n");
							double zoom = event.mouseWheelScroll.delta > 0 ? 0.1 : -0.1;
							if(CURRENT_CONTEXT)
							{
								for(Layer & layer : CURRENT_CONTEXT->layerMenu.layers)
								{
									layer.changeScale(zoom);
								}
							}
						}
					}
					else if(event.mouseWheelScroll.wheel == sf::Mouse::HorizontalWheel)
					{
						printf("wheel type: horizontal\n");
					}
					else
					{
						printf("wheel type: unknown\n");
					}

					printf("wheel movement: %f\n", event.mouseWheelScroll.delta);
					printf("mouse x: %d\n", event.mouseWheelScroll.x);
					printf("mouse y: %d\n", event.mouseWheelScroll.y);					
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