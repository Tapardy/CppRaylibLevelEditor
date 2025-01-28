#pragma once

#include <raylib.h>
#include <vector>
#include <memory>
#include <unordered_map>
#include <typeindex>

// Forward declaration, otherwise the component it doesn't know (kinda need it cuz templates have to be here)
class GameEntity;

enum class ComponentType
{
    Transform,
    Cube,
    Sphere,
};

class Component
{
public:
    virtual ~Component() = default;
    // Pointer to the entity that owns it
    GameEntity *entity = nullptr;
    const ComponentType type;

    // Ctor to the component type cannot be changed
    Component(ComponentType componentType) : type(componentType) {}
};

// Has to be made in here, as it's a template
class GameEntity
{
public:
    GameEntity() = default;
    ~GameEntity() = default;

    // Yeah, still struggling myself here
    template <typename T, typename... Args>
    T *AddComponent(Args &&...args)
    {
        // Create a new component of type T and making it unique
        auto component = std::make_unique<T>(std::forward<Args>(args)...);

        // Can't be null, so no reference, so we make it a pointer instead of direct
        T *componentPtr = component.get();

        componentPtr->entity = this;

        // Store the uPointer in the map using the type index as the key
        components[std::type_index(typeid(T))] = std::move(component);

        return componentPtr;
    }

    template <typename T>
    T *GetComponent()
    {
        auto component = components.find(std::type_index(typeid(T)));

        if (component != components.end())
        {
            // Return as pointer of T
            return static_cast<T *>(component->second.get());
        }

        return nullptr;
    }

    template <typename T>
    T *RemoveComponent()
    {
        auto component = components.find(std::type_index(typeid(T)));

        if (component != components.end())
        {
            T *componentPtr = static_cast<T *>(component->second.get());

            componentPtr->entity = nullptr;

            components.erase(component);

            return componentPtr;
        }
        return nullptr;
    }

private:
    std::unordered_map<std::type_index, std::unique_ptr<Component>> components;
};

// TODO: probably make a component header for this
struct TransformComponent : Component
{
    TransformComponent() : Component(ComponentType::Transform) {};
    Vector3 position = {0, 0, 0};
    Vector3 rotation = {0, 0, 0};
    Vector3 scale = {1, 1, 1};
};

struct CubeComponent : Component
{
    CubeComponent() : Component(ComponentType::Cube) {}
    Vector3 size = {1, 1, 1};
    Color color = RED;
};

struct SphereComponent : Component
{
    // Need to figure out how this will be converted to the vec3
    SphereComponent() : Component(ComponentType::Sphere) {}
    float radius = 1.0f;
    Color color = RED;
};