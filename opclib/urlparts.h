#pragma once

#include "utils.h"

namespace otherpeoplecode
{
	// Utility for turning a URL into its part
	// Simplistic
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
