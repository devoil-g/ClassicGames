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

    std::vector<std::vector<sf::Texture>>  avatars; // Avatars/costumes

    struct Player
    {
      unsigned int  joystick, button; // Controller ID and buttons offset
      unsigned int  avatar, skin;     // Avatar and skin index
      int           score;            // Score of the player
      sf::Sprite    sprite;           // Sprite of the player's avatar
    };

    struct Blindtest
    {
      std::filesystem::path music;  // Path to music
      std::filesystem::path cover;  // Path to album cover/movie poster
      std::string           answer; // Answer
      bool                  done;   // True if question has already been asked
    };

    std::vector<Player>     players;    // Players
    std::vector<Blindtest>  blindtests; // Audio blindtests questions

  public:
    Quiz();
    ~Quiz() = default;
  };
}