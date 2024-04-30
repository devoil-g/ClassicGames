#include <array>
#include <filesystem>
#include <iostream>
#include <random>
#include <chrono>     
#include <algorithm>
#include <exception>

#include <SFML/Audio.hpp>

#include "System/Config.hpp"
#include "System/Window.hpp"
#include "Quiz/Quiz.hpp"

QUIZ::Quiz::Quiz() :
  avatars(),
  players()
{
  const std::array<std::string, 9> textureExtensions = { "jpg", "png", "bmp", "tga", "jpeg", "gif", "psd", "hdr", "pic" };
  const std::array<std::string, 3> musicExtensions = { "ogg", "wav", "flac" };

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

        sf::Texture texture;

        // Load texture
        if (texture.loadFromFile(skin.path().string()) == false)
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

      sf::Texture texture;

      // Load texture
      if (texture.loadFromFile(avatar.path().string()) == false)
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
    sf::Music music;
    auto music_path = entry.path();
    auto cover_path = Game::Config::ExecutablePath / "assets" / "quiz" / "images" / "default.png";

    // Try to load file as a music
    if (music_path.has_extension() == false ||
      std::find(musicExtensions.begin(), musicExtensions.end(), music_path.extension().string().substr(1)) == musicExtensions.end() ||
      music.openFromFile(music_path.string()) == false)
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

      sf::Image cover;

      // Try to load image
      if (cover.loadFromFile(tmp_path.string()) == true) {
        cover_path = tmp_path;
        break;
      }
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

  // Shuffle blindtest
  std::shuffle(blindtests.begin(), blindtests.end(), std::default_random_engine((unsigned int)std::chrono::system_clock::now().time_since_epoch().count()));

  std::cout << std::endl
    << "Quiz loaded:" << std::endl
    << "  Avatars:    " << avatars.size() << std::endl
    << "  Blindtests: " << blindtests.size() << std::endl
    << std::endl;
}

QUIZ::Quiz::Entity::Entity(const std::filesystem::path& texturePath)
{
  // Default values
  setPosition(0.5f, 0.5f);
  setScale(1.f, 1.f);
  setColor(0.f, 0.f, 0.f, 0.f);
  setLerp(0.f);
  setTexture(texturePath);
}

void  QUIZ::Quiz::Entity::setPosition(float x, float y) { _position = _targetPosition = { x, y }; }
void  QUIZ::Quiz::Entity::setTargetPosition(float x, float y) { _targetPosition = { x, y }; }
void  QUIZ::Quiz::Entity::setScale(float x, float y) { _scale = _targetScale = { x, y }; }
void  QUIZ::Quiz::Entity::setTargetScale(float x, float y) { _targetScale = { x, y }; }
void  QUIZ::Quiz::Entity::setColor(float r, float g, float b, float a) { _color = _targetColor = { r, g, b, a }; }
void  QUIZ::Quiz::Entity::setTargetColor(float r, float g, float b, float a) { _targetColor = { r, g, b, a }; }
void  QUIZ::Quiz::Entity::setLerp(float l) { _lerp = l; }

void  QUIZ::Quiz::Entity::setTexture(const std::filesystem::path& path)
{
  // Load texture and generate mipmap
  if (_texture.loadFromFile(path.string()) == false || _texture.generateMipmap() == false)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Smooth textures
  _texture.setSmooth(true);

  // Set origin to center of sprite
  _sprite.setTexture(_texture, true);
  _sprite.setOrigin(_texture.getSize().x / 2.f, _texture.getSize().y / 2.f);
}

Math::Vector<2> QUIZ::Quiz::Entity::getPosition() const { return _position; }
Math::Vector<2> QUIZ::Quiz::Entity::getTargetPosition() const { return _targetPosition; }
Math::Vector<2> QUIZ::Quiz::Entity::getScale() const { return _scale; }
Math::Vector<2> QUIZ::Quiz::Entity::getTargetScale() const { return _targetScale; }
Math::Vector<4> QUIZ::Quiz::Entity::getColor() const { return _color; }
Math::Vector<4> QUIZ::Quiz::Entity::getTargetColor() const { return _targetColor; }
float           QUIZ::Quiz::Entity::getLerp() const { return _lerp; }

const sf::Sprite& QUIZ::Quiz::Entity::sprite() const
{
  // Get entity sprite
  return _sprite;
}

void  QUIZ::Quiz::Entity::update(float elapsed)
{
  // Lerp entity component
  _position = lerp(_position, _targetPosition, elapsed);
  _scale = lerp(_scale, _targetScale, elapsed);
  _color = lerp(_color, _targetColor, elapsed);
}

void  QUIZ::Quiz::Entity::draw()
{
  auto&           window = Game::Window::Instance();
  Math::Vector<2> windowSize = { (float)window.window().getSize().x, (float)window.window().getSize().y };
  Math::Vector<2> spriteSize = { (float)_texture.getSize().x, (float)_texture.getSize().y };
  float           spriteScale = std::min(
    windowSize.x() / spriteSize.x() * _scale.x(),
    windowSize.y() / spriteSize.y() * _scale.y()
  );

  // Update lerp for instant move
  update(0.f);

  // Update sprite position
  _sprite.setPosition(windowSize.x() * _position.x(), windowSize.y() * _position.y());
  _sprite.setScale(spriteScale, spriteScale);
  _sprite.setColor(sf::Color(
    (std::uint8_t)(std::clamp(_color.get<0>(), 0.f, 255.f) * 255),
    (std::uint8_t)(std::clamp(_color.get<1>(), 0.f, 255.f) * 255),
    (std::uint8_t)(std::clamp(_color.get<2>(), 0.f, 255.f) * 255),
    (std::uint8_t)(std::clamp(_color.get<3>(), 0.f, 255.f) * 255)
  ));

  // Draw sprite
  window.window().draw(_sprite);
}

bool  QUIZ::Quiz::Entity::hover() const
{
  // Check if cursor is on sprite
  return _sprite.getGlobalBounds().contains(Game::Window::Instance().window().mapPixelToCoords(Game::Window::Instance().mouse().position()));
}