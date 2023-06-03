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
	
	float tile_size = TILE_SIZE;
	static constexpr int inputMargin = 30;	

	EditorGrid::ResizeDirection resizeDirection;
	std::vector<Option> resizeAlignmentOptions;

	Option previewSizeButton;
	Option resizeButton;
	Option shrinkToFitButton;

	std::vector<Layer> * grids;
	sf::Vector2u gridSize;
	//
	std::vector<Layer> layers;
	sf::Vector2u originalSize;

	sf::RectangleShape gridOutline = sf::RectangleShape();
	sf::RectangleShape selectionOverlay = sf::RectangleShape();
	
	std::pair<sf::Vector2u, sf::Vector2u> calculatePoints()
	{		
		sf::Vector2u size;
		size.x = this->originalSize.x > gridSize.x ? gridSize.x : this->originalSize.x;
		size.y = this->originalSize.y > gridSize.y ? gridSize.y : this->originalSize.y;
		
		sf::Vector2u p1 = sf::Vector2u(0,0);
		sf::Vector2u p2 = size;
		
		if(resizeDirection == EditorGrid::ResizeDirection::NorthWest) // yes
		{
			p1 = sf::Vector2u(0, 0);
			p2 = sf::Vector2u(size.x, size.y);
		}
		else if(resizeDirection == EditorGrid::ResizeDirection::North) // yes
		{
			p1 = sf::Vector2u(gridSize.x/2 - size.x/2, 0);
			p2 = sf::Vector2u(size.x + p1.x, size.y);
		}
		else if(resizeDirection == EditorGrid::ResizeDirection::NorthEast) // yes
		{
			p1 = sf::Vector2u(gridSize.x - size.x, 0);
			p2 = sf::Vector2u(gridSize.x, size.y);
		}
		else if(resizeDirection == EditorGrid::ResizeDirection::West) // yes
		{
			p1 = sf::Vector2u(0, gridSize.y/2 - size.y/2);
			p2 = sf::Vector2u(size.x, size.y + p1.y);
		}
		else if(resizeDirection == EditorGrid::ResizeDirection::Center) // yes
		{
			p1 = sf::Vector2u(gridSize.x/2 - size.x/2, gridSize.y/2 - size.y/2);
			p2 = sf::Vector2u(size.x + p1.x, size.y + p1.y);
		}
		else if(resizeDirection == EditorGrid::ResizeDirection::East) // yes
		{
			p1 = sf::Vector2u(gridSize.x - size.x, gridSize.y/2 - size.y/2);
			p2 = sf::Vector2u(gridSize.x + p1.x, size.y + p1.y);
		}
		else if(resizeDirection == EditorGrid::ResizeDirection::SouthWest) // yes
		{
			p1 = sf::Vector2u(0, gridSize.y - size.y);
			p2 = sf::Vector2u(size.x, gridSize.y);
		}
		else if(resizeDirection == EditorGrid::ResizeDirection::South) // yes
		{
			p1 = sf::Vector2u(gridSize.x/2 - size.x/2, gridSize.y - size.y);
			p2 = sf::Vector2u(size.x + p1.x, gridSize.x);
		}
		else if(resizeDirection == EditorGrid::ResizeDirection::SouthEast) // yes
		{
			p1 = sf::Vector2u(gridSize.x - size.x, gridSize.y - size.y);
			p2 = gridSize;
		}
		
		return std::pair<sf::Vector2u, sf::Vector2u>(p1, p2);		
	}

	sf::Vector2u calculateOffset()
	{		
		sf::Vector2u offset = sf::Vector2u(0,0);
		unsigned int x = abs(originalSize.x - gridSize.x);
		unsigned int y = abs(originalSize.y - gridSize.y);
		
		if	   (resizeDirection == EditorGrid::ResizeDirection::NorthWest) 	offset = sf::Vector2u(0, 0);
		else if(resizeDirection == EditorGrid::ResizeDirection::North)	 	offset = sf::Vector2u(x/2, 0);
		else if(resizeDirection == EditorGrid::ResizeDirection::NorthEast) 	offset = sf::Vector2u(x, 0);
		else if(resizeDirection == EditorGrid::ResizeDirection::West) 		offset = sf::Vector2u(0, y/2);
		else if(resizeDirection == EditorGrid::ResizeDirection::Center) 	offset = sf::Vector2u(x/2, y/2);
		else if(resizeDirection == EditorGrid::ResizeDirection::East) 		offset = sf::Vector2u(x, y/2);
		else if(resizeDirection == EditorGrid::ResizeDirection::SouthWest)	offset = sf::Vector2u(0, y);
		else if(resizeDirection == EditorGrid::ResizeDirection::South)		offset = sf::Vector2u(x/2, y);
		else if(resizeDirection == EditorGrid::ResizeDirection::SouthEast)	offset = sf::Vector2u(x, y);
		
		return offset;
	}

	
	void resetAlignmentOptions()
	{
		for(Option & option : resizeAlignmentOptions)
		{
			option.isSelected = (false);
		}
	}	
	

	bool shrinkToFit()
	{
		resetAlignmentOptions();
		resizeDirection = EditorGrid::ResizeDirection::Center;		
		resizeAlignmentOptions[resizeDirection].isSelected = true;
		
		sf::Vector2u p1 = originalSize;
		sf::Vector2u p2 = sf::Vector2u(0,0);
		
		bool hasTile = false;
		
		for(Layer & grid : *grids)
		{
			for(int y = 0; y < originalSize.y; y++)
			{
				for(int x = 0; x < originalSize.x; x++)
				{
					if(grid.tiles[y*originalSize.x + x].getFillColor().a != 0)
					{
						p1.x = x < p1.x ? x : p1.x;
						p1.y = y < p1.y ? y : p1.y;
						
						hasTile = true;
						
						break;
					}
				}
			}			
		}
		
		for(Layer & grid : *grids)
		{
			for(int y = originalSize.y-1; y >= 0; y--)
			{
				for(int x = originalSize.x-1; x >= 0; x--)
				{
					if(grid.tiles[y*originalSize.x + x].getFillColor().a != 0)
					{
						p2.x = x > p2.x ? x : p2.x;
						p2.y = y > p2.y ? y : p2.y;
						break;
					}
				}
			}			
		}
		

		sf::Vector2u croppedSize = sf::Vector2u(abs(p1.x-p2.x) + hasTile, abs(p1.y-p2.y) + hasTile);		
		if((p1.x >= p2.x) && (p1.y >= p2.y))
		{
			croppedSize = sf::Vector2u(0, 0);
		}

//		printf("p1: x:%d y:%d  ; p2: x:%d y:%d\n", p1.x, p1.y, p2.x, p2.y);
//		printf("cropped size: %d %d\n", croppedSize.x, croppedSize.y);
		
		inputFieldMapSize_width.text.setString(std::to_string(croppedSize.x));
		inputFieldMapSize_height.text.setString(std::to_string(croppedSize.y));		

		if(inputIsCorrrect())
		{
			if((croppedSize.x == originalSize.x) && (croppedSize.y == originalSize.y)) return true;
			
			// clear grid first
			if(layers.size()) layers.clear();

			unsigned int tile_size = (*grids)[0].tile_size;
			sf::Vector2f pos = (*grids)[0].tiles[0].getPosition();
			int margin = 1;
				
			sf::Color gridColor = sf::Color::Black;
			gridColor.a = 200;
				
			for(Layer & grid : *grids)
			{			
				Layer layer = grid;
				layer.tiles.clear();
				layer.gridSize = croppedSize;
				
				for(unsigned int y = 0, oy = p1.y; oy < (p1.y+croppedSize.y); y++, oy++)
				{
					for(unsigned int x = 0, ox=p1.x; ox < (p1.x+croppedSize.x); x++, ox++)
					{
						sf::Vector2f texturePos( x*tile_size, y*tile_size );
						sf::Vector2f tilePos = sf::Vector2f(pos.x + texturePos.x + margin*x, pos.y + texturePos.y + margin*y);
						sf::Vector2f tileSize = sf::Vector2f(tile_size, tile_size);
						unsigned int tileIndex = (y*croppedSize.x) + x;

						Tile tile( tileSize, tilePos, tileIndex );
						Tile t = grid.tiles[(oy*originalSize.x) + ox];
						
						tile.setTexture(t.getTexture());
						tile.setTextureRect(t.getTextureRect());
						tile.texturePATH = t.texturePATH;						
						tile.indexInTexturePack = t.indexInTexturePack;
						tile.setFillColor(t.getFillColor());
						
						tile.hoverBox.setOutlineThickness(1);
						tile.hoverBox.setOutlineColor(gridColor);
						tile.hoverColor = sf::Color::Black;
						
						layer.tiles.push_back(tile);
					}
				}
				layers.push_back(layer);
			}
					
			// copy over the contents into the actual grids
			if(grids->size() && layers.size())
			{
				grids->clear();
				for(Layer & layer : layers)	grids->push_back(layer);
			}
			
			gridSize = croppedSize;

			return true;
		}
		return false;
	}

	void initGrid(sf::Vector2u gridSize, bool isPreview = true, sf::Color color = sf::Color::Black)
	{
		// clear grid first
		if(layers.size())
		{
			if((gridSize.x == originalSize.x) && (gridSize.y == originalSize.y)) return;
			layers.clear();
		}

		// check if gridSize is sane
		if(!gridSize.x || !gridSize.y)
		{
			printf("Cannot have a grid with a x or y dimension of 0!\n");
			return;
		}
		

		int textMargin = 10;
				
		this->gridSize = gridSize;
		
		sf::Vector2f pos = sf::Vector2f(getPosition().x + 300, getPosition().y + 70);
		
		float tile_size_x = (getPosition().x + getSize().x - pos.x - 70)/gridSize.x;
		float tile_size_y = (getPosition().y + getSize().y - pos.y - 120)/gridSize.y;

		tile_size = tile_size_x < tile_size_y ? tile_size_x : tile_size_y;
		float margin = tile_size/TILE_SIZE;
		float outlineThickness = 0.2 + margin;
		outlineThickness = outlineThickness > 1 ? 1 : outlineThickness;
//		printf("thick: %0.5f\n", outlineThickness);

		sf::Color gridColor = color;
		gridColor.a = 200;

		if(!isPreview)
		{
			tile_size = (*grids)[0].tile_size;
			pos = (*grids)[0].tiles[0].getPosition();
			outlineThickness = 1;
			margin = 1;
		}

		// calculate the area of the original grid within the resized grid
		std::pair<sf::Vector2u, sf::Vector2u> points = calculatePoints();
		// calculate the offset of the original grid within the resized grid
		// (if the new grid is smaller than the original one, the offset makes 
		//  it so the original grid gets tripped correctly)
		sf::Vector2u offset = ((originalSize.x > gridSize.x) || originalSize.y > gridSize.y) ? calculateOffset() : sf::Vector2u(0,0);

		for(Layer & grid : *grids)
		{
			Layer layer = grid;
			layer.tiles.clear();

			unsigned int ox = offset.x;
			unsigned int oy = offset.y;
			
			for(unsigned int y = 0; y < gridSize.y; y++)
			{
				for(unsigned int x = 0; x < gridSize.x; x++)
				{
					sf::Vector2f texturePos( x*tile_size, y*tile_size );
					sf::Vector2f tilePos = sf::Vector2f(pos.x + texturePos.x + margin*x, pos.y + texturePos.y + margin*y);
					sf::Vector2f tileSize = sf::Vector2f(tile_size, tile_size);
					unsigned int tileIndex = (y*gridSize.x) + x;

					Tile tile( tileSize, tilePos, tileIndex );

					if( ((x >= points.first.x) && (y >= points.first.y)) && ((x < points.second.x) && (y < points.second.y)) )
					{
						if(isPreview && (ox==offset.x) && (oy==offset.y))
						{
							sf::Vector2u p = ( ((originalSize.x > gridSize.x) || originalSize.y > gridSize.y) ? gridSize : originalSize );
							
							selectionOverlay.setSize(sf::Vector2f(p.x*tile_size + p.x*margin, p.y*tile_size + p.y*margin));		
							selectionOverlay.setPosition(tilePos);
						}
						
						
						Tile t = grid.tiles[(oy*originalSize.x) + ox];
						
						tile.setTexture(t.getTexture());
						tile.setTextureRect(t.getTextureRect());
						
						tile.indexInTexturePack = t.indexInTexturePack;
						tile.texturePATH = t.texturePATH;
						tile.setFillColor(t.getFillColor());
						
						ox++;

						if(ox && (((ox-offset.x) % (originalSize.x > gridSize.x ? gridSize.x : originalSize.x)) == 0))
						{
							ox = offset.x;
							oy += 1;
						}

					}
					else
					{
						sf::Color c = tile.getFillColor();
						c.a = 0;
						tile.setFillColor(c);
					}

					tile.hoverBox.setOutlineThickness(outlineThickness);
					tile.hoverBox.setOutlineColor(gridColor);
					tile.hoverColor = sf::Color::Black;
					
					layer.tiles.push_back(tile);
				}
			}
			layers.push_back(layer);
		}
		
		gridOutline.setPosition(sf::Vector2f(
								layers[0].tiles[0].getPosition().x - gridOutline.getOutlineThickness()/2,
								layers[0].tiles[0].getPosition().y - gridOutline.getOutlineThickness()/2));
		gridOutline.setSize(sf::Vector2f(
			abs(   gridOutline.getPosition().x 
			     - layers[0].tiles[layers[0].tiles.size()-1].getPosition().x) 
				 + tile_size + outlineThickness + margin,
			abs(   gridOutline.getPosition().y 
				 - layers[0].tiles[layers[0].tiles.size()-1].getPosition().y) 
				 + tile_size + outlineThickness + margin)
		);

		// if its not a preview, copy over the contents into the actual grids
		if(!isPreview && grids->size() && layers.size())
		{
			grids->clear();
			for(Layer & layer : layers)
			{
				layer.gridSize = gridSize;
				layer.currentScale = 1;
				grids->push_back(layer);
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
				initGrid(sf::Vector2u(
					inputFieldMapSize_width.getUnsignedInteger(), 
					inputFieldMapSize_height.getUnsignedInteger()),
					true);//, resizeDirection);
			}
		}, sf::Vector2f(0,0)),
		resizeButton("    Resize    ", [&]()
		{
//			printf("gridSize: %d %d\n", CURRENT_CONTEXT->gridSize.x, CURRENT_CONTEXT->gridSize.y);
			
			if(inputIsCorrrect())
			{
				initGrid(sf::Vector2u(
					inputFieldMapSize_width.getUnsignedInteger(), 
					inputFieldMapSize_height.getUnsignedInteger()),
					false);//, resizeDirection);
				CURRENT_CONTEXT->gridSize = gridSize;
				close();
			}
		}, sf::Vector2f(0,0)),
		shrinkToFitButton(" Shrink to fit ", [&]()
		{
			if(shrinkToFit())
			{
				CURRENT_CONTEXT->gridSize = gridSize;
				close();
			}

			if(!inputFieldMapSize_width.getUnsignedInteger())
			{	inputFieldMapSize_width.text.setString(std::to_string(gridSize.x)); }
			if(!inputFieldMapSize_height.getUnsignedInteger())
			{	inputFieldMapSize_height.text.setString(std::to_string(gridSize.y));}
			
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
		addAlignmentOption("\\", [&](){ resizeDirection = EditorGrid::ResizeDirection::NorthWest; initGrid(gridSize); }, ix, iy); ix++;
		addAlignmentOption("^", [&](){  resizeDirection = EditorGrid::ResizeDirection::North;	  initGrid(gridSize); }, ix, iy); ix++;
		addAlignmentOption("/", [&](){  resizeDirection = EditorGrid::ResizeDirection::NorthEast; initGrid(gridSize); }, ix, iy); ix=0; iy++;
		//
		addAlignmentOption("<", [&](){  resizeDirection = EditorGrid::ResizeDirection::West;      initGrid(gridSize); }, ix, iy); ix++;
		addAlignmentOption("#", [&](){  resizeDirection = EditorGrid::ResizeDirection::Center;	  initGrid(gridSize); }, ix, iy); ix++;
		addAlignmentOption(">", [&](){  resizeDirection = EditorGrid::ResizeDirection::East;      initGrid(gridSize); }, ix, iy); ix=0; iy++;
		//
		addAlignmentOption("/", [&](){  resizeDirection = EditorGrid::ResizeDirection::SouthWest; initGrid(gridSize); }, ix, iy); ix++;
		addAlignmentOption("v", [&](){  resizeDirection = EditorGrid::ResizeDirection::South;	  initGrid(gridSize); }, ix, iy); ix++;
		addAlignmentOption("\\", [&](){ resizeDirection = EditorGrid::ResizeDirection::SouthEast; initGrid(gridSize); }, ix, iy);
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
			inputPos.x, 
			inputs[inputs.size()-1]->getPosition().y + 
			inputs[inputs.size()-1]->getSize().y + 30));
		previewSizeButton.setOutlineColor(sf::Color::Black);
		previewSizeButton.setOutlineThickness(2);
		previewSizeButton.setFillColor(sf::Color::White);
		previewSizeButton.alignCenter();
		


		shrinkToFitButton.changeSize(sf::Vector2f(shrinkToFitButton.getSize().x + 20, shrinkToFitButton.getSize().y + 20));		
		shrinkToFitButton.changePosition(sf::Vector2f(
			inputPos.x, 
			previewSizeButton.getPosition().y + 
			previewSizeButton.getSize().y + 30));
		shrinkToFitButton.setOutlineColor(sf::Color::Black);
		shrinkToFitButton.setOutlineThickness(2);
		shrinkToFitButton.setFillColor(sf::Color::White);
		shrinkToFitButton.alignCenter();
		
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
		
		sf::Color outlineColor = sf::Color::Black; outlineColor.a = 100;
		selectionOverlay.setFillColor(sf::Color::Transparent);
		selectionOverlay.setOutlineThickness(4);
		selectionOverlay.setOutlineColor(outlineColor);
		
		gridOutline.setFillColor(sf::Color::Transparent);
		gridOutline.setOutlineColor(sf::Color::Black);
		gridOutline.setOutlineThickness(1);		
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
			else
			{
				input->setFillColor(input->color);
			}
		}
		return isCorrect;
	}

	bool isOpen()
	{
		return isVisible;
	}
	
	void open(std::vector<Layer> * grids)
	{			
		this->grids = grids;
		this->gridSize = (*grids)[0].gridSize;
		this->originalSize = gridSize;

		inputFieldMapSize_width.text.setString(std::to_string(gridSize.x));
		inputFieldMapSize_height.text.setString(std::to_string(gridSize.y));
		
		initGrid(gridSize);
		
		isVisible = true;
	}
	
	void close()
	{
		layers.clear();
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
				shrinkToFitButton.update();
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
			shrinkToFitButton.render(window);

			for(InputField * inputField : inputs)
			{
				inputField->render(window);
			}
			
			for(Layer & layer : layers)
			{
				for(Tile & tile : layer.tiles)
				{
					if ( (((tile.getPosition().x + tile_size) > selectionOverlay.getPosition().x) && ((tile.getPosition().y + tile_size) > selectionOverlay.getPosition().y)) && 
						 ((tile.getPosition().x < (selectionOverlay.getPosition().x + selectionOverlay.getSize().x)) && (tile.getPosition().y < (selectionOverlay.getPosition().y + selectionOverlay.getSize().y)))	)
					{
						tile.render(window);
					}
				}
			}
			
			window.draw(selectionOverlay);
			window.draw(gridOutline);
		}
	}
};

#endif // RESIZE_GRID_PANEL_HPP