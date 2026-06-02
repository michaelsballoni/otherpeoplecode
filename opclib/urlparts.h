#pragma once

#include "utils.h"

namespace otherpeoplecode
{
	class UrlParts
	{
	public:
		int port = 0;
		std::wstring server;
		std::wstring request;

		std::wstring GetCachePath(std::wstring cacheDirectoryPath);

		static std::wstring Parse(std::wstring url, UrlParts& parts);
	};
}
