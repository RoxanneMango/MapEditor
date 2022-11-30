#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <SFML/Graphics.hpp>

class Object : public sf::RectangleShape
{
protected:
	sf::Texture texture;
public:
	Object()
	{}
	Object(sf::Vector2f size) : sf::RectangleShape(size)
	{}
	virtual void update()
	{}
};

#endif // OBJECT_HPP