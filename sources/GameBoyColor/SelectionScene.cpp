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
    for (const auto& entry : std::filesystem::directory_iterator(Game::Config::ExecutablePath / "assets" / "gbc")) {
      // Only .gb or .gbc files
      if (entry.path().extension() != ".gb" && entry.path().extension() != ".gbc" && entry.path().extension() != ".bin")
        continue;

      try {
        GBC::GameBoyColor gbc(entry);

        // Add game to menu list
        if (gbc.header().header_checksum && gbc.header().global_checksum)
          add(gbc.header().title, std::function<void(Game::AbstractMenuScene::Item&)>(std::bind(&GBC::SelectionScene::selectGame, this, std::placeholders::_1, std::filesystem::absolute(entry).string())));
      }
      catch (const std::exception&e) {
        std::cout << e.what() << std::endl;
      }
    }
  }
  catch (const std::exception&) {}
  
#ifdef _WIN32
  // Add browse files option
  footer("Browse...", std::function<void(Game::AbstractMenuScene::Item&)>(std::bind(&GBC::SelectionScene::selectBrowse, this, std::placeholders::_1)));
#endif

  // Nothing to load, browse to file
  if (empty() == true)
    _selected = browse();
}

bool  GBC::SelectionScene::update(float elapsed)
{
  // Swap to game if selected
  if (_selected.empty() == false) {
    Game::SceneMachine& machine = _machine;

    try {
      machine.swap<GBC::EmulationScene>(_selected);
    }
    catch (const std::exception&) {
      machine.swap<Game::MessageScene>("Error: failed to run game\n" + _selected.string());
    }
    return false;
  }

  // Nothing to load, browse failed
  if (empty() == true) {
    _machine.pop();
    return false;
  }

  // Update menu
  return Game::AbstractMenuScene::update(elapsed);
}

void  GBC::SelectionScene::selectGame(Game::AbstractMenuScene::Item&, const std::filesystem::path& file)
{
  // Run this game next frame (avoid "Browse..." menu lag)
  _selected = file;
}

void  GBC::SelectionScene::selectBrowse(Game::AbstractMenuScene::Item& item)
{
  // Select game from browse menu
  selectGame(item, browse());
}

std::filesystem::path GBC::SelectionScene::browse() const
{
#ifdef _WIN32
  // See MSDN of GetOpenFileName
  ::OPENFILENAME  fileinfo;
  ::CHAR          path[MAX_PATH];
  std::string     dir = Game::Config::ExecutablePath.string();

  std::memset(path, 0, sizeof(path));
  fileinfo.lStructSize = sizeof(::OPENFILENAME);
  fileinfo.hwndOwner = nullptr;
  fileinfo.lpstrFilter = "All supported format (.gb, .gbc)\0*.gb;*.gbc\0Game Boy\0*.gb\0Game Boy Color\0*.gbc\0All files\0*\0\0";
  fileinfo.lpstrCustomFilter = nullptr;
  fileinfo.nFilterIndex = 1;
  fileinfo.lpstrFile = path;
  fileinfo.nMaxFile = MAX_PATH;
  fileinfo.lpstrFileTitle = nullptr;
  fileinfo.lpstrInitialDir = dir.c_str();
  fileinfo.lpstrTitle = nullptr;
  fileinfo.Flags = OFN_FILEMUSTEXIST;
  fileinfo.lpstrDefExt = nullptr;
  fileinfo.FlagsEx = 0;

  // Open file
  if (::GetOpenFileName(&fileinfo))
    return path;
#else
  // Not implemented
  throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
#endif

  // Failure
  return "";
}