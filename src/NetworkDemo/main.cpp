/*******
    Networking demo: host-and-join over TCP using sf::Packet (endianness
    handled for you by SFML -- no manual byte-order code needed), wired
    into the ECM the same way as the rest of the engine: a free-standing
    Network:: system owns the connection (engine/system_network.h, same
    shape as Physics::), a thin NetworkComponent per entity is what
    actually gets synced. See README.md.
*******/
#include "scenes/scene_network_demo.h"
#include <engine.h>
#include <iostream>
#include <system_network.h>

int main() {
  std::cout << "Host or Join? [h/j]: " << std::flush;
  std::string choice;
  std::getline(std::cin, choice);

  if (!choice.empty() && (choice[0] == 'h' || choice[0] == 'H')) {
    Network::startHost(54321);
  } else {
    std::cout << "Host IP [127.0.0.1]: " << std::flush;
    std::string ip;
    std::getline(std::cin, ip);
    if (ip.empty()) ip = "127.0.0.1";
    if (!Network::joinHost(ip, 54321)) {
      std::cout << "Could not connect. Exiting.\n";
      return 1;
    }
  }

  NetworkDemoScene scene;
  Engine::Start(800, 600, "NetworkDemo", &scene);
  return 0;
}
