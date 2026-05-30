#include "pch.h"
#include "loadworker.h"
#include "urlparts.h"

namespace otherpeoplecode
{
	LoadWorker::LoadWorker(HINTERNET session)
		: m_session(session)
	{}

	LoadWorker::~LoadWorker()
	{
		::WinHttpCloseHandle(m_connection);
		::WinHttpCloseHandle(m_request);

		if (m_file != nullptr)
			::fclose(m_file);
	}

	const char* LoadWorker::Load(const UrlParts& parts, const std::wstring& path, DWORD& statusCode)
	{
		statusCode = 0;

#pragma warning(push)
#pragma warning(disable : 4996) // _CRT_SECURE_NO_WARNINGS
		m_file = ::_wfopen(path.c_str(), L"wb");
		if (!m_file)
			return "fopen";
#pragma warning(pop)

		m_connection = ::WinHttpConnect(m_session, parts.server.c_str(), parts.port, 0);
		if (m_connection == nullptr)
			return "connect";
		const wchar_t* types[]{ L"*/*", nullptr };
		m_request = ::WinHttpOpenRequest(m_connection, nullptr, parts.request.c_str(), nullptr, WINHTTP_NO_REFERER, (LPCWSTR*)&types, parts.port == 443 ? WINHTTP_FLAG_SECURE : 0);
		if (m_request == nullptr)
			return "open_request";

		DWORD_PTR ptr = 0;
		if (!::WinHttpSendRequest(m_request, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, ptr))
			return "send_request";
		if (!::WinHttpReceiveResponse(m_request, nullptr))
			return "receive_response";

		DWORD status_size = sizeof(DWORD);
		if (!::WinHttpQueryHeaders(m_request, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER, NULL, &statusCode, &status_size, NULL))
			return "status_code";

		DWORD dwSize = 0;
		std::vector<uint8_t> buffer;
		do {
			if (!::WinHttpQueryDataAvailable(m_request, &dwSize))
				return "query_data_available";
			if (dwSize == 0)
				break;

			buffer.resize(dwSize);
			memset(buffer.data(), 0, buffer.size());

			DWORD dwDownloaded = 0;
			if (!::WinHttpReadData(m_request, buffer.data(), dwSize, &dwDownloaded))
				return "read_data";

			size_t wrote_size = ::fwrite(buffer.data(), 1, buffer.size(), m_file);
			if (wrote_size != buffer.size())
				return "fwrite";

		} while (dwSize > 0);

		::fclose(m_file);
		m_file = nullptr;
		return "";
	}
}
