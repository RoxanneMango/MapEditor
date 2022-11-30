#ifndef QUAD_TREE_HPP
#define QUAD_TREE_HPP

#include <vector>
#include <algorithm>

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

#include "object.hpp"
#include "collision.hpp"

// https://github.com/lowfriction/quadtree-sfml

class Quadtree : sf::RectangleShape
{
private:
	float x;
	float y;
	float width;
	float height;
	int	level;
	int	maxLevel;
	std::vector<Object *> objects;

	Quadtree * parent;
	Quadtree * NW;
	Quadtree * NE;
	Quadtree * SW;
	Quadtree * SE;

	bool contains(Quadtree * child, sf::FloatRect & rect)
	{
		return Collision::AABB(*child, rect);
	}

	bool contains(Quadtree * child, Object * object)
	{
		return Collision::AABB(*child, *object);
	}

public:

	Quadtree(float x, float y, float width, float height, int level, int maxLevel) :
		x(x),
		y(y),
		width(width),
		height(height),
		level(level),
		maxLevel(maxLevel)
	{
		setPosition(x, y);
		setSize(sf::Vector2f(width, height));
		setFillColor(sf::Color(0, 0, 0, 0));
		setOutlineThickness(1.0f);
		setOutlineColor(sf::Color(64, 128, 255));

		if(level < maxLevel)
		{
			NW = new Quadtree(x, y, width / 2.0f, height / 2.0f, level+1, maxLevel);
			NE = new Quadtree(x + width / 2.0f, y, width / 2.0f, height / 2.0f, level+1, maxLevel);
			SW = new Quadtree(x, y + height / 2.0f, width / 2.0f, height / 2.0f, level+1, maxLevel);
			SE = new Quadtree(x + width / 2.0f, y + height / 2.0f, width / 2.0f, height / 2.0f, level+1, maxLevel);
		}
	}
	
	~Quadtree()
	{
		if(level < maxLevel)
		{
			delete NW;
			delete NE;
			delete SW;
			delete SE;
		}
	}

	void addObject(Object * object)
	{
		if(level == maxLevel)
		{
			objects.push_back(object);
		}
		else
		{
			if(contains(NW, object))
			{
				NW->addObject(object);
			}
			if(contains(NE, object))
			{
				NE->addObject(object); 
			}
			if(contains(SW, object))
			{
				SW->addObject(object);
			} 
			if(contains(SE, object))
			{
				SE->addObject(object);
			}
		}
	}
	
	std::vector<Object *> getObjectsAt(Object & object)
	{
		return getObjectsAt(object.getPosition().x, object.getPosition().y, object.getSize().x, object.getSize().y);
	}
	
	std::vector<Object *> getObjectsAt(float _x, float _y, float _w, float _h)
	{
		if(level == maxLevel)
		{
			return objects;
		}

		std::vector<Object *> returnObjects, childReturnObjects;
		if(!objects.empty())
		{
			returnObjects = objects;
		}

		sf::FloatRect rectangle(_x, _y, _w, _h);
		
		if(contains(NW, rectangle))
		{
			childReturnObjects = NW->getObjectsAt(_x, _y, _w, _h);
			returnObjects.insert(returnObjects.end(), childReturnObjects.begin(), childReturnObjects.end());
		}
		if(contains(NE, rectangle))
		{
			childReturnObjects = NE->getObjectsAt(_x, _y, _w, _h);
			returnObjects.insert(returnObjects.end(), childReturnObjects.begin(), childReturnObjects.end());
		}

		if(contains(SW, rectangle))
		{
			childReturnObjects = SW->getObjectsAt(_x, _y, _w, _h);
			returnObjects.insert(returnObjects.end(), childReturnObjects.begin(), childReturnObjects.end());
		}
		if(contains(SE, rectangle))
		{
			childReturnObjects = SE->getObjectsAt(_x, _y, _w, _h);
			returnObjects.insert(returnObjects.end(), childReturnObjects.begin(), childReturnObjects.end());
		}
		return returnObjects;
	}

	void removeObject(Object * object)
	{
		if(level == maxLevel)
		{		
			for(unsigned int i = 0; i < objects.size(); i++)
			{	if(objects[i] == object)
				{	objects.erase(objects.begin() + i);
					break;
			}	}
			return;
		}
		NW->removeObject(object);
		NE->removeObject(object);
		SW->removeObject(object);
		SE->removeObject(object);
	}

	void clear()
	{
		if(level == maxLevel)
		{
			objects.clear();
			return;
		}
		else
		{
			NW->clear();
			NE->clear();
			SW->clear();
			SE->clear();
		}
		if(!objects.empty())
		{
			objects.clear();
		}
	}

	void render(sf::RenderTarget & window)
	{
		window.draw(*this);
		if(level != maxLevel)
		{
			NW->render(window);
			NE->render(window);
			SW->render(window);
			SE->render(window);
		}
	}

};

#endif // QUAD_TREE_HPP