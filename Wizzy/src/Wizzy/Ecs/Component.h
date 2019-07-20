#pragma once

namespace ecs {

	typedef void* EntityHandle;
	typedef std::vector<byte> ComponentMem;
	typedef uint16_t StaticCId;
	struct IComponent;
	typedef uint32_t(*ComponentCreateFn)(ComponentMem&, EntityHandle, IComponent*);
	typedef void(*ComponentFreeFn)(IComponent*);

	struct ComponentInfo {
		const ComponentCreateFn createFn;
		const ComponentFreeFn freeFn;
		const size_t size;
        const std::string name;
	};

	struct IComponent {
	public:
		EntityHandle entity = WZ_NULL_ENTITY_HANDLE;

	public:
        template <typename TComponent>
		static StaticCId __REGISTER_COMPONENT_TYPE(ComponentCreateFn createFn,
													ComponentFreeFn freeFn);
		static const ComponentInfo& StaticInfo(StaticCId componentType);

		inline static bool IsTypeValid(StaticCId staticId) {
			return staticId < componentTypes().size();
		}

		virtual void ShowGUI() {}
	private:
		inline static std::vector<ComponentInfo>& componentTypes() {
			static std::vector<ComponentInfo> s_componentTypes;
			return s_componentTypes;
		}
	};


	template <typename TComponent>
	struct Component
		: public IComponent {
		const static ComponentCreateFn __CREATE_FN;
		const static ComponentFreeFn __FREE_FN;
		const static StaticCId staticId;
		const static size_t size;
	};

	template <typename TComponent>
	inline uint32_t __COMPONENT_CREATE(ComponentMem& memory,
									EntityHandle entity,
									IComponent *component) {
		uint32_t _index = static_cast<uint32_t>(memory.size());
		memory.resize(_index + TComponent::size);
		TComponent *_c = nullptr;
        _c = new(&memory[_index]) TComponent(*static_cast<TComponent*>(component));

		_c->entity = entity;
		return _index;
	}

	template <typename TComponent>
	inline void __COMPONENT_FREE(IComponent *component) {
		TComponent *_c = static_cast<TComponent*>(component);
		_c->~TComponent();
	}

	template <typename TComponent>
	const StaticCId Component<TComponent>::staticId(
		IComponent::__REGISTER_COMPONENT_TYPE<TComponent> (
			__COMPONENT_CREATE<TComponent>,
			__COMPONENT_FREE<TComponent>
		)
	);

    template <typename TComponent>
    inline StaticCId IComponent::__REGISTER_COMPONENT_TYPE(ComponentCreateFn createFn,
													ComponentFreeFn freeFn) {
		componentTypes().push_back({
			createFn,
			freeFn,
			sizeof(TComponent),
            typestr(TComponent)
		});

		return static_cast<StaticCId>(componentTypes().size() - 1);
	}

	template <typename TComponent>
	const size_t Component<TComponent>::size(sizeof(TComponent));

	template <typename TComponent>
	const ComponentCreateFn Component<TComponent>::__CREATE_FN(__COMPONENT_CREATE<TComponent>);

	template <typename TComponent>
	const ComponentFreeFn Component<TComponent>::__FREE_FN(__COMPONENT_FREE<TComponent>);

	class ComponentGroup {
	public:

		template <typename TComponent>
		inline bool Has() {
			return Has(TComponent::staticId);
		}

		inline bool Has(const StaticCId& type) {
			bool _has = m_componentHash[type] != nullptr;
			if (!_has) m_componentHash.erase(type);
			return _has;
		}

		template <typename TComponent>
		inline TComponent* Get() {
			return static_cast<TComponent*>(Get(TComponent::staticId));
		}

		inline IComponent* Get(const StaticCId& type) {
			WZ_CORE_ASSERT(this->Has(type), "Tried getting component that doesn't exist in component group");
			return m_componentHash.at(type);
		}

		inline void Push(IComponent* component, const StaticCId& type) {
			WZ_CORE_ASSERT(component != nullptr, "Null argument in Push(IComponent*, StaticCId)");
			m_componentHash[type] = component;
			if (type == 0) {
				m_has0 = true;
			}
		}

        inline void Clear() {
            m_componentHash.clear();
            m_has0 = false;
        }

        inline void HintCount(uint32_t count) {
            m_componentHash.reserve(static_cast<size_t>(count));
        }

		inline std::unordered_map<StaticCId, IComponent*>& GetComponents() {
			return m_componentHash;
		}

	private:
		std::unordered_map<StaticCId, IComponent*> m_componentHash;
		bool m_has0 = false;
	};
}
