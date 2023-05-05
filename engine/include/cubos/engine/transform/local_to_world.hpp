#pragma once

#include <glm/glm.hpp>

namespace cubos::engine
{
    /// A matrix calculated from the position, rotation and scale of an entity.
    /// This matrix should
    struct [[cubos::component("cubos/local_to_world", VecStorage)]] LocalToWorld
    {
        glm::mat4 mat = glm::mat4(1.0F); ///< The local to world matrix.
    };

} // namespace cubos::engine