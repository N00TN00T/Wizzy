#include "wzpch.h"

#include "Wizzy/ECS/ECS.h"

namespace Wizzy {
	ECS::ECS() {
		WZ_CORE_TRACE("Constructing ECS-System");
	}
	ECS::~ECS() {
		WZ_CORE_TRACE("Destructing ECS-System");

		for (const auto& _entity : m_entites) {
			delete _entity;
		}

		for (const auto& _componentPair : m_components) {
			const auto& _staticId = _componentPair.first;
			const auto& _typeMemPool = _componentPair.second;
			const auto& _typeInfo = IComponent::StaticInfo(_staticId);

			size_t _typeSize = _typeInfo.size;
			auto& _freeFn = _typeInfo.freeFn;

			/* Iterate through the mem pool for components
				of this type and free it					*/
			for (size_t i = 0; i < _typeMemPool.size(); i += _typeSize) {
				_freeFn((IComponent*)&_typeMemPool[i]);
			}
		}
	}
	EntityHandle ECS::CreateEntity(IComponent **components, 
									const StaticCId * ids, 
									size_t numComponents) {
		WZ_CORE_TRACE("Creating entity with {0} components...", numComponents);


		Entity *_newEntity = new Entity;
		EntityHandle _handle = static_cast<EntityHandle>(_newEntity);

		for (size_t i = 0; i < numComponents; i++) {

			AddComponentInternal(_newEntity, ids[i], components[i]);
		}

		_newEntity->first = m_entites.size();

		m_entites.push_back(_newEntity);

		return _handle;
	}
	void ECS::RemoveEntity(EntityHandle handle) {
		auto _entityDataRef = ToEntityData(handle);
		
		for (const auto& _dataRefPair : _entityDataRef) {
			const auto& _componentId = _dataRefPair.first;
			const auto& _memIndex = _dataRefPair.second;

			DeleteComponent(_componentId, _memIndex);
		}
		
		/* Move the last entity to overwrite the
			entity being removed					*/
		const auto& _destIndex = ToEntityMemIndex(handle);
		const auto& _srcIndex = m_entites.size() - 1;
		delete m_entites[_destIndex];
		m_entites[_destIndex] = m_entites[_srcIndex];
		m_entites[_destIndex]->first = _destIndex;
		m_entites.pop_back();
	}

	void ECS::UpdateSystems(const SystemLayer& systems, const float& deltaTime) {
		std::vector<IComponent*> _componentsToUpdate;
		std::vector<ComponentMem*> _componentMemPools;
		for (size_t i = 0; i < systems.SystemCount(); i++) {
			auto* _system = systems[i];
			const auto& _componentTypes = _system->GetTypeIds();
			if (_componentTypes.size() == 1) {
				const auto& _componentType = _componentTypes[0];
				const auto& _typeSize = IComponent::StaticInfo(_componentType).size;
				const auto& _memPool = m_components[_componentType];
				for (size_t i = 0; i < _memPool.size(); i += _typeSize) {
					IComponent* _component = (IComponent*)&_memPool[i];
					_system->OnUpdate(deltaTime, &_component);
				}
			} else {
				
				const auto& _componentFlags = _system->GetFlags();

				_componentsToUpdate.resize(std::max(_componentsToUpdate.size(), 
													_componentTypes.size()));
				_componentMemPools.resize(std::max(_componentMemPools.size(),
													_componentTypes.size()));
				for (size_t j = 0; j < _componentTypes.size(); j++) {
					_componentMemPools[j] = &m_components[_componentTypes[j]];
				}

				u32 _leastCommonIndex = FindLeastCommonComponentIndex(_componentTypes, _componentFlags);

				size_t _typeSize = IComponent::StaticInfo(_componentTypes[_leastCommonIndex]).size;
				auto& _memPool = *_componentMemPools[_leastCommonIndex];
				for (size_t j = 0; j < _memPool.size(); j += _typeSize) {
					_componentsToUpdate[_leastCommonIndex] = (IComponent*)&_memPool[j];
					auto _entity = ToRawType(_componentsToUpdate[_leastCommonIndex]->entity);

					bool _isValid = true;
					for (size_t k = 0; k < _componentTypes.size(); k++) {
						if (k == _leastCommonIndex) continue;

						_componentsToUpdate[k] = GetComponentInternal(_entity, *_componentMemPools[k], _componentTypes[k]);
						if (!_componentsToUpdate[k] && 
							(_componentFlags[k] & System::FLAG_OPTIONAL) == 0) {
							/* Entity does not have all components for system
								and missing component is not optional */
							_isValid = false;
							break;
						}
					}
					if (_isValid) {
						_system->OnUpdate(deltaTime, &_componentsToUpdate[0]);
					}
				}
				
			}
		}
	}
	void ECS::AddComponentInternal(Entity *entity, 
									const StaticCId & componentId,
									IComponent *component) {
		WZ_CORE_ASSERT(IComponent::IsTypeValid(componentId), "Tried creating a component on entity with invalid ID ('" + std::to_string(componentId) + "')", componentId);
		const auto& _createFn = IComponent::StaticInfo(componentId).createFn;
		int hej = 5;
		m_components[componentId] = ComponentMem();
		entity->second.push_back({
			componentId,
			_createFn(
				m_components[componentId],
				static_cast<EntityHandle>(entity),
				component
			)
		});
	}
	void ECS::RemoveComponentInternal(EntityHandle handle, const StaticCId componentId) {
		auto& _entityComponents = ToEntityData(handle);
		for (auto& _componentPair : _entityComponents) {
			if (componentId == _componentPair.first) {
				DeleteComponent(_componentPair.first, _componentPair.second);
				auto _srcIndex = _entityComponents.size() - 1;
				auto _destIndex = _componentPair.first;
				_entityComponents[_destIndex] = _entityComponents[_srcIndex];
				_entityComponents.pop_back();
				break; /* Unique-component */
			}
		}
	}
	IComponent* ECS::GetComponentInternal(Entity * entity, 
											const ComponentMem& memPool, 
											const StaticCId & componentId) {
		const auto& _entityData = entity->second;

		for (const auto& _dataPair : _entityData) {
			const auto& _componentId = _dataPair.first;
			const auto& _memIndex = _dataPair.second;

			return (IComponent*)&memPool[_memIndex];
		}
		return nullptr;
	}
	void ECS::DeleteComponent(const StaticCId & componentId, 
								const MemIndex & memIdx) {
		ComponentMem& _memPool = m_components[componentId];
		
		const auto& _typeInfo = IComponent::StaticInfo(componentId);
		auto _freeFn = _typeInfo.freeFn;
		size_t _typeSize = _typeInfo.size;

		auto _srcIndex = _memPool.size() - _typeSize;
		IComponent *_srcComponent = (IComponent*)(&_memPool[_srcIndex]);

		IComponent *_destComponent = (IComponent*)(&_memPool[memIdx]);

		_freeFn(_destComponent);

		if (memIdx == _srcIndex) {
			_memPool.resize(_srcIndex);
			return;
		} 

		memcpy(_destComponent, _srcComponent, _typeSize);

		auto& _srcComponents = ToEntityData(_srcComponent->entity);
		for (auto& _componentPair : _srcComponents) {
			if (componentId == _componentPair.first && _srcIndex == _componentPair.second) {
				_componentPair.second = memIdx;
				break;
			}
		}

		_memPool.resize(_srcIndex);
	}
	u32 ECS::FindLeastCommonComponentIndex(const std::vector<StaticCId>& types, 
											const std::vector<System::ComponentFlags>& flags) {
		size_t _minNumComponents = SIZE_MAX;
		u32 _minIndex = UINT32_MAX;

		for (u32 i = 0; i < static_cast<u32>(types.size()); i++) {
			if ((flags[i] & System::FLAG_OPTIONAL) != 0) {
				continue;
			}
			const size_t& _typeSize = IComponent::StaticInfo(types[i]).size;
			auto _numComponents = m_components[types[i]].size() / _typeSize;

			if (_numComponents < _minNumComponents) {
				_minNumComponents = _numComponents;
				_minIndex = i;
			}
		}

		return _minIndex;
	}
}