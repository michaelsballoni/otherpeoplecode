#pragma once

#include "utils.h"

namespace otherpeoplecode
{
	// Utility for turning a URL into its part.
	class UrlParts
	{
	public:
		uint16_t port = 0;
		std::wstring server;
		std::wstring request;

		std::wstring GetCachePath(const std::wstring& cacheDirectoryPath);

		static std::wstring Parse(const std::wstring& url, UrlParts& parts);
	};
}
