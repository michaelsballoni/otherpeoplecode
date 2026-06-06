#include "pch.h"
#include "inifile.h"
#include "utils.h"

namespace otherpeoplecode
{
	std::optional<std::map<std::wstring, std::wstring>> IniFile::GetEntries(const std::wstring& filePath)
	{
		// open the file
		std::ifstream in_file(Utils::WstringToUtf8(filePath), std::ios::ate | std::ios::binary);
		if (!in_file.is_open())
			return std::nullopt;

		// get the file size
		std::streamsize file_size = in_file.tellg();
		in_file.seekg(0, std::ios::beg);
		if (file_size <= 0)
			return std::map<std::wstring, std::wstring>();

		// read the file into memory
		std::string file_str(file_size, L'\0');
		in_file.read(reinterpret_cast<char*>(file_str.data()), file_size);
		in_file.close();

		// erase the BOM
		if (!file_str.size() >= 2 && file_str[0] == 0xFF && file_str[1] == 0xFE)
			file_str.erase(0, 2);

		// get the lines
		std::vector<std::wstring> lines = Utils::Split(Utils::Utf8ToWString(file_str), L"\r\n");
		std::map<std::wstring, std::wstring> output;
		for (auto line : lines)
		{
			// skip blank lines
			line = Utils::Trim(line);
			if (line.empty())
				continue;

			// separate key from value
			auto split = Utils::Split(line, L"=");
			if (split.size() != 2)
				return std::nullopt;

			std::wstring key = Utils::ToLower(Utils::Trim(split[0]));
			std::wstring val = Utils::Trim(split[1]);

			// set the output, last in wins
			output[key] = val;
		}

		// All done.
		return output;
	}

	bool IniFile::PutEntries(const std::wstring& filePath, const std::map<std::wstring, std::wstring>& entries)
	{
		// open the output file
		std::ofstream out_file(Utils::WstringToUtf8(filePath), std::ios::out | std::ios::binary);
		if (!out_file.is_open())
			return false;

		// write the BOM
		unsigned char bom[] = { 0xEF, 0xBB, 0xBF };
		out_file.write(reinterpret_cast<char*>(bom), 3);

		// walk the entries building a string to output
		std::wstring output;
		for (const auto& it : entries)
			output += Utils::ToLower(Utils::Trim(it.first)) + L"=" + Utils::Trim(it.second) + L"\r\n";

		// write the output string out to disk
		out_file << Utils::WstringToUtf8(output);
		out_file.close();

		// All done.
		return true;
	}
}
