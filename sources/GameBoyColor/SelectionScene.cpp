#include <filesystem>

#include "System/Library/FontLibrary.hpp"
#include "Scenes/MessageScene.hpp"
#include "GameBoyColor/EmulationScene.hpp"
#include "GameBoyColor/GameBoyColor.hpp"
#include "GameBoyColor/SelectionScene.hpp"
#include "System/Config.hpp"

#ifdef _WIN32
#include <Windows.h>
#endif

GBC::SelectionScene::SelectionScene(Game::SceneMachine& machine) :
  Game::AbstractMenuScene(machine),
  _selected()
{
  // Set menu title
  title("GameBoy");

  // List games in game directory
  try {
    for (const auto& entry : std::filesystem::directory_iterator(Game::Config::ExecutablePath + "/assets/gbc/")) {
      try {
        GBC::GameBoyColor gbc(std::filesystem::absolute(entry).string());

        // Add game to menu list
        if (gbc.header().header_checksum && gbc.header().global_checksum)
          add(gbc.header().title, std::function<void(Game::AbstractMenuScene::Item&)>(std::bind(&GBC::SelectionScene::selectGame, this, std::placeholders::_1, std::filesystem::absolute(entry).string())));
      }
      catch (const std::exception&) {}
    }
  }
  catch (const std::exception&) {}
  
#ifdef _WIN32
  // Add browse files option
  footer("Browse...", std::function<void(Game::AbstractMenuScene::Item&)>(std::bind(&GBC::SelectionScene::selectBrowse, this, std::placeholders::_1)));
#endif

  // Nothing to load
  if (empty() == true)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
}

bool  GBC::SelectionScene::update(sf::Time elapsed)
{
  // Swap to game if selected
  if (_selected.empty() == false) {
    Game::SceneMachine& machine = _machine;

    try {
      machine.swap<GBC::EmulationScene>(_selected);
    }
    catch (const std::exception&) {
      machine.swap<Game::MessageScene>("Error: failed to run game\n" + _selected);
    }
    return false;
  }

  // Update menu
  return Game::AbstractMenuScene::update(elapsed);
}

void  GBC::SelectionScene::draw()
{
  // Draw menu
  Game::AbstractMenuScene::draw();
}

void  GBC::SelectionScene::selectGame(Game::AbstractMenuScene::Item&, const std::string& file)
{
  // Run this game next frame (avoid "Browse..." menu lag)
  _selected = file;
}

void  GBC::SelectionScene::selectBrowse(Game::AbstractMenuScene::Item& item)
{
#ifdef _WIN32
  // See MSDN of GetOpenFileName
  ::OPENFILENAME  fileinfo;
  ::CHAR          path[MAX_PATH];

  std::memset(path, 0, sizeof(path));
  fileinfo.lStructSize = sizeof(::OPENFILENAME);
  fileinfo.hwndOwner = nullptr;
  fileinfo.lpstrFilter = "All supported format (.gb, .gbc)\0*.gb;*.gbc\0Game Boy\0*.gb\0Game Boy Color\0*.gbc\0All files\0*\0\0";
  fileinfo.lpstrCustomFilter = nullptr;
  fileinfo.nFilterIndex = 1;
  fileinfo.lpstrFile = path;
  fileinfo.nMaxFile = MAX_PATH;
  fileinfo.lpstrFileTitle = nullptr;
  fileinfo.lpstrInitialDir = Game::Config::ExecutablePath.c_str();
  fileinfo.lpstrTitle = nullptr;
  fileinfo.Flags = OFN_FILEMUSTEXIST;
  fileinfo.lpstrDefExt = nullptr;
  fileinfo.FlagsEx = 0;

  // Open file
  if (::GetOpenFileName(&fileinfo))
    selectGame(item, std::string(path));
#else
  // Not implemented
  throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
#endif
}
