#include "../../includes/Helpers/SpawnHelpers.hpp"
#include "../../includes/Helpers/CollisionHelpers.hpp"
#include "../../includes/Helpers/MathHelpers.hpp"

#include <iostream>

namespace SpawnHelpers {
  std::shared_ptr<Entity> spawnPlayer(SDL_Renderer  *renderer,
                                      ConfigManager &configManager,
                                      EntityManager &entityManager) {

    const PlayerConfig &playerConfig = configManager.getPlayerConfig();
    const GameConfig   &gameConfig   = configManager.getGameConfig();

    std::shared_ptr<Entity> player = entityManager.addEntity(EntityTags::Player);

    std::shared_ptr<CTransform> &playerCTransform = player->cTransform;
    std::shared_ptr<CShape>     &playerCShape     = player->cShape;
    std::shared_ptr<CInput>     &playerCInput     = player->cInput;
    std::shared_ptr<CEffects>   &playerCEffects   = player->cEffects;

    // set the player's initial position to the center of the screen
    const Vec2 &windowSize = gameConfig.windowSize;

    playerCShape = std::make_shared<CShape>(renderer, playerConfig.shape);

    const int playerHeight = playerCShape->rect.h;
    const int playerWidth  = playerCShape->rect.w;

    const Vec2 playerPosition = windowSize / 2 - Vec2(playerWidth / 2, playerHeight / 2);
    playerCTransform          = std::make_shared<CTransform>(playerPosition, Vec2(0, 0), 0);
    playerCInput              = std::make_shared<CInput>();
    playerCEffects            = std::make_shared<CEffects>();

    entityManager.update();

    return player;
  }

  void spawnEnemy(SDL_Renderer  *renderer,
                  ConfigManager &configManager,
                  std::mt19937  &randomGenerator,
                  EntityManager &entityManager) {

    const GameConfig &gameConfig = configManager.getGameConfig();
    const Vec2       &windowSize = gameConfig.windowSize;

    std::uniform_int_distribution<int> randomXPos(0, windowSize.x);
    std::uniform_int_distribution<int> randomYPos(0, windowSize.y);

    std::uniform_int_distribution<int> randomXVel(-1, 1);
    std::uniform_int_distribution<int> randomYVel(-1, 1);

    const int xPos = randomXPos(randomGenerator);
    const int yPos = randomYPos(randomGenerator);

    const int xVel = randomXVel(randomGenerator);
    const int yVel = randomYVel(randomGenerator);

    std::shared_ptr<Entity>      enemy            = entityManager.addEntity(EntityTags::Enemy);
    std::shared_ptr<CTransform> &entityCTransform = enemy->cTransform;
    std::shared_ptr<CShape>     &entityCShape     = enemy->cShape;
    std::shared_ptr<CLifespan>  &entityLifespan   = enemy->cLifespan;

    entityCTransform = std::make_shared<CTransform>(Vec2(xPos, yPos), Vec2(xVel, yVel), 0);

    const EnemyConfig &enemyConfig = configManager.getEnemyConfig();
    entityCShape                   = std::make_shared<CShape>(renderer, enemyConfig.shape);
    entityLifespan                 = std::make_shared<CLifespan>(enemyConfig.lifespan);
    bool touchesBoundary      = CollisionHelpers::detectOutOfBounds(enemy, windowSize).any();
    bool touchesOtherEntities = false;

    for (auto &entity : entityManager.getEntities()) {
      if (CollisionHelpers::calculateCollisionBetweenEntities(entity, enemy)) {
        touchesOtherEntities = true;
        break;
      }
    }

    bool isValidVelocity = xVel != 0 || yVel == 0;
    while (!isValidVelocity) {
      const int newXVel = randomXVel(randomGenerator);
      const int newYVel = randomYVel(randomGenerator);

      enemy->cTransform->velocity = Vec2(newXVel, newYVel);
      isValidVelocity             = newXVel != 0 || newYVel != 0;
    }

    bool      isValidSpawn       = !touchesBoundary && !touchesOtherEntities;
    const int MAX_SPAWN_ATTEMPTS = 10;
    int       spawnAttempt       = 1;

    while (!isValidSpawn && spawnAttempt < MAX_SPAWN_ATTEMPTS) {
      const int newX = randomXPos(randomGenerator);
      const int newY = randomYPos(randomGenerator);

      enemy->cTransform->topLeftCornerPos = Vec2(newX, newY);
      touchesBoundary      = CollisionHelpers::detectOutOfBounds(enemy, windowSize).any();
      touchesOtherEntities = false;

      for (auto &entity : entityManager.getEntities()) {
        if (CollisionHelpers::calculateCollisionBetweenEntities(entity, enemy)) {
          touchesOtherEntities = true;
          break;
        }
      }

      spawnAttempt += 1;
      isValidSpawn = !touchesBoundary && !touchesOtherEntities;
    }

    if (!isValidSpawn) {
      enemy->destroy();
    }

    entityManager.update();
  }

  void spawnSpeedBoostEntity(SDL_Renderer  *renderer,
                             ConfigManager &configManager,
                             std::mt19937  &randomGenerator,
                             EntityManager &entityManager) {
    const GameConfig                  &gameConfig = configManager.getGameConfig();
    const Vec2                        &windowSize = gameConfig.windowSize;
    std::uniform_int_distribution<int> randomXPos(0, windowSize.x);
    std::uniform_int_distribution<int> randomYPos(0, windowSize.y);

    std::uniform_int_distribution<int> randomXVel(-1, 1);
    std::uniform_int_distribution<int> randomYVel(-1, 1);

    const int xPos = randomXPos(randomGenerator);
    const int yPos = randomYPos(randomGenerator);

    const int xVel = randomXVel(randomGenerator);
    const int yVel = randomYVel(randomGenerator);

    std::shared_ptr<Entity>      speedBoost = entityManager.addEntity(EntityTags::SpeedBoost);
    std::shared_ptr<CTransform> &entityCTransform = speedBoost->cTransform;
    std::shared_ptr<CShape>     &entityCShape     = speedBoost->cShape;
    std::shared_ptr<CLifespan>  &entityLifespan   = speedBoost->cLifespan;

    const SpeedBoostEffectConfig &speedBoostEffectConfig =
        configManager.getSpeedBoostEffectConfig();

    entityCTransform = std::make_shared<CTransform>(Vec2(xPos, yPos), Vec2(xVel, yVel), 0);
    entityCShape     = std::make_shared<CShape>(renderer, speedBoostEffectConfig.shape);
    entityLifespan   = std::make_shared<CLifespan>(speedBoostEffectConfig.lifespan);

    bool touchesBoundary = CollisionHelpers::detectOutOfBounds(speedBoost, windowSize).any();
    bool touchesOtherEntities = false;

    for (auto &entity : entityManager.getEntities()) {
      if (CollisionHelpers::calculateCollisionBetweenEntities(entity, speedBoost)) {
        touchesOtherEntities = true;
        break;
      }
    }

    bool isValidVelocity = xVel != 0 || yVel != 0;

    while (!isValidVelocity) {
      const int newXVel = randomXVel(randomGenerator);
      const int newYVel = randomYVel(randomGenerator);

      speedBoost->cTransform->velocity = Vec2(newXVel, newYVel);
      isValidVelocity                  = newXVel != 0 || newYVel != 0;
    }

    bool      isValidSpawn       = !touchesBoundary && !touchesOtherEntities;
    const int MAX_SPAWN_ATTEMPTS = 10;
    int       spawnAttempt       = 1;

    while (!isValidSpawn && spawnAttempt < MAX_SPAWN_ATTEMPTS) {
      const int newXPos = randomXPos(randomGenerator);
      const int newYPos = randomYPos(randomGenerator);

      speedBoost->cTransform->topLeftCornerPos = Vec2(newXPos, newYPos);
      touchesBoundary      = CollisionHelpers::detectOutOfBounds(speedBoost, windowSize).any();
      touchesOtherEntities = false;

      for (auto &entity : entityManager.getEntities()) {
        if (CollisionHelpers::calculateCollisionBetweenEntities(entity, speedBoost)) {
          touchesOtherEntities = true;
          break;
        }
      }

      spawnAttempt += 1;
      isValidSpawn = !touchesBoundary && !touchesOtherEntities;
    }

    if (!isValidSpawn) {
      speedBoost->destroy();
    }
    entityManager.update();
  }

  void spawnSlownessEntity(SDL_Renderer        *renderer,
                           const ConfigManager &configManager,
                           std::mt19937        &randomGenerator,
                           EntityManager       &entityManager) {
    const GameConfig                  &gameConfig = configManager.getGameConfig();
    const Vec2                        &windowSize = gameConfig.windowSize;
    std::uniform_int_distribution<int> randomXPos(0, windowSize.x);
    std::uniform_int_distribution<int> randomYPos(0, windowSize.y);

    std::uniform_int_distribution<int> randomXVel(-1, 1);
    std::uniform_int_distribution<int> randomYVel(-1, 1);

    const int xPos = randomXPos(randomGenerator);
    const int yPos = randomYPos(randomGenerator);

    const int xVel = randomXVel(randomGenerator);
    const int yVel = randomYVel(randomGenerator);

    std::shared_ptr<Entity> slownessEntity =
        entityManager.addEntity(EntityTags::SlownessDebuff);
    std::shared_ptr<CTransform> &entityCTransform = slownessEntity->cTransform;
    std::shared_ptr<CShape>     &entityCShape     = slownessEntity->cShape;
    std::shared_ptr<CLifespan>  &entityLifespan   = slownessEntity->cLifespan;

    const SlownessEffectConfig &slownessEffectConfig = configManager.getSlownessEffectConfig();

    entityCTransform = std::make_shared<CTransform>(Vec2(xPos, yPos), Vec2(xVel, yVel), 0);
    entityCShape     = std::make_shared<CShape>(renderer, slownessEffectConfig.shape);
    entityLifespan   = std::make_shared<CLifespan>(slownessEffectConfig.lifespan);

    bool touchesBoundary =
        CollisionHelpers::detectOutOfBounds(slownessEntity, windowSize).any();
    bool touchesOtherEntities = false;

    for (auto &entity : entityManager.getEntities()) {
      if (CollisionHelpers::calculateCollisionBetweenEntities(entity, slownessEntity)) {
        touchesOtherEntities = true;
        break;
      }
    }

    bool      isValidSpawn       = !touchesBoundary && !touchesOtherEntities;
    bool      isValidVelocity    = xVel != 0 || yVel != 0;
    const int MAX_SPAWN_ATTEMPTS = 10;
    int       spawnAttempt       = 1;

    while (!isValidVelocity) {
      const int newXVel = randomXVel(randomGenerator);
      const int newYVel = randomYVel(randomGenerator);

      slownessEntity->cTransform->velocity = Vec2(newXVel, newYVel);
      isValidVelocity                      = newXVel != 0 || newYVel != 0;
    }

    while (!isValidSpawn && spawnAttempt < MAX_SPAWN_ATTEMPTS) {
      const int newXPos = randomXPos(randomGenerator);
      const int newYPos = randomYPos(randomGenerator);

      slownessEntity->cTransform->topLeftCornerPos = Vec2(newXPos, newYPos);
      touchesBoundary = CollisionHelpers::detectOutOfBounds(slownessEntity, windowSize).any();
      touchesOtherEntities = false;

      for (auto &entity : entityManager.getEntities()) {
        if (CollisionHelpers::calculateCollisionBetweenEntities(entity, slownessEntity)) {
          touchesOtherEntities = true;
          break;
        }
      }

      spawnAttempt += 1;
      isValidSpawn = !touchesBoundary && !touchesOtherEntities;
    }
    if (!isValidSpawn) {

      slownessEntity->destroy();
    }

    entityManager.update();
  }

  void spawnWalls(SDL_Renderer        *renderer,
                  const ConfigManager &configManager,
                  std::mt19937        &randomGenerator,
                  EntityManager       &entityManager) {

    const GameConfig &gameConfig = configManager.getGameConfig();

    // 2% of the window width
    const int wallWidth = static_cast<int>(gameConfig.windowSize.x * 0.02);
    // 60% of the window height
    const int  wallHeight = static_cast<int>(gameConfig.windowSize.y * 0.6);
    const auto wallColor  = SDL_Color{255, 255, 255, 255};

    std::shared_ptr<Entity> wallLeft = entityManager.addEntity(EntityTags::Wall);
    wallLeft->cShape =
        std::make_shared<CShape>(renderer, ShapeConfig(wallHeight, wallWidth, wallColor));
    wallLeft->cTransform = std::make_shared<CTransform>(Vec2(400, 0), Vec2(0, 0), 0);

    std::shared_ptr<Entity> wallRight = entityManager.addEntity(EntityTags::Wall);
    wallRight->cShape =
        std::make_shared<CShape>(renderer, ShapeConfig(wallHeight, wallWidth, wallColor));
    wallRight->cTransform = std::make_shared<CTransform>(
        Vec2(gameConfig.windowSize.x * 0.7, gameConfig.windowSize.y - wallHeight), Vec2(0, 0),
        0);
  }

  void spawnBullets(SDL_Renderer                  *renderer,
                    ConfigManager                 &configManager,
                    EntityManager                 &entityManager,
                    const std::shared_ptr<Entity> &player,
                    const Vec2                    &mousePosition) {
    const GameConfig &gameConfig = configManager.getGameConfig();
    const Vec2       &windowSize = gameConfig.windowSize;

    // Get player's center position
    Vec2 playerCenter = player->cTransform->topLeftCornerPos;
    playerCenter.x += player->cShape->rect.w / 2;
    playerCenter.y += player->cShape->rect.h / 2;

    // Calculate direction vector from player to mouse
    Vec2 direction;
    direction.x = mousePosition.x - playerCenter.x;
    direction.y = mousePosition.y - playerCenter.y;

    // Normalize the direction vector
    float length = MathHelpers::pythagoras(direction.x, direction.y);
    if (length > 0) {
      direction.x /= length;
      direction.y /= length;
    }

    // Set bullet speed (adjust this value as needed)
    const float bulletSpeed    = 10.0f;
    Vec2        bulletVelocity = direction * bulletSpeed;

    // Calculate bullet rotation angle in degrees
    float angle = MathHelpers::radiansToDegrees(atan2(direction.y, direction.x));

    // Create the bullet
    std::shared_ptr<Entity> bullet = entityManager.addEntity(EntityTags::Bullet);

    // Set bullet shape
    bullet->cShape =
        std::make_shared<CShape>(renderer, ShapeConfig(20, 20, SDL_Color{255, 255, 255, 255}));

    // Set bullet position slightly offset from player center in the direction of travel

    const auto  playerHalfWidth = player->cShape->rect.w / 2;
    const float spawnOffset     = (bullet->cShape->rect.w / 2) + (playerHalfWidth + 5);
    Vec2        bulletPos;
    bulletPos.x = playerCenter.x + (direction.x * spawnOffset) - bullet->cShape->rect.w / 2;
    bulletPos.y = playerCenter.y + (direction.y * spawnOffset) - bullet->cShape->rect.h / 2;

    // Set transform with position, velocity and rotation
    bullet->cTransform = std::make_shared<CTransform>(bulletPos, bulletVelocity, angle);

    // Set lifespan
    bullet->cLifespan = std::make_shared<CLifespan>(2000);
  }
} // namespace SpawnHelpers
