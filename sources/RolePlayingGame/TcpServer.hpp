#pragma once

#include <thread>
#include <list>

#include <SFML/Network/TcpListener.hpp>
#include <SFML/Network/TcpSocket.hpp>

namespace RPG
{
  class TcpServer
  {
  private:
    const unsigned int  DefaultTickrate = 30; // Default tickrate of the server

    struct TcpClient
    {
      sf::TcpSocket socket; // TCP socket of client
      std::size_t   id;     // Unique ID of the client
      bool          kick;   // True if client should be removed

      TcpClient();
    };

    std::thread                     _thread;    // Thread of the server
    sf::TcpListener                 _listener;  // Main TCP socket
    sf::IpAddress                   _address;   // Local address
    bool                            _running;   // True when server is running
    std::list<TcpServer::TcpClient> _clients;   // Clients of the server

    void  loop(); // Server main loop

  public:
    TcpServer(std::uint16_t port = 0, sf::IpAddress address = sf::IpAddress::Any);
    ~TcpServer();

    void  run();  // Start the server (non-blocking)
    void  wait(); // Wait for server to stop (blocking)
    void  stop(); // Request server to stop (non-blocking)

    std::uint16_t getPort() const;    // Get local port of TCP listerner
    sf::IpAddress getAddress() const; // Get local address of TCP listener

  protected:
    void  send(std::size_t id, sf::Packet& packet); // Send packet to TCP client
    void  broadcast(sf::Packet& packet);            // Broadcast packet to every TCP client
    void  kick(std::size_t id);                     // Kick TCP client

    std::uint16_t getPort(std::size_t id) const;    // Get remote port of TCP client
    sf::IpAddress getAddress(std::size_t id) const; // Get remote address of TCP client

    virtual void  onConnect(std::size_t id) = 0;                      // Called when a new TCP client connect
    virtual void  onDisconnect(std::size_t id) = 0;                   // Called when a TCP client disconnect
    virtual void  onReceive(std::size_t id, sf::Packet& packet) = 0;  // Called when a packet is received from TCP client 
    virtual void  onTick() = 0;                                       // Called once per tick
  };
}