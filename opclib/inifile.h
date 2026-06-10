#pragma once

#include <map>
#include <optional>
#include <string>

namespace otherpeoplecode
{
	class IniFile
	{
	public:
		// Give the path to an INI-ish file, returns the key-value pairs found in the file
		// keys are returned lower-cased and trimmed
		// values are returned trimmed
		// Blank lines and lines starting with # are skipped
		// Returns the key-value entries on success
		static std::optional<std::map<std::wstring, std::wstring>> GetEntries(const std::wstring& filePath);

		// Given a path and key-value pairs, write an INI-ish file
		// Keys are written trimmed and lower-cased
		// Returns true on success
		static bool PutEntries(const std::wstring& filePath, const std::map<std::wstring, std::wstring>& entries);
	};
}
