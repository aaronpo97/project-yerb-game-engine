#pragma once

#include "../GameEngine/GameEngine.hpp"
#include "./Scene.hpp"

class HowToPlayScene : public Scene {
public:
  HowToPlayScene(GameEngine *gameEngine);

  void update() override;
  void onEnd() override;
  void sRender() override;
  void sDoAction(Action &action) override;
  void renderText();
};
