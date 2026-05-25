#pragma once

#include "utils.h"

namespace otherpeoplecode
{
	struct UrlParts
	{
		int port = 0;
		std::wstring server;
		std::wstring request;

		std::wstring GetCachePath(std::wstring cacheDirectoryPath)
		{
			std::wstring output = cacheDirectoryPath + L"\\" + server;
			if (!request.empty())
				output += L"_" + request;
			output += L"_" + std::to_wstring(port);
			return output;
		}
	};

	class UrlParse
	{
	public:
		static const char* Parse(std::wstring url, UrlParts& parts)
		{
			std::wstring lower_path;
			for (wchar_t u : url)
				lower_path.push_back(std::towlower(u));

			bool is_http = Utils::StartsWith(lower_path, L"http://");
			bool is_https = Utils::StartsWith(lower_path, L"https://");
			if (!is_http && !is_https)
				return "not url";

			size_t after_http_idx = url.find(L"//") + 2;
			std::wstring server_and_rest = url.substr(after_http_idx);
			size_t after_server_idx = server_and_rest.find('/');
			if (after_server_idx == std::wstring::npos)
			{
				parts.server = server_and_rest;
				parts.request.clear();
			}
			else
			{
				parts.server = server_and_rest.substr(0, after_server_idx);
				parts.request = server_and_rest.substr(after_server_idx + 1);
			}
			if (parts.server.empty())
				return "server";

			size_t port_idx = parts.server.find(':');
			if (port_idx != std::wstring::npos)
			{
				parts.port = _wtoi(parts.server.substr(port_idx + 1).c_str());
				if (parts.port <= 0 || parts.port > USHRT_MAX)
					return "port";
				parts.server = parts.server.substr(0, port_idx);
			}
			else
				parts.port = is_http ? 80 : 443;

			return "";
		}
	};
}
