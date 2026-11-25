#include "cmp_player_input.h"
#include <SFML/Window/Keyboard.hpp>
#include <maths.h>

using namespace sf;

PlayerInputComponent::PlayerInputComponent(Entity* p, float speed)
    : Component(p), _speed(speed) {}

void PlayerInputComponent::update(double dt) {
  Vector2f dir(0.f, 0.f);
  if (Keyboard::isKeyPressed(Keyboard::Left)) dir.x -= 1.f;
  if (Keyboard::isKeyPressed(Keyboard::Right)) dir.x += 1.f;
  if (Keyboard::isKeyPressed(Keyboard::Up)) dir.y -= 1.f;
  if (Keyboard::isKeyPressed(Keyboard::Down)) dir.y += 1.f;

  if (dir.x != 0.f || dir.y != 0.f) {
    _parent->setPosition(_parent->getPosition() +
                          normalize(dir) * _speed * (float)dt);
  }
}
