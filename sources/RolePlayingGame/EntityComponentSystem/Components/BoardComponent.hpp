#pragma once

namespace RPG
{
  class BoardComponent
  {
  public:
    bool  hover;  // Cell is hovered by cursor
    bool  move;   // Cell is a move target

    BoardComponent();
    BoardComponent(const BoardComponent&) = default;
    BoardComponent(BoardComponent&&) = default;
    ~BoardComponent() = default;

    BoardComponent& operator=(const BoardComponent&) = default;
    BoardComponent& operator=(BoardComponent&&) = default;
  };
}