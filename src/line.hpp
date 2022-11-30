#ifndef LINE_HPP
#define LINE_HPP

#include <SFML/Graphics.hpp>

namespace sf
{
	class Line : public RectangleShape
	{
	private:		
		void setParams()
		{
			if(direction == Direction::Up)
			{
				setPosition(sf::Vector2f(position.x, position.y - length));
				setSize(Vector2f(thickness, length));
			}
			else if(direction == Direction::Down)
			{
				setPosition(position);
				setSize(Vector2f(thickness, length));
			}
			else if(direction == Direction::Left)
			{
				setPosition(position.x - length, position.y);
				setSize(Vector2f(length, thickness));
			}
			else
			{
				setPosition(position);
				setSize(Vector2f(length, thickness));
			}			
			setFillColor(sf::Color::Black);
		}
			
	public:
		Vector2f position = Vector2f(0,0);
		float length = 1;
		float thickness = 1.0f;
		bool isHorizontal = true;
		enum Direction { Up, Left, Down, Right };
		Direction direction = Direction::Right;
	
		Line()
		{}
		Line(Vector2f position) : position(position)
		{
			setParams();
		}
		Line(Vector2f position, float length) : position(position), length(length)
		{
			setParams();
		}
		Line(Vector2f position, float length, int thickness, Direction direction = Direction::Right) : position(position), length(length), thickness(thickness), direction(direction)
		{
			setParams();
		}
		
		Line(float length, int thickness, Direction direction = Direction::Right) : length(length), thickness(thickness), direction(direction)
		{
			setParams();
		}
		
		void render(RenderWindow & window)
		{
			window.draw(*this);
		}
		float getLength()
		{
			return length;
		}
		float getThickness()
		{
			return thickness;
		}
		void setLength(float length)
		{
			this->length = length;
			setParams();
		}
		void setThickness(float thickness)
		{
			this->thickness = thickness;
			setParams();
		}
		void setDirectionHorizontal(bool isHorizonal)
		{
			this->isHorizontal = isHorizonal;
			setParams();
		}
	};
}

#endif // LINE_HPP