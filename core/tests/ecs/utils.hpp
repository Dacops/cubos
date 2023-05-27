/// @file
/// Contains utilities used by ECS-related tests which reduce the boilerplate.

#pragma once

#include <cubos/core/ecs/world.hpp>

#include "../utils.hpp"

/// A component which stores a single integer.
struct [[cubos::component("integer")]] IntegerComponent
{
    int value;
};

/// A component which references another entity.
struct [[cubos::component("parent")]] ParentComponent
{
    cubos::core::ecs::Entity id;
};

/// Adds the utility components to a world.
inline void setupWorld(cubos::core::ecs::World& world)
{
    world.registerComponent<IntegerComponent>();
    world.registerComponent<ParentComponent>();
}