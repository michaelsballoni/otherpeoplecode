#include "pch.h"
#include "download.h"
#include "loadworker.h"

namespace otherpeoplecode
{
	Download::Download()
		: m_session(NULL)
	{
		m_session = ::WinHttpOpen(L"otherpeoplecode", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
		if (!m_session)
			throw std::exception("Fatal Error: WinHttpOpen failed, no libraries can be loaded");
	}

	Download::~Download()
	{
		::WinHttpCloseHandle(m_session);
	}

	const char* Download::Load(const UrlParts& parts, const std::wstring& path, DWORD& statusCode)
	{
		LoadWorker worker(m_session);
		return worker.Load(parts, path, statusCode);
	}
}
