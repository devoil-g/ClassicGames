#include <iostream>
#include <stdexcept>

#include "RolePlayingGame/TcpClient.hpp"

RPG::TcpClient::TcpClient(std::uint16_t port, std::uint32_t address) :
  _socket()
{
  // Connect TCP socket
  if (_socket.connect(sf::IpAddress(address), port) != sf::Socket::Status::Done)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
  
  // Never block on a receive
  _socket.setBlocking(false);
}

RPG::TcpClient::~TcpClient()
{
  // Blocking socket to force packet sending
  _socket.setBlocking(true);

  // Send pending packets
  try {
    send();
  }
  catch (const std::exception& e) {
    std::cerr << "[RPG::TcpClient] Warning, failed to send pending packets (" << e.what() << ")." << std::endl;
  }
}

std::uint32_t RPG::TcpClient::getRemoteAddress() const
{
  // Get remote address
  return _socket.getRemoteAddress().value_or(sf::IpAddress(0)).toInteger();
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

Game::JSON::Object  RPG::TcpClient::receive()
{
  Game::JSON::Object  json;
  sf::Packet          packet;
  std::wstring        raw;

  // Receive a packet
  sf::Socket::Status  status = _socket.receive(packet);

  // Handle status
  switch (status) {
  case sf::Socket::Status::Done:  // Complete packet
    try {
      packet >> raw;
      return Game::JSON::Object(raw);
    }
    catch (const std::exception&) {
      return Game::JSON::Object();
    }

  case sf::Socket::Status::NotReady:  // Not complete packet, returned packet is empty
    return Game::JSON::Object();

  case sf::Socket::Status::Partial:       // Not supposed to happen
  case sf::Socket::Status::Disconnected:  // Server disconnected, should not happen as server is supposed to explicitly close connexion
  case sf::Socket::Status::Error:         // Unexpected error
  default:
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
  }
}

void  RPG::TcpClient::send(const Game::JSON::Object& json)
{
  sf::Packet  packet;

  // Serialize JSON
  packet << json.stringify();

  // Add packet to pending packets queue
  _packets.push(std::move(packet));

  // Send pending packets
  send();
}

void  RPG::TcpClient::send()
{
  // Send pending packets
  while (_packets.empty() == false) {
    sf::Socket::Status  status = _socket.send(_packets.front());

    switch (status) {
    case sf::Socket::Status::Done:  // Packet sent, next
      _packets.pop();
      continue;

    case sf::Socket::Status::Partial: // Not fully sent, stop
      break;

    case sf::Socket::Status::Disconnected:  // Server disconnected, empty queue (deconnexion detected on receive only)
      _packets = {};
      break;

    case sf::Socket::Status::NotReady:  // Not supposed to happen
    case sf::Socket::Status::Error:     // Unexpected error
    default:
      throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
    }
  }
}