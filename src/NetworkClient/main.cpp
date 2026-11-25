/*******
    Bare console SFML networking, 2.6.x syntax: connects to NetworkServer,
    sends a packet, prints the reply. No ECM, no engine loop -- see
    src/NetworkDemo/ for that version.
*******/
#include <iostream>
#include <SFML/Network.hpp>

int main() {
  sf::TcpSocket socket;
  sf::Socket::Status status = socket.connect("127.0.0.1", 53000);
  if (status != sf::Socket::Done) {
    std::cout << "Error - could not connect to server\n";
    return -1;
  }
  std::cout << "Client: connected\n";

  sf::Packet out;
  sf::Uint16 x = 10;
  std::string s = "hello";
  out << x << s;
  socket.send(out);

  sf::Packet in;
  if (socket.receive(in) != sf::Socket::Done) {
    std::cout << "Client: receive failed\n";
    return -1;
  }
  sf::Uint16 rx;
  std::string rs;
  in >> rx >> rs;
  std::cout << "Client received: x=" << rx << " s=" << rs << "\n";

  return 0;
}
