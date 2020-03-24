#pragma once

namespace Wizzy
{
	template <typename T, u64 capacity>
	class ThreadSafeBuffer
	{
	public:
		bool PushBack(const T& item);
		bool PopFront(T& item);

	private:
		T m_data[capacity];
		u64 m_head = 0;
		u64 m_tail = 0;
		std::mutex mutex;
	};
	template<typename T, u64 capacity>
	inline bool ThreadSafeBuffer<T, capacity>::PushBack(const T& item)
	{
		bool result = false;
		mutex.lock();

		u64 next = (m_head + 1) % capacity;
		if (next != m_tail)
		{
			m_data[m_head] = item;
			m_head = next;
			result = true;
		}

		mutex.unlock();
		return result;
	}
	template<typename T, u64 capacity>
	inline bool ThreadSafeBuffer<T, capacity>::PopFront(T& item)
	{
		bool result = false;
		mutex.lock();

		if (m_tail != m_head)
		{
			item = m_data[m_tail];
			m_tail = (m_tail + 1) % capacity;
			result = true;
		}

		mutex.unlock();
		return result;
	}
}