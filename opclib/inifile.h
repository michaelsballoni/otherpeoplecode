#pragma once

#include <map>
#include <optional>
#include <string>

namespace otherpeoplecode
{
	class IniFile
	{
	public:
		// Given the path to a name-value info file, returns the key-value pairs found in the file.
		// Keys are returned lower-cased and trimmed.
		// Values are returned trimmed.
		// Blank lines are skipped.
		// Returns the name-value entries on success.
		static std::optional<std::map<std::wstring, std::wstring>> GetEntries(const std::wstring& filePath);

		// Given a path and name-value pairs, write a names-and-values info file.
		// Keys are written trimmed and lower-cased.
		// Returns true on success.
		static bool PutEntries(const std::wstring& filePath, const std::map<std::wstring, std::wstring>& entries);
	};
}
