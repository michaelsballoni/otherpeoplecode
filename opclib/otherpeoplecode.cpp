#include "pch.h"
#include "otherpeoplecode.h"
#include "inifile.h"
#include "utils.h"
#include "urlparts.h"
#include "loader.h"

namespace otherpeoplecode
{
	HMODULE otherpeoplecode::LoadLibraryWeb(const TCHAR* turl)
	{
		Loader loader;

		// punt on non-URL inputs first
		std::wstring url = Utils::TcharToWString(turl);
		if (!Utils::IsUrl(url))
			return ::LoadLibraryW(url.c_str());

		// parse the URL
		UrlParts url_parts;
		std::wstring part_error = UrlParts::Parse(url, url_parts);
		if (!part_error.empty())
		{
			::SetLastError(ERROR_INTERNET_INVALID_URL);
			return NULL;
		}

		// look in the cache first
		std::vector<std::wstring> headers_to_compare{ L"Last-Modified", L"Content-Length", L"ETag" };
		std::wstring url_cache_path = url_parts.GetCachePath(Setup::GetObj().CachePath);
		std::wstring url_cache_info_path = std::filesystem::path(url_cache_path).append(".info");
		if (Utils::FileExists(url_cache_path) && Utils::FileExists(url_cache_info_path))
		{
			// do the HEAD
			HttpRequest head_request(url, L"HEAD", url_cache_path);
			DWORD head_status_code = 0;
			HttpResponse head_response = loader.Load(head_request);
			if (head_status_code / 100 != 2)
			{
				::SetLastError(ERROR_INTERNET_CANNOT_CONNECT);
				return NULL;
			}

			// Open our INI file and compare its contents with the HEAD response
			auto info_file_path_settings_opt = IniFile::GetEntries(url_cache_info_path);
			if (info_file_path_settings_opt.has_value())
			{
				const auto& info_file_path_settings = info_file_path_settings_opt.value();
				if (!info_file_path_settings.empty())
				{
					for (auto header_name : headers_to_compare)
					{
						header_name = Utils::ToLower(Utils::Trim(header_name));
						auto header_it = head_response.Headers.find(header_name);
						auto info_it = info_file_path_settings.find(header_name);
						if 
						(
							header_it != head_response.Headers.end() && info_it != info_file_path_settings.end()
							&&
							header_it->second == info_it->second
						)
						{
							return ::LoadLibraryW(url_cache_path.c_str()); // at last
						}
					}
				}
			}
		}

		// failing that, download the file to the cache
		HttpRequest get_request(url, L"GET", url_cache_path);
		HttpResponse get_response = loader.Load(get_request);
		if (get_response.StatusCode != 200)
		{
			::SetLastError(ERROR_INTERNET_CANNOT_CONNECT);
			return NULL;
		}

		// write out the info file
		std::map<std::wstring, std::wstring> headers_to_info;
		for (auto header_name : headers_to_compare)
		{
			header_name = Utils::ToLower(Utils::Trim(header_name));
			auto get_it = get_response.Headers.find(header_name);
			if (get_it != get_response.Headers.end())
				headers_to_info[header_name] = get_it->second;
		}
		IniFile::PutEntries(url_cache_info_path, headers_to_info);

		// All done.
		return ::LoadLibraryW(url_cache_path.c_str());
	}

	void SetCachePath(const TCHAR* path)
	{
		Setup::GetObj().CachePath = Utils::TcharToWString(path);
	}
}
