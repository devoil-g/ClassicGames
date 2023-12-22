#pragma once

#include <array>
#include <vector>

#include <SFML/System/Time.hpp>

#include "Doom/Flat/AbstractFlat.hpp"

namespace DOOM
{
  template<unsigned int FrameDuration = 8>
  class AnimatedFlat : public virtual DOOM::AbstractFlat
  {
  private:
    float                                   _elapsed; // Total elapsed time
    std::vector<std::array<uint8_t, 4096>>  _flats;   // Vector of flats composing animation

  public:
    AnimatedFlat(DOOM::Doom& doom, const std::vector<uint64_t>& frames) :
      DOOM::AbstractFlat(doom),
      _elapsed(),
      _flats(frames.size())
    {
      // Convert each frames in animation
      for (int index = 0; index < frames.size(); index++)
      {
        // Find frame in WAD
        const auto  rawflat = doom.wad.resources.flats.find(frames[index]);

        // Cancel if error
        if (rawflat == doom.wad.resources.flats.end())
          throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

        // Convert WAD flat
        std::memcpy(_flats[index].data(), rawflat->second.texture, sizeof(DOOM::Wad::RawResources::Flat::texture));
      }
    }

    ~AnimatedFlat() override = default;

    void  update(DOOM::Doom& doom, float elapsed) override        // Update animation sequence
    {
      // Add elapsed time to counter
      _elapsed += elapsed;
    }

    const std::array<uint8_t, 4096>& flat() const override        // Return flat to be displayed
    {
      // Return flat of current frame
      return _flats[(std::size_t)(_elapsed / (DOOM::Doom::Tic * FrameDuration)) % _flats.size()];
    }
  };
}