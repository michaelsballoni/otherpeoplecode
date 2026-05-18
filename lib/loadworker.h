#pragma once

#include "include.h"

namespace otherpeoplecode
{
	class LoadWorker
	{
	private:
		HINTERNET m_session = nullptr;
		HINTERNET m_connection = nullptr;
		HINTERNET m_request = nullptr;

		FILE* m_file = nullptr;

	public:
		LoadWorker(HINTERNET session)
			: m_session(session)
		{
		}

		~LoadWorker()
		{
			::WinHttpCloseHandle(m_connection);
			::WinHttpCloseHandle(m_request);

			fclose(m_file);
		}

		// FORNOW -	Do a HEAD request and check Last-Modified to see if download is needed
		const char* Load(const UrlParts& parts, const std::wstring& path)
		{
			m_connection = ::WinHttpConnect(m_session, parts.server.c_str(), parts.port, 0);
			if (m_connection == nullptr)
				return "WinHttpConnect";

			const wchar_t* types [] { L"*/*", nullptr };
			m_request = ::WinHttpOpenRequest(m_connection, nullptr, parts.request.c_str(), nullptr, WINHTTP_NO_REFERER, (LPCWSTR*)&types, 0);
			if (m_request == nullptr)
				return "WinHttpOpenRequest";

			DWORD_PTR ptr = 0;
			if (!::WinHttpSendRequest(m_request, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, ptr))
				return "WinHttpSendRequest";

			if (!::WinHttpReceiveResponse(m_request, nullptr))
				return "WinHttpReceiveResponse";

#pragma warning(push)
#pragma warning(disable : 4996) // _CRT_SECURE_NO_WARNINGS
			m_file = ::_wfopen(path.c_str(), L"wb");
			if (!m_file)
				return "fopen";
#pragma warning(pop)

			DWORD dwSize = 0;
			std::vector<uint8_t> buffer;
			do {
				if (!::WinHttpQueryDataAvailable(m_request, &dwSize))
					return "WinHttpQueryDataAvailable";
				if (dwSize == 0)
					break;

				buffer.resize(dwSize);
				memset(buffer.data(), 0, buffer.size());

				DWORD dwDownloaded = 0;
				if (!::WinHttpReadData(m_request, buffer.data(), dwSize, &dwDownloaded))
					return "WinHttpReadData";

				if (::fwrite(buffer.data(), buffer.size(), 0, m_file) != buffer.size())
					return "fwrite";

			} while (dwSize > 0);

			::fclose(m_file);
		}
	};
}
