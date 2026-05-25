
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

		static const char* LoadFileIntoMemory(const std::wstring& filePath, std::vector<uint8_t>& output)
		{
#pragma warning(push)
#pragma warning(disable : 4996) // _CRT_SECURE_NO_WARNINGS
			FILE* file = ::_wfopen(filePath.c_str(), L"rb");
			if (!file)
				return "fopen";
#pragma warning(pop)
			if (file == nullptr)
				return "fopen";
			if (::fseek(file, 0, SEEK_END) != 0)
				return "fseek";
			long file_len = ::ftell(file);
			if (file_len < 0)
				return "ftell";
			if (::fseek(file, 0, SEEK_SET) != 0)
				return "fseek2";

			output.reserve(file_len);

			char buffer[4096];
			while (true)
			{
				size_t read_amount = ::fread(buffer, 1, sizeof(buffer), file);
				if (read_amount == 0)
					break;

				size_t original_size = output.size();
				output.resize(original_size + read_amount);
				memcpy(output.data() + original_size, buffer, read_amount);
			}
			return "";
		}

		static std::wstring AsciiBytesToWStr(const std::vector<uint8_t>& bytes)
		{
			std::wstring output;
			for (auto b : bytes)
				output += (wchar_t)(char)b;
			return output;
		}
	};
}
