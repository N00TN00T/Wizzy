#pragma once

#define MAX_NUM_COMPONENT_TYPES	512

namespace Wizzy {

	typedef void* EntityHandle;
	typedef std::vector<byte> ComponentMem;
	typedef uint16_t ComponentId;
	struct IComponent;
	typedef u64(*ComponentCreateFn)(ComponentMem&, EntityHandle);
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
		virtual ~IComponent() {}

        template <typename TComponent>
		static ComponentId __REGISTER_COMPONENT_TYPE(ComponentCreateFn createFn,
													ComponentFreeFn freeFn);
		static const ComponentInfo& StaticInfo(ComponentId componentType);

		inline static bool IsTypeValid(ComponentId type) {
			return type <= componentTypes().size() && type > 0;
		}

		inline static ComponentId GetNumRegisteredTypes() { return componentTypes().size(); }

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
		virtual ~Component() {}
		const static ComponentCreateFn __CREATE_FN;
		const static ComponentFreeFn __FREE_FN;
		inline static ComponentId typeId()
		{
			static ComponentId id = IComponent::__REGISTER_COMPONENT_TYPE<TComponent> 
			(
				__COMPONENT_CREATE<TComponent>,
				__COMPONENT_FREE<TComponent>
			);
			return id;
		}
		const static size_t size;
	};

	template <typename TComponent>
	inline u64 __COMPONENT_CREATE(ComponentMem& memory, EntityHandle entity) 
	{
		u64 index = static_cast<u64>(memory.size());
		memory.resize(index + TComponent::size);

		TComponent* component = new(&memory[index]) TComponent();

		component->entity = entity;
		return index;
	}

	template <typename TComponent>
	inline void __COMPONENT_FREE(IComponent *component) {
		TComponent *_c = static_cast<TComponent*>(component);
		_c->~TComponent();
	}

    template <typename TComponent>
    inline ComponentId IComponent::__REGISTER_COMPONENT_TYPE(ComponentCreateFn createFn,
													ComponentFreeFn freeFn) {
		componentTypes().push_back({
			createFn,
			freeFn,
			sizeof(TComponent),
            typestr(TComponent)
		});

		

		return static_cast<ComponentId>(componentTypes().size());
	}

	template <typename TComponent>
	const size_t Component<TComponent>::size(sizeof(TComponent));

	template <typename TComponent>
	const ComponentCreateFn Component<TComponent>::__CREATE_FN(__COMPONENT_CREATE<TComponent>);

	template <typename TComponent>
	const ComponentFreeFn Component<TComponent>::__FREE_FN(__COMPONENT_FREE<TComponent>);
}
