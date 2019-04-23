#pragma once

#include <comdef.h>
#include <fstream>
#include "Const.h"

class DxException
{
public:
	explicit DxException(void) = default;
	DxException(HRESULT hr, const std::wstring& funcname, const std::wstring& filename, int lineNum)
		: ErrorCode(hr), FunctionName(funcname), FileName(filename), LineNumber(lineNum)
	{}

	std::wstring ToString() const
	{
		//Get the string description of the error code.
		_com_error err(ErrorCode);
		std::wstring msg = err.ErrorMessage();

		return FunctionName + L" failed in " + FileName + L"; line " + std::to_wstring(LineNumber) + L"; error : " + msg;
	}

	HRESULT ErrorCode = S_OK;
	std::wstring FunctionName;
	std::wstring FileName;
	int LineNumber = -1;
};

template<typename T>
inline bool ReleaseCom(T p)
{
	if (p != nullptr)
	{
		p->Release();
		p = nullptr;
		return true;
	}

	return false;
}

inline std::wstring AnsiToWString(const std::string& str)
{
	WCHAR buf[512] = { 0, };
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, buf, 512);
	return std::wstring(buf);
}


#ifndef ThrowIfFailed
#define ThrowIfFailed(x)\
{\
	HRESULT hr__ = (x);\
	std::wstring wfn = AnsiToWString(__FILE__);\
	if (FAILED(hr__)) { throw DxException(hr__, L#x, wfn, __LINE__);}\
}
// #는 인자를 문자열화 시켜주는 연산자
// https://docs.microsoft.com/en-us/cpp/preprocessor/stringizing-operator-hash?view=vs-2017
#endif

//template<typename T>
//inline bool ThrowIfFailed(T p)
//{
//	HRESULT hr__ = (p);
//	std::wstring wfn = AnsiToWString(__FILE__);
//	if (FAILED(hr__))
//	{
//		throw DxException(hr__, Lp, wfn, __LINE__);
//	}
//}


inline const float AspectRatio(void) { return static_cast<float>(WINSIZE_X) / WINSIZE_Y; }