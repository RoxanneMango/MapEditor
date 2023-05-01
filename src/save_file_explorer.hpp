#ifndef SAVE_FILE_EXPLORER_HPP
#define SAVE_FILE_EXPLORER_HPP

#include "file_explorer.hpp"
#include "confirmation_prompt.hpp"

class SaveFileExplorer : public FileExplorer
{
private:
	static constexpr int inputMargin = 30;
	Option saveAsButton;

	std::function<void(std::string)> action;

	ConfirmationPrompt confirmPrompt;

	void readDirEntries() override
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
					txt.setFillColor(sf::Color(100, 100, 100));
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

	bool changeDirectory(std::string dir) override
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
				readDirEntries();

				return true;
			}
			else
			{
				return false;
			}
		}
		catch(...)
		{
			printf("Something went wrong when changing directories!\n");
		}
		return false;	
	}

public:
	InputField input;

	SaveFileExplorer(sf::Vector2f size, sf::Vector2f pos) : FileExplorer(size, pos),
		input(sf::Vector2f(size.x - inputMargin*2 - 150, 30), InputField::Type::String),
		saveAsButton("Save as...", [&]()
		{
			if(input.isValid())
			{
				std::string PATH = cwd + "/" + input.getString();
				
				bool isMatch = false;
				for(sf::Text & file : files)
				{
					if(input.getString() == file.getString())
					{
						isMatch = true;						
						break;
					}
				}
				
				if(isMatch)
				{
					confirmPrompt.open("This will overwrite the\nselected file!", [this, PATH]()
					{
						action(PATH);
						close();
					});
				}
				else
				{
					action(PATH);
					close();
				}
			}
		}, sf::Vector2f(pos.x + size.x - 150, pos.y + size.y - inputMargin*2))
	{
		input.color = (sf::Color::White);
		input.setFillColor(sf::Color::White);
		input.changePosition(sf::Vector2f(pos.x + inputMargin, pos.y + size.y - input.getSize().y - inputMargin));
		saveAsButton.changeSize(sf::Vector2f(saveAsButton.getSize().x + 10, 30));
		saveAsButton.setOutlineThickness(2);
		saveAsButton.setOutlineColor(sf::Color::Black);
	}

	void open(std::function<void(std::string)> action)
	{
		this->action = action;
		isVisible = true;
	}

	void update() override
	{
		try
		{
			if(confirmPrompt.isOpen())
			{
				confirmPrompt.update();
			}
			else
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
					forwardButton.update();
					saveAsButton.update();

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
									std::string str = std::string(dir.getString()).c_str();
									if(!changeDirectory(str))
										input.setString(str);
								}
								dir.setStyle(sf::Text::Bold);
							}
							else
							{
								dir.setStyle(sf::Text::Regular);
							}
						}
					}

					bool textHover = false;
					if(Collision::AABB(*CURSOR, input))
					{
						textHover = true;
						if(sf::Mouse::isButtonPressed(sf::Mouse::Left))
						{
							input.isSelected = true;
							input.setFillColor(input.color);
							input.setOutlineThickness(3);
						}
					}
					else
					{
						if(sf::Mouse::isButtonPressed(sf::Mouse::Left))
						{
							input.isSelected = false;
							input.setOutlineThickness(2);
						}
					}					
					input.update();
					CURSOR->setMode(textHover ? CursorMode::Text : CursorMode::Default);
					
				}
			}
		}
		catch(...)
		{
			printf("Something went wrong with the file explorer!\n");
		}
	}
	
	void render(sf::RenderWindow & window) override
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
			saveAsButton.render(window);

			window.draw(currentDirectory);
			window.draw(currentDirectory_txt);
			
			input.render(window);
						
			if(files.size())
			{
				for(sf::Text & file : files)
				{
					window.draw(file);
				}
			}

			if(confirmPrompt.isOpen())
			{
				confirmPrompt.render(window);
			}
			
		}
	}
};

#endif // SAVE_FILE_EXPLORER_HPP