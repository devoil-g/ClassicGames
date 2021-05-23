#pragma once

#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>

#include "Doom/Doom.hpp"
#include "Scenes/AbstractScene.hpp"

namespace DOOM
{
  class IntermissionDoomScene : public Game::AbstractScene
  {
  private:
    struct Animation
    {
      int                   duration; // Frame duration (tics)
      int                   offset;   // Time offset of animation (tics)
      std::vector<uint64_t> frames;   // Keys to frame textures
      sf::Vector2i          position; // Position of animation
      int                   level;    // Level this animation has to be displayed (0 for all)
    };

    enum State
    {
      StateStats,
      StateNext,
      StateCount
    };

    static const int  TitleY;                                   // Title vertical position (centred)
    static const int  StatsSoloX, StatsSoloY, StatsSoloSpacing; // Position of kills/items/secret stats in solo mode
    static const int  StatsCoopX, StatsCoopY, StatsCoopSpacing; // Position of kills/items/secret stats in coop mode
    static const int  TimeX, TimeY;                             // Position of time
    static const int  SpeedPercent;                             // Percentage increase per tic
    static const int  SpeedTime;                                // Time increase per tic
    static const int  SpeedPistol;                              // Tic between two pistol shoot sound

    static const std::array<std::array<sf::Vector2i, 9>, 3>                         _positions;   // Position of level on the map per episode/level (ignore this in DOOM II & Episode 4)
    static const std::array<std::vector<DOOM::IntermissionDoomScene::Animation>, 3> _animations;  // Animations for DOOM I background

    DOOM::Doom&                       _doom;        // DOOM instance
    const std::pair<uint8_t, uint8_t> _previous;    // Previous level
    const std::pair<uint8_t, uint8_t> _next;        // Next level
    DOOM::Doom::Level::Statistics     _statistics;  // Previous level statistics

    DOOM::IntermissionDoomScene::State  _state;         // Current state
    sf::Time                            _elapsed;       // Elapsed time
    sf::Time                            _nextElapsed;   // Elapsed time in next level screen

    struct Counter
    {
      float value;  // Current value of counter (negative if not displayed)
      float max;    // Max value of counter
    };

    std::map<int, DOOM::IntermissionDoomScene::Counter> _kills;   // Kills counters of players
    std::map<int, DOOM::IntermissionDoomScene::Counter> _items;   // Items counters of players
    std::map<int, DOOM::IntermissionDoomScene::Counter> _secrets; // Secrets counters of players
    std::map<int, DOOM::IntermissionDoomScene::Counter> _times;   // Time counters (0: time, 1: par)

    const DOOM::Doom::Resources::Texture&                                               _background;        // Screen background
    const DOOM::Doom::Resources::Texture&                                               _playerFace;        // Face of the player
    const std::array<std::reference_wrapper<const DOOM::Doom::Resources::Texture>, 4>   _playerBackground;  // Face of the player
    const std::array<std::reference_wrapper<const DOOM::Doom::Resources::Texture>, 10>  _textNumbers;       // Individual digits
    const DOOM::Doom::Resources::Texture&                                               _textFinished;      // Text "Finished"
    const DOOM::Doom::Resources::Texture&                                               _textEntering;      // Text "Entering"
    const DOOM::Doom::Resources::Texture&                                               _textKills;         // Text "Kills"
    const DOOM::Doom::Resources::Texture&                                               _textItems;         // Text "Items"
    const DOOM::Doom::Resources::Texture&                                               _textSecret;        // Text "Secret"
    const DOOM::Doom::Resources::Texture&                                               _textScrt;          // Text "Scrt"
    const DOOM::Doom::Resources::Texture&                                               _textTime;          // Text "Time"
    const DOOM::Doom::Resources::Texture&                                               _textSucks;         // Text "Sucks"
    const DOOM::Doom::Resources::Texture&                                               _textPar;           // Text "Par"
    const DOOM::Doom::Resources::Texture&                                               _textMinus;         // Character '-'
    const DOOM::Doom::Resources::Texture&                                               _textPercent;       // Character '%'
    const DOOM::Doom::Resources::Texture&                                               _textColon;         // Character ':'
    
    float getPar(std::pair<uint8_t, uint8_t> level) const;  // Return par time of current level, NaN if none registered

    void  updateStatistics(sf::Time elapsed);
    void  updateStatisticsCounters(sf::Time& elapsed, std::map<int, DOOM::IntermissionDoomScene::Counter>& counters, int speed);
    bool  updateStatisticsCountersCheck(const std::map<int, DOOM::IntermissionDoomScene::Counter>& counters);    // Return true if all counters are completed
    void  updateStatisticsCountersComplete(std::map<int, DOOM::IntermissionDoomScene::Counter>& counters);    // Force complete counters
    void  updateNext(sf::Time elapsed);
    bool  updateSkip(); // Return true if skip button has been pressed
    void  updateEnd();  // Go to next screen/level

    void  drawBackground();
    void  drawStatistics();
    void  drawStatisticsSolo();
    void  drawStatisticsCoop();
    void  drawNext();

    void  drawPercent(sf::Vector2i position, int value);
    void  drawTime(sf::Vector2i position, int value);

  public:
    IntermissionDoomScene(Game::SceneMachine& machine, DOOM::Doom& doom, std::pair<uint8_t, uint8_t> previous, std::pair<uint8_t, uint8_t> next, const DOOM::Doom::Level::Statistics& statistics);
    ~IntermissionDoomScene() override = default;

    bool  update(sf::Time elapsed) override;  // Update state
    void  draw() override;                    // Draw state
  };
}