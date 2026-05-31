#pragma once

#include "urlparts.h"

namespace otherpeoplecode
{
	class HttpResponse
	{
	public:
		HttpResponse()
		{}

		HttpResponse(const std::wstring& outputFilePath) 
			: OutputFilePath(outputFilePath)
		{}

		std::wstring ErrorMessage;
		std::wstring OutputFilePath;
		DWORD StatusCode = 0;
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

	class LoadWorker
	{
	public:
		LoadWorker(HINTERNET session)
			: m_session(session)
		{}

		~LoadWorker()
		{
			::WinHttpCloseHandle(m_connection);
			::WinHttpCloseHandle(m_request);

			if (m_file != nullptr)
			{
				::fclose(m_file);
				m_file = nullptr;
			}
		}

		HttpResponse Load(HttpRequest request);

	private:
		static std::map<std::wstring, std::wstring> GetResponseHeaders(HINTERNET request, const std::vector<std::wstring>& headerNames);

	private:
		HINTERNET m_session = nullptr;
		HINTERNET m_connection = nullptr;
		HINTERNET m_request = nullptr;

		FILE* m_file = nullptr;
	};
}
