#include "pch.h"
#include "utils.h"

namespace otherpeoplecode
{
	Setup& Setup::GetObj()
	{
		static Setup obj;
		return obj;
	}

	std::wstring Utils::ToLower(const std::wstring& str)
	{
		std::wstring ret_val;
		ret_val.reserve(str.size());
		for (auto c : str)
			ret_val += towlower(c);
		return ret_val;
	}

	bool Utils::StartsWith(const std::wstring& str, const wchar_t* starter)
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

	std::wstring Utils::Trim(const std::wstring& str)
	{
		if (str.empty())
			return std::wstring();

		if (str.length() == 1)
			return iswspace(str[0]) ? std::wstring() : str;

		if (!iswspace(str.front()) && !iswspace(str.back()))
			return str;

		std::wstring retVal;
		retVal.reserve(str.length());

		// skip whitespace
		size_t c = 0;
		while (c < str.length() && iswspace(str[c]))
			++c;

		// copy the rest
		while (c < str.length())
			retVal.push_back(str[c++]);

		// pop whitespace
		while (!retVal.empty() && iswspace(retVal.back()))
			retVal.pop_back();

		return retVal;
	}

	bool Utils::FileExists(const std::wstring& path)
	{
		return std::filesystem::exists(path);
	}

	std::wstring Utils::SafePathStr(const std::wstring& un)
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

	void Utils::RaiseItemError(const char* msg, const wchar_t* item)
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

	std::wstring Utils::LoadFileIntoMemory(const std::wstring& filePath, std::vector<uint8_t>& output)
	{
#pragma warning(push)
#pragma warning(disable : 4996) // _CRT_SECURE_NO_WARNINGS
		FILE* file = ::_wfopen(filePath.c_str(), L"rb");
		if (!file)
			return L"fopen";
#pragma warning(pop)
		if (file == nullptr)
			return L"fopen";
		if (::fseek(file, 0, SEEK_END) != 0)
			return L"fseek";
		long file_len = ::ftell(file);
		if (file_len < 0)
			return L"ftell";
		if (::fseek(file, 0, SEEK_SET) != 0)
			return L"fseek2";

		output.reserve(file_len);

		char buffer[4096];
		size_t total_read = 0;
		while (true)
		{
			size_t read_amount = ::fread(buffer, 1, sizeof(buffer), file);
			total_read += read_amount;
			if (read_amount == 0)
			{
				if (total_read != file_len)
					return L"fread";
				else
					break;
			}

			size_t original_size = output.size();
			output.resize(original_size + read_amount);
			memcpy(output.data() + original_size, buffer, read_amount);
		}

		return L"";
	}

	std::wstring Utils::AsciiBytesToWStr(const std::vector<uint8_t>& bytes)
	{
		std::wstring output;
		for (auto b : bytes)
			output += (wchar_t)(char)b;
		return output;
	}

	std::string Utils::WstringToUtf8(const std::wstring& str)
	{
		if (str.empty())
			return std::string();

		int size_needed = WideCharToMultiByte(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0, NULL, NULL);
		std::string output(size_needed, 0);
		WideCharToMultiByte(CP_UTF8, 0, str.data(), (int)str.size(), output.data(), size_needed, NULL, NULL);
		return output;
	}

	std::vector<std::wstring> Utils::Split(const std::wstring & str, const std::wstring & seperator)
	{
		std::vector<std::wstring> retVal;
		if (seperator.empty())
		{
			retVal.push_back(str);
		}
		else if (seperator.size() == 1)
		{
			std::wstring acc;
			const wchar_t sep = seperator[0];
			for (wchar_t c : str)
			{
				if (c == sep)
				{
					retVal.push_back(acc);
					acc.clear();
				}
				else
					acc.push_back(c);
			}
			if (!acc.empty())
				retVal.push_back(acc);
		}
		else
		{
			wchar_t* last_sep = const_cast<wchar_t*>(str.c_str());
			const size_t sep_len = seperator.length();
			while (last_sep != nullptr && last_sep[0] != '\0')
			{
				wchar_t* next_sep = wcsstr(last_sep, seperator.c_str());
				if (next_sep == nullptr)
				{
					retVal.push_back(last_sep);
					break;
				}

				retVal.emplace_back(last_sep, next_sep);
				last_sep = next_sep + sep_len;
			}
		}
		return retVal;
	}

	std::wstring Utils::TcharToWString(const wchar_t* str)
	{
		if (!str || !*str)
			return L"";
		else
			return std::wstring(str);
	}

	std::wstring Utils::TcharToWString(const char* str) 
	{
		if (!str || !*str)
			return L"";

		int size_needed = MultiByteToWideChar(CP_ACP, 0, str, -1, nullptr, 0);
		if (size_needed <= 0)
			return L"";

		std::wstring result(size_needed - 1, L'\0'); // -1 to exclude the null terminator
		MultiByteToWideChar(CP_ACP, 0, str, -1, result.data(), size_needed);
		return result;
	}

	std::wstring Utils::ToWideStr(const std::string& str)
	{
		if (str.empty())
			return std::wstring();

		bool allNarrow = true;
		{
			const unsigned char* bytes = reinterpret_cast<const unsigned char*>(str.data());
			for (size_t i = 0; i < str.size(); ++i)
			{
				if (bytes[i] > 127)
				{
					allNarrow = false;
					break;
				}
			}
		}

		if (allNarrow)
		{
			std::wstring retVal;
			retVal.reserve(str.size());
			for (auto c : str)
				retVal += char(c);
			return retVal;
		}

		int needed = MultiByteToWideChar(CP_UTF8, 0, str.data(), int(str.size()), nullptr, 0);
		if (needed <= 0)
			return L"";

		std::wstring result(needed, 0);
		MultiByteToWideChar(CP_UTF8, 0, str.data(), int(str.size()), result.data(), needed);
		return result;
	}

	std::string Utils::ToNarrowStr(const std::wstring& str)
	{
		if (str.empty())
			return std::string();

		bool allAscii = true;
		for (wchar_t c : str)
		{
			if (c <= 0 || c > 127)
			{
				allAscii = false;
				break;
			}
		}

		if (allAscii)
		{
			std::string retVal;
			retVal.reserve(str.size());
			for (auto c : str)
				retVal += char(c);
			return retVal;
		}

		int needed = WideCharToMultiByte(CP_UTF8, 0, str.data(), int(str.size()), nullptr, 0, nullptr, nullptr);
		if (needed <= 0)
			return "";

		std::string output(needed, 0);
		WideCharToMultiByte(CP_UTF8, 0, str.data(), int(str.size()), output.data(), needed, nullptr, nullptr);
		return output;
	}
}
