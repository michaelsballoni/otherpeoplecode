#include "pch.h"
#include "otherpeoplecode.h"
#include "utils.h"
#include "urlparts.h"
#include "loader.h"

namespace otherpeoplecode
{
	HMODULE LoadLibraryWeb(const TCHAR* turl)
	{
		Loader loader;

		// parse the URL first
		std::wstring url = Setup::GetObj().UrlBasePath + Utils::TcharToWString(turl);
		UrlParts url_parts;
		std::wstring part_error = UrlParts::Parse(url, url_parts);
		if (!part_error.empty()) // not a valid URL?  treat it like a file!
			return ::LoadLibrary((Setup::GetObj().FileBasePath + url).c_str());

		// look in the cache first
		std::wstring cache_path = url_parts.GetCachePath(Setup::GetObj().CachePath);
		if (Utils::FileExists(cache_path))
		{
			// do a HEAD request and compare timestamp/EID/content-length
			HttpRequest head_request(url, L"HEAD", cache_path);
			DWORD head_status_code = 0;
			HttpResponse head_response = loader.Load(head_request);
			if (head_response.ErrorMessage != L"" || head_status_code != 200)
			{
				::SetLastError(ERROR_INTERNET_CANNOT_CONNECT);
				return NULL;
			}
			// FORNOW - Look for the INI file and pull out its settings

			return ::LoadLibrary(cache_path.c_str());
		}

		// failing that, download the file to the cache
		HttpRequest get_request(url, L"GET", cache_path);
		DWORD status_code = 0;
		HttpResponse get_response = loader.Load(get_request);
		if (get_response.ErrorMessage != L"" || status_code != 200)
		{
			::SetLastError(ERROR_INTERNET_CANNOT_CONNECT);
			return NULL;
		}
		else
			return ::LoadLibrary(cache_path.c_str());
	}

	void SetCachePath(const TCHAR* path)
	{
		Setup::GetObj().CachePath = Utils::TcharToWString(path);
	}

	void SetUrlBasePath(const TCHAR* path)
	{
		Setup::GetObj().UrlBasePath = Utils::TcharToWString(path);
	}

	void SetFileBasePath(const TCHAR* path)
	{
		Setup::GetObj().FileBasePath = Utils::TcharToWString(path);
	}
}
