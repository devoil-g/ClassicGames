#include "States/AbstractState.hpp"

Game::AbstractState::AbstractState(Game::StateMachine& machine) :
  _machine(machine)
{}

Game::AbstractState::~AbstractState() = default;