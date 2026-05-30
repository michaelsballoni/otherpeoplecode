#pragma once

#include "loadworker.h"
#include "urlparts.h"

namespace otherpeoplecode
{
	class Download
	{
	public:
		Download();
		~Download();
		const char* Load(const UrlParts& parts, const std::wstring& path, DWORD& statusCode);
	private:
		HINTERNET m_session;
	};
}
