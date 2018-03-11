#ifndef _START_MENU_STATE_HPP_
#define _START_MENU_STATE_HPP_

#include <SFML/Graphics/Text.hpp>

#include "States/AbstractState.hpp"

namespace Game
{
  class StartState : public Game::AbstractState
  {
  private:
    float	_elapsed;
    sf::Text	_text;
    
  public:
    StartState();
    ~StartState();

    bool	update(sf::Time) override;	// Update state
    void	draw() override;		// Draw state
  };
};

#endif