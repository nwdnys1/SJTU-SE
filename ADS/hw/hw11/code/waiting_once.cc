#include "waiting_once_api.hpp"

void waiting_once::call_once_waiting(init_function f) // 朴素版 每次调用都需要获取互斥锁 浪费资源
{
	std::lock_guard<std::mutex> lock(mtx); // 获取互斥锁
	if (!initialized)					   // 如果未初始化
	{
		f();
		initialized = true;
	}
}

void waiting_once::call_once_waiting_plus(init_function f)
// 进阶版 无法解决刚开始时多个线程同时初始化的问题 但是在初始化完成后 后续的线程不需要获取互斥锁
{
	if (!initialized) // 如果已经初始化 不做任何事
	{
		std::lock_guard<std::mutex> lock(mtx); // 未初始化 需要进行第一次初始化 则获取互斥锁
		if (!initialized)					   // 防止多个线程同时初始化
		{
			f();
			initialized = true;
		}
	}
}