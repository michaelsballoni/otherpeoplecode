#pragma once

#include "include.h"
#include "loadworker.h"

namespace otherpeoplecode
{
	class Download
	{
	public:
		Download()
			: m_session(NULL)
		{
			m_session = ::WinHttpOpen(L"otherpeoplecode", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
			if (!m_session)
				throw std::exception("Fatal Error: WinHttpOpen failed, no libraries can be loaded");
		}

		~Download()
		{
			::WinHttpCloseHandle(m_session);
		}

		const char* Load(const UrlParts& parts, const std::wstring& path, unsigned int& statusCode)
		{
			LoadWorker worker(m_session);
			return worker.Load(parts, path, statusCode);
		}

	private:
		HINTERNET m_session;
	};
}
