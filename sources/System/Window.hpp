#pragma once

/**
 * @file Window.hpp
 * @brief Game window management system with input handling
 * 
 * This file defines the Window class and its associated input handling classes (Mouse, Keyboard, Joystick).
 * It provides a wrapper around SFML's RenderWindow with additional features for input state tracking,
 * window management, and platform-specific functionality.
 */

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
  /**
   * @class Window
   * @brief Main game window manager with input handling and rendering capabilities
   * 
   * The Window class is a singleton that manages the game's main window, including:
   * - Window creation, configuration, and lifecycle management
   * - Input state tracking for mouse, keyboard, and joystick devices
   * - View and coordinate transformation
   * - Basic rendering operations
   * - Platform-specific features (e.g., Windows taskbar integration)
   * 
   * @note This class uses the singleton pattern. Access the instance via Instance()
   */
  class Window
  {
  public:
    static const Math::Vector<2, unsigned int>  DefaultSize;         ///< Default window size
    static const std::string                    DefaultTitle;        ///< Default window title
    static const unsigned int                   DefaultAntialiasing; ///< Default antialiasing level
    static const bool                           DefaultVerticalSync; ///< Default vertical synchronisation activation
    static const float                          FpsRefresh;          ///< Time between FPS refresh (in seconds)

    using View = Math::Box<2, float>;  ///< Type alias for the window view (2D bounding box)
    using Handle = sf::WindowHandle;   ///< Type alias for the native window handle

  public:
    /**
     * @class Mouse
     * @brief Mouse input state tracker
     * 
     * Tracks mouse position, movement, wheel input, and button states.
     * Provides queries for button down, pressed (on frame), and released (on frame) states.
     * 
     * @note This class is non-copyable and non-movable. Access via Window::mouse()
     */
    class Mouse
    {
      friend class Game::Window;

    public:
      using Button = sf::Mouse::Button;                              ///< SFML mouse button enumeration
      static constexpr unsigned int ButtonCount = sf::Mouse::ButtonCount; ///< Total number of mouse buttons

    private:
      Math::Vector<2, int>          _position, _relative;       ///< Mouse position and movement delta
      float                         _wheel;                     ///< Mouse wheel ticks since last update
      std::array<bool, ButtonCount> _down, _pressed, _released; ///< Button state arrays

      Mouse() : _position(), _relative(), _wheel(0.f), _down(), _pressed(), _released() {}
      Mouse(const Mouse&) = delete;
      Mouse(Mouse&&) = delete;
      ~Mouse() = default;

      Mouse& operator=(const Mouse&) = delete;
      Mouse& operator=(Mouse&&) = delete;

    public:
      /**
       * @brief Get the current mouse position in screen coordinates
       * @return Current mouse position
       */
      const Math::Vector<2, int>& position() const { return _position; }
      
      /**
       * @brief Get the relative mouse movement since last update
       * @return Mouse movement delta
       */
      const Math::Vector<2, int>& relative() const { return _relative; }
      
      /**
       * @brief Get mouse wheel movement since last update
       * @return Wheel ticks (positive = up, negative = down)
       */
      float wheel() const { return  _wheel; }

      /**
       * @brief Check if a mouse button is currently held down
       * @param button The mouse button to check
       * @return true if button is currently pressed
       */
      bool buttonDown(Button button) const { return _down[(std::size_t)button]; }
      
      /**
       * @brief Check if a mouse button was pressed this frame
       * @param button The mouse button to check
       * @return true if button was pressed since last update
       */
      bool buttonPressed(Button button) const { return _pressed[(std::size_t)button]; }
      
      /**
       * @brief Check if a mouse button was released this frame
       * @param button The mouse button to check
       * @return true if button was released since last update
       */
      bool buttonReleased(Button button) const { return _released[(std::size_t)button]; }
    };

  public:
    /**
     * @class Keyboard
     * @brief Keyboard input state tracker
     * 
     * Tracks keyboard key states and text input.
     * Provides queries for key down, pressed (on frame), and released (on frame) states.
     * 
     * @note This class is non-copyable and non-movable. Access via Window::keyboard()
     */
    class Keyboard
    {
      friend class Game::Window;

    public:
      using Key = sf::Keyboard::Key;                       ///< SFML keyboard key enumeration
      static const unsigned int KeyCount = sf::Keyboard::KeyCount; ///< Total number of keyboard keys

    private:
      std::wstring                _text;                      ///< Wide character text input buffer
      std::array<bool, KeyCount>  _down, _pressed, _released; ///< Key state arrays

      Keyboard() : _text(), _down(), _pressed(), _released() {}
      Keyboard(const Keyboard&) = delete;
      Keyboard(Keyboard&&) = delete;
      ~Keyboard() = default;

      Keyboard& operator=(const Keyboard&) = delete;
      Keyboard& operator=(Keyboard&&) = delete;

    public:
      /**
       * @brief Check if a keyboard key is currently held down
       * @param key The keyboard key to check
       * @return true if key is currently pressed, false if key is Unknown
       */
      bool keyDown(Key key) const { return key == Key::Unknown ? false : _down[(std::size_t)key]; }
      
      /**
       * @brief Check if a keyboard key was pressed this frame
       * @param key The keyboard key to check
       * @return true if key was pressed since last update, false if key is Unknown
       */
      bool keyPressed(Key key) const { return key == Key::Unknown ? false : _pressed[(std::size_t)key]; }
      
      /**
       * @brief Check if a keyboard key was released this frame
       * @param key The keyboard key to check
       * @return true if key was released since last update, false if key is Unknown
       */
      bool keyReleased(Key key) const { return key == Key::Unknown ? false : _released[(std::size_t)key]; }
      
      /**
       * @brief Get the text entered since last frame
       * @return Wide string containing typed characters
       */
      const std::wstring& text() const { return _text; }
    };

  public:
    /**
     * @class Joystick
     * @brief Joystick/gamepad input state tracker
     * 
     * Tracks state for multiple joysticks including axis positions and button states.
     * Supports up to JoystickCount simultaneous joysticks.
     * 
     * @note This class is non-copyable and non-movable. Access via Window::joystick()
     */
    class Joystick
    {
      friend class Game::Window;

    public:
      using Axis = sf::Joystick::Axis;                            ///< SFML joystick axis enumeration
      static const unsigned int JoystickCount = sf::Joystick::Count;      ///< Maximum number of supported joysticks
      static const unsigned int ButtonCount = sf::Joystick::ButtonCount;  ///< Maximum buttons per joystick
      static const unsigned int AxisCount = sf::Joystick::AxisCount;      ///< Maximum axes per joystick

    private:
      static float const  DeadZone; ///< Dead zone threshold for joystick axes

      std::array<std::array<float, AxisCount>, JoystickCount>   _position, _relative;       ///< Axis position and movement delta
      std::array<std::array<bool, ButtonCount>, JoystickCount>  _down, _pressed, _released; ///< Button state arrays

      Joystick() = default;
      Joystick(const Joystick&) = delete;
      Joystick(Joystick&&) = delete;
      ~Joystick() = default;

      Joystick& operator=(const Joystick&) = delete;
      Joystick& operator=(Joystick&&) = delete;

    public:
      /**
       * @brief Check if a joystick is connected
       * @param joystick The joystick index (0 to JoystickCount-1)
       * @return true if the joystick is connected
       */
      bool connected(unsigned int joystick) const { return sf::Joystick::isConnected(joystick); }

      /**
       * @brief Get the current position of a joystick axis
       * @param joystick The joystick index
       * @param axis The axis to query
       * @return Axis position (typically -100 to 100)
       */
      float position(unsigned int joystick, Axis axis) const { return _position[joystick][(std::size_t)axis]; }
      
      /**
       * @brief Get the relative movement of a joystick axis since last update
       * @param joystick The joystick index
       * @param axis The axis to query
       * @return Axis movement delta
       */
      float relative(unsigned int joystick, Axis axis) const { return _relative[joystick][(std::size_t)axis]; }

      /**
       * @brief Check if a joystick button is currently held down
       * @param joystick The joystick index
       * @param button The button index
       * @return true if button is currently pressed
       */
      bool buttonDown(unsigned int joystick, unsigned int button) const { return _down[joystick][button]; }
      
      /**
       * @brief Check if a joystick button was pressed this frame
       * @param joystick The joystick index
       * @param button The button index
       * @return true if button was pressed since last update
       */
      bool buttonPressed(unsigned int joystick, unsigned int button) const { return _pressed[joystick][button]; }
      
      /**
       * @brief Check if a joystick button was released this frame
       * @param joystick The joystick index
       * @param button The button index
       * @return true if button was released since last update
       */
      bool buttonReleased(unsigned int joystick, unsigned int button) const { return _released[joystick][button]; }
    };

  private:
    sf::RenderWindow              _window;        ///< Underlying SFML render window
    Game::Window::Mouse           _mouse;         ///< Mouse input state
    Game::Window::Keyboard        _keyboard;      ///< Keyboard input state
    Game::Window::Joystick        _joystick;      ///< Joystick input state
    float                         _elapsed;       ///< Time elapsed since last FPS update
    unsigned int                  _tick;          ///< Number of frames rendered since last FPS update
    Math::Vector<2, unsigned int> _size;          ///< Current window resolution
    std::string                   _title;         ///< Current window title
    std::uint32_t                 _style;         ///< Current window style flags
    unsigned int                  _antialiasing;  ///< Current anti-aliasing level
    bool                          _vsync;         ///< Vertical synchronisation enabled flag
    bool                          _fullscreen;    ///< Fullscreen mode enabled flag

#ifdef _WIN32
    ::ITaskbarList3*  _taskbar; ///< Windows 7+ taskbar interface for progress indication
#else
    void*             _taskbar; ///< Placeholder for non-Windows platforms (not supported)
#endif

    /**
     * @brief Recreate the window using current internal parameters
     * @note This destroys and recreates the SFML window, useful when changing settings
     */
    void recreateWindow();

  public:
    /**
     * @brief Construct the window with default parameters
     */
    Window();

    /**
     * @brief Deleted copy constructor (non-copyable singleton)
     */
    Window(const Window&) = delete;

    /**
     * @brief Deleted move constructor (non-movable singleton)
     */
    Window(Window&&) = delete;

    /**
     * @brief Default destructor
     * @note Releases platform-specific resources (e.g., Windows taskbar interface)
     */
    ~Window() = default;
    
    /**
     * @brief Deleted copy assignment operator (non-copyable singleton)
     */
    Window& operator=(const Window&) = delete;

    /**
     * @brief Deleted move assignment operator (non-movable singleton)
     */
    Window& operator=(Window&&) = delete;

#ifdef _WIN32
    /**
     * @enum WindowFlag
     * @brief Taskbar progress indicator states (Windows 7+ only)
     */
    enum WindowFlag
    {
      NoProgress = TBPF_NOPROGRESS,       ///< Default taskbar icon (no progress)
      Indeterminate = TBPF_INDETERMINATE, ///< Green flickering icon (indeterminate progress)
      Normal = TBPF_NORMAL,               ///< Green progress bar
      Error = TBPF_ERROR,                 ///< Red progress bar (error state)
      Paused = TBPF_PAUSED                ///< Orange progress bar (paused state)
    };
#else
    /**
     * @enum WindowFlag
     * @brief Taskbar progress indicator states (not supported on Linux)
     */
    enum WindowFlag
    {
      NoProgress,      ///< Default taskbar icon
      Indeterminate,   ///< Indeterminate progress
      Normal,          ///< Normal progress
      Error,           ///< Error state
      Paused           ///< Paused state
    };
#endif

    /**
     * @enum DrawMode
     * @brief Texture drawing modes for fitting content to window
     */
    enum class DrawMode
    {
      Fill,   ///< Enlarges to fit window width or height (maintains aspect ratio)
      Fit,    ///< Shrinks to fit within window bounds (maintains aspect ratio)
      Stretch ///< Stretches to fill entire window (may distort)
    };

    /**
     * @brief Get the singleton window instance
     * @return Reference to the global Window instance
     */
    static Game::Window&  Instance();
    
    /**
     * @brief Update window and process events
     * @param elapsed Time elapsed since last update (in seconds)
     * @return true if window is still open, false if closed
     */
    bool update(float elapsed);
    
    /**
     * @brief Set taskbar icon state (Windows 7+ only)
     * @param flag The taskbar state to set
     */
    void taskbar(WindowFlag flag);
    
    /**
     * @brief Set taskbar progress indicator (Windows 7+ only)
     * @param flag The taskbar state to set
     * @param value Progress value (0.0 to 1.0)
     */
    void taskbar(WindowFlag flag, float value);
    
    /**
     * @brief Set window transparency
     * @param transparency Transparency level (0 = fully transparent, 255 = opaque)
     */
    void transparency(std::uint8_t transparency);

    /**
     * @brief Get the current window view
     * @return Current view as a 2D bounding box
     */
    View getView() const;
    
    /**
     * @brief Get the default window view with origin at top-left
     * @return Default view
     */
    View getDefaultView() const;
    
    /**
     * @brief Get the current window resolution
     * @return Window size in pixels
     */
    Math::Vector<2, unsigned int> getSize() const { return _size; }
    
    /**
     * @brief Get the window title
     * @return Current window title string
     */
    const std::string& getTitle() const { return _title; }
    
    /**
     * @brief Get the current window style flags
     * @return Window style bitmask
     */
    std::uint32_t getStyle() const { return _style; }
    
    /**
     * @brief Get the antialiasing level
     * @return Current antialiasing level
     */
    unsigned int getAntialiasing() const { return _antialiasing; }
    
    /**
     * @brief Check if vertical sync is enabled
     * @return true if vertical sync is enabled
     */
    bool getVerticalSync() const { return _vsync; }
    
    /**
     * @brief Check if window is in fullscreen mode
     * @return true if window is fullscreen
     */
    bool getFullscreen() const { return _fullscreen; }
    
    /**
     * @brief Get the native platform window handle
     * @return Platform-specific window handle
     */
    Handle getHandle() const { return _window.getNativeHandle(); }
    
    /**
     * @brief Check if window is open
     * @return true if window is currently open
     */
    bool isOpen() const { return _window.isOpen(); }

    /**
     * @brief Transform screen pixel coordinates to world coordinates using current view
     * @param pixel Screen coordinates
     * @return World coordinates
     */
    Math::Vector<2, float> pixelToCoords(Math::Vector<2, int> pixel) const;
    
    /**
     * @brief Transform screen pixel coordinates to world coordinates using specified view
     * @param pixel Screen coordinates
     * @param view The view to use for transformation
     * @return World coordinates
     */
    Math::Vector<2, float> pixelToCoords(Math::Vector<2, int> pixel, const View& view) const;
    
    /**
     * @brief Transform world coordinates to screen pixel coordinates using current view
     * @param coords World coordinates
     * @return Screen pixel coordinates
     */
    Math::Vector<2, int> coordsToPixels(Math::Vector<2, float> coords) const;
    
    /**
     * @brief Transform world coordinates to screen pixel coordinates using specified view
     * @param coords World coordinates
     * @param view The view to use for transformation
     * @return Screen pixel coordinates
     */
    Math::Vector<2, int> coordsToPixels(Math::Vector<2, float> coords, const View& view) const;

    /**
     * @brief Set the active window view
     * @param view The view to apply
     */
    void setView(const View& view);
    
    /**
     * @brief Reset to default window view with origin at top-left
     */
    void setDefaultView();
    
    /**
     * @brief Change window resolution
     * @param size New window size in pixels
     */
    void setSize(Math::Vector<2, unsigned int> size);
    
    /**
     * @brief Set the window title
     * @param title New window title
     */
    void setTitle(const std::string& title);
    
    /**
     * @brief Set window style flags
     * @param style Window style bitmask
     */
    void setStyle(std::uint32_t style);
    
    /**
     * @brief Set antialiasing level
     * @param antialiasing New antialiasing level
     */
    void setAntialiasing(unsigned int antialiasing);
    
    /**
     * @brief Enable or disable vertical sync
     * @param vsync true to enable vertical sync
     */
    void setVerticalSync(bool vsync);
    
    /**
     * @brief Enable or disable fullscreen mode
     * @param fullscreen true to enable fullscreen
     */
    void setFullscreen(bool fullscreen);

    /**
     * @brief Get mouse input state
     * @return Const reference to Mouse object
     */
    const Game::Window::Mouse& mouse() const { return _mouse; };
    
    /**
     * @brief Get keyboard input state
     * @return Const reference to Keyboard object
     */
    const Game::Window::Keyboard& keyboard() const { return _keyboard; };
    
    /**
     * @brief Get joystick input state
     * @return Const reference to Joystick object
     */
    const Game::Window::Joystick&  joystick() const { return _joystick; };

    /**
     * @brief Clear the window with a solid color
     * @param color RGBA color value (default: transparent black)
     */
    void clear(std::uint32_t color = 0x00000000);
    
    /**
     * @brief Draw an SFML drawable object to the window
     * @param drawable The drawable object to render
     * @param states Optional render states (transform, shader, blend mode, texture)
     */
    void draw(const sf::Drawable& drawable, const sf::RenderStates& states = sf::RenderStates::Default);
    
    /**
     * @brief Draw a texture to fill the window with specified scaling mode
     * @param texture The texture to draw
     * @param ratio Aspect ratio of the texture (default: 1.0)
     * @param mode How to fit the texture to the window (default: Fit)
     */
    void draw(const sf::Texture& texture, float ratio = 1.f, DrawMode mode = DrawMode::Fit);
    
    /**
     * @brief Display rendered content to the window
     * @note Call this after all draw operations to present the frame
     */
    void display();
  };
}