#include "wzpch.h"

#include "JobSystem.h"

namespace Wizzy
{
	u32 JobSystem::s_threadCount(0);
	ThreadSafeBuffer<Job, WZ_MAX_JOBS> JobSystem::s_jobQueue;
	std::condition_variable JobSystem::s_wakeCondition;
	std::mutex JobSystem::s_wakeMutex;

	void JobSystem::Init()
	{
		auto nCores = std::thread::hardware_concurrency();

		s_threadCount = std::max(1u, nCores - 1);

		for (u32 tId = 0; tId < s_threadCount; tId++)
		{
			std::thread worker([]() {
			
				Task job;

				while (true)
				{
					if (!Work())
					{
						std::unique_lock<std::mutex> lock(s_wakeMutex);
						s_wakeCondition.wait(lock);
					}
				}
			});

			worker.detach();
		}

		WZ_CORE_INFO("Job system intialized ({0} cores, {1} worker threads)", nCores, s_threadCount);
	}
	void JobSystem::Execute(JobContext& context, const Task& job)
	{
		context.counter.fetch_add(1);

		while (!s_jobQueue.PushBack({ job, &context }))
		{
			WZ_CORE_WARN("Job system overflow!");
			s_wakeCondition.notify_all();
		}

		s_wakeCondition.notify_one();
	}
	void JobSystem::Dispatch(JobContext& context, u32 jobCount, u32 groupSize, const MultiTask& job)
	{
		if (jobCount == 0 || groupSize == 0)
		{
			return;
		}

		u32 groupCount = (jobCount + groupSize - 1) / groupSize;

		context.counter.fetch_add(groupCount);

		for (u32 groupIndex = 0; groupIndex < groupCount; groupIndex++)
		{
			const auto& jobGroup = [groupIndex, groupSize, jobCount, job]()
			{
				u32 groupJobOffset = groupIndex * groupSize;
				u32 groupJobEnd = std::min(groupJobOffset + groupSize, jobCount);

				for (u32 i = groupJobOffset; i < groupJobEnd; i++)
				{
					job(i, groupIndex);
				}
			};

			while (!s_jobQueue.PushBack({ jobGroup, &context }))
			{
				s_wakeCondition.notify_all();
			}
		}
		s_wakeCondition.notify_all();
	}
	bool JobSystem::IsBusy(const JobContext& context)
	{
		return context.counter.load() > 0;
	}
	void JobSystem::Wait(const JobContext& context)
	{
		s_wakeCondition.notify_all();

		while (IsBusy(context)) { Work(); }
	}
	bool JobSystem::Work()
	{
		Job job;
		if (s_jobQueue.PopFront(job))
		{
			job.task();
			job.context->counter.fetch_sub(1);
			return true;
		}
		return false;
	}
}