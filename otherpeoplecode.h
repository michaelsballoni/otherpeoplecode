#pragma once

#include "lib/include.h"
#include "lib/utils.h"
#include "lib/load.h"

namespace otherpeoplecode
{
	inline void SetCachePath(const wchar_t* path)
	{
		Setup::GetObj().CachePath = path;
	}

	inline void SetUrlBasePath(const wchar_t* path)
	{
		Setup::GetObj().UrlBasePath = path;
	}

	inline void SetFileBasePath(const wchar_t* path)
	{
		Setup::GetObj().FileBasePath = path;
	}

	inline HMODULE LoadLibraryWeb(const wchar_t* url)
	{
		// parse the URL first
		UrlParts url_parts;
		const char* part_error = Utils::ParseUrl(Setup::GetObj().UrlBasePath + url, url_parts);
		if (part_error != nullptr) // not a valid URL?  treat it like a file!
			return ::LoadLibrary((Setup::GetObj().FileBasePath + url).c_str());

		// look in the cache first
		std::wstring cache_path = Utils::GetCachePath(url_parts);
		if (Utils::FileExists(cache_path))
		{
			// FORNOW - Do a HEAD request and compare timestamp/EID/content-length
			return ::LoadLibrary(cache_path.c_str());
		}

		// if not download the file to the cache
		Loader loader;
		const char* load_error = loader.Load(url_parts, cache_path);
		if (load_error != nullptr)
			return NULL;
		else
			return ::LoadLibrary(cache_path.c_str());
	}
}
