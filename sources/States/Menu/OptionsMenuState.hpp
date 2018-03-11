#ifndef _OPTIONS_MENU_STATE_HPP_
#define _OPTIONS_MENU_STATE_HPP_

#include <tuple>
#include <vector>

#include <SFML/Graphics/Text.hpp>

#include "States/Menu/AbstractMenuState.hpp"

namespace Game
{
  class OptionsMenuState : public Game::AbstractMenuState
  {
  private:
    void	selectFullscreen(Game::AbstractMenuState::Item &);	// Toggle fullscreen mode
    void	selectAntialiasing(Game::AbstractMenuState::Item &);	// Change antialiasing level
    void	selectReturn(Game::AbstractMenuState::Item &);		// Return to main menu

  public:
    OptionsMenuState();
    ~OptionsMenuState();

    bool	update(sf::Time) override;	// Update state
    void	draw() override;		// Draw state
  };
};

#endif