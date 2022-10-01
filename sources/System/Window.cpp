#include <SFML/OpenGL.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/Texture.hpp>

#include "System/Config.hpp"
#include "System/Window.hpp"

std::string const   Game::Window::DefaultTitle = "Classical Games";
unsigned int const  Game::Window::DefaultWidth = 640;
unsigned int const  Game::Window::DefaultHeight = 480;
unsigned int const  Game::Window::DefaultAntialiasing = 4;
sf::Time const      Game::Window::FpsRefresh = sf::seconds(1.f);
bool const          Game::Window::DefaultVerticalSync = true;
float const         Game::Window::Joystick::DeadZone = 20.f;

Game::Window::Window() :
  _window(), _mouse(), _keyboard(), _joystick(), _elapsed(), _tick(), _sync(Game::Window::DefaultVerticalSync)
{
  // Create window with default parameters
  create(sf::VideoMode(Game::Window::DefaultWidth, Game::Window::DefaultHeight), sf::Style::Titlebar | sf::Style::Resize | sf::Style::Close, sf::ContextSettings(24, 8, Game::Window::DefaultAntialiasing, 4, 4));

#ifdef _WIN32
  // Get system handle of window
  ::CoInitialize(nullptr);
  ::CoCreateInstance(::CLSID_TaskbarList, nullptr, ::tagCLSCTX::CLSCTX_INPROC_SERVER, ::IID_ITaskbarList3, (void **)&_taskbar);
  if (_taskbar == nullptr)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());  
#endif
}

bool  Game::Window::update(sf::Time elapsed)
{
  // Check window focus
  bool  focus = _window.hasFocus() == true;

  // Clear inputs pressed/released maps
  _mouse._pressed.fill(false);
  _mouse._released.fill(false);
  _keyboard._pressed.fill(false);
  _keyboard._released.fill(false);
  _joystick._pressed.fill(std::array<bool, sf::Joystick::ButtonCount>());
  _joystick._released.fill(std::array<bool, sf::Joystick::ButtonCount>());

  // Cancel down keys when no focus
  if (focus == false) {
    _mouse._down.fill(false);
    _keyboard._down.fill(false);
    _joystick._down.fill(std::array<bool, sf::Joystick::ButtonCount>());
  }

  // Reset keyboard input text
  _keyboard._text.clear();
  
  // Reset mouse wheel ticks
  _mouse._wheel = 0;
  
  // Process pending events
  sf::Event event;
  while (_window.pollEvent(event) == true)
  {
    // Stop if window closed
    if (event.type == sf::Event::Closed)
      return true;

    // Update the view to the new size of the window
    if (event.type == sf::Event::Resized)
      _window.setView(sf::View(sf::FloatRect(0, 0, (float)event.size.width, (float)event.size.height)));

    // Get input only if window focused
    if (focus == true)
    {
      switch (event.type)
      {
        // Get mouse events
      case sf::Event::MouseButtonPressed:
        _mouse._down[event.mouseButton.button] = true;
        _mouse._pressed[event.mouseButton.button] = true;
        break;
      case sf::Event::MouseButtonReleased:
        _mouse._down[event.mouseButton.button] = false;
        _mouse._released[event.mouseButton.button] = true;
        break;
      case sf::Event::MouseWheelMoved:
        _mouse._wheel += event.mouseWheel.delta;
        break;

        // Get keyboard events
      case sf::Event::KeyPressed:
        if (event.key.code >= 0 && event.key.code < sf::Keyboard::KeyCount) {
          _keyboard._down[event.key.code] = true;
          _keyboard._pressed[event.key.code] = true;
        }
        break;
      case sf::Event::KeyReleased:
        if (event.key.code >= 0 && event.key.code < sf::Keyboard::KeyCount) {
          _keyboard._down[event.key.code] = false;
          _keyboard._released[event.key.code] = true;
        }
        break;
      case sf::Event::TextEntered:
        _keyboard._text.push_back((wchar_t)event.text.unicode);
        break;

        // Get joystick events
      case sf::Event::JoystickButtonPressed:
        _joystick._down[event.joystickButton.joystickId][event.joystickButton.button] = true;
        _joystick._pressed[event.joystickButton.joystickId][event.joystickButton.button] = true;
        break;
      case sf::Event::JoystickButtonReleased:
        _joystick._down[event.joystickButton.joystickId][event.joystickButton.button] = false;
        _joystick._released[event.joystickButton.joystickId][event.joystickButton.button] = true;
        break;

      default:
        break;
      }
      
    }
  }

  // Update mouse position
  sf::Vector2i  position = sf::Mouse::getPosition(_window);
  _mouse._relative = position - _mouse._position;
  _mouse._position = position;
  
  // Update joysticks axis positions
  for (unsigned int joystick = 0; joystick < sf::Joystick::Count; joystick++)
    for (unsigned int axis = 0; axis < sf::Joystick::AxisCount; axis++) {
      float value = sf::Joystick::getAxisPosition(joystick, (sf::Joystick::Axis)axis);

      _joystick._relative[joystick][axis] = value - _joystick._position[joystick][axis];
      _joystick._position[joystick][axis] = value;
    }

  // Update FPS counters
  _elapsed += elapsed;
  _tick += 1;

  // Update FPS display if time limit reached
  if (_elapsed >= Game::Window::FpsRefresh)
  {
    // Display new FPS in window title
    // NOTE: in unknown cases, we can get stuck in setTitle
    _window.setTitle(Game::Window::DefaultTitle + " (" + std::to_string((int)(_tick / _elapsed.asSeconds())) + "." + std::to_string((int)(10 * _tick / _elapsed.asSeconds()) % 10) + " FPS)");

    // Reset FPS counters
    _elapsed = sf::Time::Zero;
    _tick = 0;
  }

  return false;
}

void  Game::Window::create(const sf::VideoMode& video, sf::Uint32 style, const sf::ContextSettings& context)
{
#ifdef _WIN32
  ::COLORREF  pcrKey = RGB(0, 0, 0);
  ::BYTE      pbAlpha = 255;
  ::DWORD     pdwFlags = 0;

  // Save actual window transparency configuration
  ::GetLayeredWindowAttributes(_window.getSystemHandle(), &pcrKey, &pbAlpha, &pdwFlags);
#endif

  // Create window with parameters
  _window.create(video, Game::Window::DefaultTitle, style, context);

  // Restore V-sync (limit fps)
  setVerticalSync(getVerticalSync());
  
  // Disabled key repeate
  _window.setKeyRepeatEnabled(false);
  
  // Load window icon
  sf::Image icon;
  if (icon.loadFromFile(Game::Config::ExecutablePath + "assets/icons/icon128.png") == false)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Set window icon
  _window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

#ifdef _WIN32
  // Initialize window transparency
  ::SetWindowLongPtr(_window.getSystemHandle(), GWL_EXSTYLE, ::GetWindowLongPtr(_window.getSystemHandle(), GWL_EXSTYLE) | WS_EX_LAYERED);

  if (::SetLayeredWindowAttributes(_window.getSystemHandle(), pcrKey, pbAlpha, pdwFlags) == FALSE)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
  ::RedrawWindow(_window.getSystemHandle(), NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_FRAME | RDW_ALLCHILDREN);
#endif
}

void  Game::Window::taskbar(Game::Window::WindowFlag flag)
{
#ifdef _WIN32
  _taskbar->SetProgressState(_window.getSystemHandle(), (::TBPFLAG)flag);
#endif
}

void  Game::Window::taskbar(Game::Window::WindowFlag flag, float progress)
{
  // Apply flag
  taskbar(flag);

  // Check for progress value
  progress = std::clamp(progress, 0.f, 1.f);

#ifdef _WIN32
  _taskbar->SetProgressValue(_window.getSystemHandle(), (::ULONGLONG)(progress * 1000), 1000);
#endif
}

void  Game::Window::transparency(sf::Uint8 transparency)
{
#ifdef _WIN32
  if (::SetLayeredWindowAttributes(_window.getSystemHandle(), RGB(0, 0, 0), 255 - transparency, LWA_ALPHA) == FALSE)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
  ::RedrawWindow(_window.getSystemHandle(), NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_FRAME | RDW_ALLCHILDREN);
#endif
}

bool  Game::Window::getVerticalSync() const
{
  return _sync;
}

void  Game::Window::setVerticalSync(bool sync)
{
  // Save new state
  _sync = sync;

  // Set vertical sync
  window().setVerticalSyncEnabled(_sync);
}

void  Game::Window::draw(sf::Sprite& sprite, float ratio)
{
  auto spriteSize = sprite.getTexture()->getSize();
  auto windowSize = _window.getSize();

  // Compute sprite scale and position
  float scale = std::min((float)windowSize.x / (float)spriteSize.x, (float)windowSize.y / ((float)spriteSize.y * ratio));
  float pos_x = (((float)windowSize.x - ((float)spriteSize.x * scale)) / 2.f);
  float pos_y = (((float)windowSize.y - ((float)spriteSize.y * scale * ratio)) / 2.f);

  // Position sprite in window
  sprite.setScale(scale, scale * ratio);
  sprite.setPosition(pos_x, pos_y);

  // Draw DOOM rendering target
  _window.draw(sprite);
}