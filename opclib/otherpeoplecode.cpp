#include "pch.h"
#include "otherpeoplecode.h"
#include "inifile.h"
#include "utils.h"
#include "urlparts.h"
#include "loader.h"

#include <iostream>

namespace otherpeoplecode
{
	HMODULE otherpeoplecode::LoadLibraryWeb(const TCHAR* turl)
	{
		Loader loader;

		// punt on non-URL inputs first...file?
		std::wstring url = Utils::TcharToWString(turl);
		std::wcout << L"LoadLibraryWeb: " << url << std::endl;
		if (!Utils::IsUrl(url))
		{
			std::wcout << L"LoadLibraryWeb: Not a URL, loading like a file" << std::endl;
			return ::LoadLibraryW(url.c_str());
		}

		// parse the URL
		UrlParts url_parts;
		std::wstring part_error = UrlParts::Parse(url, url_parts);
		if (!part_error.empty())
		{
			std::wcout << L"LoadLibraryWeb: Invalid URL: " << part_error << std::endl;
			::SetLastError(ERROR_INTERNET_INVALID_URL);
			return NULL;
		}

		// look in the cache first
		std::vector<std::wstring> headers_to_compare{ L"Last-Modified", L"Content-Length", L"ETag" };
		std::wstring url_cache_path = url_parts.GetCachePath(Setup::GetObj().CachePath) + L".dll";
		std::wstring url_cache_info_path = url_cache_path + L".info";
		if (Utils::FileExists(url_cache_path) && Utils::FileExists(url_cache_info_path))
		{
			// do the HEAD
			std::wcout << L"LoadLibraryWeb: Local file and INI exist; will make HEAD request to check for changes" << std::endl;
			HttpRequest head_request(url, L"HEAD", url_cache_path);
			HttpResponse head_response = loader.Load(head_request);
			if (head_response.StatusCode / 100 != 2)
			{
				std::wcout << L"LoadLibraryWeb: HEAD failed: " << head_response.StatusCode << " " << head_response.ErrorMessage << std::endl;
				::SetLastError(ERROR_INTERNET_CANNOT_CONNECT);
				return NULL;
			}

			std::wcout << "HEAD Headers:" << std::endl;
			for (auto it : head_response.Headers)
				std::wcout << it.first << L": " << it.second << std::endl;

			// Open our INI file and compare its contents with the HEAD response
			auto info_file_path_settings_opt = IniFile::GetEntries(url_cache_info_path);
			if (info_file_path_settings_opt.has_value())
			{
				const auto& info_file_path_settings = info_file_path_settings_opt.value();
				if (!info_file_path_settings.empty())
				{
					bool all_match = true;
					bool any_match = false;
					for (auto header_name : headers_to_compare)
					{
						header_name = Utils::ToLower(Utils::Trim(header_name));
						auto head_it = head_response.Headers.find(header_name);
						auto info_it = info_file_path_settings.find(header_name);
						if 
						(
							head_it != head_response.Headers.end() 
							&& 
							info_it != info_file_path_settings.end()
							&&
							head_it->second != info_it->second // mismatch
						)
						{
							std::wcout 
								<< L"LoadLibraryWeb: HEAD and INI differ: " << header_name << L": " 
								<< L"HEAD " << head_it->second << L" vs. INI " << info_it->second
								<< std::endl;
							all_match = false;
							break;
						}
						else
							any_match = false;
					}

					if (all_match && any_match)
					{
						std::wcout << L"LoadLibraryWeb: HEAD and INI match...LoadLibrary from cache!" << std::endl;
						return ::LoadLibraryW(url_cache_path.c_str()); // load the library out of the cache
					}
				}
			}
		}

		// failing that, download the file to the cache
		std::wcout << L"LoadLibraryWeb: GET" << std::endl;
		HttpRequest get_request(url, L"GET", url_cache_path);
		HttpResponse get_response = loader.Load(get_request);
		if (get_response.StatusCode != 200)
		{
			std::wcout << L"LoadLibraryWeb: GET failed: " << get_response.StatusCode << " " << get_response.ErrorMessage << std::endl;
			::SetLastError(ERROR_INTERNET_CANNOT_CONNECT);
			return NULL;
		}
		std::wcout << "GET Headers:" << std::endl;
		for (auto it : get_response.Headers)
			std::wcout << it.first << L": " << it.second << std::endl;

		// write out the info file
		std::wcout << L"LoadLibraryWeb: Writing INI" << std::endl;
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
		std::wcout << L"LoadLibraryWeb: GET success" << std::endl;
		return ::LoadLibraryW(url_cache_path.c_str());
	}

	void SetCachePath(const TCHAR* path)
	{
		std::wstring wpath = Utils::TcharToWString(path);
		std::wcout << L"SetCachePath: " << wpath << std::endl;
		Setup::GetObj().CachePath = wpath;
	}
}
