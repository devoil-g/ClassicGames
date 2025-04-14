#pragma once

#include <array>
#include <cstdint>
#include <string>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Window/Joystick.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>

#include "Math/Box.hpp"
#include "Math/Vector.hpp"

#ifdef _WIN32
#include <shobjidl.h>
#include <windows.h>
#endif

namespace Game
{
  class Window
  {
  public:
    static const Math::Vector<2, unsigned int>  DefaultSize;         // Default window size
    static const std::string                    DefaultTitle;         // Window title
    static const unsigned int                   DefaultAntialiasing;  // Default antialiasing level
    static const bool                           DefaultVerticalSync;  // Vertical synchronisation activation
    static const float                          FpsRefresh;           // Time between FPS refresh

    using View = Math::Box<2, float>;
    using MouseButton = sf::Mouse::Button;
    static const unsigned int MouseButtonCount = sf::Mouse::ButtonCount;
    using Key = sf::Keyboard::Key;
    static const unsigned int KeyCount = sf::Keyboard::KeyCount;
    using JoystickAxis = sf::Joystick::Axis;
    static const unsigned int JoystickCount = sf::Joystick::Count;
    static const unsigned int JoystickButtonCount = sf::Joystick::ButtonCount;
    static const unsigned int JoystickAxisCount = sf::Joystick::AxisCount;
    using Handle = sf::WindowHandle;

  private:
    class Mouse
    {
      friend class Game::Window;

    private:
      Math::Vector<2, int>                _position, _relative;       // Mouse position
      int                                 _wheel;                     // Mouse wheel ticks since last update
      std::array<bool, MouseButtonCount>  _down, _pressed, _released; // Maps of down/pressed/released mouse button

      inline Mouse() : _position(), _relative(), _wheel(), _down(), _pressed(), _released() {}
      inline ~Mouse() = default;

    public:
      

      inline const Math::Vector<2, int>&  position() const { return _position; }  // Return mouse current position
      inline const Math::Vector<2, int>&  relative() const { return _relative; }  // Return mouse movement since last update
      inline int                          wheel() const { return  _wheel; }       // Return wheel ticks since last update

      inline bool buttonDown(MouseButton button) const { return _down[(std::size_t)button]; }         // Check if a button is currently pressed
      inline bool buttonPressed(MouseButton button) const { return _pressed[(std::size_t)button]; }   // Check if a button has been pressed since last update
      inline bool buttonReleased(MouseButton button) const { return _released[(std::size_t)button]; } // Check if a button has been released since last update
    };

    class Keyboard
    {
      friend class Game::Window;

    private:
      std::wstring                _text;                      // Buffer of text typed
      std::array<bool, KeyCount>  _down, _pressed, _released; // Maps of down/pressed/released keyboard key

      inline Keyboard() : _text(), _down(), _pressed(), _released() {}
      inline ~Keyboard() = default;

    public:
      inline bool                 keyDown(Key key) const { return _down[(std::size_t)key]; }          // Check if a key is currently pressed
      inline bool                 keyPressed(Key key) const { return _pressed[(std::size_t)key]; }    // Check if a key has been pressed since last update
      inline bool                 keyReleased(Key key) const { return _released[(std::size_t)key]; }  // Check if a key has been released since last update
      inline const std::wstring&  text() const { return _text; }                                      // Return text entered since last frame
    };

    class Joystick
    {
      friend class Game::Window;

    private:
      static float const  DeadZone; // Dead zone of joysticks

      std::array<std::array<float, JoystickAxisCount>, JoystickCount>   _position, _relative;       // Joystick position
      std::array<std::array<bool, JoystickButtonCount>, JoystickCount>  _down, _pressed, _released; // Maps of down/pressed/released joystick key

      inline Joystick() : _position(), _relative(), _down(), _pressed(), _released() {}
      inline ~Joystick() = default;

    public:
      inline bool connected(unsigned int joystick) const { return sf::Joystick::isConnected(joystick); }  // Check if a joystick is connected

      inline float  position(unsigned int joystick, JoystickAxis axis) const { return _position[joystick][(std::size_t)axis]; } // Return joystick axis current position
      inline float  relative(unsigned int joystick, JoystickAxis axis) const { return _relative[joystick][(std::size_t)axis]; } // Return joystick axis movement since last update

      inline bool buttonDown(unsigned int joystick, unsigned int button) const { return _down[joystick][button]; }          // Check if a button is currently pressed
      inline bool buttonPressed(unsigned int joystick, unsigned int button) const { return _pressed[joystick][button]; }    // Check if a button has been pressed since last update
      inline bool buttonReleased(unsigned int joystick, unsigned int button) const { return _released[joystick][button]; }  // Check if a button has been released since last update
    };

    sf::RenderWindow              _window;        // SFML window
    Game::Window::Mouse           _mouse;         // Mouse informations
    Game::Window::Keyboard        _keyboard;      // Keyboard informations
    Game::Window::Joystick        _joystick;      // Joysticks informations
    float                         _elapsed;       // Time elapsed since last FPS update
    unsigned int                  _tick;          // Number of frame rendered since last FPS update
    Math::Vector<2, unsigned int> _size;          // Window resolution
    std::string                   _title;         // Window title
    std::uint32_t                 _style;         // Window style
    unsigned int                  _antialiasing;  // Anti-aliasing level
    bool                          _vsync;         // Vertical synchronisation toggle
    bool                          _fullscreen;    // Fullscreen mode

    void  recreateWindow(); // Recreate window using internal parameters

#ifdef _WIN32
    ::ITaskbarList3*  _taskbar; // Windows 7+ window handler
#else
    void*             _taskbar; // Not supported on Linux for now
#endif

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

    enum class DrawMode
    {
      Fill,   // Enlarges to fit window width or height
      Fit,    // Shrinks to fit window width and height
      Strech  // Strech to fill window
    };

    static Game::Window&  Instance(); // Get window instance
    
    bool  update(float elapsed);                    // Update window (get events)
    void  taskbar(WindowFlag flag);                 // Set taskbar status (Windows 7+ only)
    void  taskbar(WindowFlag flag, float value);    // Set taskbar progress (Windows 7+ only)
    void  transparency(std::uint8_t transparency);  // Set window transparency

    View                          getView() const;
    Math::Vector<2, unsigned int> getSize() const { return _size; }                       // Get current resolution
    const std::string&            getTitle() const { return _title; }                     // Get window title
    std::uint32_t                 getStyle() const { return _style; }                     // Get current window style
    unsigned int                  getAntialiasing() const { return _antialiasing; }       // Get antialiasing level
    bool                          getVerticalSync() const { return _vsync; }              // Return true if vertical sync is enabled
    bool                          getFullscreen() const { return _fullscreen; }           // Return true if window is fullscreen
    Handle                        getHandle() const { return _window.getNativeHandle(); } // Get native window handle
    bool                          isOpen() const { return _window.isOpen(); }             // Check if window is open

    Math::Vector<2, float>  pixelToCoords(Math::Vector<2, int> pixel) const;      // Transform screen coordinates to world coordinates
    Math::Vector<2, float>  pixelToCoords(Math::Vector<2, int> pixel, const View& view) const;      // Transform screen coordinates to world coordinates
    Math::Vector<2, int>    coordsToPixels(Math::Vector<2, float> coords) const;  // Transform world coordinates to screen coordinates
    Math::Vector<2, int>    coordsToPixels(Math::Vector<2, float> coords, const View& view) const;  // Transform world coordinates to screen coordinates

    void  setView(const View& view);                    // Change window view
    void  setSize(Math::Vector<2, unsigned int> size);  // Change resolution
    void  setTitle(const std::string& title);           // Set window title
    void  setStyle(std::uint32_t style);                // Set window style
    void  setAntialiasing(unsigned int antialiasing);   // Set antialiasing level
    void  setVerticalSync(bool vsync);                  // Enable vertical sync
    void  setFullscreen(bool fullscreen);               // Enable fullscreen mode

    inline const Game::Window::Mouse&     mouse() const { return _mouse; };       // Get mouse informations
    inline const Game::Window::Keyboard&  keyboard() const { return _keyboard; }; // Get keyboard informations
    inline const Game::Window::Joystick&  joystick() const { return _joystick; }; // Get joystick informations

    void  clear(std::uint32_t color = 0x00000000);                                                        // Clear window
    void  draw(const sf::Drawable& drawable, const sf::RenderStates& states = sf::RenderStates::Default); // Draw a sprite to window
    void  draw(const sf::Texture& texture, float ratio = 1.f, DrawMode mode = DrawMode::Fit);             // Draw a texture to screen
    void  display();                                                                                      // Render drawn sprites
  };
}