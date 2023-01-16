#ifndef COLLISION_H
#define COLLISION_H

#include <SFML/Graphics.hpp>

class Collision
{
public:
	Collision()
	{}
	~Collision()
	{}

	static bool AABB(sf::FloatRect & aRect, sf::FloatRect & bRect)
	{
		if(aRect.contains(bRect.left, bRect.top) || bRect.contains(aRect.left, aRect.top))
		{
			return true;
		}
		if(aRect.intersects(bRect) || bRect.intersects(aRect))
		{
			return true;
		}
		return false;
	}
	
	static bool AABB(sf::RectangleShape & a, sf::FloatRect & bRect)
	{
		sf::FloatRect aRect = a.getGlobalBounds();
		
		return AABB(aRect, bRect);
	}	
	
	static bool AABB(sf::RectangleShape & a, sf::RectangleShape & b)
	{
		if(&a == &b) return false;
		
		sf::FloatRect aRect = a.getGlobalBounds();
		sf::FloatRect bRect = b.getGlobalBounds();
		
		return AABB(aRect, bRect);
	}
	
	static bool AABB(sf::RectangleShape & a, sf::Text & b)
	{
		sf::FloatRect aRect = a.getGlobalBounds();
		sf::FloatRect bRect = b.getGlobalBounds();
		
		return AABB(aRect, bRect);
	}
};

#endif // COLLISION_H