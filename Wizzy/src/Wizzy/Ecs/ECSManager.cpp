#include "wzpch.h"

#include "Wizzy/Ecs/ECSManager.h"

namespace ecs {

	enum SystemOperation {
		OPERATION_UPDATE, OPERATION_NOTIFY
	};

	ECSManager::ECSManager() {

	}
	ECSManager::~ECSManager() {

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
	EntityHandle ECSManager::CreateEntity(IComponent **components,
									const StaticCId * ids,
									size_t numComponents) {
		Entity *_newEntity = new Entity;
		EntityHandle _handle = static_cast<EntityHandle>(_newEntity);

		for (size_t i = 0; i < numComponents; i++) {

			AddComponentInternal(_newEntity, ids[i], components[i]);
		}

		_newEntity->first = m_entites.size();

		m_entites.push_back(_newEntity);

		return _handle;
	}
	void ECSManager::RemoveEntity(EntityHandle handle) {
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

	std::vector<IComponent*> ECSManager::GetComponents(EntityHandle entity) {
		std::vector<IComponent*> _components;

		for (auto& [id, memPool] : m_components) {
			_components.push_back(GetComponentInternal(ToRawType(entity), memPool, id));
		}

		return _components;
	}

	void ECSManager::NotifySystems(const SystemLayer& systems, Wizzy::Event& e) {
        ComponentGroup _toUpdate;
		for (size_t i = 0; i < systems.SystemCount(); i++) {
            uint16_t _systemUpdates = 0;
			auto* _system = systems[i];
			if (!_system->IsSubscribed(e.GetEventType())) continue;
			const auto& _systemTypes = _system->GetTypeIds();
			const auto& _systemFlags = _system->GetFlags();
			if (_systemTypes.size() == 1) {
				const auto& _componentType = _systemTypes[0];
				const auto& _typeSize = IComponent::StaticInfo(_componentType).size;
				const auto& _memPool = m_components[_componentType];
				for (size_t j = 0; j < _memPool.size(); j += _typeSize) {
					_toUpdate.Push((IComponent*)&_memPool[i], _componentType);
					_system->OnEvent(e, _toUpdate);
                    _systemUpdates++;
                    _toUpdate.Clear();
				}
			} else {
				/* _lc = least common */
				uint32_t _lcIndex = FindLeastCommonComponentIndex(_systemTypes,
															 _systemFlags);
				StaticCId _lcType = _systemTypes[_lcIndex];
				const auto& _lcTypeSize = IComponent::StaticInfo(_lcType).size;
				auto _lcMemPool = m_components[_lcType];

                /* Hint the componentgroup about how many elemets there will be
                    as to allocate all needed memory here instead of allocating
                    a small amount for each component push */
                _toUpdate.HintCount(_lcMemPool.size() / _lcTypeSize);

				bool _isValid = true;
				/* Iterate through the components of the least common of the
					system types, instead of just iterating through the first
					typegroup of components as it could be any amount of
					components. */
				for (size_t j = 0; j < _lcMemPool.size(); j += _lcTypeSize) {
					IComponent *_ofFirstType = (IComponent*)&_lcMemPool[j];

					Entity *_entity = ToRawType(_ofFirstType->entity);
					for (size_t k = 0; k < _systemTypes.size(); k++) {
						const auto& _systemType = _systemTypes[k];
						const auto& _systemFlag = _systemFlags[k];
						IComponent *_entityComp = GetComponentInternal(
													_entity,
													m_components[_systemType],
													_systemType
												);

						if (!_entityComp &&
							(_systemFlag & System::FLAG_OPTIONAL) == 0) {
							/* Entity does not have a component of a system
							 	component type AND that component is not optional*/
							_isValid = false;
							break;
						}

						if (_entityComp)
							_toUpdate.Push(_entityComp, _systemType);
					}

					if (_isValid) {
						_system->OnEvent(e, _toUpdate);
                        _systemUpdates++;
                        _toUpdate.Clear();
					}
				}
			}
			WZ_CORE_ASSERT(_systemUpdates != 0, "Unused system '" + typestr(*_system) + "'");
		}
	}
	void ECSManager::AddComponentInternal(Entity *entity,
									const StaticCId & componentId,
									IComponent *component) {
		WZ_CORE_ASSERT(IComponent::IsTypeValid(componentId), "Tried creating a component on entity with invalid ID");
		const auto& _createFn = IComponent::StaticInfo(componentId).createFn;
		entity->second.push_back({
			componentId,
			_createFn(
				m_components[componentId],
				static_cast<EntityHandle>(entity),
				component
			)
		});
	}
	void ECSManager::RemoveComponentInternal(EntityHandle handle, const StaticCId componentId) {
		auto& _entityComponents = ToEntityData(handle);
		for (auto& _componentPair : _entityComponents) {
			if (componentId == _componentPair.first) {
				DeleteComponent(_componentPair.first, _componentPair.second);
				auto _srcIndex = _entityComponents.size() - 1;
				auto _destIndex = _componentPair.first;
				_entityComponents[_destIndex] = _entityComponents[_srcIndex];
				_entityComponents.pop_back();
				break;
			}
		}
	}
	IComponent* ECSManager::GetComponentInternal(Entity * entity,
											const ComponentMem& memPool,
											const StaticCId & componentId) {
		const auto& _entityComps = entity->second;

		for (const auto& _comp : _entityComps) {
			const auto& _compId = _comp.first;
			const auto& _memIndex = _comp.second;
			if (componentId == _compId) return (IComponent*)&memPool[_memIndex];
		}

		return nullptr;
	}
	void ECSManager::DeleteComponent(const StaticCId & componentId,
								const MemIndex & memIdx) {
		ComponentMem& _memPool = m_components[componentId];

		const auto& _typeInfo = IComponent::StaticInfo(componentId);
		auto _freeFn = _typeInfo.freeFn;
		size_t _typeSize = _typeInfo.size;

		auto _srcIndex = _memPool.size() - _typeSize;
		byte *_srcBytes = &_memPool[_srcIndex];

		byte *_dstBytes = &_memPool[memIdx];

		_freeFn((IComponent*)_dstBytes);

		if (memIdx == _srcIndex) {
			_memPool.resize(_srcIndex);
			return;
		}

		memcpy(_dstBytes, _srcBytes, _typeSize);

		auto& _srcComponents = ToEntityData(((IComponent*)_srcBytes)->entity);
		for (auto& _componentPair : _srcComponents) {
			if (componentId == _componentPair.first && _srcIndex == _componentPair.second) {
				_componentPair.second = memIdx;
				break;
			}
		}

		_memPool.resize(_srcIndex);
	}
	uint32_t ECSManager::FindLeastCommonComponentIndex(const std::vector<StaticCId>& types,
											const std::vector<System::ComponentFlags>& flags) {
		size_t _minNumComponents = SIZE_MAX;
		uint32_t _minIndex = UINT32_MAX;

		for (uint32_t i = 0; i < static_cast<uint32_t>(types.size()); i++) {
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
