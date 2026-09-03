#pragma once

#include <thread>
#include <list>
#include <queue>

#include <SFML/Network/TcpListener.hpp>
#include <SFML/Network/TcpSocket.hpp>
#include <SFML/Network/SocketSelector.hpp>
#include <SFML/Network/Packet.hpp>
#include <SFML/System/Clock.hpp>

#include "System/JavaScriptObjectNotation.hpp"

namespace RPG
{
  class TcpServer
  {
  public:
    static constexpr float DefaultTimeout = 1.f / 30.f; // Default timeout of the server

  private:
    struct TcpClient
    {
      sf::TcpSocket socket; // TCP socket of client
      std::size_t   id;     // Unique ID of the client
      bool          kick;   // True if client should be removed

      sf::Packet              received; // Received packet buffer
      std::queue<sf::Packet>  sent;     // Packets to send

      TcpClient();
      TcpClient(const TcpClient&) = delete;
      TcpClient(TcpClient&&) = default;
      ~TcpClient() = default;

      TcpClient& operator=(const TcpClient&) = delete;
      TcpClient& operator=(TcpClient&&) = default;
    };

    std::thread                     _thread;    // Thread of the server
    sf::TcpListener                 _listener;  // Main TCP socket
    sf::IpAddress                   _address;   // Local address
    sf::SocketSelector              _selector;  // Socket selector
    sf::Clock                       _clock;     // Main clock
    float                           _timeout;   // Time before a timeout
    bool                            _running;   // True when server is running
    std::list<TcpServer::TcpClient> _clients;   // Clients of the server

    void  loop();                                                                               // Server main loop
    void  selected(RPG::TcpServer::TcpClient& client, sf::SocketSelector::ReadinessType type);  // Received/write for a client
    void  refresh(RPG::TcpServer::TcpClient& client);                                           // Refresh client callbacks

  public:
    static constexpr std::size_t InvalidId = 0;

    TcpServer(std::uint16_t port = 0, std::uint32_t address = 0);
    ~TcpServer();

    void  run();  // Start the server (non-blocking)
    void  wait(); // Wait for server to stop (blocking)
    void  stop(); // Request server stop (non-blocking)

    std::uint16_t getPort() const;    // Get local port of TCP listerner
    std::uint32_t getAddress() const; // Get local address of TCP listener

  protected:
    void  send(std::size_t id, const Game::JSON::Object& json); // Send packet to TCP client
    void  broadcast(const Game::JSON::Object& json);            // Broadcast packet to every TCP client
    void  kick(std::size_t id);                                 // Kick TCP client

    void  setTimeout(unsigned int timeout); // Set timeout of server, 0 for infinite
    float getTimeout() const;               // Get timeout of server

    std::uint16_t getPort(std::size_t id) const;    // Get remote port of TCP client
    std::uint32_t getAddress(std::size_t id) const; // Get remote address of TCP client

    virtual void  onConnect(std::size_t id, float elapsed) = 0;                                 // Called when a new TCP client connect
    virtual void  onDisconnect(std::size_t id, float elapsed) = 0;                              // Called when a TCP client disconnect
    virtual void  onReceive(std::size_t id, float elapsed, const Game::JSON::Object& json) = 0; // Called when a packet is received from TCP client 
    virtual void  onTimeout(float elapsed) = 0;                                                  // Called once per tick
  };
}