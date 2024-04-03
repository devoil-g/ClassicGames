#include <filesystem>
#include <iostream>

#include <SFML/Audio.hpp>

#include "System/Library/TextureLibrary.hpp"
#include "System/Config.hpp"
#include "Quiz/Quiz.hpp"

QUIZ::Quiz::Quiz() :
  avatars(),
  players()
{
  // Load avatars from directory
  for (const auto& entry : std::filesystem::directory_iterator(Game::Config::ExecutablePath / "assets" / "quiz" / "avatars")) {
    sf::Texture texture;

    // Load texture
    if (texture.loadFromFile(entry.path().string()) == false)
      continue;

    // Add avatar to collection
    avatars.push_back(texture);

    // Verbose
    std::cout << "Avatar '" << entry.path().stem().string() << "' loaded." << std::endl;
  }

  // Smooth avatar
  for (auto& avatar : avatars) {
    avatar.setSmooth(true);
    avatar.generateMipmap();
  }

  // Load blindtests from directory
  for (const auto& entry : std::filesystem::directory_iterator(Game::Config::ExecutablePath / "assets" / "quiz" / "blindtest")) {
    sf::Music music;
    auto music_path = entry.path();
    auto cover_path = Game::Config::ExecutablePath / "assets" / "quiz" / "images" / "default.png";

    // Try to load file as a music
    if ((music_path.extension() != ".ogg" && music_path.extension() != ".wav" && music_path.extension() != ".flac") ||
      music.openFromFile(music_path.string()) == false)
      continue;

    // Find image with same name in the directory
    for (const auto& ext : { "jpg", "png", "bmp", "tga", "jpeg", "gif", "psd", "hdr", "pic" }) {
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

  std::cout << std::endl
    << "Quiz loaded:" << std::endl
    << "  Avatars:    " << avatars.size() << std::endl
    << "  Blindtests: " << blindtests.size() << std::endl
    << std::endl;
}