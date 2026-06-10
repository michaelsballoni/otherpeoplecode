#pragma once

#include <windows.h>
#include <winhttp.h>

#include <map>
#include <string>

#include "utils.h"

namespace otherpeoplecode
{
	// Specify the parameters for an HTTP request
	// Simplistic
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

	// Get the parameters from the response of an HTTP request
	// StatusCode is the HTTP status code
	// ErrorMessage is a detailed message about where processing the request failed
	// Returns all HTTP Headers that WinHttp got back
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

	// Single-Use HTTP request processor
	// Use one object per request, don't reuse Loader objects, 
	// this is so the cleanup stays simple
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
