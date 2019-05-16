#pragma once

namespace Wizzy {

	typedef void* EntityHandle;
	typedef std::vector<byte> ComponentMem;
	typedef u16 StaticCId;
	typedef u64 DynamicCId;
	struct IComponent;
	typedef u32(*ComponentCreateFn)(ComponentMem&, EntityHandle, IComponent*);
	typedef void(*ComponentFreeFn)(IComponent*);

	struct ComponentInfo {
		const ComponentCreateFn createFn;
		const ComponentFreeFn freeFn;
		const size_t size;
	};

	struct IComponent {
	public:
		EntityHandle entity = WZ_NULL_ENTITY_HANDLE;

	public:
		static StaticCId __REGISTER_COMPONENT_TYPE(ComponentCreateFn createFn,
													ComponentFreeFn freeFn,
													size_t size);
		static const ComponentInfo& StaticInfo(StaticCId componentType);

		inline static bool IsTypeValid(StaticCId staticId) {
			return staticId < componentTypes().size();
		}
	private:
		inline static std::vector<ComponentInfo>& componentTypes() {
			static std::vector<ComponentInfo> s_componentTypes;
			return s_componentTypes;
		}
	};


	template <typename TComponent>
	struct Component
		: public IComponent{
		const static ComponentCreateFn __CREATE_FN;
		const static ComponentFreeFn __FREE_FN;
		const static StaticCId staticId;
		const static size_t size;
	};

	template <typename TComponent>
	inline u32 __COMPONENT_CREATE(ComponentMem& memory,
									EntityHandle entity,
									IComponent *component) {
		u32 _index = static_cast<u32>(memory.size());
		memory.resize(_index + TComponent::size);
		TComponent *_c = nullptr;
		try {
			_c = new(&memory[_index]) TComponent(*static_cast<TComponent*>(component));
		} catch (const std::bad_alloc& e) {
			WZ_CORE_ASSERT(false, "BAD ALLOC: '{0}'", e.what());
		}

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
		IComponent::__REGISTER_COMPONENT_TYPE(
			__COMPONENT_CREATE<TComponent>,
			__COMPONENT_FREE<TComponent>,
			sizeof(TComponent)
		)
	);

	template <typename TComponent>
	const size_t Component<TComponent>::size(sizeof(TComponent));

	template <typename TComponent>
	const ComponentCreateFn Component<TComponent>::__CREATE_FN(__COMPONENT_CREATE<TComponent>);

	template <typename TComponent>
	const ComponentFreeFn Component<TComponent>::__FREE_FN(__COMPONENT_FREE<TComponent>);
}
