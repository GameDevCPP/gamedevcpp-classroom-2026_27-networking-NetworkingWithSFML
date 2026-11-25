#include "scene_network_demo.h"
#include "../components/cmp_network.h"
#include "../components/cmp_player_input.h"
#include "../components/cmp_sprite.h"
#include <SFML/Graphics/CircleShape.hpp>
#include <system_network.h>

using namespace sf;

namespace {
void styleCircle(ShapeComponent& shape, Color fill) {
  shape.setShape<CircleShape>(16.f);
  shape.getShape().setOrigin(16.f, 16.f);
  shape.getShape().setFillColor(fill);
  shape.getShape().setOutlineColor(Color::Black);
  shape.getShape().setOutlineThickness(2.f);
}
} // namespace

void NetworkDemoScene::Load() {
  // You: arrow keys move this one, and it's what gets sent out.
  auto local = makeEntity();
  local->setPosition(Vector2f(200.f, 300.f));
  styleCircle(*local->addComponent<ShapeComponent>(), Color(70, 200, 220));
  local->addComponent<PlayerInputComponent>();
  local->addComponent<NetworkComponent>(NetworkComponent::Role::Local);

  // The other player -- purely network-driven, never moved locally.
  auto remote = makeEntity();
  remote->setPosition(Vector2f(400.f, 300.f));
  styleCircle(*remote->addComponent<ShapeComponent>(), Color(230, 140, 40));
  remote->addComponent<NetworkComponent>(NetworkComponent::Role::Remote);

  setLoaded(true);
}

void NetworkDemoScene::UnLoad() {
  Network::shutdown();
  Scene::UnLoad();
}

void NetworkDemoScene::Update(const double& dt) {
  Network::update(dt);
  Scene::Update(dt);
}

void NetworkDemoScene::Render() { Scene::Render(); }
