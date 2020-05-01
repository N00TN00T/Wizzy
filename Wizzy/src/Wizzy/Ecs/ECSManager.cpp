#include "wzpch.h"

#include "Wizzy/Ecs/ECSManager.h"
#include "Wizzy/Instrumentor.h"
#include "Wizzy/WizzyExceptions.h"
#include "Wizzy/Utils.h"
#include "Wizzy/JobSystem/JobSystem.h"

namespace Wizzy {
	

	ECSManager::ECSManager() {

	}
	ECSManager::~ECSManager() {

		for (const auto& _entity : m_entities) {
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

		_newEntity->first = m_entities.size();

		m_entities.push_back(_newEntity);

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
		auto _destIndex = ToEntityMemIndex(handle);
		auto _srcIndex = m_entities.size() - 1;
		delete m_entities[_destIndex];
		m_entities[_destIndex] = m_entities[_srcIndex];
		m_entities[_destIndex]->first = _destIndex;
		m_entities.pop_back();
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
		WZ_CORE_TRACE("Notifying systems");
        ComponentGroup _toUpdate;

		for (auto _system : systems.GetSystems())
		{
			if (!systems.Enabled(_system)) continue;
			uint16_t _systemUpdates = 0;
			const auto& _systemTypes = _system->GetTypeIds();
			const auto& _systemFlags = _system->GetFlags();

			bool cnt = false;
			for (int i = 0; i < _systemTypes.size(); i++)
			{
				if (m_components.find(_systemTypes[i]) == m_components.end() && !(_systemFlags[i] & System::component_flag_optional)) cnt = true;
			}
			if (cnt) continue;

			if (!_system->IsSubscribed(e.GetEventType())) continue;

			if (_system->ProcessAll())
			{

				for (const auto& _entity : m_entities)
				{

					ComponentGroup _group;

					for (auto [_id, _comp] : GetComponents(_entity))
					{
						_group.Push(_comp, _id);
					}

					_systemUpdates++;
					_system->OnEvent(e, _group);

				}

			}
			else
			{
				

				if (_systemTypes.size() == 1)
				{
					const auto& _componentType = _systemTypes[0];
					if (m_components.find(_componentType) == m_components.end()) continue;
					const auto& _typeSize = IComponent::StaticInfo(_componentType).size;
					const auto& _memPool = m_components.at(_componentType);
					for (size_t j = 0; j < _memPool.size(); j += _typeSize)
					{
						_toUpdate.Push((IComponent*)&_memPool[j], _componentType);
						_system->OnEvent(e, _toUpdate);
						_systemUpdates++;
						_toUpdate.Clear();
					}
				}
				else
				{
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
					_toUpdate.HintCount((u32)(_lcMemPool.size() / _lcTypeSize));
					bool _isValid = true;
					/* Iterate through the components of the least common of the
						system types, instead of just iterating through the first
						typegroup of components as it could be any amount of
						components. */
					for (size_t j = 0; j < _lcMemPool.size(); j += _lcTypeSize)
					{
						
						IComponent* _ofFirstType = (IComponent*)&_lcMemPool[j];
						Entity* _entity = ToRawType(_ofFirstType->entity);
						for (size_t k = 0; k < _systemTypes.size(); k++)
						{
							const auto& _systemType = _systemTypes[k];
							const auto& _systemFlag = _systemFlags[k];
							IComponent*  _entityComp = NULL;
							if (m_components.find(_systemType) != m_components.end())
							{
								_entityComp = GetComponentInternal(
									_entity,
									m_components.at(_systemType),
									_systemType
								);
							}

							if (!_entityComp &&
								!(_systemFlag & System::component_flag_optional) &&
								!(_systemFlag & System::flag_optional_min_1))
							{
								/* Entity does not have a component of a system
									component type AND that component is not optional*/
								_isValid = false;
								break;
							}

							if (_entityComp)
								_toUpdate.Push(_entityComp, _systemType);
						}

						if (_isValid)
						{
							_system->OnEvent(e, _toUpdate);
							_systemUpdates++;
							_toUpdate.Clear();
						}
					}
				}
				
			}
		}
		//JobContext ctx;
		//WZ_CORE_TRACE("Dispatching jobs");
		//JobSystem::Dispatch(ctx, systems.SystemCount(), 10, [&](u32 i, u32 groupIndex) 
		//{
		//	uint16_t _systemUpdates = 0;
		//	auto* _system = systems[i];

		//	if (!_system->IsSubscribed(e.GetEventType())) return;

		//	if (_system->ProcessAll())
		//	{

		//		for (const auto& _entity : m_entites)
		//		{

		//			ComponentGroup _group;

		//			for (auto [_id, _comp] : GetComponents(_entity))
		//			{
		//				_group.Push(_comp, _id);
		//			}

		//			_systemUpdates++;
		//			_system->OnEvent(e, _group);

		//		}

		//	}
		//	else
		//	{
		//		const auto& _systemTypes = _system->GetTypeIds();
		//		const auto& _systemFlags = _system->GetFlags();
		//		if (_systemTypes.size() == 1)
		//		{
		//			const auto& _componentType = _systemTypes[0];
		//			if (m_components.find(_componentType) == m_components.end()) return;
		//			const auto& _typeSize = IComponent::StaticInfo(_componentType).size;
		//			const auto& _memPool = m_components.at(_componentType);
		//			for (size_t j = 0; j < _memPool.size(); j += _typeSize)
		//			{
		//				_toUpdate.Push((IComponent*)&_memPool[j], _componentType);
		//				_system->OnEvent(e, _toUpdate);
		//				_systemUpdates++;
		//				_toUpdate.Clear();
		//			}
		//		}
		//		else
		//		{
		//			/* _lc = least common */
		//			uint32_t _lcIndex = FindLeastCommonComponentIndex(_systemTypes,
		//				_systemFlags);
		//			StaticCId _lcType = _systemTypes[_lcIndex];
		//			if (m_components.find(_lcType) == m_components.end()) return;
		//			const auto& _lcTypeSize = IComponent::StaticInfo(_lcType).size;
		//			auto _lcMemPool = m_components.at(_lcType);

		//			/* Hint the componentgroup about how many elemets there will be
		//				as to allocate all needed memory here instead of allocating
		//				a small amount for each component push */
		//			_toUpdate.HintCount(_lcMemPool.size() / _lcTypeSize);

		//			bool _isValid = true;
		//			/* Iterate through the components of the least common of the
		//				system types, instead of just iterating through the first
		//				typegroup of components as it could be any amount of
		//				components. */
		//			for (size_t j = 0; j < _lcMemPool.size(); j += _lcTypeSize)
		//			{
		//				IComponent* _ofFirstType = (IComponent*)&_lcMemPool[j];

		//				Entity* _entity = ToRawType(_ofFirstType->entity);
		//				for (size_t k = 0; k < _systemTypes.size(); k++)
		//				{
		//					const auto& _systemType = _systemTypes[k];
		//					if (m_components.find(_systemType) == m_components.end())
		//					{
		//						_isValid = false;
		//						break;
		//					}
		//					const auto& _systemFlag = _systemFlags[k];
		//					IComponent* _entityComp = GetComponentInternal(
		//						_entity,
		//						m_components.at(_systemType),
		//						_systemType
		//					);

		//					if (!_entityComp &&
		//						!(_systemFlag & System::FLAG_OPTIONAL) &&
		//						!(_systemFlag & System::FLAG_OPTIONAL_MIN_1))
		//					{
		//						/* Entity does not have a component of a system
		//							component type AND that component is not optional*/
		//						_isValid = false;
		//						break;
		//					}

		//					if (_entityComp)
		//						_toUpdate.Push(_entityComp, _systemType);
		//				}

		//				if (_isValid)
		//				{
		//					{
		//						std::lock_guard<std::mutex> lock(m_updateMutex);
		//						WZ_CORE_TRACE("Notifying system with matched components");
		//						_system->OnEvent(e, _toUpdate);
		//					}
		//					_systemUpdates++;
		//					_toUpdate.Clear();
		//				}
		//			}
		//		}
		//		WZ_CORE_ASSERT(_systemUpdates != 0, "Unused system '" + typestr(*_system) + "'");
		//	}
		//});
		//WZ_CORE_TRACE("Jobs dispatched, waiting");
		//JobSystem::Wait(ctx);
		//WZ_CORE_TRACE("Jobs done");
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
		// TODO: #OPTIMIZE
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
			if ((flags[i] & System::component_flag_optional) || (flags[i] & System::flag_optional_min_1)) {
				continue;
			}
			const size_t& _typeSize = IComponent::StaticInfo(types[i]).size;
			
			if (m_components.find(types[i]) != m_components.end())
			{
				auto _numComponents = m_components.at(types[i]).size() / _typeSize;

				if (_numComponents <= _minNumComponents)
				{
					_minNumComponents = _numComponents;
					_minIndex = i;
				}
			}
		}

		return _minIndex;
	}

	PropertyTable ECSManager::Serialize() const
	{
		PropertyTable table;

		PropertyTable entities;

		for (int i = 0; i < m_entities.size(); i++)
		{
			PropertyTable entity;

			for (auto& componentRef : m_entities[i]->second)
			{
				auto& id = componentRef.first;
				auto& index = componentRef.second;

				entity.Set<int32>(std::to_string(id), index);
			}

			entities.Set(std::to_string(i), entity);
		}

		table.Set<PropertyTable>("entities", entities);

		PropertyTable components;
		for (const auto& [compId, dataPool] : m_components)
		{
			string dataStr = "";

			for (byte b : dataPool)
			{
				dataStr += std::to_string(b) + " ";
			}

			if (dataStr.length() > 0)
			{
				dataStr.pop_back();
				components.Set(std::to_string(compId), dataStr);
			}
		}

		table.Set("components", components);

		return table;
	}
	void ECSManager::Deserialize(const PropertyTable& table)
	{
		// Process entities (Component references)
		if (table.Is<PropertyTable>("entities"))
		{
			WZ_CORE_TRACE("Found entities prop");
			PropertyTable entities = table.Get<PropertyTable>("entities");

			for (const auto& entityKey : entities.GetKeys())
			{
				WZ_CORE_TRACE("Processing entity of index {0}", entityKey);
				if (!entities.Is<PropertyTable>(entityKey)) continue;
				PropertyTable entity = entities.Get<PropertyTable>(entityKey);

				EntityDataRef compRefs;
				for (const auto& compKey : entity.GetKeys())
				{
					WZ_CORE_TRACE("Processing component of id {0}", compKey);
					if (!entity.Is<int32>(compKey)) continue;
					int32 id = -1;
					std::istringstream(compKey) >> id;

					if (id == -1) continue;

					int32 index = entity.Get<int32>(compKey);

					WZ_CORE_TRACE("Founnd comp reference of type {0} at index {1}", id, index);
					compRefs.push_back({ id, index });
				}

				WZ_CORE_TRACE("Creating entity {0}", m_entities.size());
				m_entities.push_back(new Entity(m_entities.size(), compRefs));
			}
		}

		// Processs components
		if (table.Is<PropertyTable>("components"))
		{
			WZ_CORE_TRACE("Found components prop");
			PropertyTable components = table.Get<PropertyTable>("components");

			for (const string& k : components.GetKeys())
			{
				if (!components.Is<string>(k)) continue;
				int32 id = -1;
				string bytesStr = components.Get<string>(k);
				std::istringstream (k) >> id;
				WZ_CORE_TRACE("Processing data pool for component type {0}", id);
				if (id != -1)
				{
					ComponentMem dataPool;
					std::stringstream ss(bytesStr);
					string byteStr;
					while (std::getline(ss, byteStr, ' '))
					{
						int32 b = -1;
						std::istringstream(byteStr) >> b;
						if (b != -1)
						{
							dataPool.push_back(b);
						}
					}
					m_components[id] = dataPool;
				}
			}
		}

		// Set component entity handles
		for (auto& entityEntry : m_entities)
		{
			for (auto& ref : entityEntry->second)
			{
				const auto& id = ref.first;
				const auto& index = ref.second;

				((IComponent*)&m_components[id][index])->entity = entityEntry;
			}
		}
	}

	void ECSManager::Clear()
	{
		for (auto e : m_entities)
		{
			auto components = e->second;

			for (auto dataRef : components)
			{
				auto id = dataRef.first;
				auto memIndex = dataRef.second;

				delete (IComponent*)(&m_components[id][memIndex]);
			}

			delete e;
		}
		m_entities.clear();
		m_components.clear();
	}

	void ECSManager::ForEachEntity(std::function<bool(EntityHandle)> callback)
	{
		for (int i = m_entities.size() - 1; i >= 0; i--)
		{
			auto hEntity = ToEntityHandle(m_entities[i]);
			if (!callback(hEntity)) break;
		}
	}
}
