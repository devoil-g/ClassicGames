#pragma once

#include <list>
#include <memory>

#include <SFML/Graphics/RenderTexture.hpp>

#include "Math/Vector.hpp"
#include "System/Window.hpp"

namespace RPG
{
  class Windows
  {
  public:
    class AbstractWindow
    {
    private:
      sf::RenderTexture     _texture;   // Render target of the window

    protected:
      const AbstractWindow& _parent;


      

      
    protected:
      sf::RenderTarget& getTarget();  // Get render target of window

      virtual void  draw() = 0; // Draw window content to target

    public:
      AbstractWindow() = delete;
      AbstractWindow(const AbstractWindow&) = delete;
      AbstractWindow(AbstractWindow&&) = delete;
      virtual ~AbstractWindow() = default;

      AbstractWindow& operator=(const AbstractWindow&) = delete;
      AbstractWindow& operator=(AbstractWindow&&) = delete;

      Math::Vector<2, unsigned int> getSize() const;                                    // Get window size
      void                          setSize(const Math::Vector<2, unsigned int>& size); // Set window size
      
      void  render(); // Draw window content and update target
      
      virtual bool  update(float elapsed, const Math::Matrix<3, 3, float>& transform) = 0;  // Update window, return true when window should be removed
      
    };

  private:
    class WindowsManager : public AbstractWindow
    {
    private:
      std::list<std::unique_ptr<AbstractWindow>>  _windows; // Windows sorted by depth (foreground on front)

      void draw() override;

    public:
      WindowsManager();
      WindowsManager(const WindowsManager&) = delete;
      WindowsManager(WindowsManager&&) = delete;
      ~WindowsManager() override = default;

      bool  update(float elapsed, const Math::Matrix<3, 3, float>& transform) override;
    };

    std::list<std::unique_ptr<AbstractWindow>>  _windows; // Windows sorted by depth (foreground on front)

  public:
    Windows() = default;
    Windows(const Windows&) = delete;
    Windows(Windows&&) = delete;
    ~Windows() = default;

    Windows& operator=(const Windows&) = delete;
    WindowsManager& operator=(Windows&&) = delete;

    void  update(float elapsed);  // Update windows
    void  render();               // Render windows to screen
  };
}