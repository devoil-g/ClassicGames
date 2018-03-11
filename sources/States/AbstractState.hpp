#ifndef _ABSTRACT_STATE_HPP_
#define _ABSTRACT_STATE_HPP_

#include <SFML/System/Time.hpp>

namespace Game
{
  class AbstractState
  {
  public:
    virtual ~AbstractState() {};

    virtual bool	update(sf::Time) = 0;	// You should override this method
    virtual void	draw() = 0;		// You should override this method
  };
};

#endif
