#include "wzpch.h"

#include "Wizzy/Ecs/ECSManager.h"
#include "Wizzy/Instrumentor.h"
#include "Wizzy/WizzyExceptions.h"
#include "Wizzy/Utils.h"

namespace Wizzy {
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

	std::vector<std::pair<StaticCId, IComponent*>> ECSManager::GetComponents(EntityHandle entity) const {
		std::vector<std::pair<StaticCId, IComponent*>> _components;

		for (auto& [id, memPool] : m_components) {
			auto _comp = GetComponentInternal(ToRawType(entity), memPool, id);
			if (_comp)
				_components.push_back({ id, _comp });
		}

		return _components;
	}

	void ECSManager::NotifySystems(const SystemLayer& systems, const Wizzy::Event& e) const {
		WZ_PROFILE_FUNCTION();
        ComponentGroup _toUpdate;
		for (size_t i = 0; i < systems.SystemCount(); i++) {
            uint16_t _systemUpdates = 0;
			auto* _system = systems[i];

			if (!_system->IsSubscribed(e.GetEventType())) continue;

			if (_system->ProcessAll()) {

				for (const auto& _entity : m_entites) {

					ComponentGroup _group;

					for (auto [_id, _comp] : GetComponents(_entity)) {
						_group.Push(_comp, _id);
					}

					_systemUpdates++;
					_system->OnEvent(e, _group);

				}

			} else {
				const auto& _systemTypes = _system->GetTypeIds();
				const auto& _systemFlags = _system->GetFlags();
				if (_systemTypes.size() == 1) {
					const auto& _componentType = _systemTypes[0];
					if (m_components.find(_componentType) == m_components.end()) continue;
					const auto& _typeSize = IComponent::StaticInfo(_componentType).size;
					const auto& _memPool = m_components.at(_componentType);
					for (size_t j = 0; j < _memPool.size(); j += _typeSize) {
						_toUpdate.Push((IComponent*)& _memPool[j], _componentType);
						_system->OnEvent(e, _toUpdate);
						_systemUpdates++;
						_toUpdate.Clear();
					}
				} else {
					/* _lc = least common */
					uint32_t _lcIndex = FindLeastCommonComponentIndex(_systemTypes,
						_systemFlags);
					StaticCId _lcType = _systemTypes[_lcIndex];
					if (m_components.find(_lcType) == m_components.end()) continue;
					const auto& _lcTypeSize = IComponent::StaticInfo(_lcType).size;
					auto _lcMemPool = m_components.at(_lcType);

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
						IComponent* _ofFirstType = (IComponent*)& _lcMemPool[j];

						Entity* _entity = ToRawType(_ofFirstType->entity);
						for (size_t k = 0; k < _systemTypes.size(); k++) {
							const auto& _systemType = _systemTypes[k];
							if (m_components.find(_systemType) == m_components.end())
							{
								_isValid = false;
								break;
							}
							const auto& _systemFlag = _systemFlags[k];
							IComponent* _entityComp = GetComponentInternal(
								_entity,
								m_components.at(_systemType),
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
											const StaticCId & componentId) const {
		WZ_PROFILE_FUNCTION();
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
											const std::vector<System::ComponentFlags>& flags) const {
		WZ_PROFILE_FUNCTION();
		size_t _minNumComponents = SIZE_MAX;
		uint32_t _minIndex = UINT32_MAX;

		for (uint32_t i = 0; i < static_cast<uint32_t>(types.size()); i++) {
			if ((flags[i] & System::FLAG_OPTIONAL) != 0) {
				continue;
			}
			const size_t& _typeSize = IComponent::StaticInfo(types[i]).size;
			auto _numComponents = m_components.at(types[i]).size() / _typeSize;

			if (_numComponents <= _minNumComponents) {
				_minNumComponents = _numComponents;
				_minIndex = i;
			}
		}

		return _minIndex;
	}

	void ECSManager::Save(string file)
	{
		WZ_CORE_TRACE("Saving ECS System to {0}", file);
		std::vector<byte> data;
		u32 dataIndex = 0;

		u32 i = 0;
		for (auto& e : m_entites)
		{
			WZ_CORE_TRACE("(1/2) {0}%", ((double)i / (double)m_entites.size()) * (double)100);
			data.resize(data.size() + TOKEN_ENTITY.size() + sizeof(size_t) + e->second.size() * (sizeof(StaticCId) + sizeof(MemIndex)) + sizeof(e->first));
			memcpy(&data[0] + dataIndex, TOKEN_ENTITY.data(), TOKEN_ENTITY.size());
			dataIndex += TOKEN_ENTITY.size();

			memcpy(&data[0] + dataIndex, &e->first, sizeof(e->first));
			dataIndex += sizeof(e->first);

			auto sz = e->second.size();
			memcpy(&data[0] + dataIndex, &sz, sizeof(size_t));
			dataIndex += sizeof(size_t);
			for (auto& dataRef : e->second)
			{
				memcpy(&data[0] + dataIndex, &dataRef.first, sizeof(dataRef.first));
				dataIndex += sizeof(dataRef.first);
				memcpy(&data[0] + dataIndex, &dataRef.second, sizeof(dataRef.second));
				dataIndex += sizeof(dataRef.second);
			}
			i++;
		}

		i = 0;
		for (auto& c : m_components)
		{
			WZ_CORE_TRACE("(2/2) {0}%", ((double)i / (double)m_components.size()) * (double)100);
			auto& info = IComponent::StaticInfo(c.first);

			data.resize(data.size() + TOKEN_COMPONENT.size() + c.second.size() + sizeof(StaticCId) + sizeof(size_t));

			memcpy(&data[0] + dataIndex, TOKEN_COMPONENT.data(), TOKEN_COMPONENT.size());
			dataIndex += TOKEN_COMPONENT.size();

			memcpy(&data[0] + dataIndex, &c.first, sizeof(StaticCId));
			dataIndex += sizeof(StaticCId);

			size_t sz = c.second.size();
			memcpy(&data[0] + dataIndex, &sz, sizeof(size_t));
			dataIndex += sizeof(size_t);

			memcpy(&data[0] + dataIndex, c.second.data(), c.second.size());
			dataIndex += c.second.size();
			i++;
		}


		if (!ulib::File::write(file, data, true))
		{
			WZ_THROW(Exception, "Failed reading file when saving ECS: " + file);
		}
	}
	void ECSManager::Load(string file)
	{
		
		WZ_CORE_TRACE("Loading Ecs from file {0}", file);
		std::vector<byte> data;
		if (!ulib::File::read(file, &data, true))
		{
			WZ_THROW(Exception, "Failed reading file when loading ECS: " + file);
		}

		WZ_CORE_TRACE("Finding entity data...");
		std::vector<byte> delimiter;
		delimiter.resize(TOKEN_ENTITY.size());
		memcpy(&delimiter[0], TOKEN_ENTITY.data(), TOKEN_ENTITY.size());

		auto entitiesData = split_vector(data, delimiter);

		u32 i = 0;
		for (auto& entityData : entitiesData)
		{
			WZ_CORE_TRACE("Processing entity data... (1/2) {0}%", (double)i / (double)entitiesData.size() * (double)100);
			Entity* entity = new Entity;

			u32 ePtr = 0;

			memcpy(&entity->first, entityData.data() + ePtr, sizeof(entity->first));
			ePtr += sizeof(entity->first);
			
			size_t count;
			memcpy(&count, entityData.data() + ePtr, sizeof(count));
			ePtr += sizeof(count);

			entity->second.resize(count);

			for (int i = 0; i < count; i++)
			{
				memcpy(&entity->second[i].first, entityData.data() + ePtr, sizeof(entity->second[i].first));
				ePtr += sizeof(entity->second[i].first);
				memcpy(&entity->second[i].second, entityData.data() + ePtr, sizeof(entity->second[i].second));
				ePtr += sizeof(entity->second[i].second);
			}
			
			m_entites.push_back(entity);
			i++;
		}

		WZ_CORE_TRACE("Finding component data...");
		delimiter.resize(TOKEN_COMPONENT.size());
		memcpy(&delimiter[0], TOKEN_COMPONENT.data(), TOKEN_COMPONENT.size());

		auto componentsData = split_vector(data, delimiter);

		for (int i = 1; i < componentsData.size(); i++)
		{
			WZ_CORE_TRACE("Processing component data... (2/2) {0}%", (double)i / (double)componentsData.size() * (double)100);
			auto& componentData = componentsData[i];

			u32 cPtr = 0;

			StaticCId id;
			ComponentMem mem;
			size_t sz;

			memcpy(&id, componentData.data() + cPtr, sizeof(StaticCId));
			cPtr += sizeof(StaticCId);
			memcpy(&sz, componentData.data() + cPtr, sizeof(size_t));
			cPtr += sizeof(size_t);

			mem.resize(sz);
			memcpy(&mem[0], componentData.data() + cPtr, sz);
			cPtr += sz;

			m_components[id] = mem;
		}

		for (auto& e : m_entites)
		{
			for (auto& dataRef : e->second)
			{
				auto id = dataRef.first;
				auto idx = dataRef.second;

				auto comp = (IComponent*)&m_components[id][idx];
				comp->entity = e;
			}
		}
	}
}
