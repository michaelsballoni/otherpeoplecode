#include "pch.h"
#include "urlparts.h"
#include "utils.h"

namespace otherpeoplecode
{
	std::wstring UrlParts::GetCachePath(const std::wstring& cacheDirectoryPath)
	{
		std::wstring output = cacheDirectoryPath;
		if (!output.empty() && output.back() != '\\')
			output += '\\';

		output += Utils::ToSafeStr(server);

		if (!request.empty())
		{
			output += '_';
			output += Utils::ToSafeStr(request);
		}

		output += '_';
		output += std::to_wstring(port);

		return output;
	}

	std::wstring UrlParts::Parse(const std::wstring& url, UrlParts& parts)
	{
		std::wstring lower_path = Utils::ToLower(url);

		bool is_http = Utils::StartsWith(lower_path, L"http://");
		bool is_https = Utils::StartsWith(lower_path, L"https://");
		if (!is_http && !is_https)
			return L"not url";

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
			return L"server";

		size_t port_idx = parts.server.find(':');
		if (port_idx != std::wstring::npos)
		{
			int port = _wtoi(parts.server.substr(port_idx + 1).c_str());
			if (port <= 0 || port > USHRT_MAX)
				return L"port";

			parts.port = static_cast<uint16_t>(port);
			parts.server = parts.server.substr(0, port_idx);
		}
		else
			parts.port = is_http ? 80 : 443;

		return L"";
	}
}
