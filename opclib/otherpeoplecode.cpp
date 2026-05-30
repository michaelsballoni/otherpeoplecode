#include "pch.h"
#include "otherpeoplecode.h"

using namespace otherpeoplecode;

#include "utils.h"
#include "urlparts.h"
#include "download.h"

namespace otherpeoplecode
{
	HMODULE LoadLibraryWeb(const TCHAR* url)
	{
		// parse the URL first
		UrlParts url_parts;
		const char* part_error = UrlParts::Parse(Setup::GetObj().UrlBasePath + url, url_parts);
		if (part_error != nullptr) // not a valid URL?  treat it like a file!
			return ::LoadLibrary((Setup::GetObj().FileBasePath + url).c_str());

		// look in the cache first
		std::wstring cache_path = url_parts.GetCachePath(Setup::GetObj().CachePath);
		if (Utils::FileExists(cache_path))
		{
			// FORNOW - Do a HEAD request and compare timestamp/EID/content-length
			return ::LoadLibrary(cache_path.c_str());
		}

		// if not download the file to the cache
		Download loader;
		DWORD status_code = 0;
		const char* load_error = loader.Load(url_parts, cache_path, status_code);
		if (load_error != nullptr || status_code != 200)
			return NULL;
		else
			return ::LoadLibrary(cache_path.c_str());
	}

	void SetCachePath(const wchar_t* path)
	{
		Setup::GetObj().CachePath = path;
	}

	void SetUrlBasePath(const wchar_t* path)
	{
		Setup::GetObj().UrlBasePath = path;
	}

	void SetFileBasePath(const wchar_t* path)
	{
		Setup::GetObj().FileBasePath = path;
	}
}
