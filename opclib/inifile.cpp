#include "pch.h"
#include "inifile.h"
#include "utils.h"

namespace otherpeoplecode
{
	std::optional<std::map<std::wstring, std::wstring>> IniFile::GetEntries(const std::wstring& filePath)
	{
		std::ifstream in_file(Utils::WstringToUtf8(filePath), std::ios::ate | std::ios::binary);
		if (!in_file.is_open())
			return std::nullopt;

		std::streamsize file_size = in_file.tellg();
		in_file.seekg(0, std::ios::beg);
		if (file_size <= 0)
			return std::map<std::wstring, std::wstring>();

		std::wstring file_str(file_size / sizeof(wchar_t), L'\0');
		in_file.read(reinterpret_cast<char*>(file_str.data()), file_size);
		in_file.close();

		if (!file_str.empty() && file_str[0] == 0xFEFF)
			file_str.erase(0, 1);

		std::vector<std::wstring> lines = Utils::Split(file_str, L"\r\n");

		std::map<std::wstring, std::wstring> output;
		for (auto line : lines)
		{
			line = Utils::Trim(line);
			if (line.empty() || line[0] == L'#') // comments
				continue;

			auto split = Utils::Split(line, L"=");
			if (split.size() != 2)
				return std::nullopt;

			std::wstring key = Utils::Trim(split[0]);
			std::wstring val = Utils::Trim(split[1]);

			if (output.find(key) == output.end())
				return std::nullopt;

			output[key] = val;
		}
		return output;
	}

	bool IniFile::PutEntries(const std::wstring& filePath, const std::map<std::wstring, std::wstring>& entries)
	{
		std::ofstream out_file(Utils::WstringToUtf8(filePath), std::ios::out | std::ios::binary);
		if (!out_file.is_open())
			return false;

		unsigned char bom[] = { 0xEF, 0xBB, 0xBF };
		out_file.write(reinterpret_cast<char*>(bom), 3);

		std::wstring output;
		for (const auto& it : entries)
			output += it.first + L"=" + it.second + L"\r\n";

		out_file << Utils::WstringToUtf8(output);
		out_file.close();

		return true;
	}
}
