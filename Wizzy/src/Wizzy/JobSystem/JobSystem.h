#pragma once
#include "ThreadSafeBuffer.h"

namespace Wizzy
{
	typedef std::function<void()> Task;
	typedef std::function<void(u32 jobIndex, u32 groupIndex)> MultiTask;

	struct JobContext
	{
		std::atomic<u32> counter{ 0 };
	};

	struct Job
	{
		Task task;
		JobContext* context;
	};

	class JobSystem
	{
	public:
		static void Init();

		inline static u32 GetThreadCount() { return s_threadCount; };

		static void Execute(JobContext& context, const Task& job);

		static void Dispatch(JobContext& context, u32 jobCount, u32 groupSize, const MultiTask& job);

		static bool IsBusy(const JobContext& context);

		static void Wait(const JobContext& context);

	private:
		static bool Work();

	private:
		static u32 s_threadCount;
		static ThreadSafeBuffer<Job, WZ_MAX_JOBS> s_jobQueue;
		static std::condition_variable s_wakeCondition;
		static std::mutex s_wakeMutex;
	};
}