#pragma once

#include <array>
#include <bitset>
#include <cassert>
#include <cstdint>
#include <memory>
#include <queue>
#include <set>
#include <unordered_map>

namespace RPG
{
  // https://austinmorlan.com/posts/entity_component_system/
  class EntityComponentSystem
  {
  public:
    // ID of an entity
    using Entity = std::uint16_t;

    // Max number of entity
    static const Entity MaxEntities = 2048;

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
      std::set<Entity>  entities; // Entities in the system
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

      virtual void destroy(Entity entity) = 0; // Remove entity data from array
    };

    // Store data of a component type
    template<typename Component>
    class ComponentArray : public IComponentArray
    {
    private:
      std::array<Component, MaxEntities>      _data;          // Packet array of component data
      std::unordered_map<Entity, std::size_t> _entityToIndex; // Map from an entity ID to a data index
      std::unordered_map<std::size_t, Entity> _indexToEntity; // Map from a data index to an entity ID

    public:
      ComponentArray() = default;
      ComponentArray(const ComponentArray<Component>&) = delete;
      ComponentArray(ComponentArray<Component>&&) = delete;
      ~ComponentArray() = default;

      ComponentArray& operator=(const ComponentArray<Component>&) = delete;
      ComponentArray& operator=(ComponentArray<Component>&&) = delete;

      void add(Entity entity, Component&& component = Component()) // Add component to entity
      {
        assert(_entityToIndex.find(entity) == _entityToIndex.end() && "Component added to same entity more than once.");

        // Get index of entity's new component
        std::size_t index = _entityToIndex.size();

        // Register new component
        _entityToIndex[entity] = index;
        _indexToEntity[index] = entity;
        _data[index] = std::move(component);
      }

      void add(Entity entity, const Component& component) // Add component to entity
      {
        assert(_entityToIndex.find(entity) == _entityToIndex.end() && "Component added to same entity more than once.");

        // Get index of entity's new component
        std::size_t index = _entityToIndex.size();

        // Register new component
        _entityToIndex[entity] = index;
        _indexToEntity[index] = entity;
        _data[index] = component;
      }

      void remove(Entity entity)
      {
        assert(_entityToIndex.find(entity) != _entityToIndex.end() && "Removing non-existent component.");

        auto removedIndex = _entityToIndex[entity];
        auto movedIndex = _entityToIndex.size() - 1;
        auto movedEntity = _indexToEntity[movedIndex];

        // Move last data to deleted index
        _data[removedIndex] = std::move(_data[movedIndex]);

        // Update entity/index maps
        _entityToIndex[movedEntity] = removedIndex;
        _indexToEntity[removedIndex] = movedEntity;

        // Remove deleted entity/index from entity/index maps
        _entityToIndex.erase(entity);
        _indexToEntity.erase(movedIndex);
      }

      Component& get(Entity entity)
      {
        assert(_entityToIndex.find(entity) == _entityToIndex.end() && "Component non register in entity.");

        // Get component data of entity
        return _data[_entityToIndex[entity]];
      }

      void destroy(Entity entity) override // Remove entity data from array
      {
        // Remove only registered entities
        if (_entityToIndex.find(entity) != _entityToIndex.end())
          remove(entity);
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

      void destroy(Entity entity)
      {
        // Remove entity from each component array
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
      ~SystemManager() = default;

      SystemManager& operator=(const SystemManager&) = delete;
      SystemManager& operator=(SystemManager&&) = delete;

      template<typename NewSystem>
      NewSystem&  add(Signature signature) // Add a new system to manager
      {
        const char* name = typeid(NewSystem).name();

        assert(_systems.find(name) == _systems.end() && "Registering system more than once.");

        auto  system = std::make_unique<NewSystem>();
        auto& ref = *system->get();

        // Add new system and its signature to manager
        _systems.insert({ name, { std::move(system), signature }});

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

    template<typename NewSystem>
    NewSystem& addSystem(Signature signature)  // Add a new system to ECS
    {
      // Add new system
      return _systems.add<NewSystem>(signature);
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