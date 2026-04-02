#pragma once

/**
 * @file EntityComponentSystem.hpp
 * @brief Entity Component System (ECS) implementation for the RPG module.
 *
 * Implements a data-driven Entity Component System architecture based on
 * https://austinmorlan.com/posts/entity_component_system/.
 * Data packing in @c ComponentArray has been intentionally removed for performance.
 *
 * The ECS is composed of three internal managers:
 * - @c EntityManager: Distributes entity IDs and tracks component signatures.
 * - @c ComponentManager: Registers component types and stores component data per entity.
 * - @c SystemManager: Registers systems and keeps their entity sets up to date.
 */

#include <array>
#include <bitset>
#include <cassert>
#include <cstdint>
#include <limits>
#include <memory>
#include <queue>
#include <set>
#include <typeindex>
#include <unordered_map>

namespace RPG
{
  /**
   * @brief A generic Entity Component System (ECS).
   *
   * The ECS manages three core concepts:
   * - **Entities**: Lightweight IDs representing game objects.
   * - **Components**: Plain data structs attached to entities.
   * - **Systems**: Logic that operates on entities sharing a specific set of components.
   */
  class EntityComponentSystem
  {
  public:
    /** @brief Unique identifier for an entity. */
    using Entity = std::uint16_t;

    /** @brief Maximum number of entities that can exist simultaneously. */
    static const Entity MaxEntities = std::numeric_limits<Entity>::max();

    /** @brief Sentinel value representing an invalid or null entity. */
    static const Entity InvalidEntity = MaxEntities;

    /** @brief Unique identifier for a component type. */
    using ComponentType = std::uint8_t;

    /** @brief Maximum number of distinct component types supported. */
    static const ComponentType MaxComponents = 32;

    /**
     * @brief Bitmask representing which component types are attached to an entity.
     *
     * Each bit corresponds to a registered @c ComponentType. A system's signature
     * defines which bits must be set for an entity to be tracked by that system.
     */
    using Signature = std::bitset<MaxComponents>;

  private:
    class SystemManager;

  public:
    /**
     * @brief Base class for all ECS systems.
     *
     * A system holds a set of entities whose component signatures match the
     * system's own signature. Derived systems implement game logic by iterating
     * over @c entities(). Systems are managed exclusively by @c SystemManager
     * and cannot be copied or moved.
     */
    class System
    {
    private:
      friend RPG::EntityComponentSystem::SystemManager;

      std::set<Entity> _entities; ///< Entities currently tracked by this system.

    public:
      RPG::EntityComponentSystem& ecs; ///< Reference to the owning ECS instance.

      System() = delete;

      /**
       * @brief Constructs a system bound to the given ECS instance.
       * @param ecs The owning EntityComponentSystem.
       */
      System(RPG::EntityComponentSystem& ecs) : ecs(ecs), _entities() {}
      System(const System&) = delete;
      System(System&&) = delete;
      virtual ~System() = default;

      System& operator=(const System&) = delete;
      System& operator=(System&&) = delete;

      /**
       * @brief Returns the set of entities currently tracked by this system.
       * @return A const reference to the entity set.
       */
      const std::set<Entity>& entities() const { return _entities; }
    };

  private:
    /**
     * @brief Manages entity IDs and their component signatures.
     *
     * Maintains a queue of available entity IDs and a flat array of signatures
     * indexed by entity ID. On creation an entity receives the next available ID;
     * on destruction its signature is reset and its ID is recycled.
     */
    class EntityManager
    {
    private:
      std::queue<Entity>                  _availableEntities; ///< Queue of unused entity IDs.
      std::array<Signature, MaxEntities>  _signatures;        ///< Component signatures indexed by entity ID.

    public:
      /**
       * @brief Initialises the manager by pre-populating the available-ID queue.
       */
      EntityManager()
      {
        // Add every possible entity ID
        for (Entity entity = 0; entity < MaxEntities; entity++)
          _availableEntities.push(entity);
      }

      EntityManager(const EntityManager&) = delete;
      EntityManager(EntityManager&&) = delete;
      ~EntityManager() = default;

      EntityManager& operator=(const EntityManager&) = delete;
      EntityManager& operator=(EntityManager&&) = delete;

      /**
       * @brief Creates a new entity by dequeuing the next available ID.
       * @return The new entity's ID.
       * @pre The available-entity queue must not be empty.
       */
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

      /**
       * @brief Destroys an entity, resetting its signature and recycling its ID.
       * @param entity The entity to destroy.
       * @pre @p entity must be less than @c MaxEntities.
       */
      void destroy(Entity entity)
      {
        assert(entity < MaxEntities && "Entity out of range.");

        // Reset destroyed entity's signature
        _signatures[entity].reset();

        // Put the destroyed entity's ID in the queue
        _availableEntities.push(entity);
      }

      /**
       * @brief Sets the component signature for an entity.
       * @param entity    The target entity.
       * @param signature The new signature to assign.
       * @pre @p entity must be less than @c MaxEntities.
       */
      void set(Entity entity, Signature signature)
      {
        assert(entity < MaxEntities && "Entity out of range.");

        // Register new entity's signature
        _signatures[entity] = signature;
      }

      /**
       * @brief Retrieves the component signature of an entity.
       * @param entity The target entity.
       * @return The entity's current @c Signature.
       * @pre @p entity must be less than @c MaxEntities.
       */
      Signature get(Entity entity) const
      {
        assert(entity < MaxEntities && "Entity out of range.");

        // Get entity's signature
        return _signatures[entity];
      }
    };

    /**
     * @brief Abstract interface for a typed component array.
     *
     * Allows @c ComponentManager to invoke @c destroy() on all component arrays
     * without knowing their concrete component type.
     */
    class IComponentArray
    {
    public:
      virtual ~IComponentArray() = default;

      /**
       * @brief Removes an entity's component data from the array.
       * @param entity The entity whose data should be removed.
       */
      virtual void  destroy(Entity entity) = 0;
    };

    /**
     * @brief Stores component data of type @p Component for all entities.
     *
     * Data is stored in a flat array indexed directly by entity ID, providing
     * O(1) random access at the cost of memory. In debug builds, a set of
     * registered entities is maintained to catch misuse via assertions.
     *
     * @tparam Component The component data type stored by this array.
     */
    template<typename Component>
    class ComponentArray : public IComponentArray
    {
    private:
#ifndef NDEBUG
      std::set<Entity>  _entities;  ///< (Debug only) Entities with data registered.
#endif
      std::array<Component, MaxEntities> _data; ///< Flat array of component data indexed by entity ID.

    public:
      ComponentArray() = default;
      ComponentArray(const ComponentArray<Component>&) = delete;
      ComponentArray(ComponentArray<Component>&&) = delete;
      ~ComponentArray() = default;

      ComponentArray& operator=(const ComponentArray<Component>&) = delete;
      ComponentArray& operator=(ComponentArray<Component>&&) = delete;

      /**
       * @brief Adds a component to an entity by move.
       * @param entity    The target entity.
       * @param component The component value to move into the array.
       * @pre The entity must not already have a component registered (debug builds).
       */
      void add(Entity entity, Component&& component = Component())
      {
#ifndef NDEBUG
        assert(_entities.contains(entity) == false && "Entity already registered.");
        _entities.insert(entity);
#endif

        // Move data to array
        _data[entity] = std::move(component);
      }

      /**
       * @brief Adds a component to an entity by copy.
       * @param entity    The target entity.
       * @param component The component value to copy into the array.
       * @pre The entity must not already have a component registered (debug builds).
       */
      void add(Entity entity, const Component& component)
      {
#ifndef NDEBUG
        assert(_entities.contains(entity) == false && "Entity already registered.");
        _entities.insert(entity);
#endif

        // Copy data to array
        _data[entity] = component;
      }

      /**
       * @brief Removes a component from an entity.
       * @param entity The entity whose component should be removed.
       * @pre The entity must have a component registered (debug builds).
       */
      void remove(Entity entity)
      {
#ifndef NDEBUG
        assert(_entities.contains(entity) == true && "Entity not registered");
        _entities.erase(entity);
#endif
      }

      /**
       * @brief Returns a reference to the component data of an entity.
       * @param entity The target entity.
       * @return A reference to the entity's component data.
       * @pre The entity must have a component registered (debug builds).
       */
      Component& get(Entity entity)
      {
#ifndef NDEBUG
        assert(_entities.contains(entity) == true && "Entity not registered");
#endif

        // Get component data of entity
        return _data[entity];
      }

      /**
       * @brief Returns a reference to the component data of an entity.
       * @param entity The target entity.
       * @return A const reference to the entity's component data.
       * @pre The entity must have a component registered (debug builds).
       */
      const Component& get(Entity entity) const
      {
#ifndef NDEBUG
        assert(_entities.contains(entity) == true && "Entity not registered");
#endif

        // Get component data of entity
        return _data[entity];
      }

      /**
       * @brief Removes an entity's component data (called on entity destruction).
       * @param entity The entity to clean up.
       */
      void  destroy(Entity entity) override
      {
#ifndef NDEBUG
        _entities.erase(entity);
#endif
      }
    };

    /**
     * @brief Manages all registered component types and their data arrays.
     *
     * Component types must be registered via @c add<Component>() before use.
     * Each type is assigned a unique @c ComponentType ID used to build entity
     * signatures. Data is stored in one @c ComponentArray<Component> per type.
     */
    class ComponentManager
    {
    private:
      std::unordered_map<std::type_index, ComponentType>                    _types;  ///< Maps type index to ComponentType ID.
      std::unordered_map<std::type_index, std::unique_ptr<IComponentArray>> _arrays; ///< Maps type index to component array.

    public:
      ComponentManager() = default;
      ComponentManager(const ComponentManager&) = delete;
      ComponentManager(ComponentManager&&) = delete;
      ~ComponentManager() = default;

      ComponentManager& operator=(const ComponentManager&) = delete;
      ComponentManager& operator=(ComponentManager&&) = delete;

      /**
       * @brief Registers a new component type and creates its data array.
       * @tparam Component The component type to register.
       * @pre The component type must not have been registered before.
       */
      template<typename Component>
      void add()
      {
        std::type_index index = typeid(Component);

        assert(_types.contains(index) == false && "Registering component type more than once.");
        assert(_types.size() < MaxComponents && "Too many component types.");

		// Insert new type and array into manager
        _types.insert({ index, static_cast<ComponentType>(_types.size()) });
        _arrays.insert({ index, std::make_unique<ComponentArray<Component>>() });
      }

      /**
       * @brief Returns the @c ComponentType ID assigned to a registered component type.
       * @tparam Component The component type to query.
       * @return The numeric component type ID.
       * @pre The component type must have been registered.
       */
      template<typename Component>
      ComponentType type() const
      {
        std::type_index index = typeid(Component);

        assert(_types.contains(index) == true && "Component not registered before use.");

		// Return component type
        return _types.find(index)->second;
      }

      /**
       * @brief Returns true if the given component type has been registered.
       * @tparam Component The component type to query.
       * @return @c true if the component type is registered, @c false otherwise.
       */
      template<typename Component>
      bool  has() const
      {
		// Return whether type is registered
        return _types.contains(typeid(Component));
	  }

      /**
       * @brief Adds a component to an entity by copy.
       * @tparam Component The component type.
       * @param entity    The target entity.
       * @param component The component value to copy.
       */
      template<typename Component>
      void add(Entity entity, const Component& component)
      {
        // Add entity to component's array
        get<Component>().add(entity, component);
      }

      /**
       * @brief Adds a component to an entity by move.
       * @tparam Component The component type.
       * @param entity    The target entity.
       * @param component The component value to move.
       */
      template<typename Component>
      void add(Entity entity, Component&& component = Component())
      {
        // Add entity to component's array
        get<Component>().add(entity, std::move(component));
      }

      /**
       * @brief Removes a component from an entity.
       * @tparam Component The component type to remove.
       * @param entity The target entity.
       */
      template<typename Component>
      void remove(Entity entity)
      {
		// Remove entity from component's array
        get<Component>().remove(entity);
      }

      /**
       * @brief Returns a reference to the component data of an entity.
       * @tparam Component The component type to retrieve.
       * @param entity The target entity.
       * @return A reference to the entity's component data.
       */
      template<typename Component>
      Component& get(Entity entity)
      {
		// Get component of entity
        return get<Component>().get(entity);
      }

      /**
       * @brief Returns a reference to the component data of an entity.
       * @tparam Component The component type to retrieve.
       * @param entity The target entity.
       * @return A const reference to the entity's component data.
       */
      template<typename Component>
      const Component& get(Entity entity) const
      {
        // Get component of entity
        return get<Component>().get(entity);
      }

      /**
       * @brief Returns a reference to the @c ComponentArray for a given component type.
       * @tparam Component The component type.
       * @return A reference to the typed component array.
       * @pre The component type must have been registered.
       */
      template<typename Component>
      ComponentArray<Component>& get()
      {
        std::type_index index = typeid(Component);

        assert(_types.contains(index) == true && "Component not registered before use.");

		// Return component array of type
        return *static_cast<ComponentArray<Component>*>(_arrays.find(index)->second.get());
      }

      /**
       * @brief Returns a reference to the @c ComponentArray for a given component type.
       * @tparam Component The component type.
       * @return A const reference to the typed component array.
       * @pre The component type must have been registered.
       */
      template<typename Component>
      const ComponentArray<Component>& get() const
      {
        std::type_index index = typeid(Component);

        assert(_types.contains(index) == true && "Component not registered before use.");

        // Return component array of type
        return *static_cast<const ComponentArray<Component>*>(_arrays.find(index)->second.get());
      }

      /**
       * @brief Removes an entity from all component arrays (called on entity destruction).
       * @param entity The entity to clean up.
       */
      void  destroy(Entity entity)
      {
        // Remove entity from every component array
        for (auto& [_, array] : _arrays)
          array->destroy(entity);
      }
    };

    /**
     * @brief Manages all registered ECS systems.
     *
     * Each system is paired with a @c Signature that defines which components
     * an entity must have to be tracked by that system. When an entity's
     * signature changes, the manager updates each system's entity set accordingly.
     */
    class SystemManager
    {
    private:
      /** @brief Associates a system instance with its required component signature. */
      struct Pair
      {
        std::unique_ptr<System> system;    ///< Owning pointer to the system.
        Signature               signature; ///< Required component signature.
      };

      std::unordered_map<std::type_index, Pair> _systems; ///< Registered systems indexed by type index.

    public:
      SystemManager() = default;
      SystemManager(const SystemManager&) = delete;
      SystemManager(SystemManager&&) = delete;

      /**
       * @brief Destroys all registered systems in a safe order.
       *
       * Each system is moved out of the map before destruction so that it
       * cannot access the manager during its own destructor.
       */
      ~SystemManager()
      {
        // A system should not access itself while destroying
        for (auto iterator = _systems.begin(); iterator != _systems.end();) {
          auto system = std::move(iterator->second.system);
          iterator = _systems.erase(iterator);
        }
      }

      SystemManager& operator=(const SystemManager&) = delete;
      SystemManager& operator=(SystemManager&&) = delete;

      /**
       * @brief Registers a new system with the given component signature.
       * @tparam NewSystem The concrete system type to create. Must derive from @c System.
       * @tparam Args      Additional constructor argument types.
       * @param signature  Component signature that entities must match to be tracked.
       * @param ecs        The owning ECS instance passed to the system constructor.
       * @param args       Additional arguments forwarded to the system constructor.
       * @return A reference to the newly created system.
       * @pre The system type must not have been registered before.
       */
      template<typename NewSystem, typename ...Args>
      NewSystem& add(Signature signature, EntityComponentSystem& ecs, Args&& ...args)
      {
        std::type_index index = typeid(NewSystem);

        assert(_systems.contains(index) == false && "Registering system more than once.");

        auto  system = std::make_unique<NewSystem>(ecs, std::forward<Args>(args)...);
        auto& ref = *system;

        // Add new system and its signature to manager
        _systems.insert({ index, Pair{ .system = std::move(system), .signature = signature } });

        // Return reference of new system
        return ref;
      }

      /**
       * @brief Retrieves a reference to a registered system.
       * @tparam GetSystem The system type to retrieve.
       * @return A reference to the system.
       * @pre The system type must have been registered.
       */
      template<typename GetSystem>
      GetSystem& get()
      {
        std::type_index index = typeid(GetSystem);

        assert(_systems.contains(index) == true && "System not registered.");

		// Return system of type
        return *static_cast<GetSystem*>(_systems.find(index)->second.system.get());
      }

      /**
       * @brief Retrieves a reference to a registered system.
       * @tparam GetSystem The system type to retrieve.
       * @return A const reference to the system.
       * @pre The system type must have been registered.
       */
      template<typename GetSystem>
      const GetSystem& get() const
      {
        std::type_index index = typeid(GetSystem);

        assert(_systems.contains(index) == true && "System not registered.");

        // Return system of type
        return *static_cast<const GetSystem*>(_systems.find(index)->second.system.get());
      }

      /**
       * @brief Updates which entities are tracked by each system after a signature change.
       *
       * An entity is inserted into a system's entity set when its signature satisfies
       * the system's required signature, and erased from it otherwise.
       *
       * @param entity    The entity whose signature has changed.
       * @param signature The entity's new component signature.
       */
      void  update(Entity entity, Signature signature)
      {
        // Update entity signature in each system
        for (auto& [_, pair] : _systems)
        {
          if ((signature & pair.signature) == pair.signature)
            pair.system->_entities.insert(entity);
          else
            pair.system->_entities.erase(entity);
        }
      }

      /**
       * @brief Removes an entity from all system entity sets (called on entity destruction).
       * @param entity The entity to remove.
       */
      void  destroy(Entity entity)
      {
        // Remove entity from each system
        for (auto& [_, pair] : _systems)
          pair.system->_entities.erase(entity);
      }

      /**
       * @brief Destroys and unregisters a system.
       * @tparam DestroySystem The system type to remove.
       * @pre The system type must have been registered.
       */
      template<typename DestroySystem>
      void  destroy()
      {
        std::type_index index = typeid(DestroySystem);

        assert(_systems.contains(index) == true && "System not registered.");

		// Remove system from manager (automatically destroyed by unique_ptr)
        _systems.erase(index);
      }
      
      /**
       * @brief Returns true if a system of the given type is currently registered.
       * @tparam QuerySystem The system type to query.
       * @return @c true if the system is registered, @c false otherwise.
       */
      template<typename QuerySystem>
      bool  has() const
      {
        return _systems.contains(typeid(QuerySystem));
      }
    };
    
    EntityManager    _entities;   ///< Manages entity IDs and signatures.
    ComponentManager _components; ///< Manages component type registration and data.
    SystemManager    _systems;    ///< Manages system registration and entity tracking.

  public:
    EntityComponentSystem() = default;
    EntityComponentSystem(const EntityComponentSystem&) = delete;
    EntityComponentSystem(EntityComponentSystem&&) = delete;
    ~EntityComponentSystem() = default;

    EntityComponentSystem& operator=(const EntityComponentSystem&) = delete;
    EntityComponentSystem& operator=(EntityComponentSystem&&) = delete;

    /**
     * @brief Creates a new entity and returns its ID.
     * @return The ID of the newly created entity.
     */
    Entity createEntity()
    {
      // Generate a new entity
      return _entities.create();
    }

    /**
     * @brief Destroys an entity and removes it from all managers.
     *
     * The entity's ID is recycled, its component data is cleared, and it is
     * removed from every system's entity set.
     *
     * @param entity The entity to destroy.
     */
    void  destroyEntity(Entity entity)
    {
      // Remove entity from each manager
      _entities.destroy(entity);
      _components.destroy(entity);
      _systems.destroy(entity);
    }

    /**
     * @brief Registers a new component type with the ECS.
     *
     * Must be called before attaching this component type to any entity.
     *
     * @tparam Component The component type to register.
     */
    template<typename Component>
    void addComponent()
    {
	  // Register new component type
      _components.add<Component>();
    }

    /**
     * @brief Attaches a component to an entity by move, updating its signature.
     * @tparam Component The component type to attach.
     * @param entity    The target entity.
     * @param component The component value to move (defaults to a default-constructed value).
     */
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

    /**
     * @brief Attaches a component to an entity by copy, updating its signature.
     * @tparam Component The component type to attach.
     * @param entity    The target entity.
     * @param component The component value to copy.
     */
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

    /**
     * @brief Detaches a component from an entity, updating its signature.
     * @tparam Component The component type to remove.
     * @param entity The target entity.
     */
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

    /**
     * @brief Returns a reference to the component data of an entity.
     * @tparam Component The component type to retrieve.
     * @param entity The target entity.
     * @return A reference to the entity's component data.
     */
    template<typename Component>
    Component& getComponent(Entity entity)
    {
      // Get component of entity
      return _components.get<Component>(entity);
    }

    /**
     * @brief Returns a const reference to the component data of an entity.
     * @tparam Component The component type to retrieve.
     * @param entity The target entity.
     * @return A const reference to the entity's component data.
     */
    template<typename Component>
    const Component&  getComponent(Entity entity) const
    {
      // Get component of entity
      return _components.get<Component>(entity);
    }

    /**
     * @brief Returns true if the entity currently has the given component attached.
     * @tparam Component The component type to query.
     * @param entity The target entity.
     * @return @c true if the component type is registered and attached to the entity,
     *         @c false otherwise.
     */
    template<typename Component>
    bool hasComponent(Entity entity) const
    {
      return _components.has<Component>() && _entities.get(entity).test(_components.type<Component>());
    }

    /**
     * @brief Registers a new system with a required component signature.
     * @tparam NewSystem The concrete system type. Must derive from @c System.
     * @tparam Args      Additional constructor argument types.
     * @param signature  Component signature that entities must satisfy to be tracked.
     * @param args       Additional arguments forwarded to the system constructor.
     * @return A reference to the newly created system.
     */
    template<typename NewSystem, typename ...Args>
    NewSystem& addSystem(Signature signature, Args&& ...args)
    {
      // Add new system
      return _systems.add<NewSystem>(signature, *this, std::forward<Args>(args)...);
    }

    /**
     * @brief Retrieves a reference to a registered system.
     * @tparam GetSystem The system type to retrieve.
     * @return A reference to the system.
     */
    template<typename GetSystem>
    GetSystem& getSystem()
    {
      // Get system
      return _systems.get<GetSystem>();
    }

    /**
     * @brief Retrieves a reference to a registered system.
     * @tparam GetSystem The system type to retrieve.
     * @return A const reference to the system.
     */
    template<typename GetSystem>
    const GetSystem& getSystem() const
    {
      // Get system
      return _systems.get<GetSystem>();
    }

    /**
     * @brief Destroys and unregisters a system.
     * @tparam DestroySystem The system type to remove.
     */
    template<typename DestroySystem>
    void  destroySystem()
    {
      // Remove system
      _systems.destroy<DestroySystem>();
    }

    /**
     * @brief Returns true if a system of the given type is currently registered.
     * @tparam QuerySystem The system type to query.
     * @return @c true if the system is registered, @c false otherwise.
     */
    template<typename QuerySystem>
    bool hasSystem() const
    {
      return _systems.has<QuerySystem>();
    }

    /**
     * @brief Constructs a @c Signature with bits set for each specified component type.
     * @tparam Components Zero or more registered component types to include.
     * @return A @c Signature with one bit set per specified component type.
     */
    template<typename... Components>
    Signature signature()
    {
      Signature signature;

      // Set each component's bit in the signature
      (signature.set(_components.type<Components>()), ...);
      return signature;
    }
  };

  /** @brief Convenience alias for @c EntityComponentSystem. */
  using ECS = EntityComponentSystem;
}