#include "pch.h"
#include "load.h"
#include "loadworker.h"

namespace otherpeoplecode
{
	Loader::Loader()
		: m_session(NULL)
	{
		m_session = ::WinHttpOpen(L"otherpeoplecode", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
		if (!m_session)
			throw std::exception("Fatal Error: WinHttpOpen failed, no libraries can be loaded");
	}

	Loader::~Loader()
	{
		::WinHttpCloseHandle(m_session);
	}

	const char* Loader::Load(const UrlParts& parts, const std::wstring& path, DWORD& statusCode)
	{
		LoadWorker worker(m_session);
		return worker.Load(parts, path, statusCode);
	}
}
