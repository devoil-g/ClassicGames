#include <filesystem>

#include "System/JavaScriptObjectNotation.hpp"
#include "System/Library/TextureLibrary.hpp"
#include "System/Config.hpp"
#include "Quiz/Quiz.hpp"

QUIZ::Quiz::Quiz(const std::string& config) :
  avatars(),
  players()
{
  const Game::JSON::Object  json(config);

  // Load avatars from directory
  for (const auto& entry : std::filesystem::directory_iterator(Game::Config::ExecutablePath + "/assets/quiz/avatars/")) {
    sf::Texture texture;

    if (texture.loadFromFile(entry.path().string()) == false)
      continue;

    texture.setSmooth(true);
    avatars.push_back(texture);
  }

  // Load blindtest
  try {
    int index = 0;
    for (const auto& entry : json.map.at("blindtest")->array().vector) {
      index++;
      try {
        blindtests.push_back({
          .music = entry->object().map.at("music")->string().value,
          .cover = entry->object().map.at("cover")->string().value,
          .answer = entry->object().map.at("answer")->string().value,
          .info = entry->object().map.at("info")->string().value,
          .score = (int)entry->object().map.at("score")->number().value,
        });
      }
      catch (const std::exception&) {
        std::cout << "Warning, invalid entry " << index << "# in blindtest." << std::endl;
      }
    }
  }
  catch (const std::exception&) {
    std::cout << "Warning, no blindtest section." << std::endl;
  }

  std::cout
    << "Quiz loaded:" << std::endl
    << "  Avatars:    " << avatars.size() << std::endl
    << "  Blindtests: " << blindtests.size() << std::endl
    << std::endl;
}