#include <cubos/core/ecs/component/manager.hpp>
#include <cubos/core/ecs/component/registry.hpp>
#include <cubos/core/reflection/type.hpp>

using namespace cubos::core;
using namespace cubos::core::ecs;

std::optional<std::string_view> cubos::core::ecs::getComponentName(const reflection::Type& type)
{
    return Registry::name(type);
}

void ComponentManager::registerComponent(const reflection::Type& type)
{
    if (!mTypeToIds.contains(type))
    {
        auto storage = Registry::createStorage(type);
        CUBOS_ASSERT(storage != nullptr, "Component type '{}' is not registered in the global registry", type.name());

        mTypeToIds.insert(type, mEntries.size() + 1); // Component ids start at 1.
        mEntries.emplace_back(std::move(storage));
    }
}

std::size_t ComponentManager::getID(const reflection::Type& type) const
{
    CUBOS_ASSERT(mTypeToIds.contains(type), "Component type '{}' is not registered in the component manager",
                 type.name());
    return mTypeToIds.at(type);
}

const reflection::Type& ComponentManager::getType(std::size_t id) const
{
    for (const auto& pair : mTypeToIds)
    {
        if (pair.second == id)
        {
            return *pair.first;
        }
    }

    CUBOS_FAIL("No component found with ID {}", id);
}

void ComponentManager::remove(uint32_t id, std::size_t componentId)
{
    mEntries[componentId - 1].storage->erase(id);
}

void ComponentManager::removeAll(uint32_t id)
{
    for (auto& entry : mEntries)
    {
        entry.storage->erase(id);
    }
}

ComponentManager::Entry::Entry(std::unique_ptr<IStorage> storage)
    : storage(std::move(storage))
{
    this->mutex = std::make_unique<std::shared_mutex>();
}

data::old::Package ComponentManager::pack(uint32_t id, std::size_t componentId, data::old::Context* context) const
{
    return mEntries[componentId - 1].storage->pack(id, context);
}

bool ComponentManager::unpack(uint32_t id, std::size_t componentId, const data::old::Package& package,
                              data::old::Context* context)
{
    return mEntries[componentId - 1].storage->unpack(id, package, context);
}
