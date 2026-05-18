#pragma once

#include "include.h"
#include "loadworker.h"

namespace otherpeoplecode
{
	class Loader
	{
	public:
		Loader()
			: m_session(NULL)
		{
			m_session = ::WinHttpOpen(L"otherpeoplecode", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
			if (!m_session)
				throw std::exception("Fatal Error: WinHttpOpen failed, no libraries can be loaded");
		}

		~Loader()
		{
			::WinHttpCloseHandle(m_session);
		}

		const char* Load(const UrlParts& parts, const std::wstring& path)
		{
			LoadWorker worker(m_session);
			return worker.Load(parts, path);
		}

	private:
		HINTERNET m_session;
	};
}
