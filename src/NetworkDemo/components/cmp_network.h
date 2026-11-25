#pragma once
#include <ecm.h>

// Attach to an entity to replicate its position over the network.
// Local: sends this entity's position each frame (you control it).
// Remote: receives the other side's position each frame and applies it
// (a "ghost" entity -- no local input, purely network-driven).
class NetworkComponent : public Component {
public:
  enum class Role { Local, Remote };

  NetworkComponent(Entity* p, Role role);
  NetworkComponent() = delete;

  void update(double dt) override;
  void render() override {}

private:
  Role _role;
};
