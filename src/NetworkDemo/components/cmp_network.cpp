#include "cmp_network.h"
#include <system_network.h>

NetworkComponent::NetworkComponent(Entity* p, Role role)
    : Component(p), _role(role) {}

void NetworkComponent::update(double) {
  if (_role == Role::Local) {
    Network::sendLocalPosition(_parent->getPosition());
  } else {
    sf::Vector2f pos;
    if (Network::receiveRemotePosition(pos)) {
      _parent->setPosition(pos);
    }
  }
}
