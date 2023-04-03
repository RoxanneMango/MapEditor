#ifndef FILE_EXPLORER_HPP
#define FILE_EXPLORER_HPP

#include <vector>
#include <deque>

#include <dirent.h>
#include <unistd.h>
#include <string.h>

#include <SFML/Graphics.hpp>
#include "line.hpp"

#include "constants.hpp"
#include "cursor.hpp"
#include "collision.hpp"
#include "toolbar_textures.hpp"
#include "globals.hpp"

namespace FileExplorerButton
{
	class NavigationButton : public sf::RectangleShape
	{
	private:
		sf::Text txt;
		bool pressed = false;
	public:
		NavigationButton()
		{}
		
		NavigationButton(std::string text, sf::Vector2f pos, int txtOffset_x, int txtOffset_y) : 
			sf::RectangleShape(sf::Vector2f(30, 35)),
			txt(text, *FONT, FONT_SIZE*2)
		{
			setFillColor(sf::Color::White);
			setOutlineThickness(1);
			setOutlineColor(sf::Color::Black);
			setPosition(pos);

			txt.setFillColor(sf::Color::Black);
			txt.setPosition(sf::Vector2f(pos.x + txtOffset_x, pos.y + txtOffset_y));			
		}
		
		void update()
		{
			pressed = false;
			if(Collision::AABB(*CURSOR, *this))
			{
				if(CURSOR->isPressed())
				{
					pressed = true;
				}
				setFillColor(sf::Color(245, 245, 245));
			}
			else
			{
				setFillColor(sf::Color::White);
			}
		}
		
		bool isPressed()
		{
			return pressed;
		}
		
		void render(sf::RenderWindow & window)
		{
			window.draw(*this);
			window.draw(txt);
		}
	};
}


class FileExplorer : public sf::RectangleShape
{
private:
	sf::Text window_title = sf::Text("File explorer", *FONT, FONT_SIZE);
	
	sf::Text closeButton_x = sf::Text("X", *FONT, FONT_SIZE);
	sf::RectangleShape closeButton;
	
	FileExplorerButton::NavigationButton backButton;
	FileExplorerButton::NavigationButton forwardButton;
	FileExplorerButton::NavigationButton upButton;
	
	sf::Text currentDirectory_txt;
	sf::RectangleShape currentDirectory;
	
	sf::Line topDivider;
	
	std::vector<sf::Text> files;
	
	std::deque<const char *> directory_entries;
	const unsigned int max_entries = 10;
	unsigned int entries_index = 0;
	
	bool isVisible = false;
	
	DIR * dir_ptr;
	dirent * entry;
	std::string cwd;
	
	void readDirEntries()
	{
		try
		{
			files.clear();
			dir_ptr = opendir(cwd.c_str());

			while((entry = readdir(dir_ptr)) != NULL)
			{
				std::string name = entry->d_name;
				
				if (entry->d_name[0] == '$')	 	continue;
				if (!strcmp (entry->d_name, ".")) 	continue;
				if (!strcmp (entry->d_name, ".."))	continue;
				
				sf::Text txt = sf::Text(entry->d_name, *FONT, FONT_SIZE);
				txt.setOutlineThickness(5);
				txt.setOutlineColor(sf::Color(245,245,245));

				std::string testFile = cwd + "/" + entry->d_name;
				DIR * testDIR = opendir(testFile.c_str());
				if(testDIR != NULL)
				{
					txt.setFillColor(sf::Color::Black);
				}
				else
				{
					txt.setFillColor(sf::Color(120, 120, 120));					
				}
				closedir(testDIR);

				files.push_back(txt);
			}
			
			if(files.size())
			{
				unsigned int i = 0;
				unsigned int j = 0;
				for(sf::Text & file : files)
				{
					if(!((i+1)%16))
					{
						i = 0;
						j++;
					}
					file.setPosition(sf::Vector2f(getPosition().x + 30 + 400*j, currentDirectory.getPosition().y + currentDirectory.getSize().y + 20 + (FONT_SIZE+10)*i++));
				}
			}
			closedir(dir_ptr);
		}
		catch(...)
		{
			printf("Something went wrong when reading directory entries!\n");
		}
	}
	
	void changeDirectory(std::string dir)
	{
		try
		{
			chdir(dir.c_str());
			char current_directory[512] = {0};
			std::string newDir = getcwd(current_directory, sizeof(current_directory));
			if(strcmp(cwd.c_str(), newDir.c_str()))
			{
				cwd = newDir;
				currentDirectory_txt.setString(cwd);
				
//				printf("dir: %s\n", cwd.c_str());
/*
				DIR * testDIR = opendir(cwd.c_str());
				if(testDIR != NULL)
				{
					// Append directory name to the directory_entries list if its at the back element
					if((entries_index >= (directory_entries.size()-1)) || (!directory_entries.size()))
					{
						printf("pushed!\n");
						directory_entries.push_back(cwd.c_str());
						if(directory_entries.size() >= max_entries)
						{
							printf("POP!\n");
							directory_entries.pop_front();
						}
						entries_index = directory_entries.size()-1;
						printf("entries_index: %d\n", entries_index);
					}
				}
				closedir(testDIR);
*/
				
				readDirEntries();				
			}
			else	// It's not a directory but a file: try to open it
			{
				if(dir.size() >= 3)
				{
					if(TOOLBAR_TEXTURES->addTexturePreview(std::string(cwd + "/" + dir))) // if this function returns (1)
					{
						close();
					}
				}
				printf("file %s\n", dir.c_str());
			}
		}
		catch(...)
		{
			printf("Something went wrong when changing directories!\n");
		}
	}
	
public:
	FileExplorer(sf::Vector2f size, sf::Vector2f pos) : sf::RectangleShape(size)
	{
		setPosition(pos);
		setFillColor(sf::Color::White);
		setOutlineThickness(2);
		setOutlineColor(sf::Color::Black);
		
		window_title.setFillColor(sf::Color::Black);
		window_title.setPosition(sf::Vector2f(pos.x + 10, pos.y + 10));
		
		closeButton.setFillColor(sf::Color(200, 80, 80));
		closeButton.setOutlineThickness(2);
		closeButton.setOutlineColor(sf::Color::Black);
		closeButton.setSize(sf::Vector2f(30, 30));
		closeButton.setPosition(sf::Vector2f(pos.x + size.x - closeButton.getSize().x - 5, pos.y + 5));
		
		closeButton_x.setFillColor(sf::Color::White);
		closeButton_x.setPosition(sf::Vector2f(closeButton.getPosition().x + 10, closeButton.getPosition().y + 3));

		topDivider = sf::Line(sf::Vector2f(pos.x, pos.y + closeButton.getSize().y + 10), size.x, 1);

		char current_directory[512] = {0};
		cwd = getcwd(current_directory, sizeof(current_directory));

		backButton = FileExplorerButton::NavigationButton("<", sf::Vector2f(pos.x+20, topDivider.getPosition().y+20), 5, -10);
		forwardButton = FileExplorerButton::NavigationButton(">", sf::Vector2f(backButton.getPosition().x + backButton.getSize().x+5, topDivider.getPosition().y+20), 5, -10);
		upButton = FileExplorerButton::NavigationButton("^", sf::Vector2f(forwardButton.getPosition().x + forwardButton.getSize().x+5, topDivider.getPosition().y+20), 5, -5);

		currentDirectory.setSize(sf::Vector2f(size.x - 180, 35));
		currentDirectory.setPosition(sf::Vector2f(upButton.getPosition().x + upButton.getSize().x + 10, forwardButton.getPosition().y));
		currentDirectory.setOutlineThickness(1);
		currentDirectory.setOutlineColor(sf::Color(122, 122, 122));

		currentDirectory_txt = sf::Text(cwd, *FONT, FONT_SIZE);
		currentDirectory_txt.setPosition(sf::Vector2f(currentDirectory.getPosition().x + 5, currentDirectory.getPosition().y + 5));
		currentDirectory_txt.setFillColor(sf::Color::Black);

//		printf("initial push!\n");
//		directory_entries.push_back(cwd.c_str());
//		entries_index = directory_entries.size()-1;		

		readDirEntries();

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
				
				upButton.update(); 			if(upButton.isPressed()) changeDirectory("..");

				backButton.update();
				forwardButton.update();//	if(forwardButton.isPressed()) ;

				if(directory_entries.size())
				{
					if(backButton.isPressed())
					{
						if(entries_index > 0)
						{
							changeDirectory(directory_entries[--entries_index]);
						}
					}
					else if(forwardButton.isPressed())
					{
						if(entries_index < (directory_entries.size()-1))
						{
							changeDirectory(directory_entries[++entries_index]);
						}
					}
				}
				
				if(files.size())
				{
					for(sf::Text & dir : files)
					{
						if(Collision::AABB(*CURSOR, dir))
						{
							if(CURSOR->isPressed())
							{
								changeDirectory(std::string(dir.getString()).c_str());
							}
							dir.setStyle(sf::Text::Bold);
						}
						else
						{
							dir.setStyle(sf::Text::Regular);
						}
					}
				}
				
			}
		}
		catch(...)
		{
			printf("Something went wrong with the file explorer!\n");
		}
	}
	
	void render(sf::RenderWindow & window)
	{
		if(isVisible)
		{
		
			window.draw(*this);
			
			window.draw(window_title);
			
			window.draw(closeButton);
			window.draw(closeButton_x);
			
			window.draw(topDivider);

			backButton.render(window);
			forwardButton.render(window);
			upButton.render(window);

			window.draw(currentDirectory);
			window.draw(currentDirectory_txt);
			
			if(files.size())
			{
				for(sf::Text & file : files)
				{
					window.draw(file);
				}
			}			
		}
	}
	
	
	
};

FileExplorer * FILE_EXPLORER;

#endif // FILE_EXPLORER.HPP