#pragma once

#include "../EntityManagement/Entity.hpp"
#include "../Helpers/Vec2.hpp"
#include <bitset>
#include <iostream>

namespace CollisionHelpers {

  std::bitset<4> detectOutOfBounds(const std::shared_ptr<Entity> &entity,
                                   const Vec2                    &window_size);

  bool calculateCollisionBetweenEntities(const std::shared_ptr<Entity> &entityA,
                                         const std::shared_ptr<Entity> &entityB);

  Vec2 calculateOverlap(const std::shared_ptr<Entity> &entityA,
                        const std::shared_ptr<Entity> &entityB);

  std::bitset<4> getPositionRelativeToEntity(const std::shared_ptr<Entity> &entityA,
                                             const std::shared_ptr<Entity> &entityB);

} // namespace CollisionHelpers

namespace CollisionHelpers::MainScene {

  void enforcePlayerBounds(const std::shared_ptr<Entity> &entity,
                           const std::bitset<4>          &collides,
                           const Vec2                    &window_size);

  void enforceNonPlayerBounds(const std::shared_ptr<Entity> &entity,
                              const std::bitset<4>          &collides,
                              const Vec2                    &window_size);

  void enforceCollisionWithWall(const std::shared_ptr<Entity> &entity,
                                const std::shared_ptr<Entity> &wall);

  void enforceEntityEntityCollision(const std::shared_ptr<Entity> &entityA,
                                    const std::shared_ptr<Entity> &entityB);

  void enforceBulletCollision(const std::shared_ptr<Entity> &bullet,
                              const bool                     bulletCollides);
} // namespace CollisionHelpers::MainScene