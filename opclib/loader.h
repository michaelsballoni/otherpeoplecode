#pragma once

#include <windows.h>
#include <winhttp.h>

#include <map>
#include <string>

#include "utils.h"

namespace otherpeoplecode
{
	class HttpRequest
	{
	public:
		HttpRequest(const std::wstring& url, const std::wstring httpVerb, const std::wstring& outputFilePath)
			: Url(url)
			, HttpVerb(httpVerb)
			, OutputFilePath(outputFilePath)
		{}

		std::wstring Url;
		std::wstring HttpVerb;
		std::wstring OutputFilePath;
	};

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

		/* FORNOW - Unused 
		bool DoHeadersMatch(const std::map<std::wstring, std::wstring>& headersToCompare)
		{
			for (auto compare_it : headersToCompare)
			{
				auto header_it = Headers.find(Utils::ToLower(compare_it.first));
				if (header_it == Headers.end() || header_it->second != compare_it.second)
					return false;
			}
			return true;
		}
		*/
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
