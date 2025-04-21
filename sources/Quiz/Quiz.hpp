#pragma once

#include <filesystem>
#include <string>
#include <vector>
#include <set>
#include <map>

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Texture.hpp>

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

    struct Fastest
    {
      std::wstring              id;       // Unique ID of question
      std::wstring              question; // Text of the question
      std::vector<std::wstring> answers;  // Answers to the question in correct order
      bool                      done;     // True if question has already been answered
    };

    struct Question
    {
      std::string id; // Unique ID of question

      std::string           questionText;   // Text of the question
      std::filesystem::path questionAudio;  // Audio of the question
      std::filesystem::path questionImage;  // Image of the question
      int                   questionPoints; // Points of the question
      std::string           questionInfo;   // Question informations displayed in host terminal

      std::vector<std::string>  answerChoices;  // Possible answers (max 4, only 1 for buzz questions)
      unsigned int              answerCorrect;  // Index of correct answer in choices
      float                     answerTimeout;  // Timeout duration after bad answer for buzz or question duration for MCQ
      bool                      answerNextout;  // Wrong player can't answer before next player

      std::string           correctText;   // Text of the correct answer
      std::filesystem::path correctAudio;  // Audio of the correct answer
      std::filesystem::path correctImage;  // Image of the correct answer
      std::string           correctInfo;   // Answer informations displayed in host terminal

      bool  done;   // True if question has already been answered
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
      sf::Sprite          _sprite;  // Sprite to draw
      sf::Texture         _texture; // Texture to draw
      sf::Text            _text;    // Text to draw
      float               _outline; // Thickness of rectangle outline

      Math::Vector<2> _position, _targetPosition; // Position in % of window [0-1]
      Math::Vector<2> _scale, _targetScale;       // Scale in % of window [0-1] (ajust to keep aspect ratio)
      Math::Vector<4> _color, _targetColor;       // Color components [0-1]
      float           _lerp;                      // Lerp level
      bool            _dead;                      // Delete flag

      template<typename Type>
      Type  lerp(Type value, Type target, float elapsed) const // Lerp a value
      {
        return value + (target - value) * ((_lerp <= 0.f) ? (1.f) : (1.f - (1.f / std::pow(2.f, elapsed / _lerp))));
      }

    public:
      Entity();
      Entity(const Entity&) = delete;
      Entity(Entity&&) = delete;
      ~Entity() = default;

      Entity& operator=(const Entity&) = delete;
      Entity& operator=(Entity&&) = delete;

      void  reset();  // Full reset of entity

      void  setPosition(float x, float y);                      // Set position, instant
      void  setTargetPosition(float x, float y);                // Move entity to position
      void  setScale(float x, float y);                         // Set scale, instant
      void  setTargetScale(float x, float y);                   // Progressive scale to given value
      void  setColor(float r, float g, float b, float a);       // Set color, instant
      void  setTargetColor(float r, float g, float b, float a); // Progressive color change
      void  setOutline(float t);                                // Set outline thickness
      void  setLerp(float l);                                   // Set lerp speed
      void  setDead(bool v);                                    // Set dead flag
      void  setTexture(const std::filesystem::path& path = ""); // Set texture
      void  setText(const std::wstring& text = L"");            // Set new text to display
      
      Math::Vector<2>     getPosition() const;        // Get current position
      Math::Vector<2>     getTargetPosition() const;  // Get target position
      Math::Vector<2>     getScale() const;           // Get current scaling
      Math::Vector<2>     getTargetScale() const;     // Get target scaling
      Math::Vector<4>     getColor() const;           // Get current color
      Math::Vector<4>     getTargetColor() const;     // Get current color
      float               getOutline() const;         // Get outline thickness
      float               getLerp() const;            // Get lerp level
      bool                getDead() const;            // Get dead flag
      std::wstring        getText() const;            // Get text

      const sf::Sprite& sprite() const; // Get entity sprite

      bool  update(float elapsed); // Update entity position/scale/color with lerp
      void  draw();                // Draw entity
      bool  hover() const;         // Check if cursor is hovering entity
    };

    class ProgressBar
    {
    private:
      float _value;   // Displayed progression value [0; 1]
      float _height;  // Current height of bar [0; 1]
      bool  _hidden;  // True if bar is hidden

    public:
      ProgressBar();
      ~ProgressBar() = default;

      void  set(float value); // Set new progress bar value [0; 1]
      float get() const;      // Get current value
      void  hide();           // Hide progress bar
      void  show();           // Show progress bar
      
      void  update(float elapsed);  // Update progress bar
      void  draw();                 // Draw progress bar
    };

    std::vector<Player>           players;    // Players
    std::vector<Blindtest>        blindtests; // Audio blindtests questions
    std::map<std::string, Entity> entities;   // Entities to draw
    ProgressBar                   progress;   // Progress bar
    std::vector<Question>         questions;  // Questions bank
    std::vector<Fastest>          fastests;   // Fastest finger question bank

  public:
    Quiz();
    ~Quiz() = default;
  };
}