/*   
Tasks:
     1. Add the Renderer plugin
     2. Create a Palette
     3. Create a Voxel Grid
     4. Create a Point Light
     5. Create a Camera and set it in the ActiveCameras
*/


#include <cubos/engine/cubos.hpp>
#include <cubos/engine/renderer/plugin.hpp>       // 1, 5
#include <cubos/engine/voxels/grid.hpp>           // 3
#include <cubos/engine/renderer/point_light.hpp>  // 4
#include <cubos/engine/renderer/camera.hpp>       // 5

// include extra required components: Position, Rotation, LocalToWorld
#include <cubos/engine/transform/plugin.hpp>


using cubos::core::ecs::Commands;
using cubos::core::ecs::Write;

using namespace cubos::engine;


// 2
static void setPaletteSystem(Write<Renderer> renderer)
{
    // Create a palette with 1 material (Red)
    (*renderer)->setPalette(VoxelPalette{{
        {{1, 0, 0, 1}},
    }});
}

// 3
static void spawnVoxelGridSystem(Commands commands, Write<Assets> assets)
{
    // Create a 1x1x1 grid (Cube)
    auto gridAsset = assets->create(VoxelGrid{{1, 1, 1}, {1}});

    // Spawn an entity with a renderable grid component and a identity transform.
    commands.create(RenderableGrid{gridAsset, {-1.0F, 0.0F, -1.0F}}, LocalToWorld{});
}

// 4
static void spawnLightSystem(Commands commands)
{
    // Spawn a point light.
    commands.create()
        .add(PointLight{.color = {1.0F, 1.0F, 1.0F}, .intensity = 5.0F, .range = 10.0F})
        .add(Position{{1.0F, 3.0F, -2.0F}});
}

// 5
static void spawnCamerasSystem(Commands commands, Write<ActiveCameras> camera)
{
    // Spawn the a camera entity for the first viewport.
    camera->entities[0] =
        commands.create()
            .add(Camera{.fovY = 60.0F, .zNear = 0.1F, .zFar = 100.0F})
            .add(Position{{-3.0, 1.0F, -3.0F}})
            .add(Rotation{glm::quatLookAt(glm::normalize(glm::vec3{1.0F, 0.0F, 1.0F}), glm::vec3{0.0F, 1.0F, 0.0F})})
            .entity();
}

int main()
{
     Cubos cubos{};
     cubos.addPlugin(rendererPlugin);             // 1

     // 2
     cubos.startupSystem(setPaletteSystem).after("cubos.renderer.init");
     cubos.startupSystem(spawnVoxelGridSystem);   // 3
     cubos.startupSystem(spawnLightSystem);       // 4
     cubos.startupSystem(spawnCamerasSystem);     // 5

     cubos.run();
}