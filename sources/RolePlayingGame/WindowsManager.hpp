#pragma once

#include <list>
#include <memory>

#include <SFML/Graphics/RenderTexture.hpp>

#include "Math/Box.hpp"
#include "Math/Transformable.hpp"
#include "Math/Vector.hpp"
#include "System/Window.hpp"

namespace RPG
{
  class WindowsManager
  {
  public:
    class AbstractWindow : public Math::Transformable<2>
    {
    private:
      friend RPG::WindowsManager;

      sf::RenderTexture _texture; // Render target of the window
      bool              _focus;   // True if window is selected
      bool              _hover;   // True if cursor is above window

      void  setFocus(bool focus); // Focus flag setter
      void  setHover(bool hover); // Hover flag setter

    protected:
      sf::RenderTarget& getTarget();  // Get render target of window

    public:
      AbstractWindow() = delete;
      AbstractWindow(const AbstractWindow&) = delete;
      AbstractWindow(AbstractWindow&&) = delete;
      virtual ~AbstractWindow() = default;

      AbstractWindow& operator=(const AbstractWindow&) = delete;
      AbstractWindow& operator=(AbstractWindow&&) = delete;

      Math::Vector<2, unsigned int> getSize() const;                                    // Get window size
      void                          setSize(const Math::Vector<2, unsigned int>& size); // Set window size
      
      Math::Box<2>  getBounds() const;  // Get bounds of window (do not support rotations)

      bool  getFocus() const; // Check if window is selected
      bool  getHover() const; // Check if cursor is above the window

      virtual bool  update(float elapsed) = 0;  // Update window, return true when window should be removed
      virtual void  render() = 0;               // Draw window content and render target to screen
    };

  private:
    std::list<std::unique_ptr<AbstractWindow>>  _windows; // Windows sorted by depth (foreground on front)

  public:
    WindowsManager() = default;
    WindowsManager(const WindowsManager&) = delete;
    WindowsManager(WindowsManager&&) = delete;
    ~WindowsManager() = default;

    WindowsManager& operator=(const WindowsManager&) = delete;
    WindowsManager& operator=(WindowsManager&&) = delete;

    void  update(float elapsed);  // Update windows
    void  render();               // Render windows to screen
  };
}