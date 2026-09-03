#include <algorithm>
#include <stdexcept>

#include <SFML/Network/SocketSelector.hpp>
#include <SFML/Network/Packet.hpp>
#include <SFML/System/Clock.hpp>

#include "RolePlayingGame/TcpServer.hpp"

RPG::TcpServer::TcpServer(std::uint16_t port, std::uint32_t address) :
  _thread(),
  _listener(),
  _address(address),
  _timeout(DefaultTimeout),
  _running(false),
  _clients()
{
  // Start TCP listener
  if (_listener.listen(port, sf::IpAddress(address)) != sf::Socket::Status::Done)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Non-blocking socket
  _listener.setBlocking(false);
}

RPG::TcpServer::~TcpServer()
{
  // Wait for server to stop
  stop();
  wait();
}

void  RPG::TcpServer::setTimeout(unsigned int timeout)
{
  // Set timeout of server
  _timeout = timeout;
}

float RPG::TcpServer::getTimeout() const
{
  // Get current timeout of server
  return _timeout;
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
  return it->socket.getRemoteAddress().value_or(sf::IpAddress(0)).toInteger();
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

void  RPG::TcpServer::selected(RPG::TcpServer::TcpClient& client, sf::SocketSelector::ReadinessType type)
{
  // Read from socket
  if (type & sf::SocketSelector::Receive) {
    auto status = client.socket.receive(client.received);

    // Full packet received
    if (status == sf::Socket::Status::Done) {
      try {
        std::wstring raw;

        client.received >> raw;
        client.received.clear();
        onReceive(client.id, _clock.restart().asSeconds(), Game::JSON::Object(raw));
      }
      catch (const std::exception& error) {
        std::cerr << "[RPG::TcpServer] Warning: invalid JSON from client #" << client.id << "." << std::endl;
        client.kick = true;
        client.sent = {};
      }
    }

    // Does nothing, wait for the complete packet
    else if (status == sf::Socket::Status::Partial) {}

    // Handle errors
    else {
      std::cerr << "[RPG::TcpServer] Warning: read socket error (" << (int)status << "#) from client #" << client.id << "." << std::endl;
      client.kick = true;
      client.sent = {};
    }
  }

  // Write to socket
  if ((type & sf::SocketSelector::Send) && client.sent.empty() == false) {
    auto status = client.socket.send(client.sent.front());

    // Send completed, removed packet
    if (status == sf::Socket::Status::Done)
      client.sent.pop();

    // Incomplete, wait for full sending
    else if (status == sf::Socket::Status::Partial) {}

    // Handle errors
    else {
      std::cerr << "[RPG::TcpServer] Warning: write socket error (" << (int)status << "#) from client #" << client.id << "." << std::endl;
      client.kick = true;
      client.sent = {};
    }
  }

  // Refresh client callbacks
  refresh(client);
}

void  RPG::TcpServer::refresh(RPG::TcpServer::TcpClient& client)
{
  // Refresh client callback
  _selector.add(client.socket,
    ((client.kick == false) ? sf::SocketSelector::Receive : 0) | ((client.sent.empty() == false) ? sf::SocketSelector::Send : 0),
    [this, &client](auto type) {
      selected(client, type);
    });
}

void  RPG::TcpServer::loop()
{
  // Reset clock
  _clock.restart();

  // Add TCP listener and UDP socket to selector
  _selector.add(_listener, sf::SocketSelector::Receive,
    [&](auto readinessType)
    {
      assert(readinessType == sf::SocketSelector::Receive && "Invalid readiness type.");

      sf::Socket::Status status;

      // Add new client
      _clients.emplace_back();
      auto& client = _clients.back();
      status = _listener.accept(client.socket);

      // Check for error
      if (status == sf::Socket::Status::Done)
      {
        // Non-blocking socket
        client.socket.setBlocking(false);

        // Register new client
        refresh(client);

        // Trigger connect event
        onConnect(_clients.back().id, _clock.restart().asSeconds());
      }
      else {
        _clients.pop_back(); _clients.pop_back();
        std::cerr << "[RPG::TcpServer] Warning: failed to accept a new client (error #" << (int)status << ")." << std::endl;
      }
    });

  // TODO: add TLS support

  // Run server loop
  while (_running == true)
  {
    // Wait for events
    if (_selector.wait(sf::seconds(_timeout)) == true)
      _selector.dispatchReadyCallbacks();

    // No event, timeout!
    else
      onTimeout(_clock.restart().asSeconds());

    // Remove kicked clients
    for (auto it = _clients.begin(); it != _clients.end();) {
      if (it->kick == true && it->sent.empty() == true) {
        onDisconnect(it->id, _clock.restart().asSeconds());
        _selector.remove(it->socket);
        it = _clients.erase(it);
      }
      else {
        it++;
      }
    }
  }
}

void  RPG::TcpServer::send(std::size_t id, const Game::JSON::Object& json)
{
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

  // Add packet to sent queue
  it->sent.push(std::move(packet));

  // Refresh client flags
  refresh(*it);
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

    // Add packet to sent queue
    client.sent.push(packet);

    // Refresh client flags
    refresh(client);
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
  kick(false),
  received(),
  sent()
{
  static std::size_t  idGenerator = 0;

  // Set unique ID of client
  id = ++idGenerator;
}