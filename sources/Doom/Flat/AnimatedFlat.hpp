#ifndef _ANIMATED_FLAT_HPP_
#define _ANIMATED_FLAT_HPP_

#include <unordered_map>

#include <SFML/System/Time.hpp>

#include "Doom/Wad.hpp"
#include "Doom/Flat/AbstractFlat.hpp"

namespace DOOM
{
  class AnimatedFlat : public virtual DOOM::AbstractFlat
  {
    static const int	FrameDuration = 8;	// Tics between two frames of animation

  private:
    sf::Time				_elapsed;	// Total elapsed time
    std::vector<std::vector<uint8_t>>	_flats;		// Vector of flats composing animation
    
  public:
    AnimatedFlat(const DOOM::Wad & wad, const std::vector<uint64_t> & flat);
    ~AnimatedFlat() override;

    void				update(sf::Time elapsed) override;	// Update animation sequence
    const std::vector<uint8_t> &	flat() const override;			// Return flat to be displayed
  };
};

#endif