#pragma once

#include <filesystem>
#include <string>
#include <vector>
#include <set>
#include <map>

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>

#include "Math/Vector.hpp"

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

    std::vector<std::vector<std::filesystem::path>>  avatars; // Avatars/costumes path

    struct Player
    {
      unsigned int  joystick, button; // Controller ID and buttons offset
      unsigned int  avatar, skin;     // Avatar and skin index
      int           score;            // Score of the player
      unsigned int  id;               // Player ID
    };

    struct Blindtest
    {
      std::filesystem::path music;  // Path to music
      std::filesystem::path cover;  // Path to album cover/movie poster
      std::string           answer; // Answer
      bool                  done;   // True if question has already been asked
    };

    class Entity
    {
    private:
      sf::Sprite  _sprite;  // Sprite to draw
      sf::Texture _texture; // Texture to draw

      Math::Vector<2> _position, _targetPosition; // Position in % of window [0-1]
      Math::Vector<2> _scale, _targetScale;       // Scale in % of window [0-1] (ajust to keep aspect ratio)
      Math::Vector<4> _color, _targetColor;       // Color components [0-1]
      float           _lerp;                      // Lerp level

      template<typename Type>
      Type  lerp(Type value, Type target, float elapsed) const // Lerp a value
      {
        return value + (target - value) * ((_lerp <= 0.f) ? (1.f) : (1.f - (1.f / std::pow(2.f, elapsed / _lerp))));
      }

    public:
      Entity(const std::filesystem::path& texturePath);
      Entity(const Entity&) = delete;
      Entity(Entity&&) = delete;
      ~Entity() = default;

      Entity& operator=(const Entity&) = delete;
      Entity& operator=(Entity&&) = delete;

      void  setPosition(float x, float y);                      // Set position, instant
      void  setTargetPosition(float x, float y);                // Move entity to position
      void  setScale(float x, float y);                         // Set scale, instant
      void  setTargetScale(float x, float y);                   // Progressive scale to given value
      void  setColor(float r, float g, float b, float a);       // Set color, instant
      void  setTargetColor(float r, float g, float b, float a); // Progressive color change
      void  setLerp(float l);                                   // Set lerp speed
      void  setTexture(const std::filesystem::path& path);      // Set texture

      Math::Vector<2> getPosition() const;       // Get current position
      Math::Vector<2> getTargetPosition() const; // Get target position
      Math::Vector<2> getScale() const;          // Get current scaling
      Math::Vector<2> getTargetScale() const;    // Get target scaling
      Math::Vector<4> getColor() const;          // Get current color
      Math::Vector<4> getTargetColor() const;    // Get current color
      float           getLerp() const;           // Get lerp level

      const sf::Sprite& sprite() const; // Get entity sprite

      void  update(float elapsed); // Update entity position/scale/color with lerp
      void  draw();                // Draw entity
      bool  hover() const;         // Check if cursor is hovering entity
    };

    std::vector<Player>           players;    // Players
    std::vector<Blindtest>        blindtests; // Audio blindtests questions
    std::map<std::string, Entity> entities;   // Entities to draw

  public:
    Quiz();
    ~Quiz() = default;
  };
}