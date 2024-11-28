#include "waiting_once_api.hpp"

void waiting_once::call_once_waiting(init_function f)
{
	if (!this->is_init.test(std::memory_order::acquire)) [[unlikely]]
	{
		std::lock_guard guard(this->mu);
		if (!this->is_init.test(std::memory_order::acquire))
		{
			f();
			this->is_init.test_and_set(std::memory_order::release);
		}
	}
}
