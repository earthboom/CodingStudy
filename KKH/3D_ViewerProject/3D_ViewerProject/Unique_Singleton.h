#pragma once

template <typename T>
class CSingleton
{
protected:
	CSingleton(void) = default;
	CSingleton(const CSingleton&) = delete;
	CSingleton &operator=(const CSingleton&) = delete;

public:
	static T& GetInstnace(void)
	{
		std::call_once(singleton_flag, []() {m_pInstance = std::make_unique<T>(); });
		return *(m_pInstance.get());
	}

private:
	static std::once_flag singleton_flag;
	static std::unique_ptr<T> m_pInstance;
};

template<typename T> std::once_flag		CSingleton<T>::singleton_flag;
template<typename T> std::unique_ptr<T> CSingleton<T>::m_pInstance;