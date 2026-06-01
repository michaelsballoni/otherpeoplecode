#pragma once

#include "urlparts.h"

#include <windows.h>
#include <winhttp.h>

#include <map>

namespace otherpeoplecode
{
	class HttpResponse
	{
	public:
		HttpResponse(const std::wstring& outputFilePath)
			: OutputFilePath(outputFilePath)
		{}

		DWORD StatusCode = 0;
		std::wstring ErrorMessage;
		std::wstring OutputFilePath;
		std::map<std::wstring, std::wstring> Headers;

		HttpResponse& OnErr(const std::wstring& msg)
		{
			ErrorMessage = msg;
			return *this;
		}
	};

	class HttpRequest
	{
	public:
		HttpRequest(const UrlParts& urlParts, const std::wstring httpVerb, const std::wstring& outputFilePath)
			: UrlParts(urlParts)
			, HttpVerb(httpVerb)
			, OutputFilePath(outputFilePath)
		{}

		UrlParts UrlParts;
		std::wstring HttpVerb;
		std::wstring OutputFilePath;
	};

	class Loader
	{
	public:
		Loader()
		{
			m_session = ::WinHttpOpen(L"otherpeoplecode", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
			if (!m_session)
				throw std::exception("Fatal Error: WinHttpOpen failed; no libraries can be loaded");
		}

		~Loader()
		{
			::WinHttpCloseHandle(m_request);
			::WinHttpCloseHandle(m_connection);
			::WinHttpCloseHandle(m_session);

			if (m_file != nullptr)
			{
				::fclose(m_file);
				m_file = nullptr;
			}
		}

		HttpResponse Load(HttpRequest request);

	private:
		HINTERNET m_session = nullptr;
		HINTERNET m_connection = nullptr;
		HINTERNET m_request = nullptr;

		FILE* m_file = nullptr;
	};
}
