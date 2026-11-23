#include "system_network.h"
#include <iostream>

using namespace sf;

namespace {
Network::Mode _mode = Network::Mode::None;
TcpListener _listener;
TcpSocket _socket;
bool _connected = false;
Vector2f _lastReceived;
bool _hasNewPosition = false;
} // namespace

void Network::startHost(unsigned short port) {
  _mode = Mode::Hosting;
  _listener.setBlocking(false);
  if (_listener.listen(port) != Socket::Done) {
    std::cerr << "Network: failed to listen on port " << port << "\n";
    _mode = Mode::None;
    return;
  }
  std::cout << "Network: hosting on port " << port
            << ", waiting for a client...\n";
}

bool Network::joinHost(const std::string& ip, unsigned short port) {
  _socket.setBlocking(true);
  if (_socket.connect(ip, port, seconds(5)) != Socket::Done) {
    std::cerr << "Network: failed to connect to " << ip << ":" << port
               << "\n";
    return false;
  }
  _socket.setBlocking(false);
  _mode = Mode::Joined;
  _connected = true;
  std::cout << "Network: connected to " << ip << ":" << port << "\n";
  return true;
}

void Network::update(const double&) {
  if (_mode == Mode::Hosting && !_connected) {
    if (_listener.accept(_socket) == Socket::Done) {
      _socket.setBlocking(false);
      _connected = true;
      std::cout << "Network: client connected\n";
    }
    return;
  }

  if (!_connected) return;

  Packet packet;
  auto status = _socket.receive(packet);
  if (status == Socket::Done) {
    float x, y;
    packet >> x >> y;
    _lastReceived = Vector2f(x, y);
    _hasNewPosition = true;
  } else if (status == Socket::Disconnected || status == Socket::Error) {
    std::cout << "Network: disconnected\n";
    _connected = false;
  }
  // Socket::NotReady: nothing arrived this frame, nothing to do.
}

void Network::shutdown() {
  _socket.disconnect();
  _listener.close();
  _connected = false;
  _mode = Mode::None;
}

Network::Mode Network::getMode() { return _mode; }
bool Network::isConnected() { return _connected; }

void Network::sendLocalPosition(const Vector2f& pos) {
  if (!_connected) return;
  Packet packet;
  packet << pos.x << pos.y;
  // On a non-blocking socket, send(Packet&) can return Partial if the OS
  // buffer won't take the whole thing in one go. That's not optional to
  // ignore: TCP has no message boundaries, so a half-sent packet corrupts
  // the framing for every packet after it, permanently, for the rest of
  // the connection. Must resend the SAME packet until it's actually Done.
  // Loopback basically never hits this (buffer's never full); real WiFi
  // does, which is why this only showed up machine-to-machine.
  Socket::Status status;
  do {
    status = _socket.send(packet);
  } while (status == Socket::Partial);
  if (status != Socket::Done) {
    _connected = false; // Disconnected or Error -- stop sending into it.
  }
}

bool Network::receiveRemotePosition(Vector2f& outPos) {
  if (!_hasNewPosition) return false;
  outPos = _lastReceived;
  _hasNewPosition = false;
  return true;
}
