#pragma once

#include "urlparts.h"

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
		LoadWorker(HINTERNET session);
		~LoadWorker();
		const char* Load(const UrlParts& parts, const std::wstring& path, DWORD& statusCode);
	};
}
