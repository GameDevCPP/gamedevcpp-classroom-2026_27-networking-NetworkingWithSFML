#pragma once
#include <SFML/Network.hpp>
#include <SFML/System/Vector2.hpp>
#include <string>

// Minimal two-player TCP transport: one host, one joiner, each frame
// exchanging a single Vector2f. Deliberately not general-purpose --
// this exists to show the shape of a networking system in this engine
// (global connection state, same as Physics::), not to be a reusable
// networking library.
namespace Network {
enum class Mode { None, Hosting, Joined };

// Starts listening on the given port. Non-blocking: call update() each
// frame afterwards, it accepts the first client that connects.
void startHost(unsigned short port);

// Connects to a host. Blocks for up to a few seconds. Returns false (and
// resets to Mode::None) if the connection failed.
bool joinHost(const std::string& ip, unsigned short port);

void update(const double& dt);
void shutdown();

Mode getMode();
bool isConnected();

// Called once a frame by the locally-controlled entity's NetworkComponent.
void sendLocalPosition(const sf::Vector2f& pos);

// Called once a frame by the remote "ghost" entity's NetworkComponent.
// Returns true (and fills outPos) only on frames where a new position
// has actually arrived.
bool receiveRemotePosition(sf::Vector2f& outPos);
} // namespace Network
