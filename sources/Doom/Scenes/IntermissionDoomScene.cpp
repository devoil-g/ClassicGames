#include "System/Window.hpp"
#include "Doom/Scenes/GameDoomScene.hpp"
#include "Doom/Scenes/MenuDoomScene.hpp"
#include "Doom/Scenes/IntermissionDoomScene.hpp"
#include "Doom/Scenes/TransitionDoomScene.hpp"
#include "Doom/Thing/PlayerThing.hpp"

const int DOOM::IntermissionDoomScene::TitleY = 2;
const int DOOM::IntermissionDoomScene::StatsSoloX = 50;
const int DOOM::IntermissionDoomScene::StatsSoloY = 50;
const int DOOM::IntermissionDoomScene::StatsSoloSpacing = 33;
const int DOOM::IntermissionDoomScene::StatsCoopX = 64;
const int DOOM::IntermissionDoomScene::StatsCoopY = 50;
const int DOOM::IntermissionDoomScene::StatsCoopSpacing = 64;
const int DOOM::IntermissionDoomScene::TimeX = 16;
const int DOOM::IntermissionDoomScene::TimeY = DOOM::Doom::RenderHeight - 32;
const int DOOM::IntermissionDoomScene::SpeedPercent = 2;
const int DOOM::IntermissionDoomScene::SpeedTime = 3;
const int DOOM::IntermissionDoomScene::SpeedPistol = 4;

const std::array<std::array<sf::Vector2i, 9>, 3> DOOM::IntermissionDoomScene::_positions =
{
  std::array<sf::Vector2i, 9>{  // KNEE-DEEP IN THE DEAD
      sf::Vector2i(185, 164),   // HANGAR
      sf::Vector2i(148, 143),   // NUCLEAR PLANT
      sf::Vector2i(69, 122),    // TOXIN REFINERY
      sf::Vector2i(209, 102),   // COMMAND CONTROL
      sf::Vector2i(116, 89),    // PHOBOS LAB
      sf::Vector2i(166, 55),    // CENTRAL PROCESSING
      sf::Vector2i(71, 56),     // COMPUTER STATION
      sf::Vector2i(135, 29),    // PHOBOS ANOMALY
      sf::Vector2i(71, 24)      // MILITARY BASE
  },
  std::array<sf::Vector2i, 9>{  // THE SHORES OF HELL
      sf::Vector2i(254, 25),    // DEIMOS ANOMALY
      sf::Vector2i(97, 50),     // CONTAINMENT AREA
      sf::Vector2i(188, 64),    // REFINERY
      sf::Vector2i(128, 78),    // DEIMOS LAB
      sf::Vector2i(214, 92),    // COMMAND CENTER
      sf::Vector2i(133, 130),   // HALLS OF THE DAMNED
      sf::Vector2i(208, 136),   // SPAWNING VATS
      sf::Vector2i(148, 140),   // TOWER OF BABEL
      sf::Vector2i(235, 158)    // FORTRESS OF MYSTERY
  },
  std::array<sf::Vector2i, 9>{  // INFERNO
      sf::Vector2i(156, 168),   // HELL KEEP
      sf::Vector2i(48, 154),    // SLOUGH OF DESPAIR
      sf::Vector2i(174, 95),    // PANDEMONIUM
      sf::Vector2i(265, 75),    // HOUSE OF PAIN
      sf::Vector2i(130, 48),    // UNHOLY CATHEDRAL
      sf::Vector2i(279, 23),    // MT. EREBUS
      sf::Vector2i(198, 48),    // GATE TO LIMBO
      sf::Vector2i(140, 25),    // DIS
      sf::Vector2i(281, 136)    // WARRENS
  }
};

const std::array<std::vector<DOOM::IntermissionDoomScene::Animation>, 3>  DOOM::IntermissionDoomScene::_animations =
{
  std::vector<DOOM::IntermissionDoomScene::Animation>{
    { 11, std::rand() % 11, { DOOM::str_to_key("WIA00000"), DOOM::str_to_key("WIA00001"), DOOM::str_to_key("WIA00002") }, { 224, 104 }, 0 },
    { 11, std::rand() % 11, { DOOM::str_to_key("WIA00100"), DOOM::str_to_key("WIA00101"), DOOM::str_to_key("WIA00102") }, { 184, 160 }, 0 },
    { 11, std::rand() % 11, { DOOM::str_to_key("WIA00200"), DOOM::str_to_key("WIA00201"), DOOM::str_to_key("WIA00202") }, { 112, 136 }, 0 },
    { 11, std::rand() % 11, { DOOM::str_to_key("WIA00300"), DOOM::str_to_key("WIA00301"), DOOM::str_to_key("WIA00302") }, { 72, 112 }, 0 },
    { 11, std::rand() % 11, { DOOM::str_to_key("WIA00400"), DOOM::str_to_key("WIA00401"), DOOM::str_to_key("WIA00402") }, { 88, 96 }, 0 },
    { 11, std::rand() % 11, { DOOM::str_to_key("WIA00500"), DOOM::str_to_key("WIA00501"), DOOM::str_to_key("WIA00502") }, { 64, 48 }, 0 },
    { 11, std::rand() % 11, { DOOM::str_to_key("WIA00600"), DOOM::str_to_key("WIA00601"), DOOM::str_to_key("WIA00602") }, { 192, 40 }, 0 },
    { 11, std::rand() % 11, { DOOM::str_to_key("WIA00700"), DOOM::str_to_key("WIA00701"), DOOM::str_to_key("WIA00702") }, { 136, 16 }, 0 },
    { 11, std::rand() % 11, { DOOM::str_to_key("WIA00800"), DOOM::str_to_key("WIA00801"), DOOM::str_to_key("WIA00802") }, { 80, 16 }, 0 },
    { 11, std::rand() % 11, { DOOM::str_to_key("WIA00900"), DOOM::str_to_key("WIA00901"), DOOM::str_to_key("WIA00902") }, { 64, 24 }, 0 }
  },
  std::vector<DOOM::IntermissionDoomScene::Animation>{
    { 11, 0, { DOOM::str_to_key("WIA10000") }, { 128, 136 }, 2 },
    { 11, 0, { DOOM::str_to_key("WIA10100") }, { 128, 136 }, 3 },
    { 11, 0, { DOOM::str_to_key("WIA10200") }, { 128, 136 }, 4 },
    { 11, 0, { DOOM::str_to_key("WIA10300") }, { 128, 136 }, 5 },
    { 11, 0, { DOOM::str_to_key("WIA10400") }, { 128, 136 }, 6 },
    { 11, 0, { DOOM::str_to_key("WIA10500") }, { 128, 136 }, 7 },
    { 11, 0, { DOOM::str_to_key("WIA10600") }, { 128, 136 }, 8 },
    { 11, 0, { DOOM::str_to_key("WIA10700"), DOOM::str_to_key("WIA10701"), DOOM::str_to_key("WIA10702") }, { 192, 144 }, 9 },
    { 11, 0, { DOOM::str_to_key("WIA10400") }, { 128, 136 }, 9 }
  },
  std::vector<DOOM::IntermissionDoomScene::Animation>{
    { 11, std::rand() % 11, { DOOM::str_to_key("WIA20000"), DOOM::str_to_key("WIA20001"), DOOM::str_to_key("WIA20002") }, { 104, 168 }, 0 },
    { 11, std::rand() % 11, { DOOM::str_to_key("WIA20100"), DOOM::str_to_key("WIA20101"), DOOM::str_to_key("WIA20102") }, { 40, 136 }, 0 },
    { 11, std::rand() % 11, { DOOM::str_to_key("WIA20200"), DOOM::str_to_key("WIA20201"), DOOM::str_to_key("WIA20202") }, { 160, 96 }, 0 },
    { 11, std::rand() % 11, { DOOM::str_to_key("WIA20300"), DOOM::str_to_key("WIA20301"), DOOM::str_to_key("WIA20302") }, { 104, 80 }, 0 },
    { 11, std::rand() % 11, { DOOM::str_to_key("WIA20400"), DOOM::str_to_key("WIA20401"), DOOM::str_to_key("WIA20402") }, { 120, 32 }, 0 },
    { 8, std::rand() % 8, { DOOM::str_to_key("WIA20500"), DOOM::str_to_key("WIA20501"), DOOM::str_to_key("WIA20502") }, { 40, 0 }, 0 }
  }
};

DOOM::IntermissionDoomScene::IntermissionDoomScene(Game::SceneMachine& machine, DOOM::Doom& doom, std::pair<uint8_t, uint8_t> previous, std::pair<uint8_t, uint8_t> next, const DOOM::Doom::Level::Statistics& statistics) :
  Game::AbstractScene(machine),
  _doom(doom),
  _previous(previous),
  _next(next),
  _statistics(statistics),
  _state(DOOM::IntermissionDoomScene::State::StateStats),
  _elapsed(sf::Time::Zero),
  _nextElapsed(sf::Time::Zero),
  _kills(),
  _items(),
  _secrets(),
  _times(),
  _background(doom.resources.getMenu(DOOM::str_to_key(
    (doom.mode == DOOM::Enum::Mode::ModeCommercial || (doom.mode == DOOM::Enum::Mode::ModeRetail && _previous.first == 4))
    ? (std::string("INTERPIC"))
    : (std::string("WIMAP") + std::to_string(previous.first - 1))
  ))),
  _playerFace(doom.resources.getMenu(DOOM::str_to_key("STFST01"))),
  _playerBackground
    {
      doom.resources.getMenu(DOOM::str_to_key("STPB0")),
      doom.resources.getMenu(DOOM::str_to_key("STPB1")),
      doom.resources.getMenu(DOOM::str_to_key("STPB2")),
      doom.resources.getMenu(DOOM::str_to_key("STPB3"))
    },
  _textNumbers
    {
      doom.resources.getMenu(DOOM::str_to_key("WINUM0")),
      doom.resources.getMenu(DOOM::str_to_key("WINUM1")),
      doom.resources.getMenu(DOOM::str_to_key("WINUM2")),
      doom.resources.getMenu(DOOM::str_to_key("WINUM3")),
      doom.resources.getMenu(DOOM::str_to_key("WINUM4")),
      doom.resources.getMenu(DOOM::str_to_key("WINUM5")),
      doom.resources.getMenu(DOOM::str_to_key("WINUM6")),
      doom.resources.getMenu(DOOM::str_to_key("WINUM7")),
      doom.resources.getMenu(DOOM::str_to_key("WINUM8")),
      doom.resources.getMenu(DOOM::str_to_key("WINUM9"))
    },
  _textFinished(doom.resources.getMenu(DOOM::str_to_key("WIF"))),
  _textEntering(doom.resources.getMenu(DOOM::str_to_key("WIENTER"))),
  _textKills(doom.resources.getMenu(DOOM::str_to_key("WIOSTK"))),
  _textItems(doom.resources.getMenu(DOOM::str_to_key("WIOSTI"))),
  _textSecret(doom.resources.getMenu(DOOM::str_to_key("WISCRT2"))),
  _textScrt(doom.resources.getMenu(DOOM::str_to_key("WIOSTS"))),
  _textTime(doom.resources.getMenu(DOOM::str_to_key("WITIME"))),
  _textSucks(doom.resources.getMenu(DOOM::str_to_key("WISUCKS"))),
  _textPar(doom.resources.getMenu(DOOM::str_to_key("WIPAR"))),
  _textMinus(doom.resources.getMenu(DOOM::str_to_key("WIMINUS"))),
  _textPercent(doom.resources.getMenu(DOOM::str_to_key("WIPCNT"))),
  _textColon(doom.resources.getMenu(DOOM::str_to_key("WICOLON")))
{
  // Initialize counter for each player
  for (const auto& player : _doom.level.players) {
    _kills[player.get().id].value = -1.f / DOOM::Doom::Tic.asSeconds() * DOOM::IntermissionDoomScene::SpeedPercent;
    _items[player.get().id].value = -1.f / DOOM::Doom::Tic.asSeconds() * DOOM::IntermissionDoomScene::SpeedPercent;
    _secrets[player.get().id].value = -1.f / DOOM::Doom::Tic.asSeconds() * DOOM::IntermissionDoomScene::SpeedPercent;
    _kills[player.get().id].max = (_statistics.total.kills == 0) ? 0.f : std::round(_statistics.players[player.get().id].kills * 100.f / _statistics.total.kills);
    _items[player.get().id].max = (_statistics.total.items == 0) ? 0.f : std::round(_statistics.players[player.get().id].items * 100.f / _statistics.total.items);
    _secrets[player.get().id].max = (_statistics.total.secrets == 0) ? 0.f : std::round(_statistics.players[player.get().id].secrets * 100.f / _statistics.total.secrets);
  }

  // Time counter
  _times[0].value = -1.f / DOOM::Doom::Tic.asSeconds() * DOOM::IntermissionDoomScene::SpeedTime;
  _times[0].max = std::round(_statistics.time.asSeconds());

  // Par counter
  _times[1].value = -1.f / DOOM::Doom::Tic.asSeconds() * DOOM::IntermissionDoomScene::SpeedTime;
  _times[1].max = getPar(_previous);
  if (std::isnan(_times[1].max) == true)
    _times[1].max = _times[1].value;
}

bool  DOOM::IntermissionDoomScene::update(sf::Time elapsed)
{
  // Add time to counter
  _elapsed += elapsed;

  switch (_state) {
    // Update statistics
  case DOOM::IntermissionDoomScene::State::StateStats:
    updateStatistics(elapsed);
    break;

    // Update next level
  case DOOM::IntermissionDoomScene::State::StateNext:
    updateNext(elapsed);
    break;

  default:
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
  }

  return false;
}

void  DOOM::IntermissionDoomScene::updateStatisticsCountersComplete(std::map<int, DOOM::IntermissionDoomScene::Counter>& counters)
{
  // Complete every counter
  for (auto& counter : counters)
    counter.second.value = counter.second.max;
}

bool  DOOM::IntermissionDoomScene::updateStatisticsCountersCheck(const std::map<int, DOOM::IntermissionDoomScene::Counter>& counters)
{
  // Check for counters completion
  for (const auto& counter : counters)
    if (counter.second.value < counter.second.max)
      return false;

  // Success!
  return true;
}

void  DOOM::IntermissionDoomScene::updateStatisticsCounters(sf::Time& elapsed, std::map<int, DOOM::IntermissionDoomScene::Counter>& counters, int speed)
{
  // Does nothing if no time or already completed
  if (elapsed == sf::Time::Zero || updateStatisticsCountersCheck(counters) == true)
    return;

  sf::Time  copy = elapsed;
  int       start = std::numeric_limits<int>::max();
  int       end = std::numeric_limits<int>::min();

  for (auto& counter : counters) {
    // Already completed
    if (counter.second.value == counter.second.max)
      continue;

    // Counter for sound effet
    start = std::min(start, (int)std::round(counter.second.value / speed / DOOM::IntermissionDoomScene::SpeedPistol));

    // Increase counter
    counter.second.value += copy.asSeconds() / DOOM::Doom::Tic.asSeconds() * speed;

    // Finished
    if (counter.second.value >= counter.second.max) {
      elapsed = std::min(elapsed, sf::seconds((counter.second.value - counter.second.max) / speed * DOOM::Doom::Tic.asSeconds()));
      counter.second.value = counter.second.max;
    }

    // Not completed, consume time
    else
      elapsed = sf::Time::Zero;

    // Counter for sound effet
    if (counter.second.value >= 0.f)
      end = std::max(end, (int)std::round(counter.second.value / speed / DOOM::IntermissionDoomScene::SpeedPistol));
  }

  // TODO: will not explode if elapsed is exactly at zero
  // Counter completed, explode!
  if (updateStatisticsCountersCheck(counters) == true)
    _doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_barexp);

  // Shoot pistol
  else if (start < end && end >= 0)
    _doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_pistol);
}

void  DOOM::IntermissionDoomScene::updateStatistics(sf::Time elapsed)
{
  // Skip statistics
  if (updateSkip() == true) {
    // Skip kills count
    if (updateStatisticsCountersCheck(_kills) == false)
      updateStatisticsCountersComplete(_kills);
    // Skip items count
    else if (updateStatisticsCountersCheck(_items) == false)
      updateStatisticsCountersComplete(_items);
    // Skip secrets count
    else if (updateStatisticsCountersCheck(_secrets) == false)
      updateStatisticsCountersComplete(_secrets);
    // Skip time count
    else if (updateStatisticsCountersCheck(_times) == false)
      updateStatisticsCountersComplete(_times);
    else {
      // Go to next level screen
      if (_next != std::pair<uint8_t, uint8_t>(0, 0)) {
        _state = DOOM::IntermissionDoomScene::State::StateNext;
        _doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_sgcock);
      }
      // No next level, skip to next screen
      else {
        updateEnd();
      }
      return;
    }

    // A stat has been skipped
    _doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_barexp);
    elapsed = sf::Time::Zero;
    return;
  }

  // Percentages
  updateStatisticsCounters(elapsed, _kills, DOOM::IntermissionDoomScene::SpeedPercent);
  updateStatisticsCounters(elapsed, _items, DOOM::IntermissionDoomScene::SpeedPercent);
  updateStatisticsCounters(elapsed, _secrets, DOOM::IntermissionDoomScene::SpeedPercent);

  // Times
  updateStatisticsCounters(elapsed, _times, DOOM::IntermissionDoomScene::SpeedTime);
}

void  DOOM::IntermissionDoomScene::updateNext(sf::Time elapsed)
{
  // Update Next state timer
  _nextElapsed += elapsed;

  // Ignore if not skipped
  if (updateSkip() == true)
    updateEnd();
}

bool  DOOM::IntermissionDoomScene::updateSkip()
{
  bool  skip = false;

  // Check skip key
  for (const auto& player : _doom.level.players) {
    // Keyboard/mouse
    if (player.get().controller == 0) {
      skip |= Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::Space);
      skip |= Game::Window::Instance().mouse().buttonPressed(sf::Mouse::Left);
    }

    else {
      skip |= Game::Window::Instance().joystick().buttonPressed(player.get().controller - 1, 0);
      skip |= Game::Window::Instance().joystick().buttonPressed(player.get().controller - 1, 1);
    }
  }

  return skip;
}

void  DOOM::IntermissionDoomScene::updateEnd()
{
  // Copy current screen
  sf::Image start = _doom.image;

  // Save references as 'this' is gonna be deleted
  auto& machine = _machine;
  auto& doom = _doom;

  // Pop to next screen
  machine.pop();

  // Simulate first frame of next screen
  machine.draw();

  // Push transition
  machine.push<DOOM::TransitionDoomScene>(doom, start, doom.image);
}

void  DOOM::IntermissionDoomScene::draw()
{
  // Resize rendering target
  if (_doom.image.getSize() != sf::Vector2u(DOOM::Doom::RenderWidth, DOOM::Doom::RenderHeight))
    _doom.image.create(DOOM::Doom::RenderWidth, DOOM::Doom::RenderHeight);

  // Clear rendering target
  std::memset((void*)_doom.image.getPixelsPtr(), 0, _doom.image.getSize().x * _doom.image.getSize().y * sizeof(sf::Color));

  // Draw background
  drawBackground();

  // Draw screen infos
  switch (_state)
  {
  case DOOM::IntermissionDoomScene::State::StateStats:
    drawStatistics();
    break;
  case DOOM::IntermissionDoomScene::State::StateNext:
    drawNext();
    break;
  default:
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
  }
}

void  DOOM::IntermissionDoomScene::drawBackground()
{
  // Draw background screen
  _background.draw(_doom, _doom.image, { 0, 0 }, { 1, 1 });

  // Draw animations only for DOOM I
  if (_doom.mode == DOOM::Enum::Mode::ModeShareware ||
    _doom.mode == DOOM::Enum::Mode::ModeRegistered ||
    _doom.mode == DOOM::Enum::Mode::ModeRetail) {

    // Cancel if no animation for current level
    if (_previous.first < 1 || _previous.first > _animations.size())
      return;

    // Draw every animations
    for (const auto& animation : _animations[_previous.first - 1]) {
      if (animation.level == 0)
        _doom.resources.getMenu(animation.frames[(_elapsed.asMicroseconds() / (animation.duration * DOOM::Doom::Tic.asMicroseconds()) + animation.offset) % animation.frames.size()]).draw(_doom, _doom.image, animation.position, sf::Vector2i(1, 1));
      else if (_next.second == animation.level)
        _doom.resources.getMenu(animation.frames[std::min((_nextElapsed.asMicroseconds() / (animation.duration * DOOM::Doom::Tic.asMicroseconds()) + animation.offset), (long long)animation.frames.size() - 1)]).draw(_doom, _doom.image, animation.position, sf::Vector2i(1, 1));
    }
  }
}

void  DOOM::IntermissionDoomScene::drawStatistics()
{
  const auto& title = _doom.resources.getMenu(DOOM::str_to_key(
    (_doom.mode == DOOM::Enum::Mode::ModeCommercial)
    ? (std::string("CWILV") + std::to_string((_previous.second - 1) / 10 % 10) + std::to_string((_previous.second - 1) % 10))
    : (std::string("WILV") + std::to_string(_previous.first - 1) + std::to_string(_previous.second - 1))
  ));
  
  // Draw title
  title.draw(_doom, _doom.image, sf::Vector2i((DOOM::Doom::RenderWidth - title.width) / 2, DOOM::IntermissionDoomScene::TitleY), { 1, 1 });
  _textFinished.draw(_doom, _doom.image, sf::Vector2i((DOOM::Doom::RenderWidth - _textFinished.width) / 2, DOOM::IntermissionDoomScene::TitleY + title.height * 5 / 4), { 1, 1 });

  // Draw statistics
  if (_doom.level.players.size() == 1)
    drawStatisticsSolo();
  else if (_doom.level.players.size() > 1)
    drawStatisticsCoop();
  else
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
}

void  DOOM::IntermissionDoomScene::drawStatisticsSolo()
{
  // Draw solo stats titles
  _textKills.draw(_doom, _doom.image, sf::Vector2i(DOOM::IntermissionDoomScene::StatsSoloX, DOOM::IntermissionDoomScene::StatsSoloY + 0 * _textNumbers[0].get().height * 3 / 2), { 1, 1 });
  _textItems.draw(_doom, _doom.image, sf::Vector2i(DOOM::IntermissionDoomScene::StatsSoloX, DOOM::IntermissionDoomScene::StatsSoloY + 1 * _textNumbers[0].get().height * 3 / 2), { 1, 1 });
  _textSecret.draw(_doom, _doom.image, sf::Vector2i(DOOM::IntermissionDoomScene::StatsSoloX, DOOM::IntermissionDoomScene::StatsSoloY + 2 * _textNumbers[0].get().height * 3 / 2), { 1, 1 });
  _textTime.draw(_doom, _doom.image, sf::Vector2i(DOOM::IntermissionDoomScene::TimeX, DOOM::IntermissionDoomScene::TimeY), { 1, 1 });
  if (std::isnan(getPar(_previous)) == false)
    _textPar.draw(_doom, _doom.image, sf::Vector2i(DOOM::Doom::RenderWidth / 2 + DOOM::IntermissionDoomScene::TimeX, DOOM::IntermissionDoomScene::TimeY), { 1, 1 });

  // Draw statistics
  if (_kills.begin()->second.value >= 0.f)
    drawPercent(sf::Vector2i(DOOM::Doom::RenderWidth - DOOM::IntermissionDoomScene::StatsSoloX, DOOM::IntermissionDoomScene::StatsSoloY + 0 * _textNumbers[0].get().height * 3 / 2), (int)_kills.begin()->second.value);
  if (_items.begin()->second.value >= 0.f)
    drawPercent(sf::Vector2i(DOOM::Doom::RenderWidth - DOOM::IntermissionDoomScene::StatsSoloX, DOOM::IntermissionDoomScene::StatsSoloY + 1 * _textNumbers[0].get().height * 3 / 2), (int)_items.begin()->second.value);
  if (_secrets.begin()->second.value >= 0.f)
    drawPercent(sf::Vector2i(DOOM::Doom::RenderWidth - DOOM::IntermissionDoomScene::StatsSoloX, DOOM::IntermissionDoomScene::StatsSoloY + 2 * _textNumbers[0].get().height * 3 / 2), (int)_secrets.begin()->second.value);
  if (_times[0].value >= 0.f)
    drawTime(sf::Vector2i(DOOM::Doom::RenderWidth / 2 - DOOM::IntermissionDoomScene::TimeX, DOOM::IntermissionDoomScene::TimeY), (int)_times[0].value);
  if (_times[1].value >= 0.f)
    drawTime(sf::Vector2i(DOOM::Doom::RenderWidth - DOOM::IntermissionDoomScene::TimeX, DOOM::IntermissionDoomScene::TimeY), (int)_times[1].value);
}

void  DOOM::IntermissionDoomScene::drawStatisticsCoop()
{
  // Stats titles
  _textKills.draw(_doom, _doom.image, sf::Vector2i(DOOM::IntermissionDoomScene::StatsCoopX + _playerFace.width / 2 + 1 * DOOM::IntermissionDoomScene::StatsCoopSpacing - _textKills.width, DOOM::IntermissionDoomScene::StatsCoopY), sf::Vector2i(1, 1));
  _textItems.draw(_doom, _doom.image, sf::Vector2i(DOOM::IntermissionDoomScene::StatsCoopX + _playerFace.width / 2 + 2 * DOOM::IntermissionDoomScene::StatsCoopSpacing - _textItems.width, DOOM::IntermissionDoomScene::StatsCoopY), sf::Vector2i(1, 1));
  _textScrt.draw(_doom, _doom.image, sf::Vector2i(DOOM::IntermissionDoomScene::StatsCoopX + _playerFace.width / 2 + 3 * DOOM::IntermissionDoomScene::StatsCoopSpacing - _textScrt.width, DOOM::IntermissionDoomScene::StatsCoopY), sf::Vector2i(1, 1));
  
  int y = DOOM::IntermissionDoomScene::StatsCoopY + _textKills.height;

  // Draw stats of each player
  for (const auto& player : _doom.level.players) {
    _playerBackground[(player.get().id - 1) % _playerBackground.size()].get().draw(_doom, _doom.image, sf::Vector2i(DOOM::IntermissionDoomScene::StatsCoopX + _playerFace.width / 2 + 0 * DOOM::IntermissionDoomScene::StatsCoopSpacing - _playerBackground[(player.get().id - 1) % _playerBackground.size()].get().width, y), sf::Vector2i(1, 1));
    _playerFace.draw(_doom, _doom.image, sf::Vector2i(DOOM::IntermissionDoomScene::StatsCoopX + _playerFace.width / 2 + 0 * DOOM::IntermissionDoomScene::StatsCoopSpacing - _playerBackground[(player.get().id - 1) % _playerBackground.size()].get().width, y), sf::Vector2i(1, 1));

    if (_kills.at(player.get().id).value >= 0.f)
      drawPercent(sf::Vector2i(DOOM::IntermissionDoomScene::StatsCoopX + _playerFace.width / 2 - _textPercent.width + 1 * DOOM::IntermissionDoomScene::StatsCoopSpacing, y + 10), (int)_kills.at(player.get().id).value);
    if (_items.at(player.get().id).value >= 0.f)
      drawPercent(sf::Vector2i(DOOM::IntermissionDoomScene::StatsCoopX + _playerFace.width / 2 - _textPercent.width + 2 * DOOM::IntermissionDoomScene::StatsCoopSpacing, y + 10), (int)_items.at(player.get().id).value);
    if (_secrets.at(player.get().id).value >= 0.f)
      drawPercent(sf::Vector2i(DOOM::IntermissionDoomScene::StatsCoopX + _playerFace.width / 2 - _textPercent.width + 3 * DOOM::IntermissionDoomScene::StatsCoopSpacing, y + 10), (int)_secrets.at(player.get().id).value);

    y += DOOM::IntermissionDoomScene::StatsSoloSpacing;
  }

  // Draw time only if available space
  if (_doom.level.players.size() <= 3) {
    // Titles
    _textTime.draw(_doom, _doom.image, sf::Vector2i(DOOM::IntermissionDoomScene::TimeX, DOOM::IntermissionDoomScene::TimeY), { 1, 1 });
    if (std::isnan(getPar(_previous)) == false)
      _textPar.draw(_doom, _doom.image, sf::Vector2i(DOOM::Doom::RenderWidth / 2 + DOOM::IntermissionDoomScene::TimeX, DOOM::IntermissionDoomScene::TimeY), { 1, 1 });

    // Stats
    if (_times[0].value >= 0.f)
      drawTime(sf::Vector2i(DOOM::Doom::RenderWidth / 2 - DOOM::IntermissionDoomScene::TimeX, DOOM::IntermissionDoomScene::TimeY), (int)_times[0].value);
    if (_times[1].value >= 0.f)
      drawTime(sf::Vector2i(DOOM::Doom::RenderWidth - DOOM::IntermissionDoomScene::TimeX, DOOM::IntermissionDoomScene::TimeY), (int)_times[1].value);
  }
}

void  DOOM::IntermissionDoomScene::drawNext()
{
  const auto& title = _doom.resources.getMenu(DOOM::str_to_key(
    (_doom.mode == DOOM::Enum::Mode::ModeCommercial)
    ? (std::string("CWILV") + std::to_string((_next.second - 1) / 10 % 10) + std::to_string((_next.second - 1) % 10))
    : (std::string("WILV") + std::to_string(_next.first - 1) + std::to_string(_next.second - 1))
  ));

  // Draw title
  _textEntering.draw(_doom, _doom.image, sf::Vector2i((DOOM::Doom::RenderWidth - _textEntering.width) / 2, DOOM::IntermissionDoomScene::TitleY), { 1, 1 });
  title.draw(_doom, _doom.image, sf::Vector2i((DOOM::Doom::RenderWidth - title.width) / 2, DOOM::IntermissionDoomScene::TitleY + _textEntering.height * 5 / 4), { 1, 1 });

  // Draw maps details only in DOOM (except eps. 4)
  if ((_doom.mode == DOOM::Enum::Mode::ModeShareware ||
    _doom.mode == DOOM::Enum::Mode::ModeRegistered ||
    _doom.mode == DOOM::Enum::Mode::ModeRetail) &&
    _next.first >= 1 && _next.first <= _positions.size()) {
    const DOOM::Doom::Resources::Texture& splat = _doom.resources.getMenu(DOOM::str_to_key("WISPLAT"));

    // Draw blood splats
    for (int index = 0; index < _positions[_next.first - 1].size(); index++) {
      if ((_next.second == 9 && _previous.second > index) ||
        (_previous.second == 9 && (_next.second - 1 > index || 9 - 1 == index)) ||
        (_next.second != 9 && _previous.second != 9 && _previous.second > index)) {
        splat.draw(_doom, _doom.image, _positions[_next.first - 1][index], { 1, 1 });
      }
    }

    // Draw '<- You are here'
    if ((int)(_nextElapsed.asSeconds() / DOOM::Doom::Tic.asSeconds()) % 32 < 20 &&
      _next.second >= 1 && _next.second <= _positions[_next.first - 1].size()) {
      const auto& here0 = _doom.resources.getMenu(DOOM::str_to_key("WIURH0"));
      const auto& here1 = _doom.resources.getMenu(DOOM::str_to_key("WIURH1"));

      // Choose left of right arrow
      if (_positions[_next.first - 1][_next.second - 1].x - here0.left >= 0 &&
        _positions[_next.first - 1][_next.second - 1].x - here0.left + here0.width < (int)_doom.image.getSize().x &&
        _positions[_next.first - 1][_next.second - 1].y - here0.top >= 0 &&
        _positions[_next.first - 1][_next.second - 1].y - here0.top + here0.height < (int)_doom.image.getSize().y)
        here0.draw(_doom, _doom.image, _positions[_next.first - 1][_next.second - 1], { 1, 1 });
      else
        here1.draw(_doom, _doom.image, _positions[_next.first - 1][_next.second - 1], { 1, 1 });
    }
  }
}

void  DOOM::IntermissionDoomScene::drawPercent(sf::Vector2i position, int value)
{
  // Draw percent symbol
  _textPercent.draw(_doom, _doom.image, position, { 1, 1 });

  // Draw percentage value
  do {
    position.x -= _textNumbers[value % 10].get().width;
    _textNumbers[value % 10].get().draw(_doom, _doom.image, position, { 1, 1 });

    value /= 10;
  } while (value != 0);
}

void  DOOM::IntermissionDoomScene::drawTime(sf::Vector2i position, int value)
{
  // Sucks if over an hour
  if (value >= 60 * 60) {
    _textSucks.draw(_doom, _doom.image, { position.x - _textSucks.width, position.y }, { 1, 1 });
  }

  // Draw time
  else {
    // Seconds unit
    position.x -= _textNumbers[value % 10].get().width;
    _textNumbers[value % 10].get().draw(_doom, _doom.image, position, { 1, 1 });
    value /= 10;

    // Seconds decimal
    position.x -= _textNumbers[value % 6].get().width;
    _textNumbers[value % 6].get().draw(_doom, _doom.image, position, { 1, 1 });
    value /= 6;

    // Colon
    position.x -= _textColon.width;
    _textColon.draw(_doom, _doom.image, position, { 1, 1 });

    // Draw minutes only if superior to zero
    if (value > 0) {
      // Minutes unit
      position.x -= _textNumbers[value % 10].get().width;
      _textNumbers[value % 10].get().draw(_doom, _doom.image, position, { 1, 1 });
      value /= 10;

      // Minutes decimal
      position.x -= _textNumbers[value % 6].get().width;
      _textNumbers[value % 6].get().draw(_doom, _doom.image, position, { 1, 1 });
      value /= 6;
    }
  }
}

float DOOM::IntermissionDoomScene::getPar(std::pair<uint8_t, uint8_t> level) const
{
  // DOOM Par Times
  std::array<std::array<float, 9>, 3> doom = {
      std::array<float, 9>{ 30.f, 75.f, 120.f, 90.f, 165.f, 180.f, 180.f, 30.f, 165.f },
      std::array<float, 9>{ 90.f, 90.f, 90.f, 120.f, 90.f, 360.f, 240.f, 30.f, 170.f },
      std::array<float, 9>{ 90.f, 45.f, 90.f, 150.f, 90.f, 90.f, 165.f, 30.f, 135.f }
  };

  // DOOM II Par Times
  std::array<float, 32> doom2 = {
      30.f, 90.f, 120.f, 120.f, 90.f, 150.f, 120.f, 120.f, 270.f, 90.f,
      210.f, 150.f, 150.f, 150.f, 210.f, 150.f, 420.f, 150.f, 210.f, 150.f,
      240.f, 150.f, 180.f, 150.f, 150.f, 300.f, 330.f, 420.f, 300.f, 180.f,
      120.f, 30.f
  };

  switch (_doom.mode) {
    // DOOM shareware, DOOM I, Ultimate DOOM
  case DOOM::Enum::Mode::ModeShareware:
  case DOOM::Enum::Mode::ModeRegistered:
  case DOOM::Enum::Mode::ModeRetail:
    if (level.first - 1 < doom.size() && level.second - 1 < doom[level.first - 1].size())
      return doom[level.first - 1][level.second - 1];
    else
      return std::nanf("");

    // DOOM II
  case DOOM::Enum::Mode::ModeCommercial:
    if (level.first == 1 && level.second - 1 < doom2.size())
      return doom2[level.second];
    else
      return std::nanf("");

    // Error
  default:
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
  }
}
