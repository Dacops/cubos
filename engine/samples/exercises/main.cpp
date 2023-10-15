/*   
Tasks:

    Exercise 1
     1. Add the Renderer plugin
     2. Create a Palette
     3. Create a Voxel Grid
     4. Create a Point Light
     5. Create a Camera and set it in the ActiveCameras

    Exercise 2
    6-11. Set up ./assets
    12. Use ./assets
*/


#include <cubos/engine/cubos.hpp>
#include <cubos/engine/renderer/plugin.hpp>       // 1, 5
#include <cubos/engine/renderer/point_light.hpp>  // 4
#include <cubos/engine/renderer/camera.hpp>       // 5
#include <cubos/engine/settings/settings.hpp>     // 12
#include <cubos/engine/voxels/plugin.hpp>         // 12

// include extra required components: Position, Rotation, LocalToWorld
#include <cubos/engine/transform/plugin.hpp>

using cubos::core::ecs::Commands;
using cubos::core::ecs::Read;
using cubos::core::ecs::Write;

using namespace cubos::engine;

// 12
// Get assets
static const Asset<VoxelGrid> CastleAsset = AnyAsset("2d9f71ff-3c5c-4a23-a8a2-ea1110148f0b");
static const Asset<VoxelPalette> PaletteAsset = AnyAsset("2a0a78ac-8c4f-4e78-ba47-f99fb2b30cca");


// 12, Set up "assets.io.path"
static void settingsSystem(Write<Settings> settings)
{
    settings->setString("assets.io.path", SAMPLE_ASSETS_FOLDER);
}


// 2 -> 12, Load palette instead of creating one
static void setPaletteSystem(Read<Assets> assets, Write<Renderer> renderer)
{
    // Create a palette with 1 material (Red)
    auto palette = assets->read(PaletteAsset);
    (*renderer)->setPalette(*palette);
}

// 3 -> 12, Load voxel grid instead of creating one
static void spawnCastleSystem(Commands commands, Read<Assets> assets)
{
    // Calculate the necessary offset to center the model on (0, 0, 0).
    auto castle = assets->read(CastleAsset);
    glm::vec3 offset = glm::vec3(castle->size().x, 0.0F, castle->size().z) / -2.0F;

    // Create the car entity
    commands.create().add(RenderableGrid{CastleAsset, offset}).add(LocalToWorld{});
}

// 4
static void spawnLightSystem(Commands commands)
{
    // Spawn a point light.
    commands.create()
        .add(PointLight{.color = {1.0F, 1.0F, 1.0F}, .intensity = 5.0F, .range = 100.0F})
        .add(Position{{10.0F, 30.0F, -20.0F}});
}

// 5
static void spawnCamerasSystem(Commands commands, Write<ActiveCameras> camera)
{
    // Spawn the a camera entity for the first viewport.
    camera->entities[0] =
        commands.create()
            .add(Camera{.fovY = 60.0F, .zNear = 0.1F, .zFar = 100.0F})
            .add(Position{{-20.0, 10.0F, -30.0F}})
            .add(Rotation{glm::quatLookAt(glm::normalize(glm::vec3{1.0F, 0.0F, 1.0F}), glm::vec3{0.0F, 1.0F, 0.0F})})
            .entity();
}

int main()
{
     Cubos cubos{};
     cubos.addPlugin(rendererPlugin);             // 1
     cubos.addPlugin(voxelsPlugin);               // 12

     // 12
     cubos.startupSystem(settingsSystem).tagged("cubos.settings");
     cubos.startupSystem(setPaletteSystem).after("cubos.renderer.init");
     cubos.startupSystem(spawnLightSystem);       // 4
     cubos.startupSystem(spawnCamerasSystem);     // 5

     cubos.system(spawnCastleSystem);             // 12

     cubos.run();
}