#include <stdexcept>

#include <SFML/Network/SocketSelector.hpp>
#include <SFML/Network/Packet.hpp>
#include <SFML/System/Clock.hpp>

#include "RolePlayingGame/TcpServer.hpp"

RPG::TcpServer::TcpServer(std::uint16_t port, std::uint32_t address) :
  _thread(),
  _listener(),
  _address(address),
  _tickrate(DefaultTickrate),
  _running(false),
  _clients()
{
  // Start TCP listener
  if (_listener.listen(port, sf::IpAddress(address)) != sf::Socket::Status::Done)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
}

RPG::TcpServer::~TcpServer()
{
  // Wait for server to stop
  stop();
  wait();
}

void  RPG::TcpServer::setTickrate(unsigned int tickrate)
{
  // Set tickrate of server
  _tickrate = tickrate;
}

unsigned int  RPG::TcpServer::getTickrate() const
{
  // Get current tickrate of server
  return _tickrate;
}

std::uint16_t RPG::TcpServer::getPort() const
{
  // Get local port from socket
  return _listener.getLocalPort();
}

std::uint32_t RPG::TcpServer::getAddress() const
{
  // Get local address of server
  return _address.toInteger();
}

std::uint16_t RPG::TcpServer::getPort(std::size_t id) const
{
  auto  it = std::find_if(_clients.begin(), _clients.end(), [id](const auto& client) { return client.id == id; });

  // Client does not exist
  if (it == _clients.end())
    return 0;

  // Get remote port
  return it->socket.getRemotePort();
}

std::uint32_t RPG::TcpServer::getAddress(std::size_t id) const
{
  auto  it = std::find_if(_clients.begin(), _clients.end(), [id](const auto& client) { return client.id == id; });

  // Client does not exist
  if (it == _clients.end())
    return 0;

  // Get remote address
  return it->socket.getRemoteAddress().toInteger();
}

void  RPG::TcpServer::run()
{
  // Can't have multiple servers running
  if (_thread.joinable() == true)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Start the server loop
  _running = true;
  _thread = std::thread([this]() { loop(); });
}

void  RPG::TcpServer::wait()
{
  // Wait for the end of server thread
  if (_thread.joinable() == true) {
    _thread.join();
    _running = false;
    _clients.clear();
  }
}

void  RPG::TcpServer::stop()
{
  // Request stop of the server loop
  _running = false;
}

void  RPG::TcpServer::loop()
{
  sf::SocketSelector  selector;
  sf::Clock           clock;
  float               elapsed = 0.f;

  // Add TCP listener and UDP socket to selector
  selector.add(_listener);

  while (_running == true)
  {
    // Handle game tick
    if (_tickrate != 0)
      for (elapsed += clock.restart().asSeconds(); elapsed >= 1.f / RPG::TcpServer::DefaultTickrate; elapsed -= 1.f / RPG::TcpServer::DefaultTickrate)
        onTick();
    else
      elapsed = 0.f;

    // Monitor sockets before next tick
    if (selector.wait((_tickrate == 0) ? (sf::Time::Zero) : (sf::seconds((1.f / _tickrate) - elapsed))) == false)
      continue;

    // Handle new TCP client
    if (selector.isReady(_listener) == true)
    {
      // Add new client
      _clients.emplace_back();
      if (_listener.accept(_clients.back().socket) == sf::Socket::Status::Done) {
        selector.add(_clients.back().socket);
        onConnect(_clients.back().id);
      }
      
      // Cancel in case of error
      else
        _clients.pop_back();
    }

    // Handle clients
    for (auto& client : _clients) {
      if (client.kick == false && selector.isReady(client.socket) == true) {
        sf::Packet          packet;
        sf::Socket::Status  status = client.socket.receive(packet);

        if (status == sf::Socket::Done) {
          try {
            std::string raw;

            packet >> raw;
            onReceive(client.id, Game::JSON::load(raw));
          }
          catch (const std::exception&) {
            client.kick = true;
          }
        }
        else
          client.kick = true;
      }
    }

    // Remove clients
    for (auto it = _clients.begin(); it != _clients.end();) {
      if (it->kick == true) {
        onDisconnect(it->id);
        selector.remove(it->socket);
        it = _clients.erase(it);
      }
      else
        it++;
    }
  }
}

void  RPG::TcpServer::send(std::size_t id, const Game::JSON::Object& json)
{
  // NOTE: blocking send, SFML does not provide a socket selector for write
  // TODO: find another network library

  auto  it = std::find_if(_clients.begin(), _clients.end(), [id](const auto& client) { return client.id == id; });

  // Client does not exist
  if (it == _clients.end())
    return;

  // Does nothing, client already disconnected
  if (it->kick == true)
    return;

  sf::Packet  packet;

  // Serialize JSON
  packet << json.stringify();

  // Kick client in case of error
  if (it->socket.send(packet) != sf::Socket::Status::Done)
    it->kick = true;
}

void  RPG::TcpServer::broadcast(const Game::JSON::Object& json)
{
  sf::Packet  packet;

  // Serialize JSON
  packet << json.stringify();

  // Send message to every connected clients
  for (auto& client : _clients)
  {
    // Does nothing, client already disconnected
    if (client.kick == true)
      continue;

    // Kick client in case of error
    if (client.socket.send(packet) != sf::Socket::Status::Done)
      client.kick = true;
  }
}

void  RPG::TcpServer::kick(std::size_t id)
{
  auto  it = std::find_if(_clients.begin(), _clients.end(), [id](const auto& client) { return client.id == id; });

  // Client does not exist
  if (it == _clients.end())
    return;

  // Request client disconnect
  it->kick = true;
}

RPG::TcpServer::TcpClient::TcpClient() :
  socket(),
  id(),
  kick(false)
{
  static std::size_t  idGenerator = 0;

  // Set unique ID of client
  id = ++idGenerator;
}