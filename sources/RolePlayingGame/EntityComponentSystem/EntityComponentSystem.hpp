#pragma once

#include <array>
#include <bitset>
#include <cassert>
#include <cstdint>
#include <limits>
#include <memory>
#include <queue>
#include <set>
#include <unordered_map>

namespace RPG
{
  // https://austinmorlan.com/posts/entity_component_system/
  // Removed data packing of ComponentArray for performance
  class EntityComponentSystem
  {
  public:
    // ID of an entity
    using Entity = std::uint16_t;

    // Max number of entity
    static const Entity MaxEntities = std::numeric_limits<Entity>::max();

    // ID of an invalid entity
    static const Entity InvalidEntity = MaxEntities;

    // ID of a component type
    using ComponentType = std::uint8_t;

    // Max number of component type
    static const ComponentType MaxComponents = 32;

    // Signature of component's types
    using Signature = std::bitset<MaxComponents>;

    // Base class of a system
    class System
    {
    public:
      RPG::EntityComponentSystem& ecs;      // Reference to ECS
      std::set<Entity>            entities; // Entities in the system

      System() = delete;
      System(RPG::EntityComponentSystem& ecs) : ecs(ecs), entities() {}
      System(const System&) = delete;
      System(System&&) = delete;
      virtual ~System() = default;

      System& operator=(const System&) = delete;
      System& operator=(System&&) = delete;
    };

  private:
    // Distribute entity IDs and manage signatures
    class EntityManager
    {
    private:
      std::queue<Entity>                  _availableEntities; // Queue of unused entity IDs
      std::array<Signature, MaxEntities>  _signatures;        // Entities signatures

    public:
      EntityManager()
      {
        // Add every possible entity ID
        for (Entity entity = 0; entity < MaxEntities; entity++)
          _availableEntities.push(entity);
      }

      EntityManager(const EntityManager&) = delete;
      EntityManager(EntityManager&&) = delete;
      ~EntityManager() = default;

      EntityManager& operator=(const EntityManager&) = default;
      EntityManager& operator=(EntityManager&&) = default;

      Entity create()
      {
        assert(_availableEntities.empty() == false && "Too many entities.");

        // Get first available ID
        Entity  entity = _availableEntities.front();

        // Remove ID from queue
        _availableEntities.pop();

        //printf("%lu\n", _availableEntities.size());

        return entity;
      }

      void destroy(Entity entity)
      {
        assert(entity < MaxEntities && "Entity out of range.");

        // Reset destroyed entity's signature
        _signatures[entity].reset();

        // Put the destroyed entity's ID in the queue
        _availableEntities.push(entity);
      }

      void set(Entity entity, Signature signature)
      {
        assert(entity < MaxEntities && "Entity out of range.");

        // Register new entity's signature
        _signatures[entity] = signature;
      }

      Signature get(Entity entity)
      {
        assert(entity < MaxEntities && "Entity out of range.");

        // Get entity's signature
        return _signatures[entity];
      }
    };

    // Interface for components array
    class IComponentArray
    {
    public:
      virtual ~IComponentArray() = default;

      virtual void  destroy(Entity entity) = 0; // Destroy entity
    };

    // Store data of a component type
    template<typename Component>
    class ComponentArray : public IComponentArray
    {
    private:
#ifndef NDEBUG
      std::set<Entity>  _entities;  // Entity with data registered
#endif
      std::array<Component, MaxEntities>      _data;          // Packed array of component data

    public:
      ComponentArray() = default;
      ComponentArray(const ComponentArray<Component>&) = delete;
      ComponentArray(ComponentArray<Component>&&) = delete;
      ~ComponentArray() = default;

      ComponentArray& operator=(const ComponentArray<Component>&) = delete;
      ComponentArray& operator=(ComponentArray<Component>&&) = delete;

      void add(Entity entity, Component&& component = Component()) // Add component to entity
      {
#ifndef NDEBUG
        assert(_entities.contains(entity) == false && "Entity already registered.");
        _entities.insert(entity);
#endif

        // Move data to array
        _data[entity] = std::move(component);
      }

      void add(Entity entity, const Component& component) // Add component to entity
      {
#ifndef NDEBUG
        assert(_entities.contains(entity) == false && "Entity already registered.");
        _entities.insert(entity);
#endif

        // Copy data to array
        _data[entity] = component;
      }

      void remove(Entity entity)
      {
#ifndef NDEBUG
        assert(_entities.contains(entity) == true && "Entity not registered");
        _entities.erase(entity);
#endif
      }

      Component& get(Entity entity)
      {
#ifndef NDEBUG
        assert(_entities.contains(entity) == true && "Entity not registered");
#endif

        // Get component data of entity
        return _data[entity];
      }

      void  destroy(Entity entity)
      {
#ifndef NDEBUG
        _entities.erase(entity);
#endif
      }
    };

    // Handle component arrays
    class ComponentManager
    {
    private:
      std::unordered_map<const char*, ComponentType>                    _types;  // Type string pointer to component type
      std::unordered_map<const char*, std::unique_ptr<IComponentArray>> _arrays; // Type string pointer to component array

    public:
      ComponentManager() = default;
      ComponentManager(const ComponentManager&) = delete;
      ComponentManager(ComponentManager&&) = delete;
      ~ComponentManager() = default;

      ComponentManager& operator=(const ComponentManager&) = delete;
      ComponentManager& operator=(ComponentManager&&) = delete;

      template<typename Component>
      void add() // Add new component type
      {
        const char* name = typeid(Component).name();

        assert(_types.find(name) == _types.end() && "Registering component type more than once.");

        // Add component type to component type map
        _types.insert({ name, static_cast<ComponentType>(_types.size()) });

        // Add type to component arrays map
        _arrays.insert({ name, std::make_unique<ComponentArray<Component>>() });
      }

      template<typename Component>
      ComponentType type() // Get component type
      {
        const char* name = typeid(Component).name();

        assert(_types.find(name) != _types.end() && "Component not registered before use.");

        // Return component's type
        return _types[name];
      }

      template<typename Component>
      void add(Entity entity, const Component& component) // Add component to entity
      {
        // Add entity to component's array
        get<Component>().add(entity, component);
      }

      template<typename Component>
      void add(Entity entity, Component&& component = Component()) // Add component to entity
      {
        // Add entity to component's array
        get<Component>().add(entity, std::move(component));
      }

      template<typename Component>
      void remove(Entity entity) // Remove component of entity
      {
        get<Component>().remove(entity);
      }

      template<typename Component>
      Component& get(Entity entity) // Get component of entity
      {
        return get<Component>().get(entity);
      }

      template<typename Component>
      ComponentArray<Component>& get() // Get component array
      {
        const char* name = typeid(Component).name();

        assert(_types.find(name) != _types.end() && "Component not registered before use.");

        return *static_cast<ComponentArray<Component>*>(_arrays[name].get());
      }

      void  destroy(Entity entity)
      {
        // Remove entity from every component array
        for (auto& [_, array] : _arrays)
          array->destroy(entity);
      }
    };

    // Handle ECS systems
    class SystemManager
    {
    private:
      struct Pair
      {
        std::unique_ptr<System> system;
        Signature               signature;
      };

      std::unordered_map<const char*, Pair> _systems; // Registered systems and their signatures

    public:
      SystemManager() = default;
      SystemManager(const SystemManager&) = delete;
      SystemManager(SystemManager&&) = delete;
      ~SystemManager()
      {
        // A system should not access itself while detroying
        for (auto iterator = _systems.begin(); iterator != _systems.end();) {
          auto system = std::move(iterator->second.system);
          iterator = _systems.erase(iterator);
        }
      }

      SystemManager& operator=(const SystemManager&) = delete;
      SystemManager& operator=(SystemManager&&) = delete;

      template<typename NewSystem, typename ...Args>
      NewSystem&  add(Signature signature, EntityComponentSystem& ecs, Args&& ...args) // Add a new system to manager
      {
        const char* name = typeid(NewSystem).name();

        assert(_systems.find(name) == _systems.end() && "Registering system more than once.");

        auto  system = std::make_unique<NewSystem>(ecs, std::forward<Args>(args)...);
        auto& ref = *system;

        // Add new system and its signature to manager
        _systems.insert({ name, Pair{ .system = std::move(system), .signature = signature }});

        // Return reference of new system
        return ref;
      }

      template<typename GetSystem>
      GetSystem& get() // Get system from manager
      {
        const char* name = typeid(GetSystem).name();

        assert(_systems.find(name) != _systems.end() && "System not registered.");

        // Get system from map
        return *static_cast<GetSystem*>(_systems[name].system.get());
      }

      void  update(Entity entity, Signature signature)  // Update entity signature in systems
      {
        // Update entity signature in each system
        for (auto& [_, pair] : _systems)
        {
          if ((signature & pair.signature) == pair.signature)
            pair.system->entities.insert(entity);
          else
            pair.system->entities.erase(entity);
        }
      }

      void  destroy(Entity entity)  // Remove entity from systems
      {
        // Remove entity from each system
        for (auto& [_, pair] : _systems)
          pair.system->entities.erase(entity);
      }
    };

    EntityManager    _entities;   // Handle entities
    ComponentManager _components; // Handle entities' components
    SystemManager    _systems;    // Handle systems
    
  public:
    EntityComponentSystem() = default;
    EntityComponentSystem(const EntityComponentSystem&) = default;
    EntityComponentSystem(EntityComponentSystem&&) = default;
    ~EntityComponentSystem() = default;

    EntityComponentSystem& operator=(const EntityComponentSystem&) = delete;
    EntityComponentSystem& operator=(EntityComponentSystem&&) = delete;

    Entity createEntity() // Create a new entity
    {
      // Generate a new entity
      return _entities.create();
    }

    Signature signatureEntity(Entity entity) // Get entity signature
    {
      // Return entity signature
      return _entities.get(entity);
    }

    void  destroyEntity(Entity entity) // Remove an entity
    {
      // Remove entity from each manager
      _entities.destroy(entity);
      _components.destroy(entity);
      _systems.destroy(entity);
    }

    template<typename Component>
    void addComponent()  // Add new component type
    {
      _components.add<Component>();
    }

    template<typename Component>
    void addComponent(Entity entity, Component&& component = Component())
    {
      // Add component to entity
      _components.add(entity, std::move(component));

      // Compute new entity signature
      auto signature = _entities.get(entity);
      signature.set(_components.type<Component>(), true);

      // Update entity's signature
      _entities.set(entity, signature);
      _systems.update(entity, signature);
    }

    template<typename Component>
    void addComponent(Entity entity, const Component& component)
    {
      // Add component to entity
      _components.add(entity, component);

      // Compute new entity signature
      auto signature = _entities.get(entity);
      signature.set(_components.type<Component>(), true);

      // Update entity's signature
      _entities.set(entity, signature);
      _systems.update(entity, signature);
    }

    template<typename Component>
    void removeComponent(Entity entity)
    {
      // Remove component from entity
      _components.remove<Component>(entity);

      // Compute new entity signature
      auto signature = _entities.get(entity);
      signature.set(_components.type<Component>(), false);

      // Update entity's signature
      _entities.set(entity, signature);
      _systems.update(entity, signature);
    }

    template<typename Component>
    Component&  getComponent(Entity entity)
    {
      // Get component of entity
      return _components.get<Component>(entity);
    }

    template<typename Component>
    ComponentType typeComponent()
    {
      // Get component type
      return _components.type<Component>();
    }

    template<typename NewSystem, typename ...Args>
    NewSystem& addSystem(Signature signature, Args&& ...args)  // Add a new system to ECS
    {
      // Add new system
      return _systems.add<NewSystem>(signature, *this, std::forward<Args>(args)...);
    }

    template<typename GetSystem>
    GetSystem& getSystem()  // Add a new system to ECS
    {
      // Get system
      return _systems.get<GetSystem>();
    }
  };

  using ECS = EntityComponentSystem;
}