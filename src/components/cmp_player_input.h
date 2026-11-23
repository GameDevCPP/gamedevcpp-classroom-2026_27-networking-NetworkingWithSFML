#pragma once
#include <ecm.h>

// Arrow-key movement, no collision -- this demo has no LevelSystem grid.
// Position-only; a sibling ShapeComponent handles the actual rendering.
class PlayerInputComponent : public Component {
public:
  explicit PlayerInputComponent(Entity* p, float speed = 150.f);
  PlayerInputComponent() = delete;

  void update(double dt) override;
  void render() override {}

private:
  float _speed;
};
