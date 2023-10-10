#include <stdexcept>

#include <SFML/Network/SocketSelector.hpp>

#include "RolePlayingGame/TcpClient.hpp"

RPG::TcpClient::TcpClient(sf::IpAddress address, std::uint16_t port) :
  _socket()
{
  // Connect TCP socket
  if (_socket.connect(address, port) != sf::Socket::Status::Done)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
}

sf::IpAddress RPG::TcpClient::getRemoteAddress() const
{
  // Get remote address
  return _socket.getRemoteAddress();
}

std::uint16_t RPG::TcpClient::getRemotePort() const
{
  // Get remote port
  return _socket.getRemotePort();
}

std::uint16_t RPG::TcpClient::getLocalPort() const
{
  // Get local port
  return _socket.getLocalPort();
}

sf::Packet  RPG::TcpClient::receive()
{
  sf::SocketSelector  selector;
  sf::Packet          packet;

  // Check if there is something to read
  selector.add(_socket);
  if (selector.wait(sf::microseconds(1)) == false)
    return packet;

  // Receive a packet
  sf::Socket::Status  status = _socket.receive(packet);

  // Success
  if (status == sf::Socket::Status::Done)
    return packet;

  // Remote disconnected
  if (status == sf::Socket::Status::Disconnected)
    ; // TODO: handle disconnect

  // Critical error
  throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
}

void  RPG::TcpClient::send(sf::Packet& packet)
{
  // Send a packet
  sf::Socket::Status  status = _socket.send(packet);

  // Success
  if (status == sf::Socket::Status::Done)
    return;

  // Remote disconnected
  if (status == sf::Socket::Status::Disconnected)
    ; // TODO: handle disconnect

  // Critical error
  throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
}