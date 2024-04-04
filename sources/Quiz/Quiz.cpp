#include <array>
#include <filesystem>
#include <iostream>
#include <random>
#include <chrono>     
#include <algorithm>

#include <SFML/Audio.hpp>

#include "System/Library/TextureLibrary.hpp"
#include "System/Config.hpp"
#include "Quiz/Quiz.hpp"

QUIZ::Quiz::Quiz() :
  avatars(),
  players()
{
  const std::array<std::string, 9> textureExtensions = { "jpg", "png", "bmp", "tga", "jpeg", "gif", "psd", "hdr", "pic" };
  const std::array<std::string, 3> musicExtensions = { "ogg", "wav", "flac" };

  // Load avatars from directory
  for (const auto& avatar : std::filesystem::directory_iterator(Game::Config::ExecutablePath / "assets" / "quiz" / "avatars")) {
    std::vector<sf::Texture>  costumes;

    // Costume directory
    if (avatar.is_directory() == true) {
      for (const auto& costume : std::filesystem::directory_iterator(avatar))
      {
        // Unsupported extension
        if (costume.path().has_extension() == false ||
          std::find(textureExtensions.begin(), textureExtensions.end(), costume.path().extension().string().substr(1)) == textureExtensions.end())
          continue;

        sf::Texture texture;

        // Load texture
        if (texture.loadFromFile(costume.path().string()) == false)
          continue;

        // Add avatar to collection
        costumes.push_back(texture);
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
      costumes.push_back(texture);
    }
    
    // No costume to load
    if (costumes.empty() == true)
      continue;

    // Add costumes to avatars
    avatars.push_back(std::move(costumes));

    // Verbose
    std::cout << "Avatar '" << avatar.path().stem().string() << "' loaded." << std::endl;
  }

  // Smooth avatar
  for (auto& avatar : avatars) {
    for (auto& costume : avatar) {
      costume.setSmooth(true);
      costume.generateMipmap();
    }
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