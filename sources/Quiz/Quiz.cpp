#include <array>
#include <filesystem>
#include <iostream>
#include <random>
#include <chrono>
#include <algorithm>
#include <exception>

#include <SFML/Graphics/Image.hpp>

#include "System/Config.hpp"
#include "System/Window.hpp"
#include "System/Library/FontLibrary.hpp"
#include "Quiz/Quiz.hpp"

QUIZ::Quiz::Quiz() :
  avatars(),
  players(),
  entities(),
  progress(),
  blindtests(),
  questions(),
  fastests(),
  millionaires()
{
  auto json = Game::JSON::Object(Game::Config::ExecutablePath / "assets" / "quiz" / "questions.json");

  constexpr std::array<std::string, 9> textureExtensions = { "jpg", "png", "bmp", "tga", "jpeg", "gif", "psd", "hdr", "pic" };
  constexpr std::array<std::string, 3> musicExtensions = { "ogg", "wav", "flac" };

  std::default_random_engine  randomEngine((unsigned int)std::chrono::system_clock::now().time_since_epoch().count());

  // Load avatars from directory
  for (const auto& avatar : std::filesystem::directory_iterator(Game::Config::ExecutablePath / "assets" / "quiz" / "avatars")) {
    std::vector<std::filesystem::path>  skins;

    // Costume directory
    if (avatar.is_directory() == true) {
      for (const auto& skin : std::filesystem::directory_iterator(avatar))
      {
        // Unsupported extension
        if (skin.path().has_extension() == false ||
          std::find(textureExtensions.begin(), textureExtensions.end(), skin.path().extension().string().substr(1)) == textureExtensions.end())
          continue;

        // Add avatar to collection
        skins.push_back(skin.path());
      }
    }

    // Single avatar
    else
    {
      // Unsupported extension
      if (avatar.path().has_extension() == false ||
        std::find(textureExtensions.begin(), textureExtensions.end(), avatar.path().extension().string().substr(1)) == textureExtensions.end())
        continue;

      // Add avatar to collection
      skins.push_back(avatar.path());
    }

    // No costume to load
    if (skins.empty() == true)
      continue;

    // Add costumes to avatars
    avatars.push_back(std::move(skins));

    // Verbose
    std::cout << "Avatar '" << avatar.path().stem().string() << "' loaded." << std::endl;
  }

  // Load blindtests from directory
  for (const auto& entry : std::filesystem::directory_iterator(Game::Config::ExecutablePath / "assets" / "quiz" / "blindtest")) {
    auto music_path = entry.path();
    auto cover_path = Game::Config::ExecutablePath / "assets" / "quiz" / "images" / "default.png";

    // Try to load file as a music
    if (music_path.has_extension() == false ||
      std::find(musicExtensions.begin(), musicExtensions.end(), music_path.extension().string().substr(1)) == musicExtensions.end())
      continue;

    // Find image with same name in the directory
    for (const auto& ext : textureExtensions) {
      std::filesystem::path extension{ ext };
      std::filesystem::path tmp_path = music_path;

      // Replace extension of music
      tmp_path.replace_extension(extension);

      // Check if file exists
      if (std::filesystem::exists(tmp_path) == false)
        continue;

      cover_path = tmp_path;
      break;
    }

    // Add blindtest to collection
    blindtests.push_back({
      .music = music_path,
      .cover = cover_path,
      .answer = music_path.stem().string(),
      .done = false
      });

    // Verbose
    std::cout << "Blindtest '" << music_path.stem().string() << "' loaded." << std::endl;
  }

  // Load fastest finger question
  for (const auto& entry : json.get(L"fastest").array()) {
    try {
      // Load question
      Fastest fastest{
        .id = entry->object().get(L"id").string(),
        .question = entry->object().get(L"question").string(),
        .answers = {},
        .info = entry->object().get(L"info").string(),
        .done = false
      };

      // Need 4 answers
      if (entry->object().get(L"answers").array().size() != 4)
        continue;

      // Load answers
      for (unsigned int index = 0; index < 4; index++)
        fastest.answers[index] = entry->object().get(L"answers").array()[index].string();

      // Add question to pool
      fastests.push_back(std::move(fastest));

      // Verbose
      std::wcout << "Fastest finger '" << fastests.back().question << "' loaded." << std::endl;
    }
    catch (const std::exception&) {
      continue;
    }
  }

  // Load Millionaire question sets
  for (const auto& entry : json.get(L"millionaire").array()) {
    try {
      Millionaire millionaire = {
        .id = entry->object().get(L"id").string(),
        .name = entry->object().get(L"name").string(),
        .questions = {},
        .done = false
      };

      // Load questions of set
      for (const auto& entryQuestion : entry->object().get(L"questions").array()) {
        try {
          Millionaire::Question question = {
            .id = entryQuestion->object().get(L"id").string(),
            .set = {},
            .question = entryQuestion->object().get(L"question").string(),
            .answers = {},
            .correct = 0,
            .info = entryQuestion->object().get(L"info").string(),
            .done = false
          };

          // Check number of answers and set
          if (entryQuestion->object().get(L"set").array().empty() == true ||
            entryQuestion->object().get(L"answers").array().size() != 4)
            continue;

          // Get set numbers
          for (const auto& setEntry : entryQuestion->object().get(L"set").array())
            question.set.insert((unsigned int)std::clamp((int)setEntry->number(), 1, 15) - 1);

          // Get answers
          for (unsigned int index = 0; index < 4; index++)
            question.answers[index] = entryQuestion->object().get(L"answers").array()[index].string();

          // Correct answer index forced, don't shuffle
          if (entryQuestion->object().contains(L"correct") == true)
            question.correct = (unsigned int)std::clamp((int)entryQuestion->object().get(L"correct").number(), 1, 4) - 1;

          // No correct index, shuffle answers
          else {
            auto correct = question.answers.front();

            std::shuffle(question.answers.begin(), question.answers.end(), randomEngine);
            question.correct = (unsigned int)std::distance(question.answers.begin(), std::find(question.answers.begin(), question.answers.end(), correct));
          }

          std::wcout << "Add question: " << question.question << std::endl;

          // Add question to set pool
          millionaire.questions.push_back(std::move(question));
        }
        catch (const std::exception&) {
          continue;
        }
      }

      // Not enough questions
      // NOTE: we should check for number of question of each step
      if (millionaire.questions.size() < 15)
        continue;

      // Add question set to pool
      millionaires.push_back(std::move(millionaire));
    }
    catch (const std::exception&) {
      continue;
    }
  }

  // Shuffle blindtest
  std::shuffle(blindtests.begin(), blindtests.end(), randomEngine);

  // Shuffle questions
  std::shuffle(questions.begin(), questions.end(), randomEngine);

  std::cout << std::endl
    << "Quiz loaded:" << std::endl
    << "  Avatars:        " << avatars.size() << std::endl
    << "  Fastest finger: " << fastests.size() << std::endl
    << "  Questions:      " << questions.size() << std::endl
    << std::endl;
}

QUIZ::Quiz::Entity::Entity() :
  _sprite(_texture),
  _texture(),
  _text(Game::FontLibrary::Instance().get(Game::Config::ExecutablePath / "assets" / "fonts" / "04b03.ttf"))
{
  // Default values
  reset();
}

void  QUIZ::Quiz::Entity::reset()
{
  // Default values
  setPosition(0.5f, 0.5f);
  setScale(1.f, 1.f);
  setColor(0.f, 0.f, 0.f, 0.f);
  setLerp(0.f);
  setTexture();
  setText();
  setDead(false);
  setOutline(0.f);
}

void  QUIZ::Quiz::Entity::setPosition(float x, float y) { _position = _targetPosition = { x, y }; }
void  QUIZ::Quiz::Entity::setTargetPosition(float x, float y) { _targetPosition = { x, y }; }
void  QUIZ::Quiz::Entity::setScale(float x, float y) { _scale = _targetScale = { x, y }; }
void  QUIZ::Quiz::Entity::setTargetScale(float x, float y) { _targetScale = { x, y }; }
void  QUIZ::Quiz::Entity::setColor(float r, float g, float b, float a) { _color = _targetColor = { r, g, b, a }; }
void  QUIZ::Quiz::Entity::setTargetColor(float r, float g, float b, float a) { _targetColor = { r, g, b, a }; }
void  QUIZ::Quiz::Entity::setOutline(float t) { _outline = t; }
void  QUIZ::Quiz::Entity::setLerp(float l) { _lerp = l; }
void  QUIZ::Quiz::Entity::setDead(bool v) { _dead = v; }

void  QUIZ::Quiz::Entity::setTexture(const std::filesystem::path& path)
{
  // Unload texture
  if (path.empty() == true) {
    sf::Image image;

    // Set texture as a transparent 1x1 image
    image.resize({ 1u, 1u }, sf::Color::Transparent);
    if (_texture.resize(image.getSize()) == false)
      throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
    _texture.update(image);
    return;
  }

  // Load texture and generate mipmap
  if (_texture.loadFromFile(path.string()) == false || _texture.generateMipmap() == false)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Smooth textures
  _texture.setSmooth(true);

  // Set origin to center of sprite
  _sprite.setTexture(_texture, true);
  _sprite.setOrigin({ _texture.getSize().x / 2.f, _texture.getSize().y / 2.f });
}

void  QUIZ::Quiz::Entity::setText(const std::wstring& text)
{
  // Change text
  _text.setString(text);
  _text.setFont(Game::FontLibrary::Instance().get(Game::Config::ExecutablePath / "assets" / "fonts" / "roboto.ttf"));
  _text.setCharacterSize(128);

  // Set origin to center of text
  _text.setOrigin({
    _text.getLocalBounds().position.x + _text.getLocalBounds().size.x / 2.f,
    _text.getLocalBounds().position.y + _text.getLocalBounds().size.y / 2.f
    }
  );
}

Math::Vector<2>     QUIZ::Quiz::Entity::getPosition() const { return _position; }
Math::Vector<2>     QUIZ::Quiz::Entity::getTargetPosition() const { return _targetPosition; }
Math::Vector<2>     QUIZ::Quiz::Entity::getScale() const { return _scale; }
Math::Vector<2>     QUIZ::Quiz::Entity::getTargetScale() const { return _targetScale; }
Math::Vector<4>     QUIZ::Quiz::Entity::getColor() const { return _color; }
Math::Vector<4>     QUIZ::Quiz::Entity::getTargetColor() const { return _targetColor; }
float               QUIZ::Quiz::Entity::getOutline() const { return _outline; }
float               QUIZ::Quiz::Entity::getLerp() const { return _lerp; }
bool                QUIZ::Quiz::Entity::getDead() const { return _dead; }
std::wstring        QUIZ::Quiz::Entity::getText() const { return _text.getString().toWideString(); }

const sf::Sprite& QUIZ::Quiz::Entity::sprite() const
{
  // Get entity sprite
  return _sprite;
}

bool  QUIZ::Quiz::Entity::update(float elapsed)
{
  // Lerp entity component
  _position = lerp(_position, _targetPosition, elapsed);
  _scale = lerp(_scale, _targetScale, elapsed);
  _color = lerp(_color, _targetColor, elapsed);

  // Check if entity is dead
  if (_dead == true)
  {
    // Will never die, instant deletion
    if (_targetColor.get<3>() != 0.f && _targetScale.get<0>() != 0.f && _targetScale.get<1>() != 0.f)
      return true;

    // Dead condition met
    if (_color.get<3>() == 0.f || _scale.get<0>() == 0.f || _scale.get<1>() == 0.f)
      return true;
  }

  return false;
}

void  QUIZ::Quiz::Entity::draw()
{
  auto&           window = Game::Window::Instance();
  Math::Vector<2> windowSize = { (float)window.getSize().x(), (float)window.getSize().y() };
  Math::Vector<2> spriteSize = { (float)_texture.getSize().x, (float)_texture.getSize().y };
  float           spriteScale = std::min(
    windowSize.x() / spriteSize.x() * _scale.x(),
    windowSize.y() / spriteSize.y() * _scale.y()
  );

  // Update lerp for instant move
  update(0.f);

  // Update sprite position
  _sprite.setPosition({ windowSize.x() * _position.x(), windowSize.y() * _position.y() });
  _sprite.setScale({ spriteScale, spriteScale });
  _sprite.setColor(sf::Color(255, 255, 255,
    (std::uint8_t)(std::clamp(_color.get<3>(), 0.f, 255.f) * 255)
  ));

  // Draw sprite
  window.draw(_sprite);

  Math::Vector<2> textSize = { _text.getLocalBounds().size.x, _text.getLocalBounds().size.y };
  float           textScale = std::min(
    windowSize.x() / textSize.x() * _scale.x() * 0.9f,
    windowSize.y() / textSize.y() * _scale.y() * (1.f - 1.f / (2.f + std::ranges::count(_text.getString().toAnsiString(), '\n')))
  ) * 0.9f;

  // Update text position
  _text.setPosition({ windowSize.x() * _position.x(), windowSize.y() * _position.y() });
  _text.setScale({ textScale, textScale });
  _text.setOutlineColor(sf::Color(0, 0, 0, (std::uint8_t)(std::clamp(_color.get<3>(), 0.f, 255.f) * 255)));
  _text.setFillColor(sf::Color(255, 255, 255,
    (std::uint8_t)(std::clamp(_color.get<3>(), 0.f, 255.f) * 255)
  ));

  // Draw text
  window.draw(_text);

  if (_outline > 0.f) {
    sf::RectangleShape  rectangle;

    // Update rectangle position
    rectangle.setPosition({ windowSize.x() * _position.x(), windowSize.y() * _position.y() });
    rectangle.setSize({ windowSize.x() * _scale.x(), windowSize.y() * _scale.y() });
    rectangle.setOrigin(rectangle.getSize() / 2.f);
    rectangle.setOutlineThickness(std::min(windowSize.x(), windowSize.y()) * _outline);
    rectangle.setOutlineColor(sf::Color(
      (std::uint8_t)(std::clamp(_color.get<0>(), 0.f, 255.f) * 255),
      (std::uint8_t)(std::clamp(_color.get<1>(), 0.f, 255.f) * 255),
      (std::uint8_t)(std::clamp(_color.get<2>(), 0.f, 255.f) * 255),
      (std::uint8_t)(std::clamp(_color.get<3>(), 0.f, 255.f) * 255)
    ));
    rectangle.setFillColor(sf::Color(rectangle.getOutlineColor().r, rectangle.getOutlineColor().g, rectangle.getOutlineColor().b, rectangle.getOutlineColor().a / 8));

    // Draw rectangle
    window.draw(rectangle);
  }
}

bool  QUIZ::Quiz::Entity::hover() const
{
  auto coords = Game::Window::Instance().pixelToCoords(Game::Window::Instance().mouse().position());

  // Check if cursor is on sprite
  return _sprite.getGlobalBounds().contains({ coords.x(), coords.y() });
}

QUIZ::Quiz::ProgressBar::ProgressBar() :
  _value(0.f),
  _height(0.f),
  _hidden(true)
{}

void  QUIZ::Quiz::ProgressBar::set(float value)
{
  // Limit value to [0; 1]
  _value = std::clamp(value, 0.f, 1.f);
}

float QUIZ::Quiz::ProgressBar::get() const
{
  // Get current value
  return _value;
}

void  QUIZ::Quiz::ProgressBar::hide()
{
  // Set hidden flag
  _hidden = true;
}

void  QUIZ::Quiz::ProgressBar::show()
{
  // Unset hidden flag
  _hidden = false;
}

void  QUIZ::Quiz::ProgressBar::update(float elapsed)
{
  if (_hidden == true)
    _height = std::max(0.f, _height - elapsed * 2.f);
  else
    _height = std::min(1.f, _height + elapsed * 2.f);
}

void  QUIZ::Quiz::ProgressBar::draw()
{
  sf::RectangleShape  bar;
  auto&               window = Game::Window::Instance();

  // Setup bar
  bar.setPosition({ 0.f, window.getSize().y() - _height * window.getSize().y() * 0.01f});
  bar.setSize({ window.getSize().x() * _value, _height * window.getSize().y() * 0.01f });
  bar.setFillColor(sf::Color::White);
  bar.setOutlineThickness(0.f);

  // Draw bar
  window.draw(bar);
}