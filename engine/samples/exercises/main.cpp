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

    Revert to Exercise1 main.cpp 
    Exercise 3
    13. Add the input plugin
    14-15. Create Input files in ./assets
    16. Do something with the input (Show cube by pressing "s")

    Exercise 4
    17. Add the Scene plugin
    18-19. Create a Scene file in ./assets
    20. Create a Scene
*/


#include <cubos/engine/cubos.hpp>
#include <cubos/engine/renderer/plugin.hpp>       // 1, 5
#include <cubos/engine/voxels/grid.hpp>           // 3
#include <cubos/engine/renderer/point_light.hpp>  // 4
#include <cubos/engine/renderer/camera.hpp>       // 5
#include <cubos/engine/input/plugin.hpp>          // 13
#include <cubos/engine/settings/plugin.hpp>       // 16
#include <cubos/engine/scene/plugin.hpp>          // 17


// include extra required components: Position, Rotation, LocalToWorld
#include <cubos/engine/transform/plugin.hpp>

using cubos::core::ecs::Commands;
using cubos::core::ecs::Read;
using cubos::core::ecs::Write;
using cubos::core::ecs::Query;

using namespace cubos::engine;

static const Asset<InputBindings> BindingsAsset = AnyAsset("7f36cf98-0f5e-4bf9-9325-066ea9550ebd");
static const Asset<Scene> SceneAsset = AnyAsset("5d3269ea-29ad-4f07-ada6-6d29c21568cb");

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
    auto entity = commands.create(RenderableGrid{gridAsset, {-1.0F, 0.0F, -1.0F}}, LocalToWorld{});
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

// 16
// Load bindings
static void config(Write<Settings> settings)
{
    settings->setString("assets.io.path", SAMPLE_ASSETS_FOLDER);
}
static void init(Read<Assets> assets, Write<Input> input)
{
    auto bindings = assets->read<InputBindings>(BindingsAsset);
    input->bind(*bindings);
}

// 16
void showCube(Commands commands, Query<Read<RenderableGrid>> entities, 
                Write<Assets> assets, Read<Input> input)
{
    // Destroy cube
    for (auto [entity, position] : entities)
    {
        commands.destroy(entity);
    }
    // If key is pressed, spawn a new cube ("show" cube)
    if (input->pressed("show-cube")) {
        auto gridAsset = assets->create(VoxelGrid{{1, 1, 1}, {1}});
        commands.create(RenderableGrid{gridAsset, {-1.0F, 0.0F, -1.0F}}, LocalToWorld{});
    }
}

// 20
static void spawnScene(Commands commands, Read<Assets> assets)
{
    auto sceneRead = assets->read(SceneAsset);
    commands.spawn(sceneRead->blueprint);
}


int main()
{
     Cubos cubos{};
     cubos.addPlugin(rendererPlugin);                   // 1
     cubos.addPlugin(inputPlugin);                      // 12
     cubos.addPlugin(scenePlugin);                      // 17

     // 2
     cubos.startupSystem(setPaletteSystem).after("cubos.renderer.init");
     cubos.startupSystem(spawnVoxelGridSystem);         // 3
     cubos.startupSystem(spawnLightSystem);             // 4
     cubos.startupSystem(spawnCamerasSystem);           // 5
     cubos.startupSystem(config).tagged("cubos.settings");  // 16
     cubos.startupSystem(init).tagged("cubos.assets");  // 16
     // 20
     cubos.startupSystem(spawnScene).tagged("spawn").tagged("cubos.assets");

     cubos.system(showCube);                            // 16

     cubos.run();
}