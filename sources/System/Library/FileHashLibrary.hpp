#pragma once

#include <filesystem>

#include "System/Library/AbstractLibrary.hpp"

namespace Game
{
  class FileHashLibrary : public Game::AbstractLibrary<std::filesystem::path, std::uint64_t>
  {
  private:
    FileHashLibrary();
    ~FileHashLibrary() override = default;

  public:
    inline static Game::FileHashLibrary& Instance() { static Game::FileHashLibrary singleton; return singleton; };  // Return unique instance (singleton)

    void  load(const std::filesystem::path&) override;  // Load a file hash in the library
  };
}