#include <SFML/OpenGL.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Image.hpp>

#include "System/Config.hpp"
#include "System/Window.hpp"

const Math::Vector<2, unsigned int> Game::Window::DefaultSize = { 960u, 540u };
const std::string                   Game::Window::DefaultTitle = "Classical Games";
const unsigned int                  Game::Window::DefaultAntialiasing = 4;
const float                         Game::Window::FpsRefresh = 1.f;
const bool                          Game::Window::DefaultVerticalSync = true;
const float                         Game::Window::Joystick::DeadZone = 20.f;

Game::Window& Game::Window::Instance()
{
  static Game::Window window;

  // Return window instance
  return window;
}

Game::Window::Window() :
  _window(), _mouse(), _keyboard(), _joystick(),
  _elapsed(0.f),
  _tick(0),
  _size(Game::Window::DefaultSize),
  _title(Game::Window::DefaultTitle),
  _style(sf::Style::Default),
  _antialiasing(Game::Window::DefaultAntialiasing),
  _vsync(Game::Window::DefaultVerticalSync),
  _fullscreen(false)
{
  // Create window with default parameters
  recreateWindow();
}

bool  Game::Window::update(float elapsed)
{
  // Clear inputs pressed/released maps
  _mouse._pressed.fill(false);
  _mouse._released.fill(false);
  _keyboard._pressed.fill(false);
  _keyboard._released.fill(false);
  _joystick._pressed.fill(std::array<bool, Game::Window::JoystickButtonCount>());
  _joystick._released.fill(std::array<bool, Game::Window::JoystickButtonCount>());

  // Cancel down keys when no focus
  // NOTE: some joysticks are registered when no focus
  if (_window.hasFocus() == false) {
    _mouse._down.fill(false);
    _keyboard._down.fill(false);
  }

  // Reset keyboard input text
  _keyboard._text.clear();
  
  // Reset mouse wheel ticks
  _mouse._wheel = 0.f;
  
  // Handle events
  {
    const auto eventClosed = [this](const sf::Event::Closed&) {
      _window.close();
      };
    const auto eventResized = [this](const sf::Event::Resized& resized) {
      _window.setView(sf::View(sf::FloatRect({ 0.f, 0.f }, { (float)resized.size.x, (float)resized.size.y })));
      _size = { resized.size.x, resized.size.y };
      };
    const auto eventKeyPressed = [this](const sf::Event::KeyPressed& key) {
      if (key.code != Key::Unknown) {
        _keyboard._down[(std::size_t)key.code] = true;
        _keyboard._pressed[(std::size_t)key.code] = true;
      }
      };
    const auto eventKeyReleased = [this](const sf::Event::KeyReleased& key) {
      if (key.code != Key::Unknown) {
        _keyboard._down[(std::size_t)key.code] = false;
        _keyboard._released[(std::size_t)key.code] = true;
      }
      };
    const auto eventTextEntered = [this](const sf::Event::TextEntered& text) {
      _keyboard._text.push_back((wchar_t)text.unicode);
      };
    const auto eventMouseButtonPressed = [this](const sf::Event::MouseButtonPressed& mouse) {
      _mouse._down[(std::size_t)mouse.button] = true;
      _mouse._pressed[(std::size_t)mouse.button] = true;
      };
    const auto eventMouseButtonReleased = [this](const sf::Event::MouseButtonReleased& mouse) {
      _mouse._down[(std::size_t)mouse.button] = false;
      _mouse._released[(std::size_t)mouse.button] = true;
      };
    const auto eventMouseWheelMoved = [this](const sf::Event::MouseWheelScrolled& wheel) {
      _mouse._wheel += wheel.delta;
      };
    const auto eventJoystickButtonPressed = [this](const sf::Event::JoystickButtonPressed& joystick) {
      _joystick._down[joystick.joystickId][joystick.button] = true;
      _joystick._pressed[joystick.joystickId][joystick.button] = true;
      };
    const auto eventJoystickButtonReleased = [this](const sf::Event::JoystickButtonReleased& joystick) {
      _joystick._down[joystick.joystickId][joystick.button] = false;
      _joystick._released[joystick.joystickId][joystick.button] = true;
      };

    // Poll events
    _window.handleEvents(
      eventClosed,
      eventResized,
      eventKeyPressed,
      eventKeyReleased,
      eventTextEntered,
      eventMouseButtonPressed,
      eventMouseButtonReleased,
      eventMouseWheelMoved,
      eventJoystickButtonPressed,
      eventJoystickButtonReleased
      );
  }

  // Update mouse position
  auto  position = sf::Mouse::getPosition(_window);
  _mouse._relative = Math::Vector<2, int>(position.x, position.y) - _mouse._position;
  _mouse._position = Math::Vector<2, int>(position.x, position.y);
  
  // Update joysticks axis positions
  for (unsigned int joystick = 0; joystick < Game::Window::JoystickCount; joystick++)
    for (unsigned int axis = 0; axis < Game::Window::JoystickAxisCount; axis++) {
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
    _window.setTitle(Game::Window::DefaultTitle + " (" + std::to_string((int)(_tick / _elapsed)) + "." + std::to_string((int)(10 * _tick / _elapsed) % 10) + " FPS, " + std::to_string(_window.getSize().x) + "x" + std::to_string(_window.getSize().y) + ")");

    // Reset FPS counters
    _elapsed = 0.f;
    _tick = 0;
  }

  return false;
}

void  Game::Window::recreateWindow()
{
#ifdef _WIN32
  ::COLORREF  pcrKey = RGB(0, 0, 0);
  ::BYTE      pbAlpha = 255;
  ::DWORD     pdwFlags = 0;

  // Save actual window transparency configuration
  ::GetLayeredWindowAttributes(_window.getNativeHandle(), &pcrKey, &pbAlpha, &pdwFlags);
#endif

  // Create window with parameters
  _window.create(
    _fullscreen == true ? sf::VideoMode::getDesktopMode() : sf::VideoMode({_size.x(), _size.y()}),
    _title,
    _style,
    _fullscreen == true ? sf::State::Fullscreen : sf::State::Windowed, 
    sf::ContextSettings{
      .depthBits = 24,
      .stencilBits = 8,
      .antiAliasingLevel = (unsigned int)std::pow(2, _antialiasing),
      .majorVersion = 4,
      .minorVersion = 4,
      .attributeFlags = sf::ContextSettings::Attribute::Default,
      .sRgbCapable = false
    });

  // Restore V-sync (limit fps)
  setVerticalSync(getVerticalSync());
  
  // Disabled key repeate
  _window.setKeyRepeatEnabled(false);

#ifdef _WIN32
  // Initialize window transparency
  ::SetWindowLongPtr(_window.getNativeHandle(), GWL_EXSTYLE, ::GetWindowLongPtr(_window.getNativeHandle(), GWL_EXSTYLE) | WS_EX_LAYERED);

  if (::SetLayeredWindowAttributes(_window.getNativeHandle(), pcrKey, pbAlpha, pdwFlags) == FALSE)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
  ::RedrawWindow(_window.getNativeHandle(), NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_FRAME | RDW_ALLCHILDREN);
#endif

  sf::Image icon;

  // Load window icon
  if (icon.loadFromFile((Game::Config::ExecutablePath / "assets" / "icons" / "icon128.png").string()) == false)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Set window icon
  _window.setIcon(icon);

#ifdef _WIN32
  // Get system handle of window
  if (::CoInitialize(nullptr) != S_OK ||
    ::CoCreateInstance(::CLSID_TaskbarList, nullptr, ::tagCLSCTX::CLSCTX_INPROC_SERVER, ::IID_ITaskbarList3, (void**)&_taskbar) != S_OK ||
    _taskbar == nullptr)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
#endif
}

void  Game::Window::taskbar(Game::Window::WindowFlag flag)
{
#ifdef _WIN32
  _taskbar->SetProgressState(_window.getNativeHandle(), (::TBPFLAG)flag);
#endif
}

void  Game::Window::taskbar(Game::Window::WindowFlag flag, float progress)
{
  // Apply flag
  taskbar(flag);

  // Check for progress value
  progress = std::clamp(progress, 0.f, 1.f);

#ifdef _WIN32
  _taskbar->SetProgressValue(_window.getNativeHandle(), (::ULONGLONG)(progress * 1000), 1000);
#endif
}

void  Game::Window::transparency(std::uint8_t transparency)
{
#ifdef _WIN32
  if (::SetLayeredWindowAttributes(_window.getNativeHandle(), RGB(0, 0, 0), 255 - transparency, LWA_ALPHA) == FALSE)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
  ::RedrawWindow(_window.getNativeHandle(), NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_FRAME | RDW_ALLCHILDREN);
#endif
}

Math::Vector<2, float>  Game::Window::pixelToCoords(Math::Vector<2, int> pixel) const
{
  return pixelToCoords(pixel, getView());
}

Math::Vector<2, float>  Game::Window::pixelToCoords(Math::Vector<2, int> pixel, const Game::Window::View& view) const
{
  auto  coords = _window.mapPixelToCoords(
    { pixel.x(), pixel.y() },
    sf::View(sf::FloatRect(
      { view.position.x(), view.position.y() },
      { view.size.x(), view.size.y() }
    ))
  );

  // Transform screen coordinates to world coordinates
  return { coords.x, coords.y };
}

Math::Vector<2, int>  Game::Window::coordsToPixels(Math::Vector<2, float> coords) const
{
  return coordsToPixels(coords, getView());
}

Math::Vector<2, int>  Game::Window::coordsToPixels(Math::Vector<2, float> coords, const Game::Window::View& view) const
{
  auto  pixel = _window.mapCoordsToPixel(
    { coords.x(), coords.y() },
    sf::View(sf::FloatRect(
    { view.position.x(), view.position.y() },
    { view.size.x(), view.size.y() }
  )));

  // Transform world coordinates to screen coordinates
  return { pixel.x, pixel.y };
}

Game::Window::View  Game::Window::getView() const
{
  auto view = _window.getView();

  // Compute current window view
  return Game::Window::View(
    { view.getCenter().x - view.getSize().x / 2.f, view.getCenter().y - view.getSize().y / 2.f },
    { view.getSize().x, view.getSize().y }
  );
}

void  Game::Window::setView(const Game::Window::View& view)
{
  // Apply new view to window
  _window.setView(sf::View(sf::FloatRect({ view.position.x(), view.position.y() }, { view.size.x(), view.size.y() })));
}

void  Game::Window::setSize(Math::Vector<2, unsigned int> size)
{
  // Apply new window resolution
  if (_size != size) {
    _size = size;

    // Change resolution only in windowed mode
    if (_fullscreen == false)
      recreateWindow();
  }
}

void  Game::Window::setTitle(const std::string& title)
{
  // Apply new title
  if (_title != title) {
    _title = title;
    _window.setTitle(_title);
  }
}

void  Game::Window::setStyle(std::uint32_t style)
{
  // Apply new style
  if (_style != style) {
    _style = style;
    recreateWindow();
  }
}

void  Game::Window::setAntialiasing(unsigned int antialiasing)
{
  // Apply new antialiasing level
  if (_antialiasing != antialiasing) {
    _antialiasing = antialiasing;
    recreateWindow();
  }
}

void  Game::Window::setVerticalSync(bool vsync)
{
  // Apply new vsync
  _vsync = vsync;
  _window.setVerticalSyncEnabled(_vsync);
}

void  Game::Window::setFullscreen(bool fullscreen)
{
  // Apply new fullscreen mode
  if (_fullscreen != fullscreen) {
    _fullscreen = fullscreen;
    recreateWindow();
  }
}

void  Game::Window::clear(std::uint32_t color)
{
  // Clear window
  _window.clear(sf::Color(color));
}

void  Game::Window::draw(const sf::Drawable& drawable, const sf::RenderStates& states)
{
  // Draw to rendering target
  _window.draw(drawable, states);
}

void  Game::Window::draw(const sf::Texture& texture, float ratio, DrawMode mode)
{
  sf::Sprite  sprite(texture);
  auto        textureSize = texture.getSize();
  auto        windowSize = _window.getSize();

  float scaleX, scaleY;
  
  // Compute sprite scale and position
  switch (mode) {
  case DrawMode::Fill:
    scaleX = std::max((float)windowSize.x / (float)textureSize.x, (float)windowSize.y / ((float)textureSize.y * ratio));
    scaleY = scaleX * ratio;
    break;
  case DrawMode::Fit:
    scaleX = std::min((float)windowSize.x / (float)textureSize.x, (float)windowSize.y / ((float)textureSize.y * ratio));
    scaleY = scaleX * ratio;
    break;
  case DrawMode::Strech:
    scaleX = (float)windowSize.x / (float)textureSize.x;
    scaleY = (float)windowSize.y / (float)textureSize.y;
    break;
  default:
    scaleX = 0.f;
    scaleY = 0.f;
    break;
  }
  
  // Position sprite in window
  sprite.setScale({ scaleX, scaleY * ratio });
  sprite.setPosition({
    ((float)windowSize.x - ((float)textureSize.x * scaleX)) / 2.f,
    ((float)windowSize.y - ((float)textureSize.y * scaleY)) / 2.f
    });

  // Draw to rendering target
  draw(sprite);
}

void  Game::Window::display()
{
  // Render drawn sprites
  _window.display();
}