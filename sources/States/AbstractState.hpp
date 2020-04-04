#pragma once

#include <SFML/System/Time.hpp>

namespace Game
{
  class AbstractState
  {
  public:
    virtual ~AbstractState() = 0 {};

    virtual bool  update(sf::Time) = 0; // You should override this method
    virtual void  draw() = 0;           // You should override this method
  };
}