#pragma once

#include <array>
#include <string>
#include <unordered_map>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Window/Joystick.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>

#ifdef _WIN32
#include <shobjidl.h>
#include <windows.h>
#endif

namespace Game
{
  class Window
  {
  public:
    static const std::string  DefaultTitle;         // Window title
    static const unsigned int DefaultWidth;         // Default window width
    static const unsigned int DefaultHeight;        // Default window height
    static const unsigned int DefaultAntialiasing;  // Default antialiasing level
    static const bool         DefaultVerticalSync;  // Vertical synchronisation activation
    static const float        FpsRefresh;           // Time between FPS refresh

  private:
    class Mouse
    {
      friend class Game::Window;

    private:
      sf::Vector2i                              _position, _relative;       // Mouse position
      int                                       _wheel;                     // Mouse wheel ticks since last update
      std::array<bool, sf::Mouse::ButtonCount>  _down, _pressed, _released; // Maps of down/pressed/released mouse button

      inline Mouse() : _position(), _relative(), _wheel(), _down(), _pressed(), _released() {}
      inline ~Mouse() = default;

    public:
      inline const sf::Vector2i&  position() const { return _position; }; // Return mouse current position
      inline const sf::Vector2i&  relative() const { return _relative; }; // Return mouse movement since last update
      inline int                  wheel() const { return  _wheel; };      // Return wheel ticks since last update

      inline bool buttonDown(sf::Mouse::Button button) const { return _down[button]; };         // Check if a button is currently pressed
      inline bool buttonPressed(sf::Mouse::Button button) const { return _pressed[button]; };   // Check if a button has been pressed since last update
      inline bool buttonReleased(sf::Mouse::Button button) const { return _released[button]; }; // Check if a button has been released since last update
    };

    class Keyboard
    {
      friend class Game::Window;

    private:
      std::wstring                              _text;                      // Buffer of text typed
      std::array<bool, sf::Keyboard::KeyCount>  _down, _pressed, _released; // Maps of down/pressed/released keyboard key

      inline Keyboard() : _text(), _down(), _pressed(), _released() {}
      inline ~Keyboard() = default;

    public:
      inline bool                 keyDown(sf::Keyboard::Key key) const { return _down[key]; };          // Check if a key is currently pressed
      inline bool                 keyPressed(sf::Keyboard::Key key) const { return _pressed[key]; };    // Check if a key has been pressed since last update
      inline bool                 keyReleased(sf::Keyboard::Key key) const { return _released[key]; };  // Check if a key has been released since last update
      inline const std::wstring&  text() const { return _text; };                                       // Return text entered since last frame
    };

    class Joystick
    {
      friend class Game::Window;

    private:
      static float const  DeadZone; // Dead zone of joysticks

      std::array<std::array<float, sf::Joystick::AxisCount>, sf::Joystick::Count>   _position, _relative;       // Joystick position
      std::array<std::array<bool, sf::Joystick::ButtonCount>, sf::Joystick::Count>  _down, _pressed, _released; // Maps of down/pressed/released joystick key

      inline Joystick() : _position(), _relative(), _down(), _pressed(), _released() {}
      inline ~Joystick() = default;

    public:
      inline float  position(unsigned int joystick, unsigned int axis) const { return _position[joystick][axis]; }; // Return joystick axis current position
      inline float  relative(unsigned int joystick, unsigned int axis) const { return _relative[joystick][axis]; }; // Return joystick axis movement since last update

      inline bool buttonDown(unsigned int joystick, unsigned int button) const { return _down[joystick][button]; };         // Check if a button is currently pressed
      inline bool buttonPressed(unsigned int joystick, unsigned int button) const { return _pressed[joystick][button]; };   // Check if a button has been pressed since last update
      inline bool buttonReleased(unsigned int joystick, unsigned int button) const { return _released[joystick][button]; }; // Check if a button has been released since last update
    };

    sf::RenderWindow        _window;    // SFML window
    Game::Window::Mouse     _mouse;     // Mouse informations
    Game::Window::Keyboard  _keyboard;  // Keyboard informations
    Game::Window::Joystick  _joystick;  // Joysticks informations
    float                   _elapsed;   // Time elapsed since last FPS update
    unsigned int            _tick;      // Number of frame rendered since last FPS update
    bool                    _sync;      // Vertical synchronisation toogle

#ifdef _WIN32
    ITaskbarList3*  _taskbar; // Windows 7+ window handler
#else
    void*           _taskbar; // Not supported on Linux for now
#endif

    static std::unordered_map<int, Game::Window>  _windows; // Map of opened windows

  public:
    Window();
    ~Window() = default;

#ifdef _WIN32
    enum WindowFlag
    {
      NoProgress = TBPF_NOPROGRESS,       // Default taskbar icon
      Indeterminate = TBPF_INDETERMINATE, // Green flickering icon
      Normal = TBPF_NORMAL,               // Green progress bar
      Error = TBPF_ERROR,                 // Red progress bar
      Paused = TBPF_PAUSED                // Orange progress bar
    };
#else
    enum WindowFlag // Not supported on Linux for now
    {
      NoProgress,
      Indeterminate,
      Normal,
      Error,
      Paused
    };
#endif

    static Game::Window&  Instance(int id = 0); // Get a window instance (create if non-existent)
    static void           Delete(int id);       // Remove a window instance (ID 0 can't be deleted)
    static void           Clear();              // Remove every window instance except ID 0

    bool  update(float elapsed);                                                                    // Update window (get events)
    void  create(const sf::VideoMode& video, sf::Uint32 style, const sf::ContextSettings& context); // (Re)create window with parameters
    void  taskbar(WindowFlag flag);                                                                 // Set taskbar status (Windows 7+ only)
    void  taskbar(WindowFlag flag, float value);                                                    // Set taskbar progress (Windows 7+ only)
    void  transparency(sf::Uint8 transparency);                                                     // Set window global transparency
    bool  getVerticalSync() const;                                                                  // Return true if vertical sync is enabled
    void  setVerticalSync(bool sync);                                                               // Enable vertical sync

    inline sf::RenderWindow&              window() { return _window; };           // Get SFML window
    inline const Game::Window::Mouse&     mouse() const { return _mouse; };       // Get mouse informations
    inline const Game::Window::Keyboard&  keyboard() const { return _keyboard; }; // Get keyboard informations
    inline const Game::Window::Joystick&  joystick() const { return _joystick; }; // Get joystick informations

    void draw(sf::Sprite& sprite, float ratio = 1.f); // Draw a sprite, fill screen
  };
}