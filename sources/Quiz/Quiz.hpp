#pragma once

#include <filesystem>
#include <string>
#include <vector>
#include <set>

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>

namespace QUIZ
{
  class Quiz
  {
  public:
    enum Button
    {
      ButtonBuzzer,
      ButtonYellow,
      ButtonGreen,
      ButtonOrange,
      ButtonBlue
    };

    std::vector<sf::Texture>  avatars;

    struct Player
    {
      unsigned int  joystick, button; // Controller ID and buttons offset
      unsigned int  avatar;           // Avatar index
      int           score;            // Score of the player
      sf::Sprite    sprite;           // Sprite of the player's avatar
    };

    struct Blindtest
    {
      std::string music;  // Path to music
      std::string cover;  // Path to album cover/movie poster
      std::string answer; // Answer
      std::string info;   // Info displayed to host
      int         score;  // Number of points for good answer
      bool        done;   // True if question has already been asked
    };

    std::vector<Player>     players;    // Players
    std::vector<Blindtest>  blindtests; // Audio blindtests questions

  public:
    Quiz(const std::filesystem::path& config);
    ~Quiz() = default;
  };
}