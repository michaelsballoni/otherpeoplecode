#pragma once

#include "utils.h"

namespace otherpeoplecode
{
	struct UrlParts
	{
		int port = 0;
		std::wstring server;
		std::wstring request;

		std::wstring GetCachePath(std::wstring cachePath)
		{
			std::wstring output = cachePath;
			output += server + L"_" + request + L"_" + std::to_wstring(port);
			return output;
		}
	};

	class UrlParse
	{
	public:
		const char* Parse(std::wstring url, UrlParts& parts)
		{
			std::wstring lower_path;
			std::transform(url.begin(), url.end(), lower_path.begin(), [](wchar_t c) { return std::towlower(c); });

			bool is_http = Utils::StartsWith(lower_path, L"http://");
			bool is_https = Utils::StartsWith(lower_path, L"https://");
			if (!is_http && !is_https)
				return "not url";

			size_t after_http_idx = url.find(L"//");
			if (after_http_idx == std::wstring::npos)
				return "server start";

			after_http_idx += 2;

			size_t after_server_idx = url.find('/', after_http_idx);
			if (after_server_idx >= url.length())
				return "after server";
			if (after_server_idx == std::wstring::npos)
			{
				parts.server = url.substr(after_http_idx);
				parts.request.clear();
			}
			else
			{
				parts.server = url.substr(after_http_idx, after_server_idx - 1 - after_http_idx);
				parts.request = url.substr(after_server_idx + 1);
			}

			size_t port_idx = parts.server.find(':');
			if (port_idx > url.length())
				return "after server";

			if (port_idx != std::wstring::npos)
			{
				parts.port = _wtoi(parts.server.substr(port_idx + 1).c_str());
				if (parts.port <= 0 || parts.port > USHRT_MAX)
					return "port";
			}
			else
				parts.port = is_http ? 80 : 443;

			return nullptr;
		}
	};
}
