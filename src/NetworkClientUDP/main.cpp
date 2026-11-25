/*******
    Bare console SFML networking, UDP, 2.6.x syntax: sends one datagram,
    no connection step first -- that's the whole point of UDP versus the
    TCP examples. Low-level send here (raw buffer, not sf::Packet),
    matching the original lab's style.
*******/
#include <iostream>
#include <SFML/Network.hpp>

int main() {
  std::cout << "UDP message for server: ";
  std::string msg;
  std::cin >> msg;

  sf::UdpSocket udp;
  sf::IpAddress server = "127.0.0.1";
  unsigned short port = 6000;
  if (udp.send(msg.c_str(), msg.length() + 1, server, port) != sf::Socket::Done) {
    std::cout << "Error - client sending via UDP\n";
    return -1;
  }
  std::cout << "Sent.\n";

  return 0;
}
