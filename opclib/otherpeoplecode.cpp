#include "pch.h"
#include "otherpeoplecode.h"
#include "inifile.h"
#include "utils.h"
#include "urlparts.h"
#include "loader.h"

#include <mutex>

namespace otherpeoplecode
{
	std::mutex g_loadMutex;

	HMODULE otherpeoplecode::LoadLibraryWeb
	(
		std::wstring url, 
		std::wstring cacheDirectory, 
		FILE* progressLog
	)
	{
#ifndef _DEBUG
		try
		{
#endif
			// one load at a time to protect the cache directory 
			// and keep heavy load off servers
			std::unique_lock lock(g_loadMutex);

			// parse the URL
			UrlParts url_parts;
			std::wstring part_error = UrlParts::Parse(url, url_parts);
			if (!part_error.empty())
			{
				OPCLOG(progressLog, "LoadLibraryWeb: Invalid URL: %ls", part_error.c_str());
				::SetLastError(ERROR_INTERNET_INVALID_URL);
				return NULL;
			}

			// punt on non-URL inputs first...file?
			OPCLOG(progressLog, "LoadLibraryWeb: %ls", url.c_str());
			// normalize and ensure the cache directory path exists
			if (!cacheDirectory.empty())
				cacheDirectory = L"OtherPeopleCode"; // wherever we are
			if (cacheDirectory.back() != '\\')
				cacheDirectory += '\\';
			cacheDirectory = std::filesystem::absolute(cacheDirectory);
			OPCLOG(progressLog, "LoadLibraryWeb: Cache directory: %ls", cacheDirectory.c_str());
			std::filesystem::create_directories(cacheDirectory);


			// look in the cache first
			std::vector<std::wstring> headers_to_compare{ L"Last-Modified", L"Content-Length", L"ETag" };
			std::wstring url_cache_path = url_parts.GetCachePath(cacheDirectory) + L".dll";
			std::wstring url_cache_info_path = url_cache_path + L".info";
			if (Utils::FileExists(url_cache_path) && Utils::FileExists(url_cache_info_path))
			{
				// do the HEAD
				OPCLOG(progressLog, "LoadLibraryWeb: Local file and INI exist; will make HEAD request to check for changes");
				Loader head_loader;
				HttpRequest head_request(url, L"HEAD", url_cache_path);
				HttpResponse head_response = head_loader.Load(head_request);
				if (head_response.StatusCode / 100 != 2)
				{
					OPCLOG(progressLog, "LoadLibraryWeb: HEAD FAILED: %i %ls", (int)head_response.StatusCode, head_response.ErrorMessage.c_str());
					::SetLastError(ERROR_WINHTTP_INVALID_SERVER_RESPONSE);
					return NULL;
				}

				OPCLOG(progressLog, "HEAD Headers:");
				for (auto it : head_response.Headers)
					OPCLOG(progressLog, "%ls: %ls", it.first.c_str(), it.second.c_str());

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
									)
							{
								if (head_it->second != info_it->second) // mismatch
								{
									OPCLOG(progressLog, "LoadLibraryWeb: HEAD and INI differ: %ls", header_name.c_str());
									OPCLOG(progressLog, "HEAD %ls", head_it->second.c_str());
									OPCLOG(progressLog, "INI  %ls", info_it->second.c_str());
									all_match = false;
									break;
								}
								else
									any_match = true;
							}
							else if
							(
								head_it == head_response.Headers.end()
								&&
								info_it == info_file_path_settings.end()
							)
							{
								all_match = false;
								break;
							}
							else // one has and other doesn't
							{
								all_match = false;
							}
						}

						if (all_match && any_match)
						{
							OPCLOG(progressLog, "LoadLibraryWeb: HEAD and INI match...LoadLibrary from cache!");
							return ::LoadLibraryW(url_cache_path.c_str()); // load the library out of the cache
						}
					}
				}
			}

			// failing that, download the file to the cache
			OPCLOG(progressLog, "LoadLibraryWeb: GET");
			Loader get_loader;
			HttpRequest get_request(url, L"GET", url_cache_path);
			HttpResponse get_response = get_loader.Load(get_request);
			if (get_response.StatusCode != 200)
			{
				OPCLOG(progressLog, "LoadLibraryWeb: GET FAILED: %i %ls", (int)get_response.StatusCode, get_response.ErrorMessage.c_str());
				::SetLastError(ERROR_WINHTTP_INVALID_SERVER_RESPONSE);
				return NULL;
			}
			OPCLOG(progressLog, "LoadLibraryWeb: GET Headers:");
			for (auto it : get_response.Headers)
				OPCLOG(progressLog, "%ls: %ls", it.first.c_str(), it.second.c_str());

			// write out the info file
			OPCLOG(progressLog, "LoadLibraryWeb: Writing INI");
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
			OPCLOG(progressLog, "LoadLibraryWeb: GET success, LoadLibrary from cache!");
			return ::LoadLibraryW(url_cache_path.c_str());
#ifndef _DEBUG
		}
		catch (const std::exception& exp)
		{
			OPCLOG("LoadLibraryWeb: EXCEPTION: %s", exp.what());
		}
#endif
	}
}
