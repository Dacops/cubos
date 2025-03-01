#include <vector>

#include <cubos/core/data/fs/file_system.hpp>

#include <cubos/engine/assets/bridges/json.hpp>
#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/settings/settings.hpp>

using cubos::core::data::FileSystem;
using cubos::core::data::old::Deserializer;
using cubos::core::data::old::Serializer;
using cubos::core::ecs::Read;
using cubos::core::ecs::Write;
using cubos::core::memory::Stream;

using namespace cubos::engine;

/// [Asset type]
/// A simple serializable type which we will be saving and loading.
struct Strings
{
    std::vector<std::string> strings;
};
/// [Asset type]

/// [Serialization definition]
template <>
void cubos::core::data::old::serialize<Strings>(Serializer& ser, const Strings& obj, const char* name)
{
    ser.beginObject(name);
    ser.write(obj.strings, "strings");
    ser.endObject();
}

template <>
void cubos::core::data::old::deserialize<Strings>(Deserializer& des, Strings& obj)
{
    des.beginObject();
    des.read(obj.strings);
    des.endObject();
}
/// [Serialization definition]

static void configSystem(Write<Settings> settings)
{
    settings->setString("assets.io.path", SAMPLE_ASSETS_FOLDER);
}

/// [Register bridge]
static void bridgeSystem(Write<Assets> assets)
{
    assets->registerBridge(".strings", std::make_unique<JSONBridge<Strings>>());
}
/// [Register bridge]

/// [Loading the asset]
static const Asset<Strings> SampleAsset = AnyAsset("6f42ae5a-59d1-5df3-8720-83b8df6dd536");

static void loadSystem(Write<Assets> assets)
{
    auto read = assets->read(SampleAsset);
    for (const auto& str : read->strings)
    {
        Stream::stdOut.printf("String: {}\n", str);
    }
}
/// [Loading the asset]

int main()
{
    Cubos cubos{};

    /// [Configuration]
    cubos.addPlugin(assetsPlugin);
    cubos.startupSystem(bridgeSystem).tagged("cubos.assets.bridge");
    cubos.startupSystem(loadSystem).tagged("cubos.assets");
    /// [Configuration]

    cubos.startupSystem(configSystem).tagged("cubos.settings");
    cubos.run();
}
