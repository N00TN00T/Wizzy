#include "spch.h"

#include "JobSystemTest.h"

void Spin(float milliseconds)
{
	milliseconds /= 1000.0f;
	std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
	double ms = 0;
	while (ms < milliseconds)
	{
		std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
		ms = time_span.count();
	}
}

struct timer
{
	string name;
	std::chrono::high_resolution_clock::time_point start;

	timer(const string& name) : name(name), start(std::chrono::high_resolution_clock::now()) {}
	~timer()
	{
		auto end = std::chrono::high_resolution_clock::now();
		WZ_DEBUG("{0}: {1} ms", name, std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());
	}
};

void JobSystemTest::Init()
{
	wz::Log::SetCoreLogLevel(LOG_LEVEL_DEBUG);
	wz::Log::SetClientLogLevel(LOG_LEVEL_TRACE);

	// Serial test
	{
		auto t = timer("Serial test: ");
		Spin(100);
		Spin(100);
		Spin(100);
		Spin(100);
		Spin(100);
		Spin(100);
		Spin(100);
		Spin(100);
		Spin(100);
		Spin(100);
		Spin(100);
		Spin(100);
	}

	wz::JobContext ctx;
	// Execute test
	{
		auto t = timer("Execute() test: ");
		wz::JobSystem::Execute(ctx, [] { Spin(100); });
		wz::JobSystem::Execute(ctx, [] { Spin(100); });
		wz::JobSystem::Execute(ctx, [] { Spin(100); });
		wz::JobSystem::Execute(ctx, [] { Spin(100); });
		wz::JobSystem::Execute(ctx, [] { Spin(100); });
		wz::JobSystem::Execute(ctx, [] { Spin(100); });
		wz::JobSystem::Execute(ctx, [] { Spin(100); });
		wz::JobSystem::Execute(ctx, [] { Spin(100); });
		wz::JobSystem::Execute(ctx, [] { Spin(100); });
		wz::JobSystem::Execute(ctx, [] { Spin(100); });
		wz::JobSystem::Execute(ctx, [] { Spin(100); });
		wz::JobSystem::Execute(ctx, [] { Spin(100); });
		wz::JobSystem::Wait(ctx);
	}
	WZ_DEBUG("DONE");
}

void JobSystemTest::Shutdown()
{

}