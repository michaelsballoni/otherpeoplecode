#pragma once

namespace otherpeoplecode
{
	class IniFile
	{
	public:
		static std::optional<std::map<std::wstring, std::wstring>> GetEntries(const std::wstring& filePath);
		static bool PutEntries(const std::wstring& filePath, const std::map<std::wstring, std::wstring>& entries);
	};
}
