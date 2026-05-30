#pragma once

#include "urlparts.h"

namespace otherpeoplecode
{
	class Loader
	{
	public:
		Loader();
		~Loader();
		const char* Load(const UrlParts& parts, const std::wstring& path, DWORD& statusCode);

	private:
		HINTERNET m_session;
	};
}
