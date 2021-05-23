#include "Scenes/AbstractScene.hpp"

Game::AbstractScene::AbstractScene(Game::SceneMachine& machine) :
  _machine(machine)
{}

Game::AbstractScene::~AbstractScene() = default;