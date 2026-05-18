#include "include.h"

namespace otherpeoplecode
{
	class Setup
	{
	public:
		std::wstring CachePath;
		std::wstring UrlBasePath;
		std::wstring FileBasePath;

		static Setup& GetObj()
		{
			static Setup obj;
			return obj;
		}
	};

	struct UrlParts
	{
		int port = 0;
		std::wstring server;
		std::wstring request;
	};

	class Utils
	{
	public:
		static const char* ParseUrl(std::wstring url, UrlParts& parts)
		{
			std::wstring lower_path;
			std::transform(url.begin(), url.end(), lower_path.begin(), [](wchar_t c) { return std::towlower(c); });

			bool is_http = lower_path.find(L"http://") == 0;
			bool is_https = lower_path.find(L"https://") == 0;
			if (is_http || is_https)
			{
				// FORNOW - Modernize this mess
				wchar_t* server_begin = (wchar_t* )wcsstr(lower_path.c_str(), L"//");
				if (server_begin == nullptr)
					return "server start";
				server_begin += 2;

				wchar_t* server_end = wcsstr(server_begin, L"/");
				if (server_end == nullptr) {
					parts.server = server_begin;
					parts.request = L"";
				}
				else 
				{
					server_end[0] = L'\0';
					parts.server = server_begin;
					parts.request = server_end + 1;
				}

				parts.port = is_http ? 80 : 443;
				wchar_t* port_str = (wchar_t*)wcsstr(parts.server.c_str(), L":");
				if (port_str != nullptr)
				{
					port_str[0] = L'\0';
					parts.port = _wtoi(port_str);
					if (parts.port <= 0 || parts.port > USHRT_MAX)
						return "port";
				}
			}
		}

		static std::wstring GetCachePath(const UrlParts& parts)
		{
			std::wstring output = Setup::GetObj().CachePath;
			output += parts.server + L"_" + parts.request + L"_" + std::to_wstring(parts.port);
			return output;
		}

		static bool FileExists(const std::wstring& path)
		{
			return std::filesystem::exists(path);
		}

		static std::wstring SafePathStr(const std::wstring& un)
		{
			std::wstring output;
			for (wchar_t c : un)
			{
				if (iswalnum(c))
					output += c;
				else
					output += '.';
			}
			
			while (!output.empty() && output.back() == '.')
				output.pop_back();

			if (output.empty())
				return L"empty";

			return output;
		}

		static void RaiseItemError(const char* msg, const wchar_t* item)
		{
			size_t converted_count = 0;
			size_t item_size = wcslen(item) + 1;
			char* item_narrow = new char[item_size];
			if (wcstombs_s(&converted_count, item_narrow, item_size, item, _TRUNCATE) != 0) {
				delete[] item_narrow;
				throw std::exception("RaiseItemError: wcstombs_s failed");
			}
			else
			{
				std::string item_str = item_narrow;
				delete[] item_narrow;
				throw std::runtime_error(msg + std::string(": ") + item_str);
			}
		}
	};
}
