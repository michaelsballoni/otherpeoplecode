#include "pch.h"
#include "loader.h"
#include "urlparts.h"

namespace otherpeoplecode
{
	HttpResponse Loader::Load(HttpRequest request)
	{
		HttpResponse response(request);
		OPCLOG(response.ProgressLog, "Load... ");

		UrlParts url_parts;
		std::wstring url_parts_error = UrlParts::Parse(request.Url, url_parts);
		if (!url_parts_error.empty())
			return response.OnErr(url_parts_error);

		bool read_response_payload = request.HttpVerb != L"HEAD";
		if (read_response_payload)
		{
			OPCLOG(response.ProgressLog, "Load: Opening output file: %ls", response.OutputFilePath.c_str());
#pragma warning(push)
#pragma warning(disable : 4996) // _CRT_SECURE_NO_WARNINGS
			m_file = ::_wfopen(response.OutputFilePath.c_str(), L"wb");
			if (!m_file)
				return response.OnErr(L"fopen");
#pragma warning(pop)
		}
		else
			OPCLOG(response.ProgressLog, "Load: Not reading response body for verb %ls", request.HttpVerb.c_str());

		OPCLOG(response.ProgressLog, "Load: Connecting...");
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

		OPCLOG(response.ProgressLog, "Load: Opening...");
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

		OPCLOG(response.ProgressLog, "Load: Sending request...");
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

		OPCLOG(response.ProgressLog, "Load: Receiving response...");
		if (!::WinHttpReceiveResponse(m_request, nullptr))
			return response.OnErr(L"receive_response");

		OPCLOG(response.ProgressLog, "Load: Reading status code...");
		DWORD status_size = sizeof(DWORD);
		if
		(
			!::WinHttpQueryHeaders
			(
				m_request, 
				WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER, 
				NULL, 
				&response.HttpStatusCode, 
				&status_size, 
				NULL
			)
		)
		{
			return response.OnErr(L"status_code");
		}
		OPCLOG(response.ProgressLog, "Load: Status code: %d", (int)response.HttpStatusCode);

		OPCLOG(response.ProgressLog, "Load: Reading all headers (size)...");
		DWORD dwHeadersSize = 0;
		::WinHttpQueryHeaders
		(
			m_request,
			WINHTTP_QUERY_RAW_HEADERS_CRLF,
			WINHTTP_HEADER_NAME_BY_INDEX,
			NULL,
			&dwHeadersSize,
			WINHTTP_NO_HEADER_INDEX
		);
		if (::GetLastError() != ERROR_INSUFFICIENT_BUFFER)
			return response.OnErr(L"query_headers");

		OPCLOG(response.ProgressLog, "Load: Reading all headers...");
		std::wstring headers_value(dwHeadersSize, 0);
		if
		(
			!::WinHttpQueryHeaders
			(
				m_request,
				WINHTTP_QUERY_RAW_HEADERS_CRLF,
				WINHTTP_HEADER_NAME_BY_INDEX,
				headers_value.data(), 
				&dwHeadersSize,
				WINHTTP_NO_HEADER_INDEX
			)
		)
		{
			return response.OnErr(L"get_headers");
		}
		if (::GetLastError() == ERROR_WINHTTP_HEADER_NOT_FOUND)
			return response.OnErr(L"headers_not_found");

		OPCLOG(response.ProgressLog, "Load: Processing headers...");
		auto headers_list = Utils::Split(headers_value, L"\r\n");
		for (const auto& header : headers_list)
		{
			size_t idx = header.find(L':');
			if (idx == std::wstring::npos)
				continue;

			std::wstring name = Utils::ToLower(Utils::Trim(header.substr(0, idx)));
			std::wstring value = Utils::Trim(header.substr(idx + 1));

			if (!name.empty() && !value.empty())
				response.Headers[name] = value; // last wins
		}

		if (read_response_payload)
		{
			OPCLOG(response.ProgressLog, "Load: Response body...");
			DWORD dwSize = 0;
			std::vector<uint8_t> buffer;
			do
			{
				if (!::WinHttpQueryDataAvailable(m_request, &dwSize))
					return response.OnErr(L"query_data_available");
				if (dwSize == 0)
					break;

				buffer.resize(dwSize);
				memset(buffer.data(), 0, buffer.size());

				DWORD dwDownloaded = 0;
				if (!::WinHttpReadData(m_request, buffer.data(), dwSize, &dwDownloaded))
				{
					response.ErrorMessage = L"read_data";
					break;
				}

				size_t wrote_size = ::fwrite(buffer.data(), 1, buffer.size(), m_file);
				if (wrote_size != buffer.size())
				{
					response.ErrorMessage = L"fwrite";
					break;
				}
			} while (dwSize > 0 && response.ErrorMessage.empty());

			::fclose(m_file);
			m_file = nullptr;

			// don't leave partial / errored responses lying around
			if ((response.HttpStatusCode / 100 != 2) || !response.ErrorMessage.empty())
			{
				OPCLOG(response.ProgressLog, "Load: Deleting errored output file");
				if (std::filesystem::exists(response.OutputFilePath))
					std::filesystem::remove(response.OutputFilePath);
			}
		}

		OPCLOG(response.ProgressLog, "Load: All done.");
		return response;
	}
}
