#ifndef FUNCTOR_H
#define FUNCTOR_H

// for vector or list
class CRelease_Single
{
public:
	explicit CRelease_Single(void) {}
	~CRelease_Single(void) {}
public:
	template <typename T> void operator () (T& Pointer)
	{
		if (Pointer != NULL)
		{
			Pointer->Release();
			Pointer = NULL;
		}
	}
};

// for map
class CRelease_Pair
{
public:
	explicit CRelease_Pair(void) {}
	~CRelease_Pair(void) {}
public:
	template <typename T> void operator () (T& Pair)
	{
		if (Pair.second != NULL)
		{
			Pair.second->Release();
			Pair.second = NULL;
		}
	}
};



class CTag_Finder
{
public:
	explicit CTag_Finder(const TCHAR* pTag) : m_pTargetTag(pTag) {}
	~CTag_Finder(void) {}
public:
	template <typename T> BOOL operator () (const T& Pair)
	{
		if (0 == lstrcmp(m_pTargetTag, Pair.first))
			return true;
		return false;
	}
private:
	const TCHAR*			m_pTargetTag;
};



class CNumFinder
{
public:
	explicit CNumFinder(UINT uiNum) : m_uiNum(uiNum) {}
	~CNumFinder(void) {}
public:
	template <typename T> BOOL operator () (T& Pair)
	{
		if (m_uiNum == Pair.first)
			return TRUE;

		return false;
	}
private:
	UINT				m_uiNum;
};

#endif