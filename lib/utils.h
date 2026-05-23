
#pragma once

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

	class Utils
	{
	public:
		static bool StartsWith(const std::wstring& str, const wchar_t* starter)
		{
			if (str.empty() || !*starter)
				return false;

			size_t starter_len = wcslen(starter);
			if (starter_len > str.length())
				return false;

			for (size_t s = 0; s < starter_len; ++s)
			{
				if (str[s] != starter[s])
					return false;
			}

			return true;
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
				else if (output.empty() || output.back() != '.')
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
			if (msg == nullptr || !*msg)
				throw std::exception("Invalid error message");

			if (item == nullptr || !*item)
				throw std::exception(msg);

			size_t converted_count = 0;
			size_t item_size = wcslen(item) + 1;
			char* item_narrow = new char[item_size];
			if (wcstombs_s(&converted_count, item_narrow, item_size, item, _TRUNCATE) != 0) {
				delete[] item_narrow;
				throw std::exception("RaiseItemError: wcstombs_s failed");
			}

			std::string item_str = item_narrow;
			delete[] item_narrow;
			throw std::runtime_error(msg + std::string(": ") + item_str);
		}
	};
}
