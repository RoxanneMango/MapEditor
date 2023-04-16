#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <string>

#include <SFML/Graphics.hpp>

static const std::string WINDOW_TITLE = "FrankenPatch Map Editor";
static const unsigned int FRAME_RATE_LIMIT = 20;
//static const unsigned int WINDOW_WIDTH = 1920;
//static const unsigned int WINDOW_HEIGHT = 1080;
static const float WINDOW_SIZE_MULTIPLIER = 0.8;
static const unsigned int VIEW_CENTER_X = 0;
static const unsigned int VIEW_CENTER_Y = 0;
static const unsigned int SPRITE_SIZE = 64;

static const unsigned int FONT_SIZE = 20;

sf::Font * FONT;

#endif // CONSTANTS_HPP