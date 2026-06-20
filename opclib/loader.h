#pragma once

#include <windows.h>
#include <winhttp.h>

#include <map>
#include <string>

#include "utils.h"

namespace otherpeoplecode
{
	// Store the parameters for an HTTP request 
	// and where to write the response and trace output.
	class HttpRequest
	{
	public:
		HttpRequest
		(
			const std::wstring& url, 
			const std::wstring httpVerb, 
			const std::wstring& outputFilePath, 
			FILE* progressLog = nullptr
		)
		: Url(url)
		, HttpVerb(httpVerb)
		, OutputFilePath(outputFilePath)
		, ProgressLog(progressLog)
		{}

		std::wstring Url;
		std::wstring HttpVerb;
		std::wstring OutputFilePath;
		FILE* ProgressLog{};
	};

	// Store the parameters from the response of an HTTP request.
	// StatusCode is the HTTP status code.
	// ErrorMessage tells where processing failed.
	// Returns all HTTP response headers.
	class HttpResponse
	{
	public:
		HttpResponse(const HttpRequest& request)
		: OutputFilePath(request.OutputFilePath)
		, ProgressLog(request.ProgressLog)
		{}

		std::wstring OutputFilePath;
		FILE* ProgressLog;

		DWORD HttpStatusCode{};
		std::wstring ErrorMessage;
		
		std::map<std::wstring, std::wstring> Headers;

		HttpResponse& OnErr(const std::wstring& msg)
		{
			OPCLOG(ProgressLog, "HttpResponse ERROR: %d %ls", (int)HttpStatusCode, msg.c_str());
			ErrorMessage = msg;
			return *this;
		}
	};

	// Single-Use HTTP request processor.
	// Use one object per request; don't reuse Loader objects.
	// This is so cleanup is simple.
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
				::fclose(m_file);
		}

		HttpResponse Load(HttpRequest request);

	private:
		HINTERNET m_session = nullptr;
		HINTERNET m_connection = nullptr;
		HINTERNET m_request = nullptr;

		FILE* m_file = nullptr;
	};
}
