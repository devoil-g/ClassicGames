#pragma once

#include <queue>

#include <SFML/Network/TcpSocket.hpp>
#include <SFML/Network/IpAddress.hpp>
#include <SFML/Network/Packet.hpp>

#include "System/JavaScriptObjectNotation.hpp"

namespace RPG
{
  class TcpClient
  {
  private:
    const unsigned int  DefaultTickrate = 30; // Default tickrate of the server

    sf::TcpSocket           _socket;  // Main TCP socket (non-blocking)
    std::queue<sf::Packet>  _packets; // Packets to be sent
    
  public:
    TcpClient(std::uint16_t port, std::uint32_t address);
    ~TcpClient();

    std::uint32_t getRemoteAddress() const; // Get remote address
    std::uint16_t getRemotePort() const;    // Get remote port
    std::uint16_t getLocalPort() const;     // Get local port

    Game::JSON::Object  receive();                            // Receive a packet (non-blocking), empty packet when nothing available
    void                send(const Game::JSON::Object& json); // Send given packet (non-blocking)
    void                send();                               // Send pending packets (non-blocking)
  };
}