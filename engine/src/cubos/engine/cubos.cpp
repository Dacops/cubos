#include <utility>

#include <cubos/core/ecs/system/commands.hpp>
#include <cubos/core/log.hpp>

#include <cubos/engine/cubos.hpp>

using namespace cubos::engine;

DeltaTime::DeltaTime(float value)
    : value(value)
{
}

ShouldQuit::ShouldQuit(bool value)
    : value(value)
{
}

Arguments::Arguments(std::vector<std::string> value)
    : value(std::move(value))
{
}

TagBuilder::TagBuilder(core::ecs::Dispatcher& dispatcher, std::vector<std::string>& tags)
    : mDispatcher(dispatcher)
    , mTags(tags)
{
}

TagBuilder& TagBuilder::before(const std::string& tag)
{
    mTags.push_back(tag);
    mDispatcher.tagSetBeforeTag(tag);
    return *this;
}

TagBuilder& TagBuilder::after(const std::string& tag)
{
    mTags.push_back(tag);
    mDispatcher.tagSetAfterTag(tag);
    return *this;
}

SystemBuilder::SystemBuilder(core::ecs::Dispatcher& dispatcher, std::vector<std::string>& tags)
    : mDispatcher(dispatcher)
    , mTags(tags)
{
}

SystemBuilder& SystemBuilder::tagged(const std::string& tag)
{
    if (std::find(mTags.begin(), mTags.end(), tag) != mTags.end())
    {
        CUBOS_WARN("Tag '{}' was defined on opposite system type (normal/startup), possible tag/startupTag mismatch? ",
                   tag);
    }
    mDispatcher.systemAddTag(tag);
    return *this;
}

SystemBuilder& SystemBuilder::before(const std::string& tag)
{
    if (std::find(mTags.begin(), mTags.end(), tag) != mTags.end())
    {
        CUBOS_WARN("Tag '{}' was defined on opposite system type (normal/startup), possible tag/startupTag mismatch? ",
                   tag);
    }
    mDispatcher.systemSetBeforeTag(tag);
    return *this;
}

SystemBuilder& SystemBuilder::after(const std::string& tag)
{
    if (std::find(mTags.begin(), mTags.end(), tag) != mTags.end())
    {
        CUBOS_WARN("Tag '{}' was defined on opposite system type (normal/startup), possible tag/startupTag mismatch? ",
                   tag);
    }
    mDispatcher.systemSetAfterTag(tag);
    return *this;
}

Cubos& Cubos::addPlugin(void (*func)(Cubos&))
{
    if (!mPlugins.contains(func))
    {
        func(*this);
        mPlugins.insert(func);
    }
    else
    {
        CUBOS_TRACE("Plugin was already registered!");
    }
    return *this;
}

TagBuilder Cubos::tag(const std::string& tag)
{
    mMainDispatcher.addTag(tag);
    TagBuilder builder(mMainDispatcher, mMainTags);

    return builder;
}

TagBuilder Cubos::startupTag(const std::string& tag)
{
    mStartupDispatcher.addTag(tag);
    TagBuilder builder(mStartupDispatcher, mStartupTags);

    return builder;
}

Cubos::Cubos()
    : Cubos(1, nullptr)
{
}

Cubos::Cubos(int argc, char** argv)
{
    std::vector<std::string> arguments(argv + 1, argv + argc);

    core::initializeLogger();

    this->addResource<DeltaTime>(0.0F);
    this->addResource<ShouldQuit>(true);
    this->addResource<Arguments>(arguments);
}

void Cubos::run()
{
    mPlugins.clear();
    mMainTags.clear();
    mStartupTags.clear();

    // Compile execution chain
    mStartupDispatcher.compileChain();
    mMainDispatcher.compileChain();

    cubos::core::ecs::CommandBuffer cmds(mWorld);

    mStartupDispatcher.callSystems(mWorld, cmds);

    auto currentTime = std::chrono::steady_clock::now();
    auto previousTime = std::chrono::steady_clock::now();
    do
    {
        mMainDispatcher.callSystems(mWorld, cmds);
        currentTime = std::chrono::steady_clock::now();
        mWorld.write<DeltaTime>().get().value = std::chrono::duration<float>(currentTime - previousTime).count();
        previousTime = currentTime;
    } while (!mWorld.read<ShouldQuit>().get().value);
}
