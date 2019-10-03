#pragma once

#include "xcall_once.h"

template <typename T>
class Singleton
{
protected:
	Singleton(void) = default;
	Singleton(const Singleton&) = delete;
	Singleton& operator=(const Singleton&) = delete;

public:
	static T& GetInstance(void)
	{
		std::call_once(singleton_flag, []() {m_pInstance = std::make_unique<T>(); });
		return *(m_pInstance.get());
	}

private:
	static std::once_flag singleton_flag;
	static std::unique_ptr<T> m_pInstance;
};

template<typename T> std::once_flag		Singleton<T>::singleton_flag;
template<typename T> std::unique_ptr<T>	Singleton<T>::m_pInstance;