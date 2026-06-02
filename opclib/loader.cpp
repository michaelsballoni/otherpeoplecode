#include "pch.h"
#include "loader.h"
#include "urlparts.h"

namespace otherpeoplecode
{
	// helper function to take load off Load
	std::map<std::wstring, std::wstring> GetResponseHeaders(HINTERNET request, const std::vector<std::wstring>& headerNames);

	HttpResponse Loader::Load(HttpRequest request)
	{
		HttpResponse response(request.OutputFilePath);

		UrlParts url_parts;
		std::wstring url_parts_error = UrlParts::Parse(request.Url, url_parts);
		if (!url_parts_error.empty())
			return response.OnErr(url_parts_error);

#pragma warning(push)
#pragma warning(disable : 4996) // _CRT_SECURE_NO_WARNINGS
		m_file = ::_wfopen(response.OutputFilePath.c_str(), L"wb");
		if (!m_file)
			return response.OnErr(L"fopen");
#pragma warning(pop)

		m_connection = 
			::WinHttpConnect
			(
				m_session, 
				url_parts.server.c_str(),
				url_parts.port,
				0
			);
		if (m_connection == nullptr)
			return response.OnErr(L"connect");

		const wchar_t* types[]{ L"*/*", nullptr };
		m_request = 
			::WinHttpOpenRequest
			(
				m_connection, 
				request.HttpVerb.c_str(), 
				url_parts.request.c_str(),
				nullptr, 
				WINHTTP_NO_REFERER, 
				(LPCWSTR*)&types, 
				url_parts.port == 443 ? WINHTTP_FLAG_SECURE : 0
			);
		if (m_request == nullptr)
			return response.OnErr(L"open_request");

		DWORD_PTR ptr = 0;
		if
		(
			!::WinHttpSendRequest
			(
				m_request, 
				WINHTTP_NO_ADDITIONAL_HEADERS, 
				0, 
				WINHTTP_NO_REQUEST_DATA, 
				0, 
				0, 
				ptr
			)
		)
		{
			return response.OnErr(L"send_request");
		}
		if (!::WinHttpReceiveResponse(m_request, nullptr))
			return response.OnErr(L"receive_response");

		DWORD status_size = sizeof(DWORD);
		if
		(
			!::WinHttpQueryHeaders
			(
				m_request, 
				WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER, 
				NULL, 
				&response.StatusCode, 
				&status_size, 
				NULL
			)
		)
		{
			return response.OnErr(L"status_code");
		}
		response.Headers = GetResponseHeaders(m_request, { L"Content-Length", L"Last-Modified", L"ETag" });

		DWORD dwSize = 0;
		std::vector<uint8_t> buffer;
		do {
			if (!::WinHttpQueryDataAvailable(m_request, &dwSize))
				return response.OnErr(L"query_data_available");
			if (dwSize == 0)
				break;

			buffer.resize(dwSize);
			memset(buffer.data(), 0, buffer.size());

			DWORD dwDownloaded = 0;
			if (!::WinHttpReadData(m_request, buffer.data(), dwSize, &dwDownloaded))
				return response.OnErr(L"read_data");

			size_t wrote_size = ::fwrite(buffer.data(), 1, buffer.size(), m_file);
			if (wrote_size != buffer.size())
				return response.OnErr(L"fwrite");
		} while (dwSize > 0);

		::fclose(m_file);
		m_file = nullptr;
		return response;
	}

	std::map<std::wstring, std::wstring> GetResponseHeaders(HINTERNET request, const std::vector<std::wstring>& headerNames)
	{
		std::map<std::wstring, std::wstring> output;
		DWORD dwSize = 0;
		WinHttpQueryHeaders
		(
			request,
			WINHTTP_QUERY_RAW_HEADERS_CRLF,
			WINHTTP_HEADER_NAME_BY_INDEX,
			NULL,
			&dwSize,
			WINHTTP_NO_HEADER_INDEX
		);
		if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
			return output;

		std::wstring value(dwSize, 0);
		BOOL result =
			WinHttpQueryHeaders
			(
				request,
				WINHTTP_QUERY_RAW_HEADERS_CRLF,
				WINHTTP_HEADER_NAME_BY_INDEX,
				value.data(), &dwSize,
				WINHTTP_NO_HEADER_INDEX
			);
		if (GetLastError() == ERROR_WINHTTP_HEADER_NOT_FOUND)
			return output;

		auto headers = Utils::Split(value, L"\r\n");
		for (const auto& header : headers)
		{
			auto parts = Utils::Split(header, L":");
			if (parts.size() != 2)
				continue;

			std::wstring name = Utils::ToLower(Utils::Trim(parts[0]));
			std::wstring value = Utils::Trim(parts[1]);

			output[name] = value; // last wins
		}

		return output;
	}
}
