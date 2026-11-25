/*******
    Bare console SFML networking, UDP, 2.6.x syntax: binds a socket and
    waits for one datagram. No connection, no accept() -- UDP just binds
    to receive on a port. Low-level send/receive here (raw buffer, not
    sf::Packet) matching the original lab's style. Run this, then
    NetworkClientUDP.
*******/
#include <iostream>
#include <SFML/Network.hpp>

int main() {
  sf::UdpSocket socket;
  if (socket.bind(6000) != sf::Socket::Done) {
    std::cout << "Error - creating UDP socket\n";
    return -1;
  }

  char buffer[100] = {};
  std::size_t received;
  sf::IpAddress sender;
  unsigned short port;
  if (socket.receive(buffer, 100, received, sender, port) != sf::Socket::Done) {
    std::cout << "Error - receiving data on UDP port\n";
    return -1;
  }
  std::string msg(buffer);
  std::cout << "Received " << msg << " from " << sender << " on port "
            << port << "\n";

  return 0;
}
