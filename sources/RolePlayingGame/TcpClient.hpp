#pragma once

#include <SFML/Network/TcpSocket.hpp>
#include <SFML/Network/IpAddress.hpp>
#include <SFML/Network/Packet.hpp>

namespace RPG
{
  class TcpClient
  {
  private:
    const unsigned int  DefaultTickrate = 30; // Default tickrate of the server

    sf::TcpSocket _socket;  // Main TCP socket
    
  public:
    TcpClient(sf::IpAddress address, std::uint16_t port);
    ~TcpClient() = default;

    sf::IpAddress getRemoteAddress() const; // Get remote address
    std::uint16_t getRemotePort() const;    // Get remote port
    std::uint16_t getLocalPort() const;     // Get local port

  protected:
    sf::Packet  receive();                // Receive a packet, empty packet when nothing available
    void        send(sf::Packet& packet); // Send packet
  };
}