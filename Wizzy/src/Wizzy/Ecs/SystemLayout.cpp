#include "wzpch.h"

#include "Wizzy/Ecs/SystemLayout.h"

namespace Wizzy
{



    struct TestComponent1 : Component<TestComponent1>
	{

	};
	struct TestComponent2 : Component<TestComponent2>
	{

	};

	class TestSystem : public System<TestSystem, TestComponent1, TestComponent2>
	{
	public:
		TestSystem()
		{
			Subscribe(EventType::app_frame_begin);

			Flag<TestComponent2>(flag_optional);
		}

		bool ProcessComponents(const Event& e, TestComponent1* c1, TestComponent2* c2) const override
		{
			
			return false;
		}
	};

    class TestSystem2 : public System<TestSystem, TestComponent1>
	{
	public:
		TestSystem2()
		{
			Subscribe(EventType::app_frame_begin);
		}

		bool ProcessComponents(const Event& e, TestComponent1* c1) const override
		{
			return false;
		}
	};

	void Test()
	{
		TestSystem::GetInstance();
		TestSystem::GetComponentPass();
		TestSystem::GetInstance()->IsSubscribed(EventType::app_frame_begin);
		TestSystem::GetInstance()->ProcessComponentsPass(AppRenderEvent(), TestSystem::GetComponentPass());
		TestSystem::GetInstance()->HasType(TestComponent2::typeId());
		TestSystem::GetInstance()->Flag<TestComponent2>(TestSystem::flag_optional);
		TestSystem::GetInstance()->HasTypeFlag(TestComponent2::typeId(), TestSystem::flag_optional);
		TestSystem::GetInstance()->GetFullSignature();

        TestSystem2::GetInstance();
		TestSystem2::GetComponentPass();
		TestSystem2::GetInstance()->IsSubscribed(EventType::app_frame_begin);
		TestSystem2::GetInstance()->ProcessComponentsPass(AppRenderEvent(), TestSystem::GetComponentPass());
		TestSystem2::GetInstance()->HasType(TestComponent2::typeId());
		TestSystem2::GetInstance()->Flag<TestComponent2>(TestSystem::flag_optional);
		TestSystem2::GetInstance()->HasTypeFlag(TestComponent2::typeId(), TestSystem::flag_optional);
		TestSystem2::GetInstance()->GetFullSignature();

        SystemLayer layer;
        layer.Push<TestSystem>();
        layer.Push<TestSystem2>(false);
        layer.Push<TestSystem>();
        layer.Pop();

        layer.Insert<TestSystem2>(0);
        layer.RemoveFirst<TestSystem>();
        layer.Remove(0);

        layer.Push<TestSystem>();

        layer.SetEnabled<TestSystem>(false);
        layer.SetEnabled<TestSystem2>(true);

        layer.IndexOfFirst<TestSystem2>();
        layer.IndexOfLast<TestSystem2>();

        layer.IsEnabled<TestSystem>();
        layer.IsEnabled<TestSystem2>();

        layer.Has<TestSystem>();
        layer.Has<TestSystem2>();

        layer.Is<TestSystem>(1);
        layer.Is<TestSystem2>(0);

        layer.Count<TestSystem>();
        layer.Count<TestSystem2>();

		EcsInstance ecs;

		ecs.ProcessLayout(layer, AppUpdateEvent(5));

		ecs.ProcessSystem<TestSystem>(AppUpdateEvent(5));
	}
}