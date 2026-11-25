/*******
    Bare console SFML networking, 2.6.x syntax: a listener that accepts
    one client, receives a packet, replies with one. No ECM, no engine
    loop -- see src/NetworkDemo/ for that version. Run this, then
    NetworkClient, to see them talk.
*******/
#include <iostream>
#include <SFML/Network.hpp>

int main() {
  sf::TcpListener listener;
  if (listener.listen(53000) != sf::Socket::Done) {
    std::cout << "Server could not open socket\n";
    return -1;
  }

  sf::TcpSocket client;
  if (listener.accept(client) != sf::Socket::Done) {
    std::cout << "Server could not accept connection\n";
    return -1;
  }
  std::cout << "Server: client connected\n";

  sf::Packet in;
  if (client.receive(in) != sf::Socket::Done) {
    std::cout << "Server: receive failed\n";
    return -1;
  }
  sf::Uint16 x;
  std::string s;
  in >> x >> s;
  std::cout << "Server received: x=" << x << " s=" << s << "\n";

  sf::Packet out;
  out << sf::Uint16(99) << std::string("hello from server");
  client.send(out);

  return 0;
}
