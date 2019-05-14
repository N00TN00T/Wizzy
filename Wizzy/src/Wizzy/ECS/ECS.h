#pragma once

#include "Wizzy/ECS/System.h"

namespace Wizzy {

	/*					ECS.h 

		This is where the parts of the ECS-system are
		tied together to form the ECS-system itself.
		The ECS-system is defined by
		- Entites
			* A collection of components
		- Components
			* Pure data
		- Systems
			* Logic with the use of data (components)

		This is a pure ECS-system, which is not to be 
		confused with the more commonly known
		'Component based entity system'
			or
		'System of entities and components'
	*/

	/* 
		Used for remembering the index in the component
		memory pool where the data of the components
		on an entity begins. 
	*/
	typedef size_t MemIndex;

	/*
		Used for referencing in the mem pool which
		components belong to an entity
	*/
	typedef std::vector<std::pair<StaticCId, MemIndex>> EntityDataRef;

	/*
		Entities are only a collection of components,
		or the indices of the components in the ECS
		component collection.
	*/
	typedef std::pair<MemIndex, EntityDataRef> Entity;


	class ECS {
	public:
		ECS();
		ECS(const ECS&) = delete;
		~ECS();

		EntityHandle CreateEntity(IComponent **components, 
									const StaticCId *ids,
									size_t numComponents);
		void RemoveEntity(EntityHandle handle);

		template <typename TComponent>
		void AddComponent(EntityHandle entity, TComponent* component);
		template <typename TComponent>
		void RemoveComponent(EntityHandle entity);
		template <typename TComponent>
		void GetComponent(EntityHandle entity);

		void UpdateSystems(const SystemLayer& systems, const float& deltaTime);

	private:
		std::unordered_map<StaticCId, ComponentMem>		m_components;
		std::deque<Entity*>								m_entites;

		inline Entity* ToRawType(EntityHandle handle) {
			return static_cast<Entity*>(handle);
		}

		inline const MemIndex& ToEntityMemIndex(EntityHandle handle) {
			return ToRawType(handle)->first;
		}

		inline EntityDataRef& ToEntityData(EntityHandle handle) {
			return ToRawType(handle)->second;
		}

		void AddComponentInternal(Entity *entity,
									const StaticCId& componentId,
									IComponent *component);
		void RemoveComponentInternal(EntityHandle handle,
										const StaticCId componentId);
		IComponent* GetComponentInternal(Entity *entity,
									const ComponentMem& memPool,
									const StaticCId& componentId);
		void DeleteComponent(const StaticCId & componentId,
								const MemIndex & memIdx);

		u32 FindLeastCommonComponentIndex(const std::vector<StaticCId>& types,
											const std::vector<System::ComponentFlags>& flags);
	};

	template<typename TComponent>
	inline void ECS::AddComponent(EntityHandle entity, TComponent * component) {
		AddComponentInternal(ToRawType(entity), TComponent::staticId, component);
	}
	template<typename TComponent>
	inline void ECS::RemoveComponent(EntityHandle entity) {
		RemoveComponentInternal(entity, TComponent::staticId);
	}
	template<typename TComponent>
	inline void ECS::GetComponent(EntityHandle entity) {
		return static_cast<TComponent*>(GetComponentInternal(ToRawType(entity),
										m_components[TComponent::staticId],
										TComponent::staticId));
	}
}